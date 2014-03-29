// Copyright (c) 2013 Francesco Pretto
// This file is subject to the MIT license

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
		FileStreamW(FileStream ^stream, std::size_t buffCapacity = 256, std::size_t putBackCap = 8);
	};
} }
