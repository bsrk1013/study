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
	class ITcpSession {
	protected:
		virtual void bindReadInternal(ReadInternalParam&) = 0;
		virtual bool readInternal(const Header&, Buffer&) = 0;
	};

	class Cell;
	class TcpServer;
	class TcpSession
		: public std::enable_shared_from_this<TcpSession>
	{
	public:
		typedef std::shared_ptr<TcpSession> pointer;
		static pointer create(TcpServer* server_context, IoContextSP context);
		static pointer create(SocketSP socket);
		void start();

		// getter, setter
	public:
		SocketSP getSocket() { return socket; }
		IoContextSP getContext() { return context; }
		size_t getSessionId() { return sessionId; }
		void setSessionId(size_t value) { sessionId = value; }
		void write(Cell* data);

	private:
		TcpSession(TcpServer* server, IoContextSP context);
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
		std::mutex readLock;
		std::mutex writeLock;
	};
}