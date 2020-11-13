#pragma once
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
		static pointer create(TcpServer* server, io_context& context);
		void start();

		// getter, setter
	public:
		ip::tcp::socket& getSocket() { return socket; }

	private:
		TcpSession(TcpServer* server, io_context& context);
		void read();
		void handleRead(const error_code& error, size_t bytesTransfrred);
		void write(const std::string& data);
		void handleWrite(const error_code& error, size_t bytesTransferred);

	private:
		TcpServer* server;
		ip::tcp::socket socket;
		Buffer sendBuffer;
		Buffer receiveBuffer;
	};
}