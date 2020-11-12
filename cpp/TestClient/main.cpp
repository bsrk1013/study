#include <cstdlib>
#include <cstring>
#include <iostream>
#include <boost/asio.hpp>
#include "DBBDBuffer.h"
#include "DBBDSerialize.h"

using boost::asio::ip::tcp;

static const int maxBufferSize = 8192;

int main()
{
    try
    {
        boost::asio::io_context io_context;

        tcp::socket s(io_context);
        tcp::resolver resolver(io_context);
        boost::asio::connect(s, resolver.resolve("127.0.0.1", "8100"));

		DBBD::Buffer buffer(8192);

        for (;;)
        {
            std::cout << "Enter message: ";
            char request[maxBufferSize];
            std::cin.getline(request, maxBufferSize);

			/*DBBD::Serialize serialize(&buffer);

			serialize.write(request);*/

            size_t request_length = std::strlen(request);
            boost::asio::write(s, boost::asio::buffer(request, request_length));

            //char reply[max_length];
            //size_t reply_length = boost::asio::read(s,
            //    boost::asio::buffer(reply, request_length));
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