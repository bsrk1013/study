using System;
using System.Collections.Generic;
using System.Net.Sockets;
using System.Text;

namespace DBBD
{
    public class TcpSessionBase : SessionBase
    {
        public TcpSessionBase(Socket socket, uint bufferSize) 
        {
            this.socket = socket;
            readBuffer = new Buffer(bufferSize);
            writeBuffer = new Buffer(bufferSize);

            readEventArgs = new SocketAsyncEventArgs();
            sendEventArgs = new SocketAsyncEventArgs();

            readEventArgs.Completed += ReadEventArgs_Completed;
            sendEventArgs.Completed += SendEventArgs_Completed;
        }

        public override void Start()
        {
            StartInternal();
            Read();
        }

        public override void Stop()
        {
            if (isDisposed) { return; }
            isDisposed = true;

            if(socket != null && socket.Connected)
            {
                socket.Close();
            }
        }

        public override void Send(ICell data)
        {
            Serialize.Write(writeBuffer, data);
            Write();
        }

        protected override void Read()
        {
            readBuffer.Adjust();
            if(socket != null && socket.Connected)
            {
                socket.ReceiveAsync(readEventArgs);
            }
        }

        private void SendEventArgs_Completed(object sender, SocketAsyncEventArgs e)
        {
            e.BufferList = null;

            if(e.SocketError == SocketError.Success)
            {

            }
            else
            {

            }
        }

        private void ReadEventArgs_Completed(object sender, SocketAsyncEventArgs e)
        {

        }

        private SocketAsyncEventArgs readEventArgs;
        private SocketAsyncEventArgs sendEventArgs;
    }
}