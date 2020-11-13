#pragma once
#include <boost/asio.hpp>
#include "TcpSession.h"

using namespace boost::asio;
using namespace boost::system;

namespace DBBD {
	class TcpSession;
	class TcpServer
	{
	public:
		TcpServer(io_context& context);
		TcpServer(io_context& context, short port);
		TcpServer(io_context& context, std::string address);
		TcpServer(io_context& context, std::string address, short port);
		~TcpServer();

	public:
		void sessionDisconnected(size_t sessionId);

	private:
		void startAccept();
		void handleAccept(TcpSession::pointer session, const error_code& error);
		size_t increaseAndGetSessionId();

	private:
		io_context& context;
		ip::tcp::acceptor acceptor;
		std::atomic<size_t> sessionIdCounter = 0;
		std::map<size_t, std::shared_ptr<TcpSession>> sessionMap;
	};
}