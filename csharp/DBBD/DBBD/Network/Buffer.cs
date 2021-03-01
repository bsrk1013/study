using System;
using System.Collections.Generic;
using System.Text;

namespace DBBD
{
    public class Buffer
    {
        public Buffer(uint size) { }
        
        public void PutByte(char byteData) { }
        public char[] ReadByteBlock(uint size) { return new char[1]; }
        public char[] ViewByteBlock(uint size) { return new char[1]; }

        private char[] buffer;
        private char[] block;
        private uint blockSize = 0;
        private uint bufferOffset = 0;
        private uint bufferLastPos = 0;
        private uint bufferCapacity = 0;
    }
}