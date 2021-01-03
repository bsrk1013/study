#pragma once
#include <iostream>
#include "Request.h"
#include <boost/asio.hpp>

namespace DBBD {
// MACRO
#define TIMER_BINDING(method) std::bind(method, this)

#define READ_INTERNAL_BINDING(method) std::bind(method, this, std::placeholders::_1, std::placeholders::_2)

// typedef
	// shared_ptr
	typedef std::shared_ptr<boost::asio::io_context> IoContextSP;
	typedef std::shared_ptr<boost::asio::ip::tcp::socket> SocketSP;
	typedef std::shared_ptr<boost::asio::deadline_timer> TimerSP;
	//std::shared_ptr<TimerObject> basePtr;

	// std::function
	typedef std::function<void()> TimerParam;
	typedef std::function<bool(const Header&, Buffer&)> ReadInternalParam;
}