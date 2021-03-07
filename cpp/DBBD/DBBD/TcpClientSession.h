#pragma once
#include "TcpSessionBase.h"

namespace DBBD
{
	class TcpClientSession : public TcpSessionBase
	{
	public:
		TcpClientSession(IoContextSP, SocketSP, unsigned int,
			std::function<void(DBBD::Buffer)>);
		virtual ~TcpClientSession();

	protected:
		// TcpSessionBase을(를) 통해 상속됨
		virtual void startInternal() override;
		virtual void readInternal(DBBD::Header) override;
		virtual void writeInternal() override;

	private:
		std::function<void(DBBD::Buffer)> bindReadInternal;
	};
}