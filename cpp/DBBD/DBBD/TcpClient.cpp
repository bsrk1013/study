#include <boost/bind.hpp>
#include <thread>
#include "TcpClient.h"
#include "iostream"

namespace DBBD {
	TcpClient::TcpClient(const std::string& address, const short& port) {
		context = std::make_unique<io_context>();

		// 家南 积己
		socket = std::make_shared<ip::tcp::socket>(*context);

		ip::tcp::endpoint endpoint(ip::address_v4::from_string(address), 8100);
		socket->async_connect(endpoint,
			boost::bind(&TcpClient::handleConnect, this, placeholders::error));

		t1 = new std::thread([&]() {
			context->run();
			});

		//ip::tcp::resolver resolver(*context);
		/*ip::tcp::resolver::query query(address, port);
		ip::tcp::resolver::iterator endpointIter = resolver.resolve(query);*/


		/*socket->async_connect(endpointIter, 
			boost::bind(&TcpClient::handleConnect, this, placeholders::error));*/

		/*connect(*socket, resolver.resolve(address, port));

		this->session = TcpSession::create(socket);;
		this->session->start();*/
	}

	TcpClient::~TcpClient() {
		if (socket) {
			socket->close();
		}

		if (context) {
			context->stop();
		}

		if (t1 != nullptr) {
			t1->join();
			delete t1;
		}
	}

	void TcpClient::send(const std::string& data) {
		session->write(data);
	}

	void TcpClient::handleConnect(const error_code& error) {
		if (!error) {
			this->session = TcpSession::create(socket);;
			this->session->start();
			std::cout << "session connected..." << std::endl;
		}
	}
	/*void TcpClient::send(const T& data) {
		session->write(data);
	}*/
}