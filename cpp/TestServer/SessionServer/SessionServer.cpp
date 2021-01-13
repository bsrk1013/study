#include "SessionServer.h"
#include "DBBD/TcpSession.h"
#include "PlayerClientSession.h"

SessionServer::SessionServer(std::string name, std::string address, int port)
	: DBBD::TcpServer(name, address, port)
{
	communityClient = std::make_shared<CommunityClient>("127.0.0.1", 8100, this);
}

SessionServer::~SessionServer() {}

void SessionServer::acceptInternal(DBBD::TcpSession::pointer session)
{
	auto sessionId = session->getSessionId();
	auto iter = sessionMap.find(sessionId);
	if (iter != sessionMap.end()) {
		std::cerr << "acceptInternal, duplicated session id, sessionId: " << sessionId << std::endl;
		return;
	}

	auto playerSession = std::make_shared<PlayerClientSession>();
	playerSession->init(session);
	sessionMap[sessionId] = playerSession;
}

void SessionServer::disconnectInternal(size_t sessionId)
{
	auto iter = sessionMap.find(sessionId);
	if (iter == sessionMap.end()) {
		std::cerr << "disconnectInternal, not found session id, sessionId: " << sessionId << std::endl;
		return;
	}

	sessionMap.erase(sessionId);
}

void SessionServer::closeInternal() 
{
	communityClient->close();
	//communityClient.reset();
}
