#include "pch.h"
#include "CppUnitTest.h"
#include <iostream>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TestProject
{
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
	};
}
