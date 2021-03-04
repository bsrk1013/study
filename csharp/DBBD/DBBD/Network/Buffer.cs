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
            buffer = new sbyte[size];
        }
        
        public void PutByte(byte byteData)
        {
            buffer[bufferLastPos++] = (sbyte)byteData;
        }

        public void PutByte(sbyte byteData)
        {
            buffer[bufferLastPos++] = byteData;
        }

        public byte[] ViewByteBlock(uint size) 
        {
            return ReadByte(size);
        }

        public byte[] ReadByteBlock(uint size) 
        {
            byte[] block = ReadByte(size);

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
            else if(bufferOffset > 0 && spare > 0)
            {
                sbyte[] tempArray = new sbyte[spare];
                for(uint i = 0; i < spare; i++)
                {
                    tempArray[i] = buffer[bufferOffset + i];
                }
                
                System.Array.Clear(buffer, 0, (int)bufferCapacity);
                for(uint i = 0; i < spare; i++)
                {
                    buffer[i] = tempArray[i];
                }

                bufferOffset = 0;
                bufferLastPos = (uint)spare;
            }
        }

        public void IncreaseLastPos(uint size) { bufferLastPos += size; }

        private byte[] ReadByte(ulong size)
        {
            uint dataIndex = 0;
            byte[] block = new byte[size];
            for(uint i = bufferOffset; i < bufferOffset + size; i++)
            {
                block[dataIndex++] = (byte)buffer[i];
            }

            return block;
        }

        public void ClearBuffer()
        {
            System.Array.Clear(buffer, 0, (int)bufferCapacity);
            bufferOffset = 0;
            bufferLastPos = 0;
        }

        public sbyte[] RawBuffer { get { return buffer; } }
        public uint BufferLastPos { get { return bufferLastPos; } set { bufferLastPos = value; } }
        public uint BufferOffset { get { return bufferOffset; } set { bufferOffset = value; } }

        private sbyte[] buffer;
        private uint bufferOffset = 0;
        private uint bufferLastPos = 0;
        private uint bufferCapacity = 0;
    }
}