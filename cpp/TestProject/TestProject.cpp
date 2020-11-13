#include "pch.h"
#include "CppUnitTest.h"
#include "../TestClient/DBBDBuffer.h"
#include "../TestClient/DBBDCell.h"
#include "../TestClient/DBBDSerialize.h"
#include "../TestClient/DBBDDeserialize.h"
#include <iostream>
#include <vector>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TestProject
{
	TEST_CLASS(TestProject)
	{
	public:
		TEST_METHOD(IntArrayToBytes) {
			int a[] = {1, 2, 3};
			DBBD::Buffer buffer(8192);
			DBBD::Serialize serialze(&buffer);

			serialze.writeArray(a);

			buffer.setBufferOffset(0);

			DBBD::Deserialize deserialize(&buffer);

			int* b = nullptr;
			deserialize.readArray<int>(b);

			for (size_t i = 0; i < 3; i++) {
				int data1 = a[i];
				int data2 = b[i];

				Assert::AreEqual(data1, data2);
			}
		}

		TEST_METHOD(VectorToBytes) {
			std::vector<int> a;
			a.push_back(10);
			a.push_back(20);
			DBBD::Buffer buffer(8192);
			DBBD::Serialize serialize(&buffer);

			serialize.writeVector<std::vector<int>, int>(a);

			buffer.setBufferOffset(0);

			DBBD::Deserialize deserialize(&buffer);

			std::vector<int> b;
			deserialize.readVector<std::vector<int>, int>(b);

			Assert::AreEqual(a[0], b[0]);
			Assert::AreEqual(a[1], b[1]);
		}

		TEST_METHOD(StructToBytes) {
			class Test : DBBD::Cell{
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
				virtual void serialize(DBBD::Serialize* serialize) {
					serialize->write(a);
					serialize->write(b);
				}

				virtual void deserialize(DBBD::Deserialize* deserialize) {
					deserialize->read(a);
					deserialize->read(b);
				}

			protected:
				virtual size_t getSize() {
					return sizeof(a) + b.length();
				}

			public:
				int a;
				std::string b;
			};

			Test testData(29, "Doby");
			DBBD::Buffer sendBuffer(1024);
			
			DBBD::Serialize serialize(&sendBuffer);
			serialize.write(testData);
			//testData.serialize(&serialize);

			char* temp = sendBuffer.getBuffer();

			DBBD::Buffer readBuffer(sendBuffer.getBuffer());

			sendBuffer.setBufferOffset(0);
			DBBD::Deserialize deserialize(&sendBuffer);
			Test testData2;
			testData2.deserialize(&deserialize);
			Assert::AreEqual(testData.a, testData2.a);
			Assert::AreEqual(testData.b, testData2.b);
		}
	};
}
