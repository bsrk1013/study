#include <iostream>
#include <boost/bind.hpp>
#include "TcpSession.h"
#include "TcpServer.h"
#include "Serialize.h"
#include "Deserialize.h"
#include "Request.h"

namespace DBBD {
	TcpSession::pointer TcpSession::create(TcpServer* server, io_context& context) {
		return TcpSession::pointer(new TcpSession(server, context));
	}

	TcpSession::pointer TcpSession::create(std::shared_ptr<ip::tcp::socket> socket) {
		return TcpSession::pointer(new TcpSession(socket));
	}

	TcpSession::TcpSession(TcpServer* server_context, io_context& context)
		: server(server_context), sendBuffer(8192), receiveBuffer(8192) {
		socket = std::make_shared<ip::tcp::socket>(context);
	}

	TcpSession::TcpSession(std::shared_ptr<ip::tcp::socket> socket)
	: sendBuffer(8192), receiveBuffer(8192){
		this->socket = socket;
	}

	void TcpSession::start() {
		read();
	}

	void TcpSession::read() {
		receiveBuffer.adjust();
		socket->async_read_some(buffer(receiveBuffer.getBuffer(), 8192),
			boost::bind(&TcpSession::handleRead, shared_from_this(),
				placeholders::error, placeholders::bytes_transferred));
	}

	void TcpSession::handleRead(const error_code& error, size_t bytesTransferred) {
		if (error) {
			std::cerr << "session disconnected..." << " sessionId: " << sessionId
				<< ", error: " << error << std::endl;

			dieconnect();
			return;
		}

		std::cout << "Bytes : " << bytesTransferred << std::endl;

		receiveBuffer.increaseLastPos(bytesTransferred);

		while (true) {
			if (receiveBuffer.getBufferLastPos() < HeaderSize) {
				break;
			}
			else {
				auto headerBlock = receiveBuffer.viewByteBlock(HeaderSize);
				Header header(headerBlock);

				if (receiveBuffer.getBufferLastPos() < header.length) {
					break;
				}
				else {
					if (readInternal) {
						readInternal(header, receiveBuffer);
					}
					/*if (readInternal) {
						readInternal(header, receiveBuffer);
					}*/
					/*if (readDelegate != nullptr) {
						readDelegate->readInternal(header, receiveBuffer);
					}
					else {
						receiveBuffer.readByteBlock(HeaderSize);
						receiveBuffer.readByteBlock(header.length);
					}*/
				}
			}
		}

		read();
	}

	void TcpSession::write(Cell* data) {
		Serialize::write(sendBuffer, data);

		async_write(*socket,
			buffer(sendBuffer.getBuffer(), sendBuffer.getBufferLastPos()),
			boost::bind(&TcpSession::handleWrite, shared_from_this(),
				placeholders::error, placeholders::bytes_transferred));
	}

	void TcpSession::handleWrite(const error_code& error, size_t bytesTransferred) {
		std::cout << "sendData[" << bytesTransferred << "]" << std::endl;
		sendBuffer.clearBuffer();
	}

	void TcpSession::dieconnect() {
		if (server != nullptr) {
			server->sessionDisconnected(sessionId);
		}
	}
}