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

	class ChattingReq : DBBD::Request {
	public:
		ChattingReq() { typeId = 1; }
		virtual ~ChattingReq() {}

		// Request을(를) 통해 상속됨
		virtual void serialize(DBBD::Buffer& buffer)
		{
			writeHeader(buffer, getLength());
			DBBD::Serialize::write(buffer, msg);
		}
		virtual void deserialize(DBBD::Buffer& buffer)
		{
			readHeader(buffer);
			DBBD::Deserialize::read(buffer, msg);
		}

		virtual size_t getLength() {
			return Request::getLength() + sizeof(size_t) + msg.size();
		}

	public:
		std::string getMsg() { return msg; }
		void setMsg(std::string value) { msg = value; }

	private:
		std::string msg;
	};

	void TcpSession::read() {
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

		auto headerBlock = receiveBuffer.viewByteBlock(HeaderSize);
		Header header(headerBlock);

		readInternal(header);

		/*switch (header.typeId) {
		case 1:
			ChattingReq req;
			Deserialize::read(receiveBuffer, (Cell*)&req);
			std::cout << "Read : " << req.getMsg() << std::endl;
			break;
		}*/

		receiveBuffer.clearBuffer();

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