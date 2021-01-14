#pragma once
#include <boost/asio.hpp>
#include "Define.h"
#include "Buffer.h"

namespace DBBD
{
	class SessionBase
	{
	public:
		virtual ~SessionBase() {}

	public:
		virtual void start() = 0;
		virtual void stop() = 0;

	protected:
		virtual void read() = 0;
		virtual void handleRead(const boost::system::error_code&, size_t) = 0;
		virtual void readInternal(DBBD::Header) = 0;

		virtual void write() = 0;
		virtual void handleWrite(const boost::system::error_code&, size_t) = 0;
		virtual void writeInternal() = 0;

	protected:
		bool isDisposed = false;
		Buffer* readBuffer;
		Buffer* writeBuffer;
	};
}