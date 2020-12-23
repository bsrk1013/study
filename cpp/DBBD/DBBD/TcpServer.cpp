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

		context = std::make_shared<io_context>(5);
		auto tempGuard = make_work_guard(*context);
		//guard = new executor_work_guard<io_context::executor_type>(context.get());

		acceptor = std::make_unique<ip::tcp::acceptor>(*context,
			ip::tcp::endpoint(ip::address_v4::from_string(address), port));
		acceptor->set_option(ip::tcp::acceptor::reuse_address(true));

		startAccept();

		for (size_t i = 0; i < 3; i++) {
			threads.create_thread(boost::bind(&io_context::run, &(*context)));
		}

		//acceptor->listen();

		std::cout << name << " Server Started..." << std::endl;

		//context->run();
	}

	TcpServer::~TcpServer() {
		stop();
	}

	void TcpServer::stop() {
		if (acceptor && acceptor->is_open()) {
			acceptor->close();
		}

		if (context) {
			context->stop();
			//context->restart();
		}

		threads.join_all();
	}

	void TcpServer::startAccept() {
		auto session = TcpSession::create(this, context);
		acceptor->async_accept(*session->getSocket(),
			boost::bind(&TcpServer::handleAccept, this, session, placeholders::error));
	}

	void TcpServer::handleAccept(TcpSession::pointer session, const error_code& error) {
		if (!error) {
			size_t sessionId = increaseAndGetSessionId();

			lockObject.lock();
			session->setSessionId(sessionId);
			sessionMap[sessionId] = session;
			lockObject.unlock();
			session->start();

			acceptInternal(session);
			auto id = std::this_thread::get_id();
			std::cout << "[" << id << "]session connected... id: " << sessionId << std::endl;
		}

		startAccept();
	}

	void TcpServer::sessionDisconnected(size_t sessionId) {
		lockObject.lock();
		sessionMap.erase(sessionId);
		std::cout << "session map count: " << sessionMap.size() << std::endl;
		lockObject.unlock();
	}

	size_t TcpServer::increaseAndGetSessionId() {
		return ++sessionIdCounter;
	}
}