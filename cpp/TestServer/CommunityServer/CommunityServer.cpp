#include "CommunityServer.h"

CommunityServer::CommunityServer(std::string name, std::string address, int port)
	: DBBD::TcpServer(name, address, port)
{}

CommunityServer::~CommunityServer() {}

void CommunityServer::acceptInternal(DBBD::TcpSession::pointer session)
{
	auto sessionId = session->getSessionId();
	auto iter = sessionMap.find(sessionId);
	if (iter != sessionMap.end()) {
		std::cerr << "acceptInternal, duplicated session id, sessionId: " << sessionId << std::endl;
		return;
	}

	auto baseSession = std::make_shared<BaseClientSession>();
	baseSession->init(session);
	sessionMap[sessionId] = baseSession;
}

void CommunityServer::disconnectInternal(size_t sessionId)
{
	auto iter = sessionMap.find(sessionId);
	if (iter == sessionMap.end()) {
		std::cerr << "disconnectInternal, not found session id, sessionId: " << sessionId << std::endl;
		return;
	}

	sessionMap.erase(sessionId);
}
