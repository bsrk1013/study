#include <iostream>
#include <boost/bind.hpp>
#include "TestSession.hpp"
#include "TestServer.hpp"

TestSession::pointer TestSession::create(TestServer* server, io_context& context) {
	return TestSession::pointer(new TestSession(server, context));
}

TestSession::TestSession(TestServer* server, io_context& context)
: socket(context){
}

void TestSession::start() {
	socket.async_read_some(buffer(bufferData, maxBufferSize),
		boost::bind(&TestSession::handleRead, shared_from_this(),
			placeholders::error, placeholders::bytes_transferred));
	/*async_write(socket, buffer(bufferData, maxBufferSize),
		boost::bind(&TestSession::handleWrite, shared_from_this(),
			placeholders::error, placeholders::bytes_transferred));*/
}

void TestSession::handleRead(const error_code& error, size_t bytesTransfrred) {
	if (error) {
		std::cerr << "session disconnected... error: " << error << std::endl;
		return;
	}


	auto first = buffer_cast<const char*>(bufferData);
	/*auto a = &bufferData;
	for (size_t i = 0; i < bytesTransfrred; i++) {
		auto b = a[i];
	}*/
}

void TestSession::handleWrite(const error_code& error, size_t bytesTransferred) {

}