using DBBD;
using NUnit.Framework;
using System.Collections.Generic;
using System.Linq;
using System.Text;

static class ProtocolType
{
    public enum Value
    {
        PingCheckReq = 1001,
        PingCheckResp = 1002,
        LoginReq = 1003,
        LoginResp = 1004,
    };
    private static Dictionary<Value, string> stringMap = new Dictionary<Value, string>
        {
            { Value.PingCheckReq, "PingCheckReq" },
        };
    public static string Get(Value value)
    {
        if (!stringMap.ContainsKey(value))
        {
            return "";
        }
        return stringMap[value];
    }
}

namespace DBBDTest
{
    class User : DBBD.ICell
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
            if (fingerPrinter[1]) { totalLength += (uint)(sizeof(short)); }
            return totalLength;
        }

        public string Nickname { get { return nickname; } set { nickname = value; fingerPrinter[0] = true; } }
        public short Level { get { return level; } set { level = value; fingerPrinter[1] = true; } }

        private List<bool> fingerPrinter = new List<bool>();
        private string nickname;
        private short level;
    }

    class LoginReq : Request
    {
        public LoginReq()
        {
            typeId = (uint)ProtocolType.Value.LoginReq;
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
            uint totalLength = base.GetLength() + (uint)(sizeof(uint) + fingerPrinter.Count);
            if (fingerPrinter[0]) { totalLength += user.GetLength(); }
            return totalLength;
        }

        public User User { get { return user; } set { user = value; fingerPrinter[0] = true; } }

        private List<bool> fingerPrinter = new List<bool>();
        private User user;
    }

    class LoginResp : Response
    {
        public LoginResp()
        {
            typeId = (uint)ProtocolType.Value.LoginResp;
            fingerPrinter.AddRange(Enumerable.Repeat(false, 1));
        }

        public override void Serialize(DBBD.Buffer buffer)
        {
            base.WriteHeader(buffer, GetLength());
            DBBD.Serialize.Write(buffer, fingerPrinter);
            if (fingerPrinter[0]) { DBBD.Serialize.Write(buffer, loginResult); }
        }

        public override void Deserialize(DBBD.Buffer buffer)
        {
            base.ReadHeader(buffer);
            DBBD.Deserialize.Read(buffer, out fingerPrinter);
            if (fingerPrinter[0]) { DBBD.Deserialize.Read(buffer, out loginResult); }
        }

        public override uint GetLength()
        {
            uint totalLength = (uint)(base.GetLength() + sizeof(uint) + fingerPrinter.Count);
            totalLength += sizeof(bool);
            return totalLength;
        }

        public bool LoginResult { get { return loginResult; } set { loginResult = value; fingerPrinter[0] = true; } }

        private List<bool> fingerPrinter = new List<bool>();
        private bool loginResult;
    }

    class GameClient : TcpClientBase
    {
        protected override void ReadInternal(DBBD.Buffer buffer)
        {
            byte[] headerBlock = buffer.ViewByteBlock(Header.HeaderSize);
            Header header = new Header(headerBlock);

            switch (header.TypeId)
            {
                case 2:
                    LoginResp loginResp;
                    Deserialize.Read(buffer, out loginResp);

                    Assert.IsTrue(loginResp.LoginResult);
                    break;
            }
        }

        protected override void StartInternal()
        {
        }

        protected override void StopInternal()
        {
        }
    }
}