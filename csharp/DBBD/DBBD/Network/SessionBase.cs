using System;
using System.Collections.Generic;
using System.Net.Sockets;
using System.Text;

namespace DBBD
{
    public abstract class SessionBase
    {
        public abstract void Start();
        public abstract void Stop();
        public abstract void Send(ICell data);

        protected abstract void StartInternal();

        protected abstract void Read();
        protected abstract void ReadInternal(Header header);

        protected abstract void Write();
        protected abstract void WriteInternal();

        public ulong SessionId { get; set; }

        protected bool isDisposed = false;
        protected Socket socket;
        protected Buffer readBuffer;
        protected Buffer writeBuffer;
    }
}