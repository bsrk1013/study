using System;
using System.Collections.Generic;
using System.Text;

namespace DBBD
{
    public class Deserialize
    {
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
    }
}
