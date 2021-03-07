using System;
using System.Collections.Generic;
using System.Net;
using System.Net.Sockets;
using System.Text;

namespace DBBD
{
    public abstract class TcpClientBase : ClientBase
    {
        public void Start(string address, int port, bool tryReconnect)
        {
            Start(IPAddress.Parse(address), port, tryReconnect);
        }

        public override void Start(IPAddress address, int port, bool tryReconnect)
        {
            this.tryReconnect = tryReconnect;

            endPoint = new IPEndPoint(address, port);

            socket = new Socket(endPoint.AddressFamily, SocketType.Stream, ProtocolType.Tcp);
            connectEventArgs = new SocketAsyncEventArgs();
            connectEventArgs.Completed += ConnectEventArgs_Completed;
            
            connectEventArgs.RemoteEndPoint = endPoint;
            connectEventArgs.UserToken = socket;

            bool pending = socket.ConnectAsync(connectEventArgs);
            if(!pending)
            {
                OnConnect(connectEventArgs);
            }
        }

        public override void Stop()
        {
        }

        public override void Send(ICell data)
        {
            session.Send(data);
        }

        protected override void DisconnectInternal()
        {
        }

        private void ConnectEventArgs_Completed(object sender, SocketAsyncEventArgs e)
        {
            OnConnect(e);
        }

        private void OnConnect(SocketAsyncEventArgs e)
        {
            if(e.SocketError == SocketError.Success)
            {
                connectEventArgs.Completed -= ConnectEventArgs_Completed;
                connectEventArgs.Dispose();
                connectEventArgs = null;

                session = new TcpClientSession((Socket)e.UserToken, 8192, ReadInternal);
                session.Start();
            }
            else
            {

            }
        }

        private TcpClientSession session;
        private SocketAsyncEventArgs connectEventArgs;
    }
}