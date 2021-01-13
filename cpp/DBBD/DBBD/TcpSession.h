#pragma once
#include <mutex>
#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include "Define.h"
#include "Buffer.h"
#include "Request.h"

using namespace boost::asio;
using namespace boost::system;

namespace DBBD {
	class Cell;
	class TcpServer;
	class TcpSession
		: public std::enable_shared_from_this<TcpSession>
	{
	public:
		using pointer = std::shared_ptr<TcpSession>;
		static pointer create(TcpServer* server_context, IoContextSP context, SocketSP socket);
		static pointer create(SocketSP socket);

		~TcpSession();
		void start();
		void stop();

		// getter, setter
	public:
		bool isConnect() { if (socket) { return socket->is_open(); } return false; }
		SocketSP getSocket() { return socket; }
		IoContextSP getContext() { return context; }
		size_t getSessionId() { return sessionId; }
		void setSessionId(size_t value) { sessionId = value; }
		void write(Cell* data);

	private:
		TcpSession(TcpServer* server, IoContextSP context, SocketSP socket);
		TcpSession(SocketSP socket);

	private:
		void read();
		void handleRead(const error_code& error, size_t bytesTransfrred);
		void handleWrite(const error_code& error, size_t bytesTransferred);
		void dieconnect();

	public:
		std::function<bool(const Header&, Buffer&)> readInternal;

	private:
		TcpServer* server = nullptr;
		IoContextSP context;
		SocketSP socket;
		Buffer sendBuffer;
		Buffer receiveBuffer;
		size_t sessionId;
	};

	class ITcpSession {
	public:
		virtual void init(DBBD::TcpSession::pointer session) = 0;
		virtual void send(DBBD::Cell*) = 0;
	protected:
		virtual void bindReadInternal(DBBD::ReadInternalParam&) = 0;
		virtual bool readInternal(const DBBD::Header&, DBBD::Buffer&) = 0;
	};
}