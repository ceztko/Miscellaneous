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

			test << "CiaoPorcoCazzoQuindi";
		}

		static void foo2(FileStream ^stream)
		{
			iostream &test = FileStreamW(stream, 8, 8);

			char buf[20];
			test.read(buf, 20);
		}
	};
}
