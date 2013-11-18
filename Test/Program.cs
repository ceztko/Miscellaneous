using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;

namespace Test
{
    class Program
    {
        static void Main(string[] args)
        {
            TestClass.foo(new FileStream(@"C:\Users\ceztko\Desktop\ciao.txt", FileMode.OpenOrCreate, FileAccess.Write));
        }
    }
}
