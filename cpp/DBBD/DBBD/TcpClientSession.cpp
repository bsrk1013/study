#include "TcpClientSession.h"

DBBD::TcpClientSession::TcpClientSession(IoContextSP context, SocketSP socket, size_t sessionId,
	std::function<void(DBBD::Buffer)> method)
	: TcpSessionBase(context, socket, sessionId)
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
	Buffer buffer(block);
	bindReadInternal(buffer);
}

void DBBD::TcpClientSession::writeInternal()
{
}
