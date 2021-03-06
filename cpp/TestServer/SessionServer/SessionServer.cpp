#include "SessionServer.h"
#include <boost/bind.hpp>
#include "PlayerClientSession.h"
#include "DBBD/Log.h"

SessionServer::SessionServer(std::string name, std::string address, int port, size_t threadCount)
	: TcpServerBase(name, address, port, threadCount)
{}

SessionServer::~SessionServer()
{}

void SessionServer::stopInternal()
{
	std::vector<size_t> keys;
	for (auto pair : sessionMap) {
		keys.push_back(pair.first);
	}

	for (auto key : keys) {
		auto session = sessionMap[key];
		session->stop();
		session.reset();
	}

	sessionMap.clear();
}

void SessionServer::acceptInternal(DBBD::SocketSP socket, size_t sessionId)
{
	auto session = std::make_shared<PlayerClientSession>(context, socket, 8192);
	session->setSessionId(sessionId);
	session->bindingStopInternal(boost::bind(
		&SessionServer::disconnectInternal, this, boost::placeholders::_1));
	sessionMap[sessionId] = session;
	session->start();

	LOG_INFO("acceptInternal, sessionId: {}", sessionId);
}

void SessionServer::disconnectInternal(size_t sessionId)
{
	auto iter = sessionMap.find(sessionId);
	if (iter == sessionMap.end()) {
		return;
	}

	sessionMap.erase(sessionId);

	LOG_INFO("disconnectInternal, sessionId: {}", sessionId);
}