#include "TestSession.h"
#include <iostream>

TestSession::TestSession(ip::tcp::socket socket, int sessionId) 
: socket(std::move(socket)), sessionId(sessionId){
}

TestSession::~TestSession() {
	if (socket.is_open()) {
		socket.close();
	}
}

void TestSession::startRead() {
	read();
}

void TestSession::read() {
	auto self = shared_from_this();
	socket.async_read_some(buffer(bufferData, maxBufferSize),
		[this, self](error_code error, size_t readLength) {
		if (!error) {
			write(readLength);
		}
		else {
			sessionDisconnected();
		}
	});
}

void TestSession::write(size_t readLength) {
	auto self = shared_from_this();
	async_write(socket, buffer(bufferData, readLength),
		[this, self](error_code error, size_t writeLength) {
		if (!error) {
			read();
		}
		else {
			sessionDisconnected();
		}
	});
}

void TestSession::sessionDisconnected() {
	std::cout << "session disconnected... id: " << sessionId << std::endl;

}