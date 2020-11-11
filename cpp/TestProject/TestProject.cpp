#include "pch.h"
#include "CppUnitTest.h"
#include <iostream>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TestProject
{
	class Buffer {
	public:
		Buffer(size_t size) {
			buffer = new char[size];
		}

		~Buffer() {
			if (buffer != nullptr) {
				delete[] buffer;
			}
		}

	public:
		void putByte(char byte) {
			buffer[buffer_offset++] = byte;
			buffer_last_pos++;
		}

		char* readByte(size_t size) {
			if (buffer_offset + size > buffer_last_pos) {
				throw std::invalid_argument("buffer write size over");
			}

			char* data = new char[size];
			size_t dataIndex = 0;
			for (size_t i = buffer_offset; i < buffer_offset + size; i++) {
				data[dataIndex++] = buffer[i];
			}

			buffer_offset += size;

			return data;
		}

	public:
		char* getBuffer() { return buffer; }
		size_t getBufferOfset() { return buffer_offset; }
		void setBufferOffset(size_t value) { buffer_offset = value; }

	private:
		char* buffer = nullptr;
		size_t buffer_offset = 0;
		size_t buffer_last_pos = 0;
	};

	class Serialize {
	public:
		template<typename T>
		void write(Buffer* buffer, const T& value) {
			size_t size = sizeof(value);
			for (size_t i = 0; i < size; i++) {
				char data = value >> i * byte;
				buffer->putByte(data);
			}
		}

		template<>
		void write(Buffer* buffer, const std::string& value) {
			size_t strSize = value.length();
			write(buffer, strSize);
			for (size_t i = 0; i < value.length(); i++) {
				char data = value[i];
				buffer->putByte(data);
			}
		}

		template<typename T>
		T read(Buffer* buffer) {
			size_t size = sizeof(T);
			char* dataBuffer = buffer->readByte(size);
			T data;
			memcpy(&data, dataBuffer, size);

			delete[] dataBuffer;

			return data;
		}

		template<>
		std::string read(Buffer* buffer) {
			size_t strSize = read<size_t>(buffer);
			char* dataBuffer = buffer->readByte(strSize);
			std::string str(&dataBuffer[0], &dataBuffer[0] + strSize);
			return str;
		}
	};

	static const size_t bit = 1;
	static const size_t byte = bit * 8;

	TEST_CLASS(TestProject)
	{
	public:
		
		TEST_METHOD(TestMethod1)
		{
			Assert::AreEqual(1, 1);
		}

		TEST_METHOD(IntToBytes) {
			int a = 10, b = 5;
			size_t totalSize = sizeof(a) * 2;
			char* buffer = new char[totalSize];
			size_t bufferOffset = 0;

			for (size_t i = 0; i < 4; i++) {
				char data = static_cast<char>(a >> i * byte);
				buffer[bufferOffset++] = data;
			}

			for (size_t i = 0; i < 4; i++) {
				char data = static_cast<char>(b >> i * byte);
				buffer[bufferOffset++] = data;
			}

			int c = 0, d = 0;
			bufferOffset = 0;
			memcpy(&c, buffer + bufferOffset, sizeof(c));
			bufferOffset += sizeof(c);
			memcpy(&d, buffer + bufferOffset, sizeof(c));

			Assert::AreEqual(a, c);
			Assert::AreEqual(b, d);
		}

		TEST_METHOD(ShortToBytes) {
			short a = 156, b = 421;
			size_t totalSize = sizeof(a) * 2;
			char* buffer = new char[totalSize];
			size_t bufferOffset = 0;

			for (size_t i = 0; i < sizeof(a); i++) {
				char data = static_cast<char>(a >> i * byte);
				buffer[bufferOffset++] = data;
			}

			for (size_t i = 0; i < sizeof(a); i++) {
				char data = static_cast<char>(b >> i * byte);
				buffer[bufferOffset++] = data;
			}

			short c = 0, d = 0;
			bufferOffset = 0;
			memcpy(&c, buffer + bufferOffset, sizeof(c));
			bufferOffset += sizeof(c);
			memcpy(&d, buffer + bufferOffset, sizeof(c));

			Assert::AreEqual(a, c);
			Assert::AreEqual(b, d);
		}

		TEST_METHOD(StringToBytes) {
			std::string a = "Douner is";
			size_t totalSize = a.length();
			char* buffer = new char[totalSize];
			size_t bufferOffset = 0;

			for (size_t i = 0; i < totalSize; i++) {
				buffer[i] = a[i];
			}
			//memcpy(buffer, a.c_str(), a.length());
			
			bufferOffset = 0;

			std::string c(&buffer[bufferOffset], &buffer[bufferOffset] + totalSize);

			auto result = a == c;

			Assert::IsTrue(a == c);
		}

		TEST_METHOD(BooleanToBytes) {
			bool a = false, b = true;
			size_t totalSize = sizeof(a) * 2;
			char* buffer = new char[totalSize];
			size_t bufferOffset = 0;

			for (size_t i = 0; i < sizeof(a); i++) {
				char data = static_cast<char>(a >> i * byte);
				buffer[bufferOffset++] = data;
			}

			for (size_t i = 0; i < sizeof(a); i++) {
				char data = static_cast<char>(b >> i * byte);
				buffer[bufferOffset++] = data;
			}

			bool c = false, d = false;
			bufferOffset = 0;
			memcpy(&c, buffer + bufferOffset, sizeof(c));
			bufferOffset += sizeof(c);
			memcpy(&d, buffer + bufferOffset, sizeof(c));

			Assert::AreEqual(a, c);
			Assert::AreEqual(b, d);
		}

		TEST_METHOD(StructToBytes) {
			struct Test {
				int a;
				std::string b;

				size_t getSize() {
					return sizeof(a) + b.length();;
				}
			};

			Test testData{ 29, "Doby" };
			size_t size = testData.getSize();

			Serialize serialize;
			Buffer buffer(1024);

			serialize.write(&buffer, size);
			serialize.write(&buffer, testData.a);
			serialize.write(&buffer, testData.b);

			buffer.setBufferOffset(0);

			size_t readSize = serialize.read<size_t>(&buffer);
			int a = serialize.read<int>(&buffer);
			std::string b = serialize.read<std::string>(&buffer);
			int c = serialize.read<int>(&buffer);

			Test testData2{ a, b };

			Assert::AreEqual(testData.a, testData2.a);
			Assert::AreEqual(testData.b, testData2.b);
		}
	};
}
