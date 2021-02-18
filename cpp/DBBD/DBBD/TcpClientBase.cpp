#include "TcpClientBase.h"
#include <boost/bind.hpp>
#include "TcpClientSession.h"
#include "Log.h"

DBBD::TcpClientBase::TcpClientBase(const std::string& address, const short& port, const bool& tryReconnect)
{
	this->address = address;
	this->port = port;
	this->tryReconnect = tryReconnect;
}

DBBD::TcpClientBase::~TcpClientBase()
{
	stop();
}

void DBBD::TcpClientBase::start()
{
	context = NEW_CONTEXT_SP();
	socket = NEW_SOCKET_SP(*context);

	connect();
}

void DBBD::TcpClientBase::stop()
{
	if (isDisposed) { return; }

	stopInternal();

	if (session) {
		session->stop();
	}

	if (socket && socket->is_open()) {
		socket->close();
	}

	if (context) {
		context->stop();
	}

	if (thread) {
		thread->join();
		thread.reset();
	}

	isDisposed = true;
}

void DBBD::TcpClientBase::send(DBBD::Cell* data)
{
	session->send(data);
}

void DBBD::TcpClientBase::connect()
{
	boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::address_v4::from_string(address), port);
	socket->async_connect(endpoint,
		boost::bind(&TcpClientBase::handleConnect, this, boost::asio::placeholders::error));

	if (!thread) {
		thread = NEW_THREAD_SP([&]() {
			context->run();
			});
	}
}

void DBBD::TcpClientBase::handleConnect(const boost::system::error_code& error)
{
	if (error) {
		LOG_ERROR("{}:{} connect failed...", address, port);
		if (tryReconnect) {
			reconnect();
		}
		else {
			stop();
		}
		return;
	}

	session = std::make_shared<TcpClientSession>(context, socket, 8192,
		boost::bind(&TcpClientBase::readInternal, this, boost::placeholders::_1));
	session->bindingStopInternal(boost::bind(
	&TcpClientBase::disconnectInternal, this));
	session->start();
}

void DBBD::TcpClientBase::disconnectInternal()
{
	if (tryReconnect) {
		reconnect();
		return;
	}
}

void DBBD::TcpClientBase::reconnect()
{
	std::this_thread::sleep_for(std::chrono::seconds(1));
	connect();
}