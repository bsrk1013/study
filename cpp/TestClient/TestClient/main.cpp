#include <iostream>
#include <thread>
#include "DBBD\TcpClient.h"

int main() {
	try {
		//DBBD::TcpClient* client = nullptr;
		DBBD::TcpClient client("127.0.0.1", 8100);

		//std::thread t1([](DBBD::TcpClient* lambdaClient) {
		//	lambdaClient = new DBBD::TcpClient("127.0.0.1", 8100);
		//	}, client);

		while (true) {
			std::string a;
			std::getline(std::cin, a);

			if (a == "exit") {
				break;
			}

			//client->send(a);
			client.send(a);
		}

		//t1.join();
	}
	catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
	}

	return 0;
}