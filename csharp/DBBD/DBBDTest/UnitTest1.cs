using NUnit.Framework;
using DBBD;
using System;
using System.Text;
using System.Collections.Generic;
using System.Linq;

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

            string s1 = "Doby";
            string s2;

            writeString(buffer, s1);
            readString(buffer, out s2);

            Assert.AreEqual(s1, s2);

            buffer.Adjust();

            int t1 = -987654;
            uint t2 = 987654321;
            string t3 = "빵칼법사";

            int r1 = 0;
            uint r2 = 0;
            string r3 = "";

            writeInt(buffer, t1);
            writeUint(buffer, t2);
            writeString(buffer, t3);

            readInt(buffer, out r1);
            readUint(buffer, out r2);
            readString(buffer, out r3);

            Assert.AreEqual(t1, r1);
            Assert.AreEqual(t2, r2);
            Assert.AreEqual(t3, r3);

            void writeString(DBBD.Buffer buffer, string value)
            {
                byte[] strBlock = Encoding.UTF8.GetBytes(value);
                int size = strBlock.Length;
                writeUint(buffer, (uint)size);
                for(int i = 0; i < size; i++)
                {
                    byte data = strBlock[i];
                    buffer.PutByte(data);
                }
            }

            void readString(DBBD.Buffer buffer, out string value)
            {
                uint size = 0;
                readUint(buffer, out size);
                byte[] block = buffer.ReadByteBlock(size);
                value = Encoding.UTF8.GetString(block);
                //value = BitConverter.ToString(block);
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
        public void DeSerializeTest()
        {
            DBBD.Buffer buffer = new DBBD.Buffer(8192);

            string nickname = "빵칼법사";
            int level = 10;

            Serialize.Write(buffer, nickname);
            Serialize.Write(buffer, level);

            Deserialize.Read(buffer, out string nickname2);
            Deserialize.Read(buffer, out int level2);

            Assert.AreEqual(nickname, nickname2);
            Assert.AreEqual(level, level2);
        }

        class User : ICell
        {
            public User()
            {
                fingerPrinter.AddRange(Enumerable.Repeat(false, 2));
            }
            public void Serialize(DBBD.Buffer buffer)
            {
                DBBD.Serialize.Write(buffer, fingerPrinter);
                if (fingerPrinter[0]) { DBBD.Serialize.Write(buffer, nickname); }
                if (fingerPrinter[1]) { DBBD.Serialize.Write(buffer, level); }
            }

            public void Deserialize(DBBD.Buffer buffer)
            {
                DBBD.Deserialize.Read(buffer, out fingerPrinter);
                if (fingerPrinter[0]) { DBBD.Deserialize.Read(buffer, out nickname); }
                if (fingerPrinter[1]) { DBBD.Deserialize.Read(buffer, out level); }
            }

            public uint GetLength()
            {
                uint totalLength = (uint)(sizeof(uint) + fingerPrinter.Count);
                if (fingerPrinter[0]) { totalLength += (uint)(sizeof(uint) + Encoding.UTF8.GetByteCount(nickname)); }
                if (fingerPrinter[1]){ totalLength += (uint)(sizeof(short)); }
                return totalLength;
            }

            public string Nickname { get { return nickname; } set { nickname = value; fingerPrinter[1] = true; } }
            public short Level { get { return level; } set { level = value; fingerPrinter[1] = true; } }

            private List<bool> fingerPrinter = new List<bool>();
            private string nickname;
            private short level;
        }

        class LoginReq : Request
        {
            public LoginReq() 
            { 
                typeId = 1;
                fingerPrinter.AddRange(Enumerable.Repeat(false, 1));
            }

            public override void Serialize(DBBD.Buffer buffer)
            {
                base.WriteHeader(buffer, GetLength());
                DBBD.Serialize.Write(buffer, fingerPrinter);
                if (fingerPrinter[0]) { DBBD.Serialize.Write(buffer, user); }
            }

            public override void Deserialize(DBBD.Buffer buffer)
            {
                base.ReadHeader(buffer);
                DBBD.Deserialize.Read(buffer, out fingerPrinter);
                if (fingerPrinter[0]) { DBBD.Deserialize.Read(buffer, out user); }
            }

            public override uint GetLength()
            {
                return (uint)(base.GetLength() + sizeof(uint) + fingerPrinter.Count + user.GetLength());
            }

            public User User { get { return user; } set { user = value; fingerPrinter[0] = true; } }

            private List<bool> fingerPrinter = new List<bool>();
            private User user;
        }

        [Test]
        public void RequestTest()
        {
            LoginReq req = new LoginReq();
            User user = new User();
            user.Level = 15;

            req.User = user;

            DBBD.Buffer sendBuffer = new DBBD.Buffer(8192);
            DBBD.Buffer receiveBuffer = new DBBD.Buffer(8192);

            DBBD.Serialize.Write(sendBuffer, req);
            sendBuffer.BufferOffset = 0;
            LoginReq loginReq;
            while (true)
            {
                uint currentOffset = sendBuffer.BufferOffset;
                uint transfrred = 5;

                uint lastPos = sendBuffer.BufferLastPos;
                if(transfrred + currentOffset > lastPos)
                {
                    transfrred = lastPos - currentOffset;//transfrred + currentOffset - lastPos;
                }

                byte[] block = sendBuffer.ReadByteBlock(transfrred);
                for(uint i = 0; i < transfrred; i++)
                {
                    receiveBuffer.PutByte(block[i]);
                }

                if(DBBD.Header.HeaderSize > receiveBuffer.BufferLastPos)
                {
                    continue;
                }

                byte[] headerBlock = receiveBuffer.ViewByteBlock(DBBD.Header.HeaderSize);
                DBBD.Header header = new DBBD.Header(headerBlock);
                if(receiveBuffer.BufferLastPos < header.Length)
                {
                    continue;
                }

                if(header.TypeId == 1)
                {
                    DBBD.Deserialize.Read(receiveBuffer, out loginReq);
                    break;
                }
            }

            Assert.AreEqual(req.TypeId, loginReq.TypeId);
            Assert.AreEqual(req.User.Level, loginReq.User.Level);
        }

        [Test]
        public void Test1()
        {
            int a1 = -15674;
            uint size = sizeof(int);
            byte[] b1 = new byte[size];
            sbyte[] b2 = new sbyte[size];
            for (int i = 0; i < size; i++)
            {
                byte data1 = (byte)(a1 >> i * 8);
                sbyte data2 = (sbyte)data1;

                b1[i] = data1;
                b2[i] = data2;
                //buffer.PutByte(data);
            }

            byte[] b3 = Array.ConvertAll(b2, (c) => (byte)c);//(byte[])(Array)b2;

            int a2 = BitConverter.ToInt32(b1, 0);
            int a3 = BitConverter.ToInt32(b3, 0);

            Assert.AreEqual(a2, a3);

            //sbyte byte1 = -1;
            //byte byte2 = (byte)byte1;

            byte[] block1 = new byte[4] { 100, 111, 98, 121 };
            string s1 = Encoding.UTF8.GetString(block1);
            Assert.AreEqual(s1, "doby");

            //string s2 = "빵칼법사";
            //var b2 = Encoding.Unicode.GetBytes(s2);

            sbyte[] block2 = new sbyte[] { 117, -66, 124, -50, -107, -68, -84, -64 };
            byte[] block3 = new byte[8];
            for(int i = 0; i < 8; i++)
            {
                block3[i] = (byte)block2[i];
            }

            string s2 = Encoding.Unicode.GetString(block3);

            ulong sizetSize = sizeof(ulong);
            bool a = true;



            sbyte[] block4 = new sbyte[] { -32, -72, -108, -32, -71, -118, -32, -72, -83, -32, -72, -102, -32, -72, -102, -32, -72, -75, -32, -71, -119 };
            byte[] block5 = (byte[])(Array)block4;

            string s3 = Encoding.UTF8.GetString(block5);
        }
    }
}