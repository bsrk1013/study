#pragma once
#include <boost/asio.hpp>
#include <thread>

namespace DBBD
{
	class ClientBase
	{
	public:
		virtual ~ClientBase() {}

	protected:
		bool isDisposed = false;
		std::thread* thread = nullptr;
		//std::shared_ptr
	};
}