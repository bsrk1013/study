#include <boost/bind.hpp>
#include "TcpSessionBase.h"
#include "Serialize.h"

DBBD::TcpSessionBase::TcpSessionBase(IoContextSP context, SocketSP socket, unsigned int bufferSize)
{
	this->context = context;
	this->socket = socket;
	this->readBuffer = new Buffer(bufferSize);
	this->writeBuffer = new Buffer(bufferSize);
}

DBBD::TcpSessionBase::~TcpSessionBase()
{
	stop();
}

void DBBD::TcpSessionBase::start()
{
	startInternal();
	read();
}

void DBBD::TcpSessionBase::stop()
{
	if (isDisposed) { return; }
	isDisposed = true;

	if (socket && socket->is_open()) {
		socket->close();
	}

	if (readBuffer != nullptr) {
		delete readBuffer;
		readBuffer = nullptr;
	}

	if (writeBuffer != nullptr) {
		delete writeBuffer;
		writeBuffer = nullptr;
	}

	if (bindStopInternal) {
		bindStopInternal(sessionId);
	}
}

void DBBD::TcpSessionBase::send(DBBD::Cell* data)
{
	Serialize::write(*writeBuffer, data);
	write();
}

void DBBD::TcpSessionBase::bindingStopInternal(std::function<void(const size_t&)> method)
{
	bindStopInternal = method;
}

void DBBD::TcpSessionBase::read()
{
	readBuffer->adjust();
	if (socket && socket->is_open()) {
		socket->async_read_some(boost::asio::buffer(readBuffer->getBuffer(), 8192),
			boost::bind(&TcpSessionBase::handleRead, this,
				boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
	}
}

void DBBD::TcpSessionBase::handleRead(const boost::system::error_code& error, unsigned int bytesTransferred)
{
	if (error) {
		stop();
		return;
	}

	readBuffer->increaseLastPos(bytesTransferred);
	while (true) {
		if (readBuffer->getBufferLastPos() < HeaderSize) {
			break;
		}

		auto headerBlock = readBuffer->viewByteBlock(HeaderSize);
		Header header(headerBlock);

		if (readBuffer->getBufferLastPos() < header.length) {
			break;
		}

		readInternal(header);
	}

	read();
}

void DBBD::TcpSessionBase::write()
{
	writeInternal();

	if (socket && socket->is_open()) {
		boost::asio::async_write(*socket,
			boost::asio::buffer(writeBuffer->getBuffer(), writeBuffer->getBufferLastPos()),
			boost::bind(&TcpSessionBase::handleWrite, this,
				boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
	}
}

void DBBD::TcpSessionBase::handleWrite(const boost::system::error_code& error, unsigned int bytesTransferred)
{
	if (error) {
		stop();
		return;
	}
	
	writeBuffer->setBufferOffset(writeBuffer->getBufferOffset() + bytesTransferred);
	writeBuffer->adjust();
}