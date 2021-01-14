#pragma once
#include "SessionBase.h"

namespace DBBD {
	class TcpSessionBase : public SessionBase
	{
	public:
		TcpSessionBase(IoContextSP , SocketSP, size_t);
		virtual ~TcpSessionBase();

	public:
		virtual void start() override;
		virtual void stop() override;

	protected:
		// SessionBase을(를) 통해 상속됨
		virtual void read() override;
		virtual void handleRead(const boost::system::error_code&, size_t) override;
		virtual void readInternal() = 0;
		virtual void write() override;
		virtual void handleWrite(const boost::system::error_code&, size_t) override;
		virtual void writeInternal()  = 0;

	protected:
		IoContextSP context;
		SocketSP socket;
	};
}