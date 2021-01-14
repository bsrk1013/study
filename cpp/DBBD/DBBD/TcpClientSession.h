#pragma once
#include "TcpSessionBase.h"

namespace DBBD
{
	class TcpClientSession : public TcpSessionBase
	{
	public:
		TcpClientSession(IoContextSP, SocketSP, size_t,
			std::function<void(DBBD::Buffer)>);
		virtual ~TcpClientSession();

	protected:
		// TcpSessionBase��(��) ���� ��ӵ�
		virtual void readInternal(DBBD::Header) override;
		virtual void writeInternal() override;

	private:
		std::function<void(DBBD::Buffer)> bindReadInternal;
	};
}