#pragma once
#include "SessionBase.h"

namespace DBBD {
	class TcpSessionBase : public SessionBase
	{
	public:
		TcpSessionBase(IoContextSP , SocketSP, unsigned int);
		virtual ~TcpSessionBase();

	public:
		virtual void start() override;
		virtual void stop() override;
		virtual void send(DBBD::Cell* data) override;
		virtual void bindingStopInternal(std::function<void(const size_t&)>) override;

	private:
		// SessionBase��(��) ���� ��ӵ�
		virtual void startInternal() = 0;

		virtual void read() override;
		virtual void handleRead(const boost::system::error_code&, unsigned int) override;
		virtual void readInternal(DBBD::Header) = 0;
		virtual void write() override;
		virtual void handleWrite(const boost::system::error_code&, unsigned int) override;
		virtual void writeInternal()  = 0;

	protected:
		IoContextSP context;
		SocketSP socket;
	};
}