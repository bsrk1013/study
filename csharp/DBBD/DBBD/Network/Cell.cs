using System;
using System.Collections.Generic;
using System.Text;

namespace DBBD
{
    public abstract class Cell
    {
        public abstract void Serialize(Buffer buffer);
        public abstract void Deserialize(Buffer buffer);
        public abstract int GetLength();
    }

    public class Header
    {
        public Header(byte[] block)
        {
            Length = BitConverter.ToInt32(block, 0);
            TypeId = BitConverter.ToInt32(block, sizeof(int));
        }

        public int Length;
        public int TypeId;
    }
}