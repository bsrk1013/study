#include <boost/bind.hpp>
#include <thread>
#include <iostream>
#include "TcpClient.h"
#include "Cell.h"

namespace DBBD {
	TcpClient::TcpClient(const std::string& address, const short& port) {
		context = std::make_unique<io_context>();

		// 家南 积己
		socket = NEW_SOCKET_SP(*context);

		ip::tcp::endpoint endpoint(ip::address_v4::from_string(address), 8100);
		socket->async_connect(endpoint,
			boost::bind(&TcpClient::handleConnect, this, placeholders::error));

		threads.create_thread(boost::bind(&io_context::run, &(*context)));
	}

	TcpClient::~TcpClient() {
		close();
	}

	void TcpClient::close() {
		if (socket) {
			socket->close();
			socket.reset();
		}

		if (context) {
			context->stop();
			//context.reset();
		}

		threads.join_all();
	}

	void TcpClient::send(Cell* data) {
		session->write(data);
	}

	void TcpClient::handleConnect(const error_code& error) {
		if (!error) {
			this->session = TcpSession::create(socket);;
			connectInternal(session);
			this->session->start();
		}
	}
}