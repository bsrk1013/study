using System;
using System.Collections.Generic;
using System.Text;

namespace DBBD
{
    public class Deserialize
    {
        #region Variable
        public static void Read(Buffer buffer, out char value)
        {
            byte[] block = buffer.ReadByteBlock(1);
            value = BitConverter.ToChar(block);
        }

        public static void Read(Buffer buffer, out byte value)
        {
            byte[] block = buffer.ReadByteBlock(1);
            value = block[0];
        }

        public static void Read(Buffer buffer, out sbyte value)
        {
            byte[] block = buffer.ReadByteBlock(1);
            value = (sbyte)block[0];
        }

        public static void Read(Buffer buffer, out bool value)
        {
            byte[] block = buffer.ReadByteBlock(1);
            value = BitConverter.ToBoolean(block);
        }

        public static void Read(Buffer buffer, out short value)
        {
            byte[] block = buffer.ReadByteBlock(sizeof(short));
            value = BitConverter.ToInt16(block);
        }

        public static void Read(Buffer buffer, out ushort value)
        {
            byte[] block = buffer.ReadByteBlock(sizeof(ushort));
            value = BitConverter.ToUInt16(block);
        }

        public static void Read(Buffer buffer, out int value)
        {
            byte[] block = buffer.ReadByteBlock(sizeof(int));
            value = BitConverter.ToInt32(block);
        }

        public static void Read(Buffer buffer, out uint value)
        {
            byte[] block = buffer.ReadByteBlock(sizeof(uint));
            value = BitConverter.ToUInt32(block);
        }

        public static void Read(Buffer buffer, out long value)
        {
            byte[] block = buffer.ReadByteBlock(sizeof(long));
            value = BitConverter.ToInt64(block);
        }

        public static void Read(Buffer buffer, out ulong value)
        {
            byte[] block = buffer.ReadByteBlock(sizeof(ulong));
            value = BitConverter.ToUInt64(block);
        }

        public static void Read(Buffer buffer, out float value)
        {
            byte[] block = buffer.ReadByteBlock(sizeof(float));
            value = BitConverter.ToSingle(block);
        }

        public static void Read(Buffer buffer, out double value)
        {
            byte[] block = buffer.ReadByteBlock(sizeof(double));
            value = BitConverter.ToDouble(block);
        }

        public static void Read(Buffer buffer, out string value)
        {
            Read(buffer, out uint size);
            byte[] block = buffer.ReadByteBlock(size);
            value = Encoding.UTF8.GetString(block);
        }

        public static void Read<T>(Buffer buffer, out T value)
            where T : ICell, new()
        {
            T temp = new T();
            temp.Deserialize(buffer);
            value = temp;
        }
        //public static void Read(Buffer buffer, ICell value)
        //{
        //    value.Deserialize(buffer);
        //}
        #endregion
        #region List
        public static void Read(Buffer buffer, out List<char> values)
        {
            byte[] sizeBlock = buffer.ReadByteBlock(sizeof(uint));
            uint size = BitConverter.ToUInt32(sizeBlock);

            values = new List<char>((int)size);
            for(int i = 0; i < size; i++)
            {
                Read(buffer, out char value);
                values.Add(value);
            }
        }

        public static void Read(Buffer buffer, out List<byte> values)
        {
            byte[] sizeBlock = buffer.ReadByteBlock(sizeof(uint));
            uint size = BitConverter.ToUInt32(sizeBlock);

            values = new List<byte>((int)size);
            for (int i = 0; i < size; i++)
            {
                Read(buffer, out byte value);
                values.Add(value);
            }
        }

        public static void Read(Buffer buffer, out List<sbyte> values)
        {
            byte[] sizeBlock = buffer.ReadByteBlock(sizeof(uint));
            uint size = BitConverter.ToUInt32(sizeBlock);

            values = new List<sbyte>((int)size);
            for (int i = 0; i < size; i++)
            {
                Read(buffer, out sbyte value);
                values.Add(value);
            }
        }

        public static void Read(Buffer buffer, out List<bool> values)
        {
            byte[] sizeBlock = buffer.ReadByteBlock(sizeof(uint));
            uint size = BitConverter.ToUInt32(sizeBlock);

            values = new List<bool>((int)size);
            for (int i = 0; i < size; i++)
            {
                Read(buffer, out bool value);
                values.Add(value);
            }
        }

        public static void Read(Buffer buffer, out List<short> values)
        {
            byte[] sizeBlock = buffer.ReadByteBlock(sizeof(uint));
            uint size = BitConverter.ToUInt32(sizeBlock);

            values = new List<short>((int)size);
            for (int i = 0; i < size; i++)
            {
                Read(buffer, out short value);
                values.Add(value);
            }
        }

        public static void Read(Buffer buffer, out List<ushort> values)
        {
            byte[] sizeBlock = buffer.ReadByteBlock(sizeof(uint));
            uint size = BitConverter.ToUInt32(sizeBlock);

            values = new List<ushort>((int)size);
            for (int i = 0; i < size; i++)
            {
                Read(buffer, out ushort value);
                values.Add(value);
            }
        }

        public static void Read(Buffer buffer, out List<int> values)
        {
            byte[] sizeBlock = buffer.ReadByteBlock(sizeof(uint));
            uint size = BitConverter.ToUInt32(sizeBlock);

            values = new List<int>((int)size);
            for (int i = 0; i < size; i++)
            {
                Read(buffer, out int value);
                values.Add(value);
            }
        }

        public static void Read(Buffer buffer, out List<uint> values)
        {
            byte[] sizeBlock = buffer.ReadByteBlock(sizeof(uint));
            uint size = BitConverter.ToUInt32(sizeBlock);

            values = new List<uint>((int)size);
            for (int i = 0; i < size; i++)
            {
                Read(buffer, out uint value);
                values.Add(value);
            }
        }

        public static void Read(Buffer buffer, out List<long> values)
        {
            byte[] sizeBlock = buffer.ReadByteBlock(sizeof(uint));
            uint size = BitConverter.ToUInt32(sizeBlock);

            values = new List<long>((int)size);
            for (int i = 0; i < size; i++)
            {
                Read(buffer, out long value);
                values.Add(value);
            }
        }

        public static void Read(Buffer buffer, out List<ulong> values)
        {
            byte[] sizeBlock = buffer.ReadByteBlock(sizeof(uint));
            uint size = BitConverter.ToUInt32(sizeBlock);

            values = new List<ulong>((int)size);
            for (int i = 0; i < size; i++)
            {
                Read(buffer, out ulong value);
                values.Add(value);
            }
        }

        public static void Read(Buffer buffer, out List<float> values)
        {
            byte[] sizeBlock = buffer.ReadByteBlock(sizeof(uint));
            uint size = BitConverter.ToUInt32(sizeBlock);

            values = new List<float>((int)size);
            for (int i = 0; i < size; i++)
            {
                Read(buffer, out float value);
                values.Add(value);
            }
        }

        public static void Read(Buffer buffer, out List<double> values)
        {
            byte[] sizeBlock = buffer.ReadByteBlock(sizeof(uint));
            uint size = BitConverter.ToUInt32(sizeBlock);

            values = new List<double>((int)size);
            for (int i = 0; i < size; i++)
            {
                Read(buffer, out double value);
                values.Add(value);
            }
        }

        public static void Read(Buffer buffer, out List<string> values)
        {
            byte[] sizeBlock = buffer.ReadByteBlock(sizeof(uint));
            uint size = BitConverter.ToUInt32(sizeBlock);

            values = new List<string>((int)size);
            for (int i = 0; i < size; i++)
            {
                Read(buffer, out string value);
                values.Add(value);
            }
        }

        public static void Read<T>(Buffer buffer, out List<T> values)
            where T : ICell, new()
        {
            byte[] sizeBlock = buffer.ReadByteBlock(sizeof(uint));
            uint size = BitConverter.ToUInt32(sizeBlock);

            values = new List<T>((int)size);
            for (int i = 0; i < size; i++)
            {
                Read(buffer, out T value);
                values.Add(value);
            }
        }

        //public static void Read(Buffer buffer, out List<ICell> values)
        //{
        //    byte[] sizeBlock = buffer.ReadByteBlock(sizeof(uint));
        //    uint size = BitConverter.ToUInt32(sizeBlock);

        //    values = new List<ICell>((int)size);
        //    for (int i = 0; i < size; i++)
        //    {
        //        Read(buffer, value);
        //        values.Add(value);
        //    }
        //}
        #endregion
    }
}
