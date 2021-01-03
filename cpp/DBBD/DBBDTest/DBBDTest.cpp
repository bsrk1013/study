#include "pch.h"
#include "CppUnitTest.h"
#include "../DBBD/TcpServer.h"
#include "../DBBD/TcpClient.h"
#include "../DBBD/TcpSession.h"
#include "../DBBD/Buffer.h"
#include "../DBBD/Cell.h"
#include "../DBBD/Serialize.h"
#include "../DBBD/Deserialize.h"
#include "../DBBD/Request.h"
#include "../DBBD/Response.h"
#include "../DBBD/Random.h"
#include "../DBBD/TcpSession.h"
#include "../DBBD/TimerObject.h"
#include <boost/asio.hpp>
#include <boost/timer.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>

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
					return Request::getLength() + sizeof(size_t) + token.size() + sizeof(size_t) + deviceId.size();
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

			auto headerBlock = receiveBuffer.viewByteBlock(HeaderSize);
			Header header(headerBlock);
			receiveBuffer.readByteBlock(header.length);

			Assert::AreEqual(receiveBuffer.getBufferLastPos(), receiveBuffer.getBufferOffset());

			/*Deserialize::read(receiveBuffer, (Cell*)&tempReq);

			Assert::AreEqual(req.token, tempReq.token);
			Assert::AreEqual(req.deviceId, tempReq.deviceId);*/
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

#define BINDING(method) std::bind(method, this)
		TEST_METHOD(TimerTest) {
			static const int CREATURE_UPDATE_TICK = 2000;
			enum CREATURE_TIMER_TYPE {
				None = 0,
				Update = 1,
			};
			class Creature : TimerObject {
			public:
				Creature(std::shared_ptr<boost::asio::io_context> context)
					: TimerObject(context) {
					registTimerEvent();
				}
				~Creature() {
					removeTimerEvent(CREATURE_TIMER_TYPE::Update);
				}

			protected:
				virtual void registTimerEvent() override {
					addTimerEvent(CREATURE_TIMER_TYPE::Update,
						BINDING(&Creature::update),
						CREATURE_UPDATE_TICK, true);
				}

			private:
				void update() {
					Assert::IsTrue(true);
				}
			};

			std::shared_ptr<boost::asio::io_context> context;
			context = std::make_shared<boost::asio::io_context>();

			std::vector<Creature*> mobList;
			/*for (int i = 0; i < 100; i++) {
				Creature* mob = new Creature(context);
				mobList.push_back(mob);
			}

			for (auto mob : mobList) {
				delete mob;
			}*/

			Creature* doby = new Creature(context);

			boost::thread_group threads;
			threads.create_thread(boost::bind(&boost::asio::io_context::run, &(*context)));
			
			std::this_thread::sleep_for(std::chrono::seconds(1));
			delete doby;

			std::this_thread::sleep_for(std::chrono::seconds(10));
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
		}

		TEST_METHOD(FunctionTest) {
			class Session : public std::enable_shared_from_this<Session> {
			public:
				typedef std::shared_ptr<Session> pointer;
				static pointer create() {
					return Session::pointer(new Session());
				}
				
			private:
				Session(){}

			public:
				std::function<int(int&, int&)> readInternal;
			};

			class ITcpSession {
			public:
				virtual void bindReadInternal(std::function<int(int&, int&)>& dest) = 0;
				virtual int readInternal(int&, int&) = 0;
			};

			class Player : public ITcpSession{
			public:
				Player(Session::pointer session)
				: session(session) {
					bindReadInternal(this->session->readInternal);
				}

				virtual void bindReadInternal(std::function<int(int&, int&)>& dest) override {
					dest = std::bind(&Player::readInternal, this, std::placeholders::_1, std::placeholders::_2);
				}

				virtual int readInternal(int& a, int& b) override {
					int tempA = a;
					int tempB = b;
					a = 5;
					b = 3;
					return tempA + tempB;
				}

			private:
				Session::pointer session;
			};

			Session::pointer session = Session::create();
			Player player(session);

			int a = 1;
			int b = 2;
			if (session->readInternal) {
				Assert::AreEqual(3, session->readInternal(a, b));
				Assert::AreEqual(5, a);
				Assert::AreEqual(3, b);
			}
		}

		TEST_METHOD(ServerTest) {
			enum Protocol {
				PingReq = 1,
				PingResp = 2,
			};

			class PingCheckReq : Request {
			public:
				PingCheckReq() {
					typeId = Protocol::PingReq;
				}

			public:
				virtual void serialize(Buffer& buffer) {
					Request::writeHeader(buffer, getLength());
				}

				virtual void deserialize(Buffer& buffer) {
					Request::readHeader(buffer);
				}

				virtual size_t getLength() {
					return Request::getLength();
				}
			};

			class PingCheckResp : Response {
			public:
				PingCheckResp() {
					typeId = Protocol::PingResp;
				}

			public:
				virtual void serialize(Buffer& buffer) {
					Response::writeHeader(buffer, getLength());
				}

				virtual void deserialize(Buffer& buffer) {
					Response::readHeader(buffer);
				}

				virtual size_t getLength() {
					return Response::getLength();
				}
			};

			class TestServerSession : public ITcpSession, TimerObject {
			public:
				TestServerSession(TcpSession::pointer session) :
				session(session), TimerObject(session->getContext()){
					bindReadInternal(this->session->readInternal);
				}
				~TestServerSession() {
					TimerObject::~TimerObject();
				}

			protected:
				virtual void registTimerEvent() override {
					addTimerEvent(1, TIMER_BINDING(&TestServerSession::pingCheck), 500, true);
				}

				virtual void bindReadInternal(ReadInternalParam& dest) {
					dest = READ_INTERNAL_BINDING(&TestServerSession::readInternal);
				}

				virtual bool readInternal(const DBBD::Header& header, DBBD::Buffer& buffer) {
					switch (header.typeId) {
					case Protocol::PingResp: {
						PingCheckResp pingResp;
						Deserialize::read(buffer, (Cell*)&pingResp);
						pingFlag = true;
						invalidPingCount = 0;
						pingCount++;
						break;
					}
					}

					return true;
				}

			private:
				void pingCheck() {
					if (!pingFlag) {
						invalidPingCount++;
					}

					if (invalidPingCount > 2) {
						session->stop();
						return;
					}

					PingCheckReq ping;
					session->write((Cell*)&ping);

					pingFlag = false;
				}

			public:
				bool isConnect() { return session->isConnect(); }
				size_t getPingCount() { return pingCount; }

			private:
				TcpSession::pointer session;
				bool pingFlag = true;
				size_t invalidPingCount = 0;
				size_t pingCount = 0;
			};
			
			class TestClientSession : public ITcpSession, TimerObject {
			public:
				TestClientSession(TcpSession::pointer session)
					: session(session), TimerObject(session->getContext()) {
					bindReadInternal(this->session->readInternal);
				}
				~TestClientSession() {}

			protected:
				virtual void registTimerEvent() override {
					//addTimerEvent(1, TIMER_BINDING(&TestServerSession::pingCheck), 500, true);
				}

				virtual void bindReadInternal(ReadInternalParam& dest) {
					dest = READ_INTERNAL_BINDING(&TestClientSession::readInternal);
				}

				virtual bool readInternal(const Header& header, Buffer& buffer) {
					switch (header.typeId) {
					case Protocol::PingReq: {
						PingCheckReq pingReq;
						Deserialize::read(buffer, (Cell*)&pingReq);

						pingCount++;

						PingCheckResp pingResp;
						session->write((Cell*)&pingResp);
						break;
					}
					}

					return true;
				}

			public:
				bool isConnect() { return session->isConnect(); }
				size_t getPingCount() { return pingCount; }

			private:
				TcpSession::pointer session;
				size_t pingCount = 0;
			};

			class TestServer : public TcpServer {
			public:
				TestServer(std::string name, std::string address, int port)
				:TcpServer(name, address, port){}
				virtual ~TestServer(){}

			public:
				TestServerSession* getSession() { return testClientSession; }

			protected:
				virtual void acceptInternal(TcpSession::pointer session) {
					testClientSession = new TestServerSession(session);
				}

				virtual void disconnectInternal(size_t sessionId) {
					delete testClientSession;
				}

			public:
				TestServerSession* testClientSession = nullptr;
			};

			class ChatServer : public TcpServer {
			public:
				ChatServer(std::string name, std::string address, int port)
				:TcpServer(name, address, port) {}
				virtual ~ChatServer() {}

			public:
			};

			class TestClient : public TcpClient {
			public:
				TestClient(std::string name, std::string address, int port)
					: TcpClient(address, port){}
				virtual ~TestClient() {}

			public:
				TestClientSession* getSession() { return testSession; }

			protected:
				virtual void connectInternal(TcpSession::pointer session) {
					testSession = new TestClientSession(session);
				}

			private:
				TestClientSession* testSession = nullptr;
			};


			auto start = std::chrono::system_clock::now();

			TestServer* server;
			TestClient* client;

			std::string address = "127.0.0.1";
			int port = 8100;

			std::thread serverThread([&]() {
				server = new TestServer("TestServer", address, port);
				});
			std::thread clientThread([&]() {
				std::this_thread::sleep_for(std::chrono::milliseconds(500));
				client = new TestClient("TestClient", address, port);
				});

			std::this_thread::sleep_for(std::chrono::milliseconds(500));

			while (true) {
				auto session = server->getSession();
				if (!session) { continue; }
				if (!session->isConnect()) { break; }
				size_t pingCount = session->getPingCount();
				// 1초에 2번 증가
				if (pingCount > 20) {
					break;
				}
			}

			auto end = std::chrono::system_clock::now();

			std::chrono::duration<double> elapsed = end - start;

			auto serverSession = server->getSession();
			auto clientSession = client->getSession();

			auto elapsedSecond = elapsed.count();
			Assert::IsTrue(elapsedSecond >= 11);
			size_t serverPingCount = serverSession->getPingCount();
			size_t clientPingCount = clientSession->getPingCount();
			Assert::AreEqual(serverPingCount, clientPingCount);
			//server->stop();

			serverThread.join();
			clientThread.join();
		}
	};
}
