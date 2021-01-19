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
		virtual void bindingStopInternal(std::function<void(const size_t&)>) = 0;

		virtual void send(DBBD::Cell* data) = 0;

	public:
		inline size_t getSessionId() { return sessionId; }
		inline void setSessionId(const size_t& value) { sessionId = value; }

	protected:
		virtual void startInternal() = 0;

		virtual void read() = 0;
		virtual void handleRead(const boost::system::error_code&, size_t) = 0;
		virtual void readInternal(DBBD::Header) = 0;

		virtual void write() = 0;
		virtual void handleWrite(const boost::system::error_code&, size_t) = 0;
		virtual void writeInternal() = 0;

	protected:
		bool isDisposed = false;
		size_t sessionId;
		Buffer* readBuffer;
		Buffer* writeBuffer;
		std::function<void(const size_t&)> bindStopInternal;
	};
}