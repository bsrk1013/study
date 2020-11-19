#pragma once
#include <mutex>
#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include "Buffer.h"

using namespace boost::asio;
using namespace boost::system;

namespace DBBD {
	class TcpServer;
	class TcpSession 
		: public boost::enable_shared_from_this<TcpSession>
	{
	public:
		typedef boost::shared_ptr<TcpSession> pointer;
		static pointer create(TcpServer* server_context, io_context& context);
		static pointer create(std::shared_ptr<ip::tcp::socket> socket);
		void start();
		void send();
		void broadcast();

		// getter, setter
	public:
		std::shared_ptr<ip::tcp::socket> getSocket() { return socket; }
		size_t getSessionId() { return sessionId; }
		void setSessionId(size_t value) { sessionId = value; }
		void write(const std::string& data);

	private:
		TcpSession(TcpServer* server, io_context& context);
		TcpSession(std::shared_ptr<ip::tcp::socket> socket);
		
	private:
		void read();
		void handleRead(const error_code& error, size_t bytesTransfrred);
		void handleWrite(const error_code& error, size_t bytesTransferred);
		void dieconnect();

	private:
		TcpServer* server = nullptr;
		std::shared_ptr<ip::tcp::socket> socket;
		Buffer sendBuffer;
		Buffer receiveBuffer;
		size_t sessionId;
		std::mutex readLock;
		std::mutex writeLock;
	};
}