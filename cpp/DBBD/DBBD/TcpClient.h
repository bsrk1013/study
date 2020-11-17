#pragma once
#include <string>
#include <thread>
#include <boost/asio.hpp>
#include "TcpSession.h"

using namespace boost::asio;
using namespace boost::system;

namespace DBBD {
	class TcpClient
	{
	public:
		TcpClient(const std::string& address, const short& port);
		~TcpClient();
		
	public:
		void send(const std::string& data);

	public:
		TcpSession::pointer getSession() { return session; }

	private:
		void handleConnect(const error_code& error);

	private:
		std::thread* t1 = nullptr;
		std::unique_ptr<io_context> context;
		std::shared_ptr<ip::tcp::socket> socket;
		TcpSession::pointer session;
	};
}