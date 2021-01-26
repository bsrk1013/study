#pragma once
#include <iostream>
#include "Request.h"
#include <boost/asio.hpp>

namespace DBBD {
// MACRO
	using TimerParam = std::function<void()>;
#define TIMER_BINDING(method) std::bind(method, this)
	using ReadInternalParam = std::function<bool(const Header&, Buffer&)>;
#define READ_INTERNAL_BINDING(method) std::bind(method, this, std::placeholders::_1, std::placeholders::_2)

// typedef
	// shared_ptr
	using ThreadSP = std::shared_ptr<std::thread>;
#define NEW_THREAD_SP(method) std::make_shared<std::thread>(method)
	using IoContextSP = std::shared_ptr<boost::asio::io_context>;
#define NEW_CONTEXT_SP(concurrency_hint) std::make_shared<boost::asio::io_context>(concurrency_hint)
	using TcpAcceptorSP = std::shared_ptr<boost::asio::ip::tcp::acceptor>;
#define NEW_TCP_ACCEPTOR_SP(context, address, port) std::make_shared<boost::asio::ip::tcp::acceptor>(context, boost::asio::ip::tcp::endpoint(boost::asio::ip::address_v4::from_string(address), port))
	using SocketSP = std::shared_ptr<boost::asio::ip::tcp::socket>;
#define NEW_SOCKET_SP(context) std::make_shared<boost::asio::ip::tcp::socket>(context)
	/*using TimerSP = std::shared_ptr<boost::asio::deadline_timer>;
#define NEW_TIMER_SP(context, waitTime) std::make_shared<boost::asio::deadline_timer>(context, waitTime)*/

	// short naming
	using StringVector = std::vector<std::string>;
}