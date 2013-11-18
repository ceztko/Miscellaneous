#include "StreambufW.h"

using namespace std;
using namespace Interop::Miscellaneous;

StreambufW::StreambufW(FileStream ^stream, size_t buffCapacity, size_t putBack) :
	_wrapped(stream),
    _putBack(max(putBack, size_t(1)))
{
	array<Byte> ^buff = gcnew array<Byte>(max(buffCapacity, _putBack) + _putBack);

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

	// buffer not exhausted
    if (gptr() < egptr())
        return traits_type::to_int_type(*gptr());

    char *base = (char *)_buffHandle.AddrOfPinnedObject().ToPointer();
    char *start = base;

    if (eback() == base) // true when this isn't the first fill
    {
        // Make arrangements for putback characters
        memmove(base, egptr() - _putBack, _putBack);
        start += _putBack;
    }

    // start is now the start of the buffer, proper.
    // Read from FileStream in to the provided buffer

	size_t n;
	array<Byte> ^buff = static_cast<array<Byte> ^>(_buffHandle.Target);
	try
	{
		n = _wrapped->Read(buff, 0, buff->Length - (start - base));
	}
	catch (...)
	{
		return EOF;
	}

    if (n == 0)
        return traits_type::eof();

    // Set buffer pointers
    setg(base, start, start + n);

    return traits_type::to_int_type(*gptr());
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
