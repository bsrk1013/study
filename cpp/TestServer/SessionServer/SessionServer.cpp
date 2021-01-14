#include "SessionServer.h"

SessionServer::SessionServer(std::string name, std::string address, int port, size_t threadCount)
	: TcpServerBase(name, address, port, threadCount)
{}

SessionServer::~SessionServer()
{}

void SessionServer::stopInternal()
{
}

void SessionServer::acceptInternal(DBBD::SocketSP, size_t)
{
}
