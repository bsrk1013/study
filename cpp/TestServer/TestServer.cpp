#include <iostream>
#include "TestServer.h"

TestServer::TestServer(io_context& context)
: TestServer(context, 8100){

}

TestServer::TestServer(io_context& context, short port)
: acceptor(context, ip::tcp::endpoint(ip::tcp::v4(), port)){
	accept();
}

TestServer::~TestServer() {
	if (acceptor.is_open()) {
		acceptor.close();
	}
}

void TestServer::accept() {
	acceptor.async_accept([this](error_code error, ip::tcp::socket socket) {
		if (!error) {
			size_t sessionId = increaseAndGetSessionId();
			auto session = std::make_shared<TestSession>(std::move(socket), sessionId);
			session->startRead();

			std::cout << "session connected... id: " << sessionId << std::endl;

			sessionMap[sessionId] = session;
		}

		accept();
	});
}

size_t TestServer::increaseAndGetSessionId() {
	return ++sessionIdCounter;
}