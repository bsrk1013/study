#include <iostream>
#include <boost/bind.hpp>
#include "TcpServer.h"

namespace DBBD {
	TcpServer::TcpServer(std::string name)
		: TcpServer(name, 8100) {}

	TcpServer::TcpServer(std::string name, short port)
		: TcpServer(name, "127.0.0.1", port) {}

	TcpServer::TcpServer(std::string name, std::string address)
		: TcpServer(name, address, 8100) {}

	TcpServer::TcpServer(std::string name, std::string address, short port)
		: name(name) {

		context = std::make_unique<io_context>();
		acceptor = std::make_unique<ip::tcp::acceptor>(*context,
			ip::tcp::endpoint(ip::address_v4::from_string(address), port));

		std::cout << name << " Server started..." << std::endl;
		startAccept();

		context->run();
	}

	TcpServer::~TcpServer() {
		if (acceptor &&
				acceptor->is_open()) {
			acceptor->close();
		}

		if (context) {
			context->stop();
		}
	}

	void TcpServer::startAccept() {
		auto session = TcpSession::create(this, *context);
		acceptor->async_accept(*session->getSocket(),
			boost::bind(&TcpServer::handleAccept, this, session, placeholders::error));
	}

	void TcpServer::handleAccept(TcpSession::pointer session, const error_code& error) {
		if (!error) {
			size_t sessionId = increaseAndGetSessionId();
			session->setSessionId(sessionId);

			sessionMap[sessionId] = session;

			//implementAccept(session);

			session->start();
			std::cout << "session connected... id: " << sessionId << std::endl;
		}

		startAccept();
	}

	void TcpServer::sessionDisconnected(size_t sessionId) {
		sessionMap.erase(sessionId);
		std::cout << "session map count: " << sessionMap.size() << std::endl;
	}

	size_t TcpServer::increaseAndGetSessionId() {
		return ++sessionIdCounter;
	}
}