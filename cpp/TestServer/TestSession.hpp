#pragma once
#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>

using namespace boost::asio;
using namespace boost::system;

class TestServer;
class TestSession : public boost::enable_shared_from_this<TestSession>
{
public:
	typedef boost::shared_ptr<TestSession> pointer;
	static pointer create(TestServer* server, io_context& context);
	void start();

// getter, setter
public:
	ip::tcp::socket& getSocket() { return socket; }

private:
	TestSession(TestServer* server, io_context& context);
	void read();
	void handleRead(const error_code& error, size_t bytesTransfrred);
	void write();
	void handleWrite(const error_code& error, size_t bytesTransferred);

private:
	static const size_t maxBufferSize = 8192;
	TestServer* server;
	ip::tcp::socket socket;
	char bufferData[maxBufferSize];
};
