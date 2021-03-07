using System;
using System.Collections.Generic;
using System.Text;
using System.Net.Sockets;
using System.Net;

namespace DBBD
{
    public abstract class ClientBase
    {
        public abstract void Start(IPAddress address, int port, bool tryReconnect);
        public abstract void Stop();
        public abstract void Send(ICell data);

        protected abstract void StartInternal();
        protected abstract void StopInternal();

        protected abstract void ReadInternal(Buffer buffer);
        protected abstract void DisconnectInternal();

        protected IPEndPoint endPoint;
        protected bool tryReconnect = false;
        protected Socket socket;
        private bool isConnect = false;
        protected bool isDisposed = false;
    }
}
