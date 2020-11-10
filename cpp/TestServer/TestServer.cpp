#include <iostream>
#include <boost/bind.hpp>
#include "TestServer.hpp"
#include "TestSession.hpp"

TestServer::TestServer(io_context& context)
: TestServer(context, 8100){

}

TestServer::TestServer(io_context& context, short port)
: TestServer(context, "127.0.0.1", port){
}

TestServer::TestServer(io_context& context, std::string address) 
: TestServer(context, address, 8100){

}

TestServer::TestServer(io_context& context, std::string address, short port) 
: context(context), acceptor(context, ip::tcp::endpoint(ip::address_v4::from_string(address), port)) {
	startAccept();
}

TestServer::~TestServer() {
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

void TestServer::startAccept() {
	auto session = TestSession::create(this, context);
	acceptor.async_accept(session->getSocket(),
		boost::bind(&TestServer::handleAccept, this, session, placeholders::error));
}

void TestServer::handleAccept(TestSession::pointer session, const error_code& error) {
	if (!error) {
		size_t sessionId = increaseAndGetSessionId();
		session->start();
		std::cout << "session connected... id: " << sessionId << std::endl;
	}

	startAccept();
}

void TestServer::sessionDisconnected(size_t sessionId) {
	sessionMap.erase(sessionId);
	std::cout << "session map count: " << sessionMap.size() << std::endl;
}

size_t TestServer::increaseAndGetSessionId() {
	return ++sessionIdCounter;
}