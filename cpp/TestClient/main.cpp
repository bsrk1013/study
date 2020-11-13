#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include "DBBDBuffer.h"
#include "DBBDSerialize.h"
#include "DBBDDeserialize.h"

using boost::asio::ip::tcp;

static const int maxBufferSize = 8192;

void handleRead(const boost::system::error_code& error, size_t bytesTransfrred) {

}

int main()
{
    try
    {
        boost::asio::io_context io_context;

        tcp::socket s(io_context);
        tcp::resolver resolver(io_context);
        boost::asio::connect(s, resolver.resolve("127.0.0.1", "8100"));

		DBBD::Buffer sendBuffer(maxBufferSize);
        DBBD::Buffer receiveBuffer(maxBufferSize);

        for (;;)
        {
            std::cout << "Enter message: ";
            std::string request;
            std::getline(std::cin, request);

            //DBBD::Serialize serialize(&sendBuffer);
            //serialize.write(request);

            //boost::asio::write(s, boost::asio::buffer(
            //    sendBuffer.getBuffer(),
            //    sendBuffer.getBufferLastPos()));

            //sendBuffer.clearBuffer();

            //s.async_read_some(boost::asio::buffer(receiveBuffer.getBuffer(), 8192),
            //    boost::bind(&receiveBuffer, boost::shared_from_this(),
            //        boost::));

            //size_t reply_length = boost::asio::read(s,
            //    boost::asio::buffer(receiveBuffer.getBuffer(), 8192));

            //receiveBuffer.setBufferLastPos(reply_length);

            //DBBD::Deserialize deserialize(&receiveBuffer);

            //std::string response;
            //deserialize.read(response);
            //std::cout << response << std::endl;

            //receiveBuffer.clearBuffer();

            //std::cout << "Reply is: ";
            //std::cout.write(reply, reply_length);
            //std::cout << "\n";
        }
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}