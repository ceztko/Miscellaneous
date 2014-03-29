// Copyright (c) 2013 Francesco Pretto
// This file is subject to the MIT license

#include "FileStreamW.h"

using namespace std;
using namespace Interop::Miscellaneous;

FileStreamW::FileStreamW(FileStream ^stream, size_t buffCapacity, std::size_t putBack) :
	iostream(NULL),
	_buffer(stream, buffCapacity, putBack)
{
	rdbuf(&_buffer);
}
