using System;
using System.Collections.Generic;
using System.Net;
using System.Net.Sockets;
using System.Text;

namespace DBBD
{
    public class TcpClientBase : ClientBase
    {
        public override void Connect(IPAddress ip, int port)
        {
            ConnectInternal(new IPEndPoint(ip, port));
        }

        public override void Disconnect()
        {
        }

        public override void Send()
        {
        }

        protected override void ConnectInternal(IPEndPoint endPoint)
        {
            Socket socket = new Socket(endPoint.AddressFamily, SocketType.Stream, ProtocolType.Tcp);
            socket.BeginConnect(endPoint, HandleConnect, socket);
        }

        protected override void HandleConnect(IAsyncResult result)
        {
        }

        protected override void DisconnectInternal()
        {
        }
    }
}