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
#include "../DBBD/YamlParser.hpp"
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
				virtual ~Timer() {

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
					Timer(context) {
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
				Session() {}

			public:
				std::function<int(int&, int&)> readInternal;
			};

			class ITcpSession {
			public:
				virtual void bindReadInternal(std::function<int(int&, int&)>& dest) = 0;
				virtual int readInternal(int&, int&) = 0;
			};

			class Player : public ITcpSession {
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
			sword.name = "�Ϻ���";
			sword.uid = 1001;
			sword.enchant = 1;
			itemList.push_back(&sword);

			ConsumedItem healingPotion;
			healingPotion.type = ItemType::Consumed;
			healingPotion.name = "��������";
			healingPotion.uid = 1000;
			healingPotion.count = 100;
			itemList.push_back(&healingPotion);

			for (auto item : itemList) {
				switch (item->type) {
				case ItemType::Consumed: {
					auto consumed = (ConsumedItem*)item;
					Assert::IsTrue(strcmp(consumed->name.c_str(), "��������") == 0);
					Assert::IsTrue(consumed->uid == 1000);
					Assert::IsTrue(consumed->count == 100);
					break;
				}
				case ItemType::Equip: {
					auto equip = (EquipItem*)item;
					Assert::IsTrue(strcmp(equip->name.c_str(), "�Ϻ���") == 0);
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
			sword.name = "�Ѽհ�";
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

		TEST_METHOD(YamlTest) {
			auto testConfig = YamlParser::loadConfig("test.yml");

			auto version = testConfig.get<int>("Version");
			auto whitelist = testConfig.get<std::vector<int>>("Whitelist");
			
			Assert::IsTrue(version == 1);
			for (size_t i = 0; i < whitelist.size(); i++) {
				Assert::IsTrue(whitelist[i] == (i + 1));
			}

			auto server = testConfig.get<Config>("Server");
			auto ip = server.get<std::string>("ip");
			auto port = server.get<int>("port");
			auto serverWhitelist = server.get<StringVector>("whitelist");

			Assert::IsTrue(strcmp(ip.c_str(), "127.0.0.1") == 0);
			Assert::IsTrue(port == 8200);
			auto white1 = serverWhitelist[0];
			auto white2 = serverWhitelist[1];
			Assert::IsTrue(strcmp(white1.c_str(), "127.0.0.1") == 0);
			Assert::IsTrue(strcmp(white2.c_str(), "172.168.50.1") == 0);
		}
	};
}
