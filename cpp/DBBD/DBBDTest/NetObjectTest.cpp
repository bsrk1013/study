#include "pch.h"
#include "CppUnitTest.h"
#include "DBBD/Request.h"
#include "DBBD/Response.h"
#include "DBBD/Cell.h"
#include "DBBD/Random.h"
#include <map>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace DBBD;

namespace DBBDTest
{
	namespace ProtocolType {
		enum Value {
			PingCheckReq = 1001,
			PingCheckResp = 1002,
		};

		std::map<Value, std::string> stringMap = {
			{ PingCheckReq, "PingCheckReq" },
			{ PingCheckResp, "PingCheckResp" },
		};

		std::string Get(Value value) {
			auto iter = ProtocolType::stringMap.find(value);
			if (iter == ProtocolType::stringMap.end()) {
				return "";
			}
			return iter->second;
		}
	}

	TEST_CLASS(NetObjectTest)
	{
	public:
		TEST_METHOD(ProtocolTypeTest) {
			ProtocolType::Value type = ProtocolType::PingCheckReq;
			std::string name = ProtocolType::Get(type);
			Assert::IsTrue(type == 1001);
			Assert::IsTrue(strcmp(ProtocolType::Get(type).c_str(), "PingCheckReq") == 0);
			//Assert::IsTrue()
			//Assert::IsTrue(ProtocolType::PingCheckReq == 1001);
			//Assert::IsTrue(strcmp(ProtocolType::Get(ProtocolType::PingCheckReq).c_str(), "PingCheckReq") == 0);
		}

		TEST_METHOD(BufferTest) {
			class Equip : public Cell {
			public:
				Equip() {}
				Equip(std::string name, short level) :name(name), level(level) {}
				virtual ~Equip() {}

			public:
				virtual void serialize(Buffer& buffer) {
					Serialize::write(buffer, name);
					Serialize::write(buffer, level);
				}

				virtual void deserialize(Buffer& buffer) {
					Deserialize::read(buffer, name);
					Deserialize::read(buffer, level);
				}

				virtual size_t getLength() {
					return 0;
				}

				virtual std::string toString() { return ""; }

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
				virtual void serialize(Buffer& buffer) {
					Serialize::write(buffer, name);
					Serialize::write(buffer, age);
					Serialize::write(buffer, dynamic_cast<Cell*>(&equip));
				}

				virtual void deserialize(Buffer& buffer) {
					Deserialize::read(buffer, name);
					Deserialize::read(buffer, age);
					Deserialize::read(buffer, dynamic_cast<Cell*>(&equip));
				}

				virtual size_t getLength() {
					return 0;
				}

				virtual std::string toString() { return ""; }

			public:
				std::string name;
				short age;
				Equip equip;
			};

			User person("»§Ä®¹ý»ç", 29, Equip("Sword", 15));

			Buffer buffer(8192);

			Serialize::write(buffer, dynamic_cast<Cell*>(&person));

			buffer.setBufferOffset(0);

			User person2;

			Deserialize::read(buffer, dynamic_cast<Cell*>(&person2));

			Assert::AreEqual(person.name, person2.name);
			Assert::AreEqual(person.age, person2.age);
			Assert::AreEqual(person.equip.name, person2.equip.name);
			Assert::AreEqual(person.equip.level, person2.equip.level);
		}

		TEST_METHOD(RequestTest) {
			class User : public Cell {
			public:
				User() {}
				virtual ~User() {}

			public:
				virtual void serialize(Buffer& buffer) {
					Serialize::writeArray(buffer, fingerPrinter);
					if (fingerPrinter[0]) { Serialize::write(buffer, nickname); }
					if (fingerPrinter[1]) { Serialize::write(buffer, level); }
				}

				virtual void deserialize(Buffer& buffer) {
					Deserialize::readArray(buffer, fingerPrinter);
					if (fingerPrinter[0]) { Deserialize::read(buffer, nickname); }
					if (fingerPrinter[1]) { Deserialize::read(buffer, level); }
				}

				virtual size_t getLength() {
					size_t totalLength = sizeof(size_t) + sizeof(fingerPrinter);
					if (fingerPrinter[0]) {
						totalLength += sizeof(size_t) + nickname.length();
					}
					if (fingerPrinter[1]) {
						totalLength += sizeof(level);
					}
					return totalLength;
				}

				virtual std::string toString() { return ""; }

			public:
				void setNickname(std::string value) {
					nickname = value;
					fingerPrinter[0] = true;
				}
				std::string getNickname() { return nickname; }
				void setLevel(short value) {
					level = value;
					fingerPrinter[1] = true;
				}
				short getLevel() { return level; }

			private:
				bool fingerPrinter[2] = { false, };
				std::string nickname;
				short level;
			};

			class LoginReq : public Request {
			public:
				LoginReq() {
					typeId = 1;
				}
				virtual ~LoginReq() {}

			public:
				virtual void serialize(Buffer& buffer) {
					Request::writeHeader(buffer, getLength());
					Serialize::write(buffer, dynamic_cast<Cell*>(&user));
				}

				virtual void deserialize(Buffer& buffer) {
					Request::readHeader(buffer);
					Deserialize::read(buffer, dynamic_cast<Cell*>(&user));
				}

				virtual size_t getLength() {
					return Request::getLength() + user.getLength();
				}

				virtual std::string toString() { return ""; }

			public:
				size_t getTypeId() { return typeId; }
				User getUser() { return user; }

			public:
				void setUser(User value) {
					user = value;
				}

			private:
				User user;
			};

			LoginReq req;
			User user;
			user.setLevel(15);

			req.setUser(user);

			Buffer sendBuffer(8192);
			Buffer receiveBuffer(8192);

			Serialize::write(sendBuffer, dynamic_cast<Cell*>(&req));

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

				if (HeaderSize > receiveBuffer.getBufferLastPos()) {
					continue;
				}

				auto headerBlock = receiveBuffer.viewByteBlock(HeaderSize);
				Header header(headerBlock);
				if (receiveBuffer.getBufferLastPos() < header.length) {
					continue;
				}

				if (header.typeId == 1) {
					Deserialize::read(receiveBuffer, dynamic_cast<Cell*>(&loginReq));
					receiveBuffer.clearBuffer();
					break;
				}
			}

			Assert::AreEqual(req.getTypeId(), loginReq.getTypeId());
			//Assert::AreEqual(req.getUser().nickname, loginReq.getUser().nickname);
			Assert::AreEqual(req.getUser().getLevel(), loginReq.getUser().getLevel());
		}

		TEST_METHOD(RequestTest2) {
			class LoginReq : public Request {
			public:
				LoginReq() {
					typeId = 1;
				}
				virtual ~LoginReq() {}

			public:
				virtual void serialize(Buffer& buffer) {
					Request::writeHeader(buffer, getLength());
					Serialize::write(buffer, token);
					Serialize::write(buffer, deviceId);
				}

				virtual void deserialize(Buffer& buffer) {
					Request::readHeader(buffer);
					Deserialize::read(buffer, token);
					Deserialize::read(buffer, deviceId);
				}

				virtual size_t getLength() {
					return Request::getLength() + sizeof(size_t) + token.size() + sizeof(size_t) + deviceId.size();
				}

				virtual std::string toString() { return ""; }

			public:
				std::string token;
				std::string deviceId;
			};

			LoginReq req;
			req.token = "fdagjdkasldgjiafd1fdaw5432";
			req.deviceId = "android";

			Buffer sendBuffer(8192);
			Buffer receiveBuffer(8192);

			Serialize::write(sendBuffer, (Cell*)&req);

			char* byteBlock = sendBuffer.viewByteBlock(sendBuffer.getBufferLastPos());
			for (size_t i = 0; i < sendBuffer.getBufferLastPos(); i++) {
				receiveBuffer.putByte(byteBlock[i]);
			}

			LoginReq tempReq;

			auto headerBlock = receiveBuffer.viewByteBlock(HeaderSize);
			Header header(headerBlock);
			receiveBuffer.readByteBlock(header.length);

			Assert::AreEqual(receiveBuffer.getBufferLastPos(), receiveBuffer.getBufferOffset());

			/*Deserialize::read(receiveBuffer, (Cell*)&tempReq);

			Assert::AreEqual(req.token, tempReq.token);
			Assert::AreEqual(req.deviceId, tempReq.deviceId);*/
		}
	};
}