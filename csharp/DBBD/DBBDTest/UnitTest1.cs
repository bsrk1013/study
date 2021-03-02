using NUnit.Framework;
using DBBD;

namespace DBBDTest
{
    public class Tests
    {
        [SetUp]
        public void Setup()
        {
        }

        [Test]
        public void BufferTest()
        {
            Buffer buffer = new Buffer(256);

            int a1 = 10;

            //void write<T>(Buffer buffer, T value)
            //{
            //    uint size = (uint)System.Runtime.InteropServices.Marshal.SizeOf(value);
            //    for(uint i = 0; i < size; i++)
            //    {
            //        char data = (char)(value >> i * 8);
            //    }
            //}
        }

        [Test]
        public void Test1()
        {
            Assert.Pass();
        }
    }
}