#include <iostream>
#include <boost/bind.hpp>
#include <thread>
#include <mutex>
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

		context = NEW_CONTEXT_SP();
		auto tempGuard = make_work_guard(*context);

		acceptor = std::make_unique<ip::tcp::acceptor>(*context,
			ip::tcp::endpoint(ip::address_v4::from_string(address), port));
		acceptor->set_option(ip::tcp::acceptor::reuse_address(true));

		startAccept();

		for (size_t i = 0; i < std::thread::hardware_concurrency(); i++) {
			threads.create_thread(boost::bind(&io_context::run, &(*context)));
		}

		std::cout << name << " Server Started..." << std::endl;
	}

	TcpServer::~TcpServer() {
		stop();
	}

	void TcpServer::stop() {
		if (isDispose) {
			return;
		}

		isDispose = true;

		if (acceptor && acceptor->is_open()) {
			acceptor->cancel();
		}

		if (context) {
			context->stop();
		}

		threads.join_all();
	}

	void TcpServer::startAccept() {
		if (!acceptor 
			|| !acceptor->is_open()
			|| isDispose) {
			return;
		}

		std::cout << "[" << std::this_thread::get_id() << "]start accept..." << std::endl;
		SocketSP socket = NEW_SOCKET_SP(*context);
		if (socket) {
			acceptor->async_accept(*socket,
				boost::bind(&TcpServer::handleAccept, this, socket, placeholders::error));
		}
	}

	void TcpServer::handleAccept(SocketSP socket, const error_code& error) {
		if (!error) {
			size_t sessionId = increaseAndGetSessionId();

			auto session = TcpSession::create(this, context, socket);
			session->setSessionId(sessionId);
			acceptInternal(session);

			session->start();

			auto id = std::this_thread::get_id();
			std::cout << "[" << id << "]session connected... id: " << sessionId << std::endl;
		}

		startAccept();
	}

	void TcpServer::sessionDisconnected(size_t sessionId) {
		disconnectInternal(sessionId);
	}

	size_t TcpServer::increaseAndGetSessionId() {
		return ++sessionIdCounter;
	}
}