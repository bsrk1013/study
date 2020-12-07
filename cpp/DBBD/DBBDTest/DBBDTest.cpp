#include "pch.h"
#include "CppUnitTest.h"
#include "../DBBD/Buffer.h"
#include "../DBBD/Cell.h"
#include "../DBBD/Serialize.h"
#include "../DBBD/Deserialize.h"
#include "../DBBD/Request.h"
#include "../DBBD/Random.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace DBBD;

namespace DBBDTest
{
	class A {
	public:
		virtual ~A() {}
	public:
		virtual std::string getType() = 0;
	};

	class B : public A {
	public:
		B() {}
		virtual ~B(){}
	public:
		virtual std::string getType() {
			return std::string("B");
		}
	};

	template<typename T>
	std::string foo(T& data) {
		bool result = std::is_base_of<A, T>::value;
		if (result) {
			return data.getType();
		}
		return "NULL";
	}

	std::string foo2(A* data) {
		return data->getType();
	}

	TEST_CLASS(DBBDTest)
	{
	public:
		TEST_METHOD(ClassTest) {
			B b;

			std::string result = foo(b);
			Assert::AreEqual(result, std::string("B"));

			std::string result2 = foo2(dynamic_cast<A*>(&b));
			Assert::AreEqual(result2, std::string("B"));
		}

		TEST_METHOD(BufferTest) {
			class Equip : public Cell {
			public:
				Equip() {}
				Equip(std::string name, short level):name(name), level(level) {}
				virtual ~Equip() {}

			public:
				virtual void serialize(Buffer* buffer) {
					Serialize::write(buffer, name);
					Serialize::write(buffer, level);
				}

				virtual void deserialize(Buffer* buffer) {
					Deserialize::read(buffer, name);
					Deserialize::read(buffer, level);
				}

				virtual size_t getLength() {
					return 0;
				}

			public:
				std::string name;
				short level;
			};

			class User : public Cell {
			public:
				User() {}
				User(std::string name, short age, Equip equip) 
					:name(name), age(age), equip(equip) {}
				virtual ~User() {}

			private:
				virtual void serialize(Buffer* buffer) {
					Serialize::write(buffer, name);
					Serialize::write(buffer, age);
					Serialize::write(buffer, dynamic_cast<Cell*>(&equip));
				}

				virtual void deserialize(Buffer* buffer) {
					Deserialize::read(buffer, name);
					Deserialize::read(buffer, age);
					Deserialize::read(buffer, dynamic_cast<Cell*>(&equip));
				}
				
				virtual size_t getLength() {
					return 0;
				}

			public:
				std::string name;
				short age;
				Equip equip;
			};

			User person("doby", 29, Equip("Sword", 15));

			Buffer buffer(8192);

			Serialize::write(&buffer, dynamic_cast<Cell*>(&person));

			buffer.setBufferOffset(0);

			User person2;

			Deserialize::read(&buffer, dynamic_cast<Cell*>(&person2));

			Assert::AreEqual(person.name, person2.name);
			Assert::AreEqual(person.age, person2.age);
			Assert::AreEqual(person.equip.name, person2.equip.name);
			Assert::AreEqual(person.equip.level, person2.equip.level);
		}

		TEST_METHOD(RequestTest) {
			class User : public Cell {
			public:
				User() {}
				User(std::string nickname, short level) :nickname(nickname), level(level) {}
				virtual ~User() {}
			
			public:
				virtual void serialize(Buffer* buffer) {
					Serialize::write(buffer, nickname);
					Serialize::write(buffer, level);
				}

				virtual void deserialize(Buffer* buffer) {
					Deserialize::read(buffer, nickname);
					Deserialize::read(buffer, level);
				}

				virtual size_t getLength() {
					return sizeof(size_t) + nickname.length() + sizeof(level);
				}

			public:
				std::string nickname;
				short level;
			};

			class LoginReq : public Request {
			public:
				LoginReq() {}
				virtual ~LoginReq() {}
			
			public:
				virtual void serialize(Buffer* buffer) {
					Serialize::write(buffer, getLength());
					Serialize::write(buffer, typeId);
					Serialize::write(buffer, dynamic_cast<Cell*>(&user));
				}

				virtual void deserialize(Buffer* buffer) {
					size_t length = 0;
					Deserialize::read(buffer, length);
					Deserialize::read(buffer, typeId);
					Deserialize::read(buffer, dynamic_cast<Cell*>(&user));
				}

				virtual size_t getLength() {
					return sizeof(size_t) + sizeof(typeId) + user.getLength();
				}

			public:
				size_t getTypeId() { return typeId; }
				User getUser() { return user; }

			protected:
				size_t typeId = 1;

			public:
				void setUser(User value) {
					user = value;
				}

			private:
				User user;
			};

			LoginReq req;
			User user{"doby", 99};

			req.setUser(user);

			Buffer sendBuffer(8192);
			Buffer receiveBuffer(8192);

			size_t length = req.getLength();
			Serialize::write(&sendBuffer, dynamic_cast<Cell*>(&req));

			sendBuffer.setBufferOffset(0);
			LoginReq loginReq;
			while (true) {
				size_t currentOffset = sendBuffer.getBufferOffset();
				int transfrred = Random::instance().next(1, 10);

				size_t lastPos = sendBuffer.getBufferLastPos();
				if (transfrred + currentOffset > lastPos) {
					transfrred = transfrred + currentOffset - lastPos;
				}

				char* block1 = sendBuffer.readByteBlock(static_cast<size_t>(transfrred));
				for (size_t i = 0; i < transfrred; i++) {
					receiveBuffer.putByte(block1[i]);
				}

				if (sizeof(size_t) > receiveBuffer.getBufferLastPos()) {
					continue;
				}

				char* lengthBlock = receiveBuffer.readByteBlock(sizeof(size_t), false);
				size_t length = 0;
				memcpy(&length, lengthBlock, sizeof(size_t));
				if (receiveBuffer.getBufferLastPos() < length) {
					continue;
				}

				/*char* typeIdBlock = receiveBuffer.readByteBlock(sizeof(size_t), false);
				size_t typeId = 0;
				memcpy(&typeId, typeIdBlock, sizeof(size_t));*/

				/*if (typeId == 1) {
					Deserialize::read(&receiveBuffer, dynamic_cast<Cell*>(&loginReq));
					receiveBuffer.clearBuffer();
					break;
				}*/
			}

			Assert::AreEqual(req.getTypeId(), loginReq.getTypeId());
			Assert::AreEqual(req.getUser().nickname, loginReq.getUser().nickname);
			Assert::AreEqual(req.getUser().level, loginReq.getUser().level);
		}

		TEST_METHOD(RandomTest) {
			for (size_t i = 0; i < 1000; i++) {
				int irandom = Random::instance().next(0, 10);
				Assert::IsTrue(0 <= irandom && irandom <= 10);
				Assert::IsTrue(!(irandom < 0) || !(irandom > 10));
			}

			for (size_t i = 0; i < 1000; i++) {
				float frandom = Random::instance().next(0.0f, 10.0f);
				Assert::IsTrue(0.0f <= frandom && frandom <= 10.0f);
				Assert::IsTrue(!(frandom < 0.0f) || !(frandom > 10.0f));
			}
		}
	};
}
