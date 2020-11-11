#include "pch.h"
#include "CppUnitTest.h"
#include <iostream>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TestProject
{
	static const size_t bit = 1;
	static const size_t byte = bit * 8;

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
		Serialize(Buffer* buffer) {
			this->buffer = buffer;
		}

		template<typename T>
		void write(const T& value) {
			size_t size = sizeof(T);
			for (size_t i = 0; i < size; i++) {
				char data = value >> i * byte;
				buffer->putByte(data);
			}
		}

		void write(const std::string& value) {
			size_t strSize = value.length();
			write(strSize);
			for (size_t i = 0; i < value.length(); i++) {
				char data = value[i];
				buffer->putByte(data);
			}
		}

	private:
		Buffer* buffer;
	};

	class Deserialize {
	public:
		Deserialize(Buffer* buffer) {
			this->buffer = buffer;
		}

	public:

		template<typename T>
		T read() {
			size_t size = sizeof(T);
			char* dataBuffer = buffer->readByte(size);
			T data;
			memcpy(&data, dataBuffer, size);

			delete[] dataBuffer;

			return data;
		}

		template<>
		std::string read() {
			size_t strSize = read<size_t>();
			char* dataBuffer = buffer->readByte(strSize);
			std::string str(&dataBuffer[0], &dataBuffer[0] + strSize);
			return str;
		}

	private:
		Buffer* buffer;
	};

	class Cell {
	public:
		virtual ~Cell();

	public:
		virtual void serialize(Serialize* serialize) = 0;
		virtual void deserialize(Deserialize* deserialize) = 0;

	protected:
		virtual size_t getSize() = 0;
	};

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
			class Test : Cell{
			public:
				Test() {};
				Test(const int a, const std::string& b) {
					this->a = a;
					this->b = b;
				}
				Test(const int a, const char* b) 
				: a(a), b(b){
				}

				virtual ~Test() {};

			public:
				virtual void serialize(Serialize* serialize) {
					serialize->write(getSize());
					serialize->write(a);
					serialize->write(b);
				}

				virtual void deserialize(Deserialize* deserialize) {
					size_t size = deserialize->read<size_t>();
					a = deserialize->read<int>();
					b = deserialize->read<std::string>();
				}

			protected:
				virtual size_t getSize() {
					return sizeof(a) + b.length();
				}

			private:
				int a;
				std::string b;
			};

			Test testData(29, "Doby");
			Buffer buffer(1024);
			
			Serialize serialize(&buffer);
			testData.serialize(&serialize);
			buffer.setBufferOffset(0);
			
			Deserialize deserialize(&buffer);
			Test testData2;
			testData2.deserialize(&deserialize);
			//Assert::AreEqual(testData.a, testData2.a);
			//Assert::AreEqual(testData.b, testData2.b);
		}
	};
}
