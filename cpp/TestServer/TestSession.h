#pragma once
#ifndef TESTSESSION_H
#define TESTSESSION_H
#include <boost/asio.hpp>

using namespace boost::asio;
using namespace boost::system;

class TestSession : public std::enable_shared_from_this<TestSession>
{
public:
	TestSession(ip::tcp::socket socket, int sessionId);
	~TestSession();

	void startRead();

private:
	void read();
	void write(size_t readLength);
	void sessionDisconnected();

private:
	static const size_t maxBufferSize = 8192;

private:
	ip::tcp::socket socket;
	int sessionId;
	char bufferData[maxBufferSize];
};
#endif