#pragma once
#include <iostream>
#include "boost/asio.hpp"

namespace DBBD {
// MACRO
#define TIMER_BINDING(method) std::bind(method, this, std::placeholders::_1)

// typedef
	typedef std::shared_ptr<boost::asio::io_context> IoContextSP;
	typedef std::shared_ptr<boost::asio::ip::tcp::socket> SocketSP;
	typedef std::function<void(const boost::system::error_code&)> TimerParam;
}