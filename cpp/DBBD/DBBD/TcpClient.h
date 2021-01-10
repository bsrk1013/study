#pragma once
#include <string>
#include <thread>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include "TcpSession.h"

using namespace boost::asio;
using namespace boost::system;

namespace DBBD {
	class Cell;
	class TcpClient
	{
	public:
		TcpClient() {}
		TcpClient(const TcpClient&) {}
		TcpClient(const std::string& address, const short& port);
		virtual ~TcpClient();
		
	public:
		void close();
		void send(Cell* data);

	public:
		TcpSession::pointer getSession() { return session; }

	protected:
		virtual void connectInternal(TcpSession::pointer session) = 0;

	private:
		void handleConnect(const boost::system::error_code& error);

	private:
		boost::thread_group threads;
		std::unique_ptr<io_context> context;
		std::shared_ptr<ip::tcp::socket> socket;
		TcpSession::pointer session;
	};
}