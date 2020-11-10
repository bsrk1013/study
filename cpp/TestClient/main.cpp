#include <cstdlib>
#include <cstring>
#include <iostream>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

enum { max_length = 1024 };

int main()
{
    try
    {
        boost::asio::io_context io_context;

        tcp::socket s(io_context);
        tcp::resolver resolver(io_context);
        boost::asio::connect(s, resolver.resolve("127.0.0.1", "8100"));

        static const size_t maxBufferSize = 8192;
        for (;;)
        {
            std::cout << "Enter message: ";
            char request[max_length];
            std::cin.getline(request, max_length);
            if (strcmp(request, "quit") == 0)
            {
                break;
            }


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