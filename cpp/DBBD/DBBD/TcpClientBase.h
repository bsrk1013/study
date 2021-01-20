#pragma once
#include "ClientBase.h"

namespace DBBD
{
	class TcpClientSession;
	class TcpClientBase : public ClientBase
	{
	public:
		TcpClientBase(const std::string& address, const short& port, const bool& tryReconnect);
		virtual ~TcpClientBase();

	public:
		virtual void start() override;
		virtual void stop() override;
		virtual void send(DBBD::Cell* data) override;

	protected:
		virtual void startInternal() = 0;
		virtual void stopInternal() = 0;
		virtual void readInternal(DBBD::Buffer) = 0;

	private:
		virtual void connect() override;
		virtual void handleConnect(const boost::system::error_code&) override;
		virtual void disconnectInternal() override;

	private:
		void reconnect();


	private:
		std::shared_ptr<TcpClientSession> session;
	};
}