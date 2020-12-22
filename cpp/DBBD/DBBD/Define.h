#pragma once
#include <iostream>
#include "DBBD/Request.h"
#include <boost/asio.hpp>

namespace DBBD {
// MACRO
#define TIMER_BINDING(method) std::bind(method, this, std::placeholders::_1)
#define READ_INTERNAL_BINDING(method) std::bind(method, this, std::placeholders::_1, std::placeholders::_2)

// typedef
	typedef std::shared_ptr<boost::asio::io_context> IoContextSP;
	typedef std::shared_ptr<boost::asio::ip::tcp::socket> SocketSP;
	typedef std::function<void(const boost::system::error_code&)> TimerParam;
	typedef std::function<bool(const Header&, Buffer&)> ReadInternalParam;
}