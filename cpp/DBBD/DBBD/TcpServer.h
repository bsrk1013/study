#pragma once
#include <memory>
#include <vector>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include "TcpSession.h"
#include "Define.h"

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

		void stop();

	public:
		void sessionDisconnected(size_t sessionId);

	public:
		const std::string getName() { return name; }

	protected:
		virtual void acceptInternal(DBBD::TcpSession::pointer session) = 0;
		virtual void disconnectInternal(size_t sessionId) = 0;
		virtual void closeInternal() = 0;

	private:
		void startAccept();
		void handleAccept(SocketSP socket, const error_code& error);
		/*void handleAccept(TcpSession::pointer session, const error_code& error);*/
		size_t increaseAndGetSessionId();

	protected:
			std::mutex lockObject;

	private:
		bool isDispose = false;
		std::string name;
		IoContextSP context;
		std::unique_ptr<ip::tcp::acceptor> acceptor;
		std::atomic<size_t> sessionIdCounter = 0;
		boost::thread_group threads;
	};
}