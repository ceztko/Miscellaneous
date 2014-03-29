// Copyright (c) 2013 Francesco Pretto
// This file is subject to the MIT license

#pragma once

#include <vector>
#include <streambuf>

#include <gcroot.h>

using namespace System;
using namespace System::IO;
using namespace System::Runtime::InteropServices;

namespace Interop { namespace Miscellaneous
{
	// Started from: http://www.mr-edd.co.uk/blog/beginners_guide_streambuf
	// Fixes from "The C++ Standard Library", Nicolai M. Josuttis, Addison-Wesley, 1999
	//     ftp://docenti.ing.units.it/arc_stud/TELEDID/CE3/esempi/io/inbuf1.hpp
	// Ideas from USC Groovx: http://ilab.usc.edu/rjpeters/groovx/stdiobuf_8cc-source.html
	class StreambufW: public std::streambuf
	{
	private:
		gcroot<FileStream ^> _wrapped;
        const std::size_t _putBackCap;
		GCHandle _buffHandle;
	public:
		StreambufW(FileStream ^stream, std::size_t buffCapacity, std::size_t putBackCap);
		~StreambufW();
        virtual int_type underflow();
        virtual int_type overflow(int_type c);
        virtual int sync();
	private:
		int flush();
	};
} }
