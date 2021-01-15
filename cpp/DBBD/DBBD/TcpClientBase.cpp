#include "TcpClientBase.h"
#include <boost/bind.hpp>
#include "TcpClientSession.h"

DBBD::TcpClientBase::TcpClientBase(const std::string& address, const short& port)
{
	this->address = address;
	this->port = port;
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

	if (socket && socket->is_open()) {
		socket->close();
	}

	if (context) {
		context->stop();
	}

	if (thread) {
		thread->join();
		delete thread;
		thread = nullptr;
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

	thread = new std::thread([&]() {
		context->run();
		});
}

void DBBD::TcpClientBase::handleConnect(const boost::system::error_code& error)
{
	if (error) {
		stop();
		return;
	}

	session = std::make_shared<TcpClientSession>(context, socket, 8192,
		boost::bind(&TcpClientBase::readInternal, this, boost::placeholders::_1));
	session->start();
}
