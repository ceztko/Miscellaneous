#pragma once

#include "FileStreamW.h"

using namespace System;
using namespace System::IO;
using namespace Interop::Miscellaneous;
using namespace std;

namespace Test
{
	public ref class TestClass
	{
	public:
		static void foo(FileStream ^stream)
		{
			iostream &test = FileStreamW(stream);

			test << "Ciao";
		}
	};
}
