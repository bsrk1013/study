#include "SessionServer.h"
#include "Player.h"
#include "DBBD\TcpSession.h"

SessionServer::SessionServer(std::string name)
	: TcpServer(name) {}
SessionServer::SessionServer(std::string name, short port)
	: TcpServer(name, port) {}
SessionServer::SessionServer(std::string name, std::string address)
	: TcpServer(name, address) {}
SessionServer::SessionServer(std::string name, std::string address, short port)
	: TcpServer(name, address, port) {}

SessionServer::~SessionServer() {}

void SessionServer::implementAccept(DBBD::TcpSession::pointer session) {
	Player player(session);
}