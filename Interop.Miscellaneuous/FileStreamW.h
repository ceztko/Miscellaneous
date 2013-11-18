#pragma once

#include <iostream>

#include "StreambufW.h"

using namespace System;
using namespace System::IO;

namespace Interop { namespace Miscellaneous
{
	class FileStreamW : public std::iostream
	{
	private:
		StreambufW _buffer;
	public:
		FileStreamW(FileStream ^stream, std::size_t buffCapacity = 256, std::size_t putBack = 8);
	};
} }
