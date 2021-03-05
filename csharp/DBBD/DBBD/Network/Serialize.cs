using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;

namespace DBBD
{
    public class Serialize
    {
        #region Variable
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
            uint size = sizeof(short);
            for(int i = 0; i < size; i++)
            {
                byte data = (byte)(value >> i * 8);
                Write(buffer, data);
            }
        }

        public static void Write(Buffer buffer, ushort value)
        {
            uint size = sizeof(ushort);
            for(int i = 0; i < size; i++)
            {
                byte data = (byte)(value >> i * 8);
                Write(buffer, data);
            }
        }

        public static void Write(Buffer buffer, int value)
        {
            uint size = sizeof(int);
            for(int i = 0; i < size; i++)
            {
                byte data = (byte)(value >> i * 8);
                Write(buffer, data);
            }
        }

        public static void Write(Buffer buffer, uint value)
        {
            uint size = sizeof(uint);
            for (int i = 0; i < size; i++)
            {
                byte data = (byte)(value >> i * 8);
                Write(buffer, data);
            }
        }

        public static void Write(Buffer buffer, long value)
        {
            uint size = sizeof(long);
            for(int i = 0; i < size; i++)
            {
                byte data = (byte)(value >> i * 8);
                Write(buffer, data);
            }
        }

        public static void Write(Buffer buffer, ulong value)
        {
            uint size = sizeof(ulong);
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
            byte[] strBlock = Encoding.UTF8.GetBytes(value);
            uint size = (uint)strBlock.Length;
            Write(buffer, size);
            for(int i = 0; i < size; i++)
            {
                byte data = strBlock[i];
                Write(buffer, data);
            }
        }

        public static void Write(Buffer buffer, ICell value)
        {
            value.Serialize(buffer);
        }
        #endregion
        #region List
        public static void Write(Buffer buffer, List<char> values)
        {
            uint size = (uint)values.Count;
            Write(buffer, size);
            foreach(var val in values)
            {
                Write(buffer, val);
            }
        }

        public static void Write(Buffer buffer, List<byte> values)
        {
            uint size = (uint)values.Count;
            Write(buffer, size);
            foreach (var val in values)
            {
                Write(buffer, val);
            }
        }

        public static void Write(Buffer buffer, List<sbyte> values)
        {
            uint size = (uint)values.Count;
            Write(buffer, size);
            foreach (var val in values)
            {
                Write(buffer, val);
            }
        }

        public static void Write(Buffer buffer, List<bool> values)
        {
            uint size = (uint)values.Count;
            Write(buffer, size);
            foreach (var val in values)
            {
                Write(buffer, val);
            }
        }

        public static void Write(Buffer buffer, List<short> values)
        {
            uint size = (uint)values.Count;
            Write(buffer, size);
            foreach (var val in values)
            {
                Write(buffer, val);
            }
        }

        public static void Write(Buffer buffer, List<ushort> values)
        {
            uint size = (uint)values.Count;
            Write(buffer, size);
            foreach (var val in values)
            {
                Write(buffer, val);
            }
        }

        public static void Write(Buffer buffer, List<int> values)
        {
            uint size = (uint)values.Count;
            Write(buffer, size);
            foreach (var val in values)
            {
                Write(buffer, val);
            }
        }

        public static void Write(Buffer buffer, List<uint> values)
        {
            uint size = (uint)values.Count;
            Write(buffer, size);
            foreach (var val in values)
            {
                Write(buffer, val);
            }
        }

        public static void Write(Buffer buffer, List<long> values)
        {
            uint size = (uint)values.Count;
            Write(buffer, size);
            foreach (var val in values)
            {
                Write(buffer, val);
            }
        }

        public static void Write(Buffer buffer, List<ulong> values)
        {
            uint size = (uint)values.Count;
            Write(buffer, size);
            foreach (var val in values)
            {
                Write(buffer, val);
            }
        }

        public static void Write(Buffer buffer, List<float> values)
        {
            uint size = (uint)values.Count;
            Write(buffer, size);
            foreach (var val in values)
            {
                Write(buffer, val);
            }
        }

        public static void Write(Buffer buffer, List<double> values)
        {
            uint size = (uint)values.Count;
            Write(buffer, size);
            foreach (var val in values)
            {
                Write(buffer, val);
            }
        }

        public static void Write(Buffer buffer, List<string> values)
        {
            uint size = (uint)values.Count;
            Write(buffer, size);
            foreach (var val in values)
            {
                Write(buffer, val);
            }
        }

        public static void Write(Buffer buffer, List<ICell> values)
        {
            uint size = (uint)values.Count;
            Write(buffer, size);
            foreach(var val in values)
            {
                Write(buffer, val);
            }
        }
        #endregion
    }
}
