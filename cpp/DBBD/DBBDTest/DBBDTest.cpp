#include "pch.h"
#include "CppUnitTest.h"
#include "../DBBD/Buffer.h"
#include "../DBBD/Cell.h"
#include "../DBBD/Serialize.h"
#include "../DBBD/Deserialize.h"

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
				User(std::string name, short age, Equip equip) :name(name), age(age), equip(equip) {}
				virtual ~User() {}

			private:
				virtual void serialize(Buffer* buffer) {
					Serialize::write(buffer, name);
					Serialize::write(buffer, age);
					Serialize::write(buffer, dynamic_cast<Cell*>(&equip));
					//equip.serialize(buffer);
					//Serialize::write(buffer, equip);
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
			class Request{
			public:
				virtual ~Request() = 0;

			public:
				virtual void serialize(Buffer* buffer) = 0;
				virtual void deserialize(Buffer* buffer) = 0;
				virtual size_t getLength() = 0;

			protected:
				size_t typeId;
			};

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
					return nickname.size(); + sizeof(level);
				}

			private:
				std::string nickname;
				short level;
			};

			class LoginReq : public Request {
			public:
				LoginReq() {}
				virtual ~LoginReq() {}
			
			public:
				virtual void serialize(Buffer* buffer) {
					Serialize::write(buffer, typeId);
					Serialize::write(buffer, dynamic_cast<Cell*>(&user));
				}

				virtual void deserialize(Buffer* buffer) {
					Deserialize::read(buffer, typeId);
					Deserialize::read(buffer, dynamic_cast<Cell*>(&user));
				}

				virtual size_t getLength() {
					return sizeof(typeId) + user.getLength();
				}

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

			Buffer buffer(8192);

			Serialize::write(&buffer, req.getLength());
			//Serialize::write(&buffer, req);
		}
	};
}
