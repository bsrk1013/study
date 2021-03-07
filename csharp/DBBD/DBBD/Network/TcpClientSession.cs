using System;
using System.Collections.Generic;
using System.Net.Sockets;
using System.Text;

namespace DBBD
{
    public class TcpClientSession : TcpSessionBase
    {
        public TcpClientSession(Socket socket, uint bufferSize, Action<Buffer> method)
            : base(socket, bufferSize)
        {
            this.bindReadInternal = method;
        }

        protected override void ReadInternal(Header header)
        {
            byte[] block = readBuffer.ReadByteBlock(header.Length);
            Buffer buffer = new Buffer(block);
            bindReadInternal(buffer);
        }

        protected override void StartInternal()
        {
        }

        protected override void WriteInternal()
        {
        }

        private Action<Buffer> bindReadInternal;
    }
}
