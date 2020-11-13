#include <iostream>
#include <boost/bind.hpp>
#include "TcpServer.h"

namespace DBBD {
	TcpServer::TcpServer(io_context& context)
		: TcpServer(context, 8100) {

	}

	TcpServer::TcpServer(io_context& context, short port)
		: TcpServer(context, "127.0.0.1", port) {
	}

	TcpServer::TcpServer(io_context& context, std::string address)
		: TcpServer(context, address, 8100) {

	}

	TcpServer::TcpServer(io_context& context, std::string address, short port)
		: context(context), acceptor(context, ip::tcp::endpoint(ip::address_v4::from_string(address), port)) {
		startAccept();
	}

	TcpServer::~TcpServer() {
		/*if (sessionMap.size() > 0) {
			for (auto pair : sessionMap) {
				auto session = pair.second;
				session->sessionClose();
			}
		}*/

		if (acceptor.is_open()) {
			acceptor.close();
		}
	}

	void TcpServer::startAccept() {
		auto session = TcpSession::create(this, context);
		acceptor.async_accept(session->getSocket(),
			boost::bind(&TcpServer::handleAccept, this, session, placeholders::error));
	}

	void TcpServer::handleAccept(TcpSession::pointer session, const error_code& error) {
		if (!error) {
			size_t sessionId = increaseAndGetSessionId();
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