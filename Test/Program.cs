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
            FileStream write = new FileStream(@"C:\Users\ceztko\Desktop\ciao.txt", FileMode.OpenOrCreate, FileAccess.Write);
            TestClass.foo(write);
            write.Close();
            TestClass.foo2(new FileStream(@"C:\Users\ceztko\Desktop\ciao.txt", FileMode.Open, FileAccess.Read));
        }
    }
}
