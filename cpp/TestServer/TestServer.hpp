#pragma once
#include <boost/asio.hpp>
#include <atomic>
#include <map>
#include "TestSession.hpp"

using namespace boost::asio;
using namespace boost::system;

class TestSession;
class TestServer
{
public:
	TestServer(io_context& context);
	TestServer(io_context& context, short port);
	TestServer(io_context& context, std::string address);
	TestServer(io_context& context, std::string address, short port);
	~TestServer();

public:
	void sessionDisconnected(size_t sessionId);

private:
	void startAccept();
	void handleAccept(TestSession::pointer session, const error_code& error);
	size_t increaseAndGetSessionId();

private:
	io_context& context;
	ip::tcp::acceptor acceptor;
	std::atomic<size_t> sessionIdCounter = 0;
	std::map<size_t, std::shared_ptr<TestSession>> sessionMap;
};
