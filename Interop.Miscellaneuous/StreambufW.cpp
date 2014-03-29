// Copyright (c) 2013 Francesco Pretto
// This file is subject to the MIT license

#include "StreambufW.h"

using namespace std;
using namespace Interop::Miscellaneous;

/*

  Input Stream Buffer:
 
  [.]  [.]  .. [.]  [.]  [.]  .. [.]  [ ]
   ^            ^                      ^
   |            |                      |
 eback         gptr                  egptr

 Output Stream Buffer:

  [.]  [.]  .. [.]  [.]  [.]  .. [.]  [ ]
   ^            ^                      ^
   |            |                      |
 pbase         pptr                  eeptr

*/

StreambufW::StreambufW(FileStream ^stream, size_t buffCapacity, size_t putBackCap) :
	_wrapped(stream),
    _putBackCap(max(putBackCap, size_t(1)))
{
	array<Byte> ^buff = gcnew array<Byte>(max(buffCapacity, putBackCap) + putBackCap);

	// The buffer is stored in a GCHandle and not on a gcroot so we can pin it for the
	// StreambufW istance lifetime
	_buffHandle = GCHandle::Alloc(buff, GCHandleType::Pinned);

	char *base = (char *)_buffHandle.AddrOfPinnedObject().ToPointer();
    char *end = base + buff->Length;

	// Set input pointer sequence (end of the buffer: no reads yet)
    setg(end, end, end);

	// Set output pointer sequence (max size - 1, so we can detect overflow one
	// characteer before the buffer is actually filled)
	setp(base, end - 1);
}

StreambufW::~StreambufW()
{
	flush();
	_buffHandle.Free();
}

streambuf::int_type StreambufW::underflow()
{
	if (!_wrapped->CanRead)
		return EOF;

	char *gPtr = gptr();
	char *egPtr = egptr();

	// buffer not exhausted
    if (gPtr < egPtr)
        return traits_type::to_int_type(*gPtr);

	// Begin of the buffer
    char *base = (char *)_buffHandle.AddrOfPinnedObject().ToPointer();

	char *eBack = eback();
	char *eBackNext = base;
	char *gPtrNext = base;
    if (eBack == base) // true when this isn't the first fill
    {
		// Calculate put back area size. It can't be more than its capacity!
		size_t putBackSize = gPtr - eBack;
		if (putBackSize > _putBackCap)
			putBackSize = _putBackCap;

        // Copy characters to the putback area up to its max capacity 
		size_t putBackStartIndex = _putBackCap - putBackSize;
        memmove(base + putBackStartIndex, egPtr - putBackSize, putBackSize);

		eBackNext = base + putBackStartIndex; // Begin of putback area
		char *gPtrNext = base + _putBackCap;  // Set read position after putback area
    }

	size_t n;
	array<Byte> ^buff = static_cast<array<Byte> ^>(_buffHandle.Target);
	try
	{
		// Read from FileStream in to the provided buffer. If this is not the first fill,
		// the max capacity of the put back area is the offset where to start writing
		// in the buffer
		n = _wrapped->Read(buff, gPtrNext - base, buff->Length - _putBackCap);
	}
	catch (...)
	{
		return EOF;
	}

    if (n == 0)
        return traits_type::eof();

	// Set the end of the buffer after putback area + n bytes read
	char *egPtrNext = gPtrNext + n;

    // Set the new buffer pointers
    setg(eBackNext, gPtrNext, egPtrNext);

    return traits_type::to_int_type(*egPtrNext);
}

streambuf::int_type StreambufW::overflow(int_type c)
{
	if (!_wrapped->CanWrite)
		return EOF;

	if (c != EOF)
	{
		// We still have one left char room in the output seqeunce
		*pptr() = c;
		pbump(1);
	}

	if (flush() == -1)
		return EOF;

	return c;
}

int StreambufW::sync()
{
	if (!_wrapped->CanWrite)
		return -1;

	if (flush() == -1)
		return -1;

	return 0;
}

int StreambufW::flush()
{
	int toWriteCount = pptr() - pbase();
	array<Byte> ^buff = static_cast<array<Byte> ^>(_buffHandle.Target);

	try
	{
		_wrapped->Write(buff, 0, toWriteCount);
	}
	catch (...)
	{
		return -1;
	}

	// Shift current pointer position
	pbump(-toWriteCount);

	return toWriteCount;
}
