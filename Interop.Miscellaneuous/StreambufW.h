#pragma once

#include <vector>
#include <streambuf>

#include <gcroot.h>

using namespace System;
using namespace System::IO;
using namespace System::Runtime::InteropServices;

namespace Interop { namespace Miscellaneous
{
	// Reference: http://www.mr-edd.co.uk/blog/beginners_guide_streambuf
	class StreambufW: public std::streambuf
	{
	private:
		gcroot<FileStream ^> _wrapped;
        const std::size_t _putBack;
		GCHandle _buffHandle;
	public:
		StreambufW(FileStream ^stream, std::size_t buffCapacity, std::size_t putBack);
		~StreambufW();
        virtual int_type underflow();
        virtual int_type overflow(int_type c);
        virtual int sync();
	private:
		int flush();
	};
} }
