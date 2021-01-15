#pragma once
#include <boost/asio.hpp>
#include <thread>
#include "Define.h"
#include "TcpClientSession.h"

namespace DBBD
{
	class ClientBase
	{
	public:
		virtual ~ClientBase() {}

	public:
		virtual void start() = 0;
		virtual void stop() = 0;
		virtual void send(DBBD::Cell* data) = 0;

	protected:
		virtual void startInternal() = 0;
		virtual void stopInternal() = 0;

		virtual void connect() = 0;
		virtual void handleConnect(const boost::system::error_code&) = 0;
		virtual void readInternal(DBBD::Buffer) = 0;

	protected:
		std::string address;
		int port;
		bool isDisposed = false;
		std::thread* thread = nullptr;
		IoContextSP context;
		SocketSP socket;
	};
}