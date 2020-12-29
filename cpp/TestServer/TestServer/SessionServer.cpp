#include "SessionServer.h"
#include "DBBD/TcpSession.h"
#include "Player.h"

SessionServer::SessionServer(std::string name)
	: TcpServer(name) {}
SessionServer::SessionServer(std::string name, short port)
	: TcpServer(name, port) {}
SessionServer::SessionServer(std::string name, std::string address)
	: TcpServer(name, address) {}
SessionServer::SessionServer(std::string name, std::string address, short port)
	: TcpServer(name, address, port) {}

SessionServer::~SessionServer() {}

void SessionServer::acceptInternal(DBBD::TcpSession::pointer session) {
	Player* player = new Player(session);
	playerMap[session->getSessionId()] = player;
}

void SessionServer::disconnectInternal(size_t sessionId) {
	auto player = playerMap[sessionId];
	playerMap.erase(sessionId);
	if (player) {
		delete player;
	}
	std::cout << "player map count: " << playerMap.size() << std::endl;
}