#include <iostream>
#include <boost/bind.hpp>
#include "TestSession.hpp"
#include "TestServer.hpp"
#include "DBBDSerialize.h"
#include "DBBDDeserialize.h"

TestSession::pointer TestSession::create(TestServer* server, io_context& context) {
	return TestSession::pointer(new TestSession(server, context));
}

TestSession::TestSession(TestServer* server, io_context& context)
: socket(context), sendBuffer(8192), receiveBuffer(8192){

}

void TestSession::start() {
	read();
	/*async_write(socket, buffer(bufferData, maxBufferSize),
		boost::bind(&TestSession::handleWrite, shared_from_this(),
			placeholders::error, placeholders::bytes_transferred));*/
}

void TestSession::read() {
	socket.async_read_some(buffer(receiveBuffer.getBuffer(), 8192),
		boost::bind(&TestSession::handleRead, shared_from_this(),
			placeholders::error, placeholders::bytes_transferred));
}

void TestSession::handleRead(const error_code& error, size_t bytesTransfrred) {
	if (error) {
		std::cerr << "session disconnected... error: " << error << std::endl;
		return;
	}

	receiveBuffer.setBufferLastPos(bytesTransfrred);
	DBBD::Deserialize deserialize(&receiveBuffer);

	std::string data;
	deserialize.read(data);

	std::cout << "recieveData[" << bytesTransfrred << "] : " << data << std::endl;

	write(data);

	receiveBuffer.clearBuffer();

	read();
}

void TestSession::write(const std::string& data) {

	DBBD::Serialize serialize(&sendBuffer);

	serialize.write(data);

	async_write(socket,
		buffer(sendBuffer.getBuffer(), sendBuffer.getBufferLastPos()),
			boost::bind(&TestSession::handleWrite, shared_from_this(),
				placeholders::error, placeholders::bytes_transferred));
}

void TestSession::handleWrite(const error_code& error, size_t bytesTransferred) {
	sendBuffer.clearBuffer();
}