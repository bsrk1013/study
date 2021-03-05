using System;
using System.Collections.Generic;
using System.Text;

namespace DBBD
{
    public interface ICell
    {
        void Serialize(Buffer buffer);
        void Deserialize(Buffer buffer);
        uint GetLength();
    }

    public class Header
    {
        public Header(byte[] block)
        {
            Length = BitConverter.ToUInt32(block, 0);
            TypeId = BitConverter.ToUInt32(block, sizeof(uint));
        }

        public uint  Length;
        public uint TypeId;

        public const uint HeaderSize = sizeof(uint) + sizeof(uint);
    }
}