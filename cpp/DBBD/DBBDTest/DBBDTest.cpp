#include "pch.h"
#include "CppUnitTest.h"
#include "../DBBD/Buffer.h"
#include "../DBBD/Cell.h"
#include "../DBBD/Serialize.h"
#include "../DBBD/Deserialize.h"
#include "../DBBD/Request.h"
#include "../DBBD/Random.h"
#include "../DBBD/TcpSession.h"
#include <boost/asio.hpp>
#include <boost/timer.hpp>
#include <boost/bind.hpp>

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
		virtual ~B() {}
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

		TEST_METHOD(PointerTest) {
			typedef unsigned char BYTE;
			BYTE* buffer = new BYTE[8192];

			int nLevel = 99;
			wchar_t strNickname[256] = L"Doby";

			// ==================== Write
			int nCurWriteBufferPos = 0;

			*(int*)(buffer + nCurWriteBufferPos) = nLevel;
			nCurWriteBufferPos += sizeof(nLevel);

			memcpy(buffer + nCurWriteBufferPos, strNickname, sizeof(strNickname));
			nCurWriteBufferPos += sizeof(strNickname);
			// ==================== Write

			// ==================== Read
			int nTempLevel = 0;
			wchar_t strTempNickname[256] = L"";

			int nCurReadBufferPos = 0;

			nTempLevel = *(int*)(buffer + nCurReadBufferPos);
			nCurReadBufferPos += sizeof(nTempLevel);

			memcpy(strTempNickname, (buffer + nCurReadBufferPos), sizeof(strTempNickname));
			nCurReadBufferPos += sizeof(strTempNickname);
			// ==================== Read

			Assert::AreEqual(nLevel, nTempLevel);
			Assert::AreEqual(strNickname, strTempNickname);

			class BasePlayer {
			public:
				BasePlayer() {}
				BasePlayer(int _nLevel, wchar_t _strNickname[256])
					:m_nLevel(_nLevel)
				{
					wcscpy_s(m_strNickname, _strNickname);
				}

			public:
				const int GetLevel() { return m_nLevel; }
				const wchar_t* GetNickname() { return m_strNickname; }

			private:
				int m_nLevel;
				wchar_t m_strNickname[256];
			};

			BasePlayer player(99, L"Doby2");
			memcpy(buffer + nCurWriteBufferPos, (void*)&player, sizeof(BasePlayer));
			nCurWriteBufferPos += sizeof(BasePlayer);

			BasePlayer tempPlayer;
			memcpy((void*)&tempPlayer, buffer + nCurReadBufferPos, sizeof(BasePlayer));
			nCurReadBufferPos += sizeof(BasePlayer);

			Assert::AreEqual(player.GetLevel(), tempPlayer.GetLevel());
			Assert::AreEqual(player.GetNickname(), tempPlayer.GetNickname());
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

			public:
				std::string name;
				short age;
				Equip equip;
			};

			User person("doby", 29, Equip("Sword", 15));

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
				User(std::string nickname, short level) :nickname(nickname), level(level) {}
				virtual ~User() {}

			public:
				virtual void serialize(Buffer& buffer) {
					Serialize::write(buffer, nickname);
					Serialize::write(buffer, level);
				}

				virtual void deserialize(Buffer& buffer) {
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
			User user{ "doby", 99 };

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
			Assert::AreEqual(req.getUser().nickname, loginReq.getUser().nickname);
			Assert::AreEqual(req.getUser().level, loginReq.getUser().level);
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
					return Request::getLength();
				}

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
			Deserialize::read(receiveBuffer, (Cell*)&tempReq);

			Assert::AreEqual(req.token, tempReq.token);
			Assert::AreEqual(req.deviceId, tempReq.deviceId);
		}

		TEST_METHOD(RandomTest) {
			for (size_t i = 0; i < 1000; i++) {
				int nRandom = Random::instance().next(0, 10);
				Assert::IsTrue(0 <= nRandom && nRandom <= 10);
				Assert::IsTrue(!(nRandom < 0) || !(nRandom > 10));
			}

			for (size_t i = 0; i < 1000; i++) {
				float fRandom = Random::instance().next(0.0f, 10.0f);
				Assert::IsTrue(0.0f <= fRandom && fRandom <= 10.0f);
				Assert::IsTrue(!(fRandom < 0.0f) || !(fRandom > 10.0f));
			}
		}

		TEST_METHOD(TimerTest) {
			boost::asio::io_context context;

			auto foo = [&](boost::system::error_code error) {
				Assert::IsTrue(true);
			};

			boost::asio::steady_timer timer(context, std::chrono::seconds(1));
			timer.async_wait(foo);
			/*timer.async_wait([&](boost::system::error_code error) {
				Assert::IsTrue(false);
				});*/

			context.run();

			std::this_thread::sleep_for(std::chrono::seconds(2));
		}

		TEST_METHOD(AnyTest) {
			using fooPtr = int(*)();

			fooPtr a = []()->int {
				return 1;
			};

			fooPtr b = []()->int {
				return 2;
			};

			Assert::AreEqual(1, a());
			Assert::AreEqual(2, b());

			class Session {
			public:
				std::function<int()> readInternal;
			};

			class Player {
			public:
				void bindReadInternal(std::function<int()>& dest) {
					dest = std::bind(&Player::readInternal, this);
				}
			private:
				int readInternal() {
					return 1;
				}
			};

			Session session;
			Player player;

			player.bindReadInternal(session.readInternal);

			Assert::AreEqual(1, session.readInternal());
		}
	};
}
