#pragma once
#include <memory>
#include <boost/asio.hpp>
#include "TcpSession.h"

using namespace boost::asio;
using namespace boost::system;

namespace DBBD {
	class TcpSession;
	class TcpServer
	{
	public:
		TcpServer(std::string name);
		TcpServer(std::string name, short port);
		TcpServer(std::string name, std::string address);
		TcpServer(std::string name, std::string address, short port);
		virtual ~TcpServer();

	public:
		void sessionDisconnected(size_t sessionId);

	public:
		const std::string getName() { return name; }

	protected:
		virtual void implementAccept(TcpSession::pointer session) = 0;

	private:
		void startAccept();
		void handleAccept(TcpSession::pointer session, const error_code& error);
		size_t increaseAndGetSessionId();

	private:
		std::string name;
		std::unique_ptr<io_context> context;
		std::unique_ptr<ip::tcp::acceptor> acceptor;
		std::atomic<size_t> sessionIdCounter = 0;
		std::map<size_t, TcpSession::pointer> sessionMap;
	};
}