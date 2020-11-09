#pragma once
#ifndef TESTSERVER_H
#define TESTSERVER_H
#include <boost/asio.hpp>
#include <atomic>
#include <map>
#include "TestSession.h"

using namespace boost::asio;
using namespace boost::system;

class TestServer
{
public:
	TestServer(io_context& context);
	TestServer(io_context& context, short port);
	~TestServer();

private:
	void accept();
	size_t increaseAndGetSessionId();

private:
	ip::tcp::acceptor acceptor;
	std::atomic<size_t> sessionIdCounter = 0;
	std::map<size_t, std::shared_ptr<TestSession>> sessionMap;
};
#endif