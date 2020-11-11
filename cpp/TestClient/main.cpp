#include <cstdlib>
#include <cstring>
#include <iostream>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

enum { max_length = 1024 };

static const size_t Bit = 1;
static const size_t Byte = 8 * Bit;

template<typename T>
void serialize(char* buffer, const T& value, size_t size) {
    char data[size];
    for (size_t i = 0; i < size; i++) {
        data[i] = 
    }
}

int main()
{
    int age1 = 10;
    short kg1 = 62;
    size_t ageSize = sizeof(age1);
    size_t kgSize = sizeof(kg1);
    size_t totalSize = ageSize + kgSize;
    char* buffer = new char[totalSize];

    for (size_t i = 0; i < ageSize; i++) {
        unsigned char temp = (age1 >> (i * Byte));
        std::cout << "[" << i << "] : " << temp << std::endl;
        buffer[i] = temp;
    }

    std::cout << buffer << std::endl;

    int age2 = 0;
    memcpy(&age2, buffer, ageSize);

    std::cout << age2 << std::endl;

    /*buffer = (char*)memcpy(buffer, &age1, ageSize);
    buffer = (char*)memcpy(buffer, &name1, nameSize);

    for (int i = 0; i < totalSize; i++) {
        char a = buffer[i];
        std::cout << "[" << i << "] = " << a << std::endl;
    }

    int age2 = 0;
    std::string name2 = "";
    
    memcpy(&age2, buffer, ageSize);
    memcpy(&name2, buffer, nameSize);

    std::cout << "age : " << age2 << ", name : " << name2 << std::endl;*/

    return 0;

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