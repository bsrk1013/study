#pragma once
#include <boost/asio.hpp>
#include <thread>
#include <vector>
#include <queue>
#include "Define.h"

namespace DBBD
{
	class ServerBase
	{
	public:
		virtual ~ServerBase() {}

	public:
		virtual void start() = 0;
		virtual void stop() = 0;

	protected:
		virtual void stopInternal() = 0;

		virtual void accept() = 0;
		virtual void handleAccept(SocketSP, const boost::system::error_code&) = 0;
		virtual void acceptInternal(SocketSP, size_t) = 0;

	protected:
		size_t increaseAndGetSessionId() { return ++sessionIdCounter; }

	protected:
		bool isDisposed = false;
		std::string name;
		std::string address;
		int port;
		size_t threadCount;
		IoContextSP context;
		std::vector<std::thread*> threadList;

	private:
		std::atomic<size_t> sessionIdCounter = 0;
	};
}