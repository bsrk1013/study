#include <iostream>
#include <boost/bind.hpp>
#include "TcpServerBase.h"
#include "Log.h"

DBBD::TcpServerBase::TcpServerBase(const std::string& name,
	const std::string& address,
	const short& port,
	const size_t& threadCount)
{
	this->name = name;
	this->address = address;
	this->port = port;
	this->threadCount = threadCount;
}

DBBD::TcpServerBase::~TcpServerBase()
{
	name = "";
	address = "";
	port = 0;
	threadCount = 0;
}

void DBBD::TcpServerBase::start()
{
	if (address == "") { throw std::exception(std::string("invalid address, address: " + address).c_str()); }
	if (port <= 0) { throw std::exception(std::string("invalid port, port: " + port).c_str()); }
	if (threadCount <= 0) { throw std::exception(std::string("invalid thread count, threadCount: " + threadCount).c_str()); }

	context = NEW_CONTEXT_SP();
	acceptor = NEW_TCP_ACCEPTOR_SP(*context, address, port);
	acceptor->set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));

	accept();

	for (size_t i = 0; i < threadCount; i++)
	{
		auto thread = NEW_THREAD_SP([&]() {
			context->run();
			});

		threadList.push_back(thread);
	}

	LOG_INFO("{} Server started...", name);
}

void DBBD::TcpServerBase::stop()
{
	if (isDisposed) { return; }

	stopInternal();

	if (acceptor && acceptor->is_open()) {
		acceptor->cancel();
	}

	if (context && context->stopped() == false) {
		context->stop();
	}

	for (auto thread : threadList) {
		thread->join();
		thread.reset();
	}

	isDisposed = true;
}

void DBBD::TcpServerBase::accept()
{
	SocketSP socket = NEW_SOCKET_SP(*context);
	if (socket) {
		acceptor->async_accept(*socket,
			boost::bind(&TcpServerBase::handleAccept, this, socket, boost::asio::placeholders::error));
	}
}

void DBBD::TcpServerBase::handleAccept(SocketSP socket, const boost::system::error_code& error)
{
	if(!error) {
		acceptInternal(socket, increaseAndGetSessionId());
	}

	accept();
}