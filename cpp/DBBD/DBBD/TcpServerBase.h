#pragma once
#include "ServerBase.h"

namespace DBBD {
	class TcpServerBase : public ServerBase
	{
	public:
		TcpServerBase(const std::string& name,
			const std::string& address,
			const short& port,
			const size_t& threadCount);
		virtual ~TcpServerBase();

	public:
		virtual void start() override;
		virtual void stop() override;

	protected:
		virtual void stopInternal() = 0;

		virtual void handleAccept(DBBD::SocketSP, const boost::system::error_code&) override;
		virtual void acceptInternal(DBBD::SocketSP, size_t) = 0;
		virtual void disconnectInternal(size_t) = 0;

	private:
		virtual void accept() override;


	private:
		TcpAcceptorSP acceptor;
	};
}