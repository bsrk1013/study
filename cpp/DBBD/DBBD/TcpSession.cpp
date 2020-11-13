#include <iostream>
#include <boost/bind.hpp>
#include "TcpSession.h"
#include "TcpServer.h"

namespace DBBD {
	TcpSession::pointer TcpSession::create(TcpServer* server, io_context& context) {
		return TcpSession::pointer(new TcpSession(server, context));
	}

	TcpSession::TcpSession(TcpServer* server, io_context& context)
		: socket(context), sendBuffer(8192), receiveBuffer(8192) {

	}

	void TcpSession::start() {
		read();
		/*async_write(socket, buffer(bufferData, maxBufferSize),
			boost::bind(&TestSession::handleWrite, shared_from_this(),
				placeholders::error, placeholders::bytes_transferred));*/
	}

	void TcpSession::read() {
		socket.async_read_some(buffer(receiveBuffer.getBuffer(), 8192),
			boost::bind(&TcpSession::handleRead, shared_from_this(),
				placeholders::error, placeholders::bytes_transferred));
	}

	void TcpSession::handleRead(const error_code& error, size_t bytesTransfrred) {
		if (error) {
			std::cerr << "session disconnected... error: " << error << std::endl;
			return;
		}

		//receiveBuffer.setBufferLastPos(bytesTransfrred);
		//Deserialize deserialize(&receiveBuffer);

		//std::string data;
		//deserialize.read(data);

		//std::cout << "recieveData[" << bytesTransfrred << "] : " << data << std::endl;

		//write(data);

		//receiveBuffer.clearBuffer();

		//read();
	}

	void TcpSession::write(const std::string& data) {
		/*Serialize serialize(&sendBuffer);

		serialize.write(data);

		async_write(socket,
			buffer(sendBuffer.getBuffer(), sendBuffer.getBufferLastPos()),
			boost::bind(&TestSession::handleWrite, shared_from_this(),
				placeholders::error, placeholders::bytes_transferred));*/
	}

	void TcpSession::handleWrite(const error_code& error, size_t bytesTransferred) {
		sendBuffer.clearBuffer();
	}
}