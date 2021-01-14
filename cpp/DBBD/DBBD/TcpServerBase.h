#pragma once
#include "ServerBase.h"

namespace DBBD {
	class TcpServerBase : public ServerBase
	{
	public:
		TcpServerBase(std::string name, std::string address, int port, size_t threadCount);
		virtual ~TcpServerBase();

	public:
		virtual void start() override;
		virtual void stop() override;

	protected:
		virtual void stopInternal() = 0;

		virtual void handleAccept(SocketSP, const boost::system::error_code&) override;
		virtual void acceptInternal(SocketSP, size_t) = 0;

	private:
		virtual void accept() override;


	private:
		TcpAcceptorSP acceptor;
	};
}