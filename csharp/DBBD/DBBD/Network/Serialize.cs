using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;

namespace DBBD
{
    public class Serialize
    {
        public static void Write(Buffer buffer, char value)
        {
            buffer.PutByte((byte)value);
        }

        public static void Write(Buffer buffer, byte value)
        {
            buffer.PutByte(value);
        }

        public static void Write(Buffer buffer, sbyte value)
        {
            buffer.PutByte(value);
        }

        public static void Write(Buffer buffer, bool value)
        {
            Write(buffer, (byte)(value ? 1 : 0));
        }

        public static void Write(Buffer buffer, short value)
        {
            int size = sizeof(short);
            for(int i = 0; i < size; i++)
            {
                byte data = (byte)(value >> i * 8);
                Write(buffer, data);
            }
        }

        public static void Write(Buffer buffer, ushort value)
        {
            int size = sizeof(ushort);
            for(int i = 0; i < size; i++)
            {
                byte data = (byte)(value >> i * 8);
                Write(buffer, data);
            }
        }

        public static void Write(Buffer buffer, int value)
        {
            int size = sizeof(int);
            for(int i = 0; i < size; i++)
            {
                byte data = (byte)(value >> i * 8);
                Write(buffer, data);
            }
        }

        public static void Write(Buffer buffer, uint value)
        {
            int size = sizeof(uint);
            for (int i = 0; i < size; i++)
            {
                byte data = (byte)(value >> i * 8);
                Write(buffer, data);
            }
        }

        public static void Write(Buffer buffer, long value)
        {
            int size = sizeof(long);
            for(int i = 0; i < size; i++)
            {
                byte data = (byte)(value >> i * 8);
                Write(buffer, data);
            }
        }

        public static void Write(Buffer buffer, ulong value)
        {
            int size = sizeof(ulong);
            for (int i = 0; i < size; i++)
            {
                byte data = (byte)(value >> i * 8);
                Write(buffer, data);
            }
        }

        public static void Write(Buffer buffer, float value)
        {
            byte[] block = BitConverter.GetBytes(value);
            for(int i = 0; i < block.Length; i++)
            {
                byte data = block[i];
                Write(buffer, data);
            }
        }

        public static void Write(Buffer buffer, double value)
        {
            byte[] block = BitConverter.GetBytes(value);
            for (int i = 0; i < block.Length; i++)
            {
                byte data = block[i];
                Write(buffer, data);
            }
        }

        public static void Write(Buffer buffer, string value)
        {
            byte[] strBlock = Encoding.Unicode.GetBytes(value);
            int size = strBlock.Length;
            for(int i = 0; i < size; i++)
            {
                byte data = strBlock[i];
                Write(buffer, data);
            }
        }
    }
}
