using NUnit.Framework;
using DBBD;
using System;

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
            DBBD.Buffer buffer = new DBBD.Buffer(256);

            int a1 = -15674;
            int a2 = 0;

            writeInt(buffer, a1);
            readInt(buffer, out a2);

            Assert.AreEqual(a1, a2);

            uint a3 = 1231564681;
            uint a4 = 0;

            writeUint(buffer, a3);
            readUint(buffer, out a4);

            Assert.AreEqual(a3, a4);

            buffer.Adjust();

            int a5 = -98713;
            uint a6 = 789452315;
            int a7 = 0;
            uint a8 = 0;

            writeInt(buffer, a5);
            writeUint(buffer, a6);

            readInt(buffer, out a7);
            readUint(buffer, out a8);

            Assert.AreEqual(a5, a7);
            Assert.AreEqual(a6, a8);

            buffer.Adjust();

            String s1 = "Doby";
            String s2;

            writeString(buffer, s1);
            readString(buffer, out s2);

            Assert.AreEqual(s1, s2);

            void writeString(DBBD.Buffer buffer, String value)
            {
                int size = value.Length;
                writeUint(buffer, (uint)size);
                for(int i = 0; i < size; i++)
                {
                    byte data = (byte)value[i];
                    buffer.PutByte(data);
                }
            }

            void readString(DBBD.Buffer buffer, out String value)
            {
                uint size = 0;
                readUint(buffer, out size);
                byte[] block = buffer.ReadByteBlock(size);
                value = BitConverter.ToString(block);
            }

            void writeInt(DBBD.Buffer buffer, int value)
            {
                uint size = sizeof(int);
                for(int i = 0; i < size; i++)
                {
                    byte data = (byte)(value >> i * 8);
                    buffer.PutByte(data);
                }
            }

            void readInt(DBBD.Buffer buffer, out int value)
            {
                byte[] block = buffer.ReadByteBlock(sizeof(int));
                value = BitConverter.ToInt32(block, 0);
            }

            void writeUint(DBBD.Buffer buffer, uint value)
            {
                uint size = sizeof(uint);
                for(int i = 0; i < size; i++)
                {
                    byte data = (byte)(value >> i * 8);
                    buffer.PutByte(data);
                }
            }

            void readUint(DBBD.Buffer buffer, out uint value)
            {
                byte[] block = buffer.ReadByteBlock(sizeof(uint));
                value = BitConverter.ToUInt32(block, 0);
            }
        }

        [Test]
        public void Test1()
        {
            Assert.Pass();
        }
    }
}