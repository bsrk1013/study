#include <iostream>
#include <boost/bind.hpp>
#include "TcpSession.h"
#include "TcpServer.h"
#include "Serialize.h"
#include "Deserialize.h"
#include "Request.h"

namespace DBBD {
	TcpSession::pointer TcpSession::create(TcpServer* server, IoContextSP context, SocketSP socket) {
		return TcpSession::pointer(new TcpSession(server, context, socket));
	}

	TcpSession::pointer TcpSession::create(SocketSP socket) {
		return TcpSession::pointer(new TcpSession(socket));
	}

	TcpSession::TcpSession(TcpServer* server_context, IoContextSP context, SocketSP socket)
		: server(server_context),
		context(context),
		sendBuffer(8192),
		receiveBuffer(8192),
		sessionId(0),
		socket(socket){
		std::cout << "TcpSession[server] call..." << std::endl;
	}

	TcpSession::TcpSession(SocketSP socket)
	: sendBuffer(8192),
	receiveBuffer(8192),
	sessionId(0),
	socket(socket){
		std::cout << "TcpSession[client] call..." << std::endl;
	}

	TcpSession::~TcpSession() {
		std::cout << "~TcpSession call..." << std::endl;
		dieconnect();
	}

	void TcpSession::start() {
		read();
	}

	void TcpSession::stop() {
		dieconnect();
	}

	void TcpSession::read() {
		std::cout << "session read..." << std::endl;
		//lockObject.lock();
		receiveBuffer.adjust();
		//lockObject.unlock();
		if (socket) {
			socket->async_read_some(buffer(receiveBuffer.getBuffer(), 8192),
				boost::bind(&TcpSession::handleRead, shared_from_this(),
					placeholders::error, placeholders::bytes_transferred));
		}
	}

	void TcpSession::handleRead(const error_code& error, size_t bytesTransferred) {
		lockObject.lock();
		if (error) {
			std::cerr << "session disconnected..." << " sessionId: " << sessionId
				<< ", error: " << error << std::endl;
			dieconnect();
			lockObject.unlock();
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
					else {
						receiveBuffer.readByteBlock(header.length);
					}
				}
			}
		}

		lockObject.unlock();

		read();
	}

	void TcpSession::write(Cell* data) {
		std::cout << "session write..." << std::endl;
		//lockObject.lock();
		Serialize::write(sendBuffer, data);
		//lockObject.unlock();

		if (socket) {
			async_write(*socket,
				buffer(sendBuffer.getBuffer(), sendBuffer.getBufferLastPos()),
				boost::bind(&TcpSession::handleWrite, shared_from_this(),
					placeholders::error, placeholders::bytes_transferred));
		}
	}

	void TcpSession::handleWrite(const error_code& error, size_t bytesTransferred) {
		lockObject.lock();
		if (error) {
			std::cerr << "session disconnected..." << " sessionId: " << sessionId
				<< ", error: " << error << std::endl;
			dieconnect();
			lockObject.unlock();
			return;
		}

		std::cout << "sendData[" << bytesTransferred << "]" << std::endl;
		sendBuffer.clearBuffer();
		lockObject.unlock();
	}

	void TcpSession::dieconnect() {
		if (!isConnect()) {
			return;
		}

		if (server != nullptr) {
			server->sessionDisconnected(sessionId);
			server = nullptr;
		}

		if (socket && socket->is_open()) {
			socket->close();
			socket.reset();
		}
	}
}