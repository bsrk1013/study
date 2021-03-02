using System;
using System.Collections.Generic;
using System.Text;

namespace DBBD
{
    public class Buffer
    {
        public Buffer(uint size) 
        {
            bufferCapacity = size;
            buffer = new char[size];
            blockSize = size / 16;
            block = new char[blockSize];
        }
        
        public void PutByte(char byteData)
        {
            buffer[bufferLastPos++] = byteData;
        }

        public char[] ViewByteBlock(uint size) 
        {
            return ReadByte(size);
        }

        public char[] ReadByteBlock(uint size) 
        {
            ReadByte(size);

            bufferOffset += size;
            
            return block;
        }

        public void Adjust()
        {
            uint spare = bufferLastPos - bufferOffset;
            if(spare > bufferCapacity
                || spare == 0)
            {
                bufferOffset = 0;
                bufferLastPos = 0;
            }
            else if(bufferOffset > 0 && spare >0)
            {
                char[] tempArray = new char[spare];
                for(int i = 0; i < spare; i++)
                {
                    tempArray[i] = buffer[bufferOffset + i];
                }
                
                System.Array.Clear(buffer, 0, (int)bufferCapacity);
                for(int i = 0; i < spare; i++)
                {
                    buffer[i] = tempArray[i];
                }

                bufferOffset = 0;
                bufferLastPos = spare;
            }
        }

        public void IncreaseLastPos(uint size) { bufferLastPos += size; }

        private char[] ReadByte(uint size)
        {
            if(size > blockSize)
            {
                blockSize *= 2;
                block = new char[blockSize];
            }

            uint dataIndex = 0;
            for(uint i = bufferOffset; i < bufferOffset + size; i++)
            {
                block[dataIndex++] = buffer[i];
            }

            return block;
        }

        public void ClearBuffer()
        {
            System.Array.Clear(buffer, 0, (int)bufferCapacity);
            bufferOffset = 0;
            bufferLastPos = 0;
        }

        public char[] RawBuffer { get { return buffer; } }
        public uint BufferLastPos { get { return bufferLastPos; } set { bufferLastPos = value; } }
        public uint BufferOffset { get { return bufferOffset; } set { bufferOffset = value; } }

        private char[] buffer;
        private char[] block;
        private uint blockSize = 0;
        private uint bufferOffset = 0;
        private uint bufferLastPos = 0;
        private uint bufferCapacity = 0;
    }
}