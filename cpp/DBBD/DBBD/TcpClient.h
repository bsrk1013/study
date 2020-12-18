#pragma once
#include <string>
#include <thread>
#include <boost/asio.hpp>
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
		~TcpClient();
		
	public:
		void send(Cell* data);

	public:
		TcpSession::pointer getSession() { return session; }

	private:
		void handleConnect(const error_code& error);

	private:
		std::thread* mainThread = nullptr;
		std::unique_ptr<io_context> context;
		std::shared_ptr<ip::tcp::socket> socket;
		TcpSession::pointer session;
	};
}