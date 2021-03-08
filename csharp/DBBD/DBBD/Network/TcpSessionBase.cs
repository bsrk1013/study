using System;
using System.Collections.Generic;
using System.Net.Sockets;
using System.Text;

namespace DBBD
{
    public abstract class TcpSessionBase : SessionBase
    {
        public TcpSessionBase(Socket socket, uint bufferSize) 
        {
            this.socket = socket;
            readBuffer = new Buffer(bufferSize);
            writeBuffer = new Buffer(bufferSize);

            readEventArgs = new SocketAsyncEventArgs();
            sendEventArgs = new SocketAsyncEventArgs();

            readEventArgs.SetBuffer(readBuffer.RawBuffer, 0, (int)bufferSize);

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
                bool pending = socket.ReceiveAsync(readEventArgs);
                if (!pending)
                {
                    OnReceive(readEventArgs);
                }
            }
        }

        protected override void Write()
        {
            WriteInternal();
            sendEventArgs.SetBuffer(writeBuffer.RawBuffer, 0, (int)writeBuffer.BufferLastPos);
            bool pending = socket.SendAsync(sendEventArgs);
            if (!pending)
            {
                OnSend(sendEventArgs);
            }
        }

        private void SendEventArgs_Completed(object sender, SocketAsyncEventArgs e)
        {
            OnSend(e);
        }

        private void ReadEventArgs_Completed(object sender, SocketAsyncEventArgs e)
        {
            OnReceive(e);
        }

        private void OnReceive(SocketAsyncEventArgs e)
        {
            if(e.SocketError == SocketError.Success)
            {
                readBuffer.BufferLastPos += (uint)e.BytesTransferred;
                while(true)
                {
                    if(readBuffer.BufferLastPos < Header.HeaderSize)
                    {
                        break;
                    }

                    byte[] headerBlock = readBuffer.ViewByteBlock(Header.HeaderSize);
                    Header header = new Header(headerBlock);

                    if(readBuffer.BufferLastPos < header.Length)
                    {
                        break;
                    }

                    ReadInternal(header);
                    break;
                }
                Read();
            }
            else
            {
                Stop();
            }
        }

        private void OnSend(SocketAsyncEventArgs e)
        {
            if(e.SocketError == SocketError.Success)
            {
                writeBuffer.BufferOffset += (uint)e.BytesTransferred;
                writeBuffer.Adjust();
            }
            else
            {
                Stop();
            }
        }

        private SocketAsyncEventArgs readEventArgs;
        private SocketAsyncEventArgs sendEventArgs;
    }
}