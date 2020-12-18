#include <boost/bind.hpp>
#include <thread>
#include <iostream>
#include "TcpClient.h"
#include "Cell.h"

namespace DBBD {
	TcpClient::TcpClient(const std::string& address, const short& port) {
		context = std::make_unique<io_context>();

		// 家南 积己
		socket = std::make_shared<ip::tcp::socket>(*context);

		ip::tcp::endpoint endpoint(ip::address_v4::from_string(address), 8100);
		socket->async_connect(endpoint,
			boost::bind(&TcpClient::handleConnect, this, placeholders::error));

		mainThread = new std::thread([&]() {
			context->run();
			});
	}

	TcpClient::~TcpClient() {
		if (socket) {
			socket->close();
		}

		if (context) {
			context->stop();
		}

		if (mainThread) {
			mainThread->join();
			delete mainThread;
		}
	}

	void TcpClient::send(Cell* data) {
		session->write(data);
	}

	void TcpClient::handleConnect(const error_code& error) {
		if (!error) {
			this->session = TcpSession::create(socket);;
			this->session->start();
			std::cout << "session connected..." << std::endl;
		}
	}
}