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
        }

        [Test]
        public void Test1()
        {
            Assert.Pass();
        }
    }
}