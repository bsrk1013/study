#include "pch.h"
#include "CppUnitTest.h"
#include "../DBBD/Buffer.h"
#include "../DBBD/Cell.h"
#include "../DBBD/Serialize.h"
#include "../DBBD/Deserialize.h"
#include "../DBBD/Request.h"
#include "../DBBD/Response.h"
#include "../DBBD/Random.h"
#include "../DBBD/TimerObject.h"
#include "../DBBD/RedisManager.h"
#include "../DBBD/MariaDBManager.h"
#include <boost/asio.hpp>
#include <boost/timer.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <queue>
#include <vector>
#include <cmath>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace DBBD;

namespace DBBDTest
{
	template <typename T>
	class ItemReq : public Request {
	public:
		virtual ~ItemReq() {}

	public:
		virtual void serialize(Buffer& buffer) {
			writeHeader(buffer, getLength());
			item.serialize(buffer);
		}

		virtual void deserialize(Buffer& buffer) {
			readHeader(buffer);
			item.deserialize(buffer);
		}

		virtual size_t getLength() {
			return Request::getLength() + item.getLength();
		}

		T item;
	};

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

	//static class ProtocolType {
	//public:
	//	enum Value {
	//		PingCheckReq = 1001,
	//		PingCheckResp = 1002,
	//	};

	//	/*ProtocolType() {
	//		stringMap[PingCheckReq] = "PingCheckReq";
	//		stringMap[PingCheckResp] = "PingCheckResp";
	//	}*/

	//	static std::string Get(Value value) {
	//		auto iter = stringMap.find(value);
	//		if (iter == stringMap.end()) {
	//			return "";
	//		}
	//		return iter->second;
	//	}

	//	static std::map<Value, std::string> stringMap;
	//	/*static std::map<Value, std::string> stringMap = {
	//		{Value::PingCheckReq, "PingCheckReq"},
	//		{Value::PingCheckResp, "PingCheckResp"},
	//	};*/
	//};

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

	TEST_CLASS(DBBDTest)
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
				BasePlayer(int _nLevel, std::string _strNickname)
					:m_nLevel(_nLevel), m_strNickname(_strNickname)
				{
					//wcscpy_s(m_strNickname, _strNickname);
				}

			public:
				const int GetLevel() { return m_nLevel; }
				const std::string GetNickname() { return m_strNickname; }

			private:
				int m_nLevel;
				std::string m_strNickname;
			};

			BasePlayer player(99, "Doby2");
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
			class Timer : public std::enable_shared_from_this<Timer> {
			public:
				Timer(IoContextSP context) :
					context(context)
				{}
				virtual ~Timer(){

				}

			public:
				void start() {
					registTimer();
				}

			protected:
				virtual void registTimer() = 0;
				void addTimer(std::function<void()> method, size_t waitMs) {
					if (!timer) {
						timer = new boost::asio::deadline_timer(*context);
					}

					auto waitTime = boost::posix_time::milliseconds(waitMs);
					timer->expires_from_now(waitTime);

					timer->async_wait(std::bind(&Timer::timerMethod, this, std::placeholders::_1,
						std::weak_ptr<Timer>(shared_from_this()), method));
				}

			private:
				void timerMethod(const boost::system::error_code& error,
					std::weak_ptr<Timer> ptr, std::function<void()> method) {
					auto sharedPtr = ptr.lock();
					if (!sharedPtr) {
						return; 
					}
					method();
				}

			private:
				IoContextSP context;
				boost::asio::deadline_timer* timer = nullptr;
			};

			class FooTimer : public Timer {
			public:
				FooTimer(IoContextSP context) :
				Timer(context){
				}

			protected:
				virtual void registTimer() {
					auto method = std::bind(&FooTimer::update, this);
					addTimer(method, 1000);
					//auto method = std::bind(&FooTimer::update, std::weak_ptr<Timer>(shared_from_this()), std::placeholders::_1);
				}

			private:
				void update() {
					Assert::IsTrue(true);
				}
			};

			IoContextSP context = std::make_shared<boost::asio::io_context>();

			std::shared_ptr<FooTimer> foo = std::make_shared <FooTimer>(context);
			foo->start();

			std::thread thread([&]() {context->run(); });

			std::this_thread::sleep_for(std::chrono::seconds(10));

			thread.join();
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

			class Foo : public std::enable_shared_from_this<Foo> {
			public:
				Foo() { x = 10; }
				virtual ~Foo() {}

			public:
				virtual int getX() {
					auto ptr = shared_from_this();
					return ptr->x;
				}

			protected:
				int x;
			};

			class Foo2 : public Foo {
			public:
				Foo2() {}
				virtual ~Foo2() {}
			};

			std::shared_ptr<Foo2> foo = std::shared_ptr<Foo2>(new Foo2());
			Assert::AreEqual(foo->getX(), 10);

			std::shared_ptr<Foo2> fooSP1 = std::make_shared<Foo2>();
			std::shared_ptr<Foo2> fooSP2(fooSP1);

			Assert::AreEqual((int)fooSP2.use_count(), 2);
			fooSP2.reset();
			Assert::AreEqual((int)fooSP1.use_count(), 1);

			std::any anyVal = std::string("aaaaaaaa");
			auto castInt = anyVal._Cast<int>();
			auto castString = anyVal._Cast<std::string>();
			Assert::IsTrue(castInt == nullptr);
			Assert::IsTrue(strcmp(castString->c_str(), "aaaaaaaa") == 0);

			std::multimap<std::string, std::string> multiMap;
			multiMap.insert(std::pair<std::string, std::string>("a", "b"));
			anyVal = multiMap;
			if (auto castMultiMap = anyVal._Cast<std::multimap<std::string, std::string>>()) {
				for (auto pair : *castMultiMap) {
					Assert::IsTrue(strcmp(pair.first.c_str(), "a") == 0);
					Assert::IsTrue(strcmp(pair.second.c_str(), "b") == 0);
				}
			}

			int lastClearFloor = 111;
			float baseFloor = lastClearFloor / 10.0f;
			int minFloor = std::floor(baseFloor) * 10;
			int maxFloor = std::ceill(baseFloor) * 10;

			int targetFloor1 = 120;
			int targetFloor2 = 121;
			int targetFloor3 = 30;

			Assert::IsTrue(minFloor < targetFloor1&& targetFloor1 <= maxFloor);
			Assert::IsFalse(minFloor < targetFloor2&& targetFloor2 <= maxFloor);
			Assert::IsFalse(minFloor < targetFloor3&& targetFloor3 <= maxFloor);

			/*float fSection = lastClearFloor / 10.0f;
			int nSection = std::ceill(fSection) * 10;
			Assert::IsTrue(nSection >= targetFloor);
			targetFloor = 41;
			Assert::IsFalse(nSection >= targetFloor);*/
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

		TEST_METHOD(ScopeTest) {
			class ScopeTestFoo {
			public:
				ScopeTestFoo(int a) {
					this->a = a;
				}

				int a;
			};

			std::queue<ScopeTestFoo*> fooQueue;
			{
				ScopeTestFoo scopeTestFoo(10);
				fooQueue.push(&scopeTestFoo);
			}

			auto scopeTestFoo = fooQueue.front();

			Assert::AreEqual(scopeTestFoo->a, 10);
		}

		TEST_METHOD(DataTest) {
			enum ItemType {
				Consumed = 0,
				Equip,
			};

			class ItemBase {
			public:
				BYTE type;
				std::string name;
				long uid;
			};

			class ConsumedItem : public ItemBase {
			public:
				int count;
			};

			class EquipItem : public ItemBase {
			public:
				int enchant;
			};

			std::vector<ItemBase*> itemList;

			EquipItem sword;
			sword.type = ItemType::Equip;
			sword.name = "일본도";
			sword.uid = 1001;
			sword.enchant = 1;
			itemList.push_back(&sword);

			ConsumedItem healingPotion;
			healingPotion.type = ItemType::Consumed;
			healingPotion.name = "빨강포션";
			healingPotion.uid = 1000;
			healingPotion.count = 100;
			itemList.push_back(&healingPotion);

			for (auto item : itemList) {
				switch (item->type) {
				case ItemType::Consumed:{
					auto consumed = (ConsumedItem*)item;
					Assert::IsTrue(strcmp(consumed->name.c_str(), "빨강포션") == 0);
					Assert::IsTrue(consumed->uid == 1000);
					Assert::IsTrue(consumed->count == 100);
					break;
				}
				case ItemType::Equip: {
					auto equip = (EquipItem*)item;
					Assert::IsTrue(strcmp(equip->name.c_str(), "일본도") == 0);
					Assert::IsTrue(equip->uid == 1001);
					Assert::IsTrue(equip->enchant == 1);
					break;
				}
				}
			}
		}

		TEST_METHOD(InheritTest) {
			enum ItemType {
				Consumed = 0,
				Equip,
			};

			class ItemBase : public Cell {
			public:
				virtual ~ItemBase() {}

			public:
				virtual void serialize(Buffer& buffer) {
					Serialize::write(buffer, type);
					Serialize::write(buffer, name);
					Serialize::write(buffer, uid);
				}

				virtual void deserialize(Buffer& buffer) {
					Deserialize::read(buffer, type);
					Deserialize::read(buffer, name);
					Deserialize::read(buffer, uid);
				}

				virtual size_t getLength() {
					return sizeof(BYTE) + sizeof(size_t) + name.size() + sizeof(long);
				}

			public:
				BYTE type;
				std::string name;
				long uid;
			};

			class EquipItem : public ItemBase {
			public:
				virtual ~EquipItem() {}

			public:
				virtual void serialize(Buffer& buffer) {
					ItemBase::serialize(buffer);
					Serialize::write(buffer, enchant);
				}

				virtual void deserialize(Buffer& buffer) {
					ItemBase::deserialize(buffer);
					Deserialize::read(buffer, enchant);
				}

				virtual size_t getLength() {
					return ItemBase::getLength() + sizeof(int);
				}

			public:
				int enchant;
			};

			EquipItem sword;
			sword.type = ItemType::Equip;
			sword.name = "한손검";
			sword.uid = 100101;
			sword.enchant = 7;

			ItemReq<EquipItem> req;
			req.item = sword;

			Buffer buffer(8192);
			req.serialize(buffer);

			ItemReq<EquipItem> tempReq;
			tempReq.deserialize(buffer);

			Assert::IsTrue(req.item.uid == tempReq.item.uid);
		}

		TEST_METHOD(RedisHashTest) {
			RedisManager::Instance()->init("118.67.134.160", 6379);

			RedisManager::Instance()->hset(0, "study:test", "hello", "world");
			auto stringGet = RedisManager::Instance()->hget<std::string>(0, "study:test", "hello");
			Assert::IsTrue(strcmp(stringGet.c_str(), "world") == 0);


			RedisManager::Instance()->hset(0, "study:test", "doby", 150);
			auto integerGet = RedisManager::Instance()->hget<int>(0, "study:test", "doby");
			Assert::IsTrue(integerGet == 150);

			auto arrayGet = RedisManager::Instance()->hgetall<std::string>(0, "study:test");
			Assert::IsTrue(strcmp(arrayGet["hello"].c_str(), "world") == 0);
			Assert::IsTrue(strcmp(arrayGet["doby"].c_str(), "150") == 0);

			Assert::IsTrue(RedisManager::Instance()->hexists(0, "study:test", "hello"));
			Assert::IsTrue(RedisManager::Instance()->hexists(0, "study:test", "doby"));

			auto keys = RedisManager::Instance()->hkeys(0, "study:test");

			RedisManager::Instance()->hdel(0, "study:test", keys);
			/*RedisManager::Instance()->hdel("study:test", "hello");
			RedisManager::Instance()->hdel("study:test", "doby");*/

			Assert::IsFalse(RedisManager::Instance()->hexists(0, "study:test", "hello"));
			Assert::IsFalse(RedisManager::Instance()->hexists(0, "study:test", "doby"));

			RedisManager::Instance()->hset(0, "test", "hello", "world");

			std::this_thread::sleep_for(std::chrono::seconds(10));
			Assert::IsFalse(RedisManager::Instance()->hexists(0, "test", "hello"));
		}

		TEST_METHOD(RedisSortedSetTest) {
			RedisManager::Instance()->init("118.67.134.160", 6379);

			RedisManager::Instance()->zadd(0, "test", "doby", 100);
			auto dobyScore = RedisManager::Instance()->zscore(0, "test", "doby");
			Assert::IsTrue(dobyScore == 100);

			auto incr = RedisManager::Instance()->zincrby(0, "test", "doby", 10);
			dobyScore = RedisManager::Instance()->zscore(0, "test", "doby");
			Assert::IsTrue(dobyScore == incr && incr == 110);

			auto rank = RedisManager::Instance()->zrank(0, "test", "doby");
			Assert::IsTrue(rank == 0);
			rank = RedisManager::Instance()->zrank(0, "test", "douner");
			Assert::IsTrue(rank == -1);

			auto rangeVec = RedisManager::Instance()->zrange<std::string>(0, "test", 0, -1);
			Assert::IsTrue(strcmp(rangeVec[0].c_str(), "doby") == 0);

			auto rangeMap = RedisManager::Instance()->zrangeWithscore<std::string>(0, "test", 0, -1);
			Assert::IsTrue(rangeMap["doby"] == 110);

			auto expireTime = std::chrono::system_clock::now() + std::chrono::hours(24);
			RedisManager::Instance()->expireat(0, "test", expireTime);
		}

		TEST_METHOD(RedisKeyTest) {
			RedisManager::Instance()->init("118.67.134.160", 6379);

			RedisManager::Instance()->sadd(0, "test1", "hello");
			RedisManager::Instance()->sadd(0, "test2", "hello");
			RedisManager::Instance()->del(0, StringVector{"test1", "test2"});
		}

		TEST_METHOD(MariaDBTest) {
			struct TestInfo {
				long uid;
				std::string name;
			};

			enum GAME_SP {
				TEST_INSERT_SP,
				TEST_SELECT_SP,
			};

			MariaDBManager::Instance()->init("118.67.134.160", 3306, "root", "1231013a", "Test");

			std::string a = "test";
			auto b = a.c_str();
			const char* value = "test";
			MariaDBManager::Instance()->exeQuery("delete from TestTable where name = ?", b);
			MariaDBManager::Instance()->exeSP("TEST_INSERT_SP", b);
			auto result = MariaDBManager::Instance()->exeSP("TEST_SELECT_SP", b);

			auto name = result["name"];
			Assert::IsTrue(strcmp(name.c_str(), "test") == 0);
		}

		TEST_METHOD(QueryParsingTest) {
			std::string query = "delete from TestTable where name = ? and uid = ?";
			std::string value1 = "test";
			int value2 = 15;

			std::vector<std::any> args;
			args.push_back(std::any(value1));
			args.push_back(std::any(value2));

			//auto resultQuery = MariaDBManager::Instance()->queryBind(query, args);

			//Assert::IsTrue(strcmp(resultQuery.c_str(), "delete from TestTable where name = test and uid = 15") == 0);
		}
	};
}
