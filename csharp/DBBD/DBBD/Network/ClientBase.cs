using System;
using System.Collections.Generic;
using System.Text;
using System.Net.Sockets;
using System.Net;

namespace DBBD
{
    public abstract class ClientBase
    {
        protected class ConnectContext
        {
            public Socket Socket { get; set; }
            public EndPoint RemoteEndPoint { get; set; }
        }
        protected ConnectContext context;

        public abstract void Connect(IPAddress ip, int port);
        public abstract void Disconnect();
        public abstract void Send();

        protected abstract void ConnectInternal(IPEndPoint endPoint);
        protected abstract void HandleConnect(IAsyncResult result);
        protected abstract void DisconnectInternal();
        protected virtual void ReadInternal() { }

        protected IPEndPoint endPoint;
        protected bool tryReconnect = false;
        protected Socket socket;
        private bool isConnect = false;
        protected bool isDisposed = false;
    }
}
