#include "TcpClientSession.h"

DBBD::TcpClientSession::TcpClientSession(IoContextSP context, SocketSP socket, unsigned int bufferSize,
	std::function<void(DBBD::Buffer)> method)
	: TcpSessionBase(context, socket, bufferSize)
	, bindReadInternal(method)
{
}

DBBD::TcpClientSession::~TcpClientSession()
{
}

void DBBD::TcpClientSession::startInternal()
{
}

void DBBD::TcpClientSession::readInternal(DBBD::Header header)
{
	char* block = readBuffer->readByteBlock(header.length);
	Buffer buffer(block, header.length);
	bindReadInternal(buffer);
}

void DBBD::TcpClientSession::writeInternal()
{
}
