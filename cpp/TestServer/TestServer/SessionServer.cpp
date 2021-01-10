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
	auto iter = playerMap.find(session->getSessionId());
	if (iter != playerMap.end()) {
		std::cout << "invalid session id, sessionId: " << session->getSessionId() << std::endl;
		return;
	}

	//auto player = playerSPPool.pop();
	auto player = std::make_shared<Player>();
	player->init(session);
	playerMap.insert(std::pair<size_t, std::shared_ptr<Player>>(session->getSessionId(), player));
	//playerMap[session->getSessionId()] = player;
	std::cout << "acceptInternal, mapCount: " << playerMap.size() << std::endl;
}

void SessionServer::disconnectInternal(size_t sessionId) {
	auto iter = playerMap.find(sessionId);
	if (iter == playerMap.end()) {
		std::cout << "invalid session id, sessionId: " << sessionId << std::endl;
		return;
	}

	auto player = iter->second;
	playerMap.erase(sessionId);
	player->dispose();

	std::cout << "disconnectInternal, mapCount: " << playerMap.size() << std::endl;
	/*playerSPPool.push(player);*/
}