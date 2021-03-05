using System;
using System.Collections.Generic;
using System.Text;

namespace DBBD
{
    public abstract class Response : ICell
    {
        public void WriteHeader(Buffer buffer, uint length)
        {
            DBBD.Serialize.Write(buffer, length);
            DBBD.Serialize.Write(buffer, typeId);
        }

        public void ReadHeader(Buffer buffer)
        {
            DBBD.Deserialize.Read(buffer, out uint length);
            DBBD.Deserialize.Read(buffer, out typeId);
        }

        public abstract void Serialize(Buffer buffer);
        public abstract void Deserialize(Buffer buffer);
        public virtual uint GetLength() { return DBBD.Header.HeaderSize; }

        public uint GetTypeId() { return typeId; }

        protected uint typeId;
    }
}