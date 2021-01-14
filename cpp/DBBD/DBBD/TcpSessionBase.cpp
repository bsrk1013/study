#include <boost/bind.hpp>
#include "TcpSessionBase.h"

DBBD::TcpSessionBase::TcpSessionBase(IoContextSP context, SocketSP socket)
{
	this->context = context;
	this->socket = socket;
}

DBBD::TcpSessionBase::~TcpSessionBase()
{
	stop();
}

void DBBD::TcpSessionBase::start()
{
	read();
}

void DBBD::TcpSessionBase::stop()
{
	if (context) {
		context.reset();
	}

	if (socket) {
		socket->close();
		socket.reset();
	}
}

void DBBD::TcpSessionBase::read()
{
	readBuffer.adjust();
	if (socket && socket->is_open()) {
		socket->async_read_some(boost::asio::buffer(readBuffer.getBuffer(), 8192),
			boost::bind(&TcpSessionBase::handleRead, this,
				boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
	}
}

void DBBD::TcpSessionBase::handleRead(const boost::system::error_code& error, size_t bytesTransferred)
{
	if (error) {
		stop();
		return;
	}

	readBuffer.increaseLastPos(bytesTransferred);
	while (true) {
		if (readBuffer.getBufferLastPos() < HeaderSize) {
			break;
		}

		auto headerBlock = readBuffer.viewByteBlock(HeaderSize);
		Header header(headerBlock);

		if (readBuffer.getBufferLastPos() < header.length) {
			break;
		}

		//readInternal();
	}

	read();
}

void DBBD::TcpSessionBase::write()
{
	//writeInternal();

	if (socket && socket->is_open()) {
		boost::asio::async_write(*socket,
			boost::asio::buffer(writeBuffer.getBuffer(), writeBuffer.getBufferLastPos()),
			boost::bind(&TcpSessionBase::handleWrite, this,
				boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
	}
}

void DBBD::TcpSessionBase::handleWrite(const boost::system::error_code& error, size_t bytesTransferred)
{
	if (error) {
		stop();
		return;
	}
	
	writeBuffer.clearBuffer();
}