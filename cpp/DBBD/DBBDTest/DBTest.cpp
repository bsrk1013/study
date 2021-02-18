#include "pch.h"
#include "CppUnitTest.h"
#include "../DBBD/RedisManager.h"
#include "../DBBD/MariaDBManager.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace DBBD;

namespace DBBDTest
{
	TEST_CLASS(DBTest)
	{
	public:
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

			auto name = result[0]["name"];
			Assert::IsTrue(strcmp(name.c_str(), "test") == 0);
		}

		TEST_METHOD(EnqueQueryTest) {
			MariaDBManager::Instance()->init("118.67.134.160", 3306, "root", "1231013a", "Test");

			const char* value1 = "ENQUEU_QUERY_TEST1";
			const char* value2 = "ENQUEU_QUERY_TEST2";

			MariaDBManager::Instance()->exeQuery("delete from TestTable where name = ?", "ENQUEU_QUERY_TEST1");
			MariaDBManager::Instance()->exeQuery("delete from TestTable where name = ?", "ENQUEU_QUERY_TEST2");

			MariaDBManager::Instance()->enqueSP("TEST_INSERT_SP", value1);
			MariaDBManager::Instance()->enqueSP("TEST_INSERT_SP", value2);

			std::this_thread::sleep_for(std::chrono::seconds(1));

			auto result1 = MariaDBManager::Instance()->exeSP("TEST_SELECT_SP", "ENQUEU_QUERY_TEST1");
			auto result2 = MariaDBManager::Instance()->exeSP("TEST_SELECT_SP", value2);

			Assert::IsTrue(strcmp(result1[0]["name"].c_str(), "ENQUEU_QUERY_TEST1") == 0);
			Assert::IsTrue(strcmp(result2[0]["name"].c_str(), "ENQUEU_QUERY_TEST2") == 0);
		}

		TEST_METHOD(DBReleaseTest) {
			MariaDBManager::Instance()->init("118.67.134.160", 3306, "root", "1231013a", "Test");

			const char* value1 = "ENQUEU_QUERY_TEST1";
			const char* value2 = "ENQUEU_QUERY_TEST2";

			MariaDBManager::Instance()->exeSP("TEST_INSERT_SP", value1);
			MariaDBManager::Instance()->exeSP("TEST_INSERT_SP", value2);

			MariaDBManager::Instance()->enqueQuery("delete from TestTable where name = ?", value1);
			MariaDBManager::Instance()->enqueQuery("delete from TestTable where name = ?", value2);
		}

		TEST_METHOD(DBThreadSafeTest) {
			MariaDBManager::Instance()->init("118.67.134.160", 3306, "root", "1231013a", "Test");

			std::vector<ThreadSP> threadList;

			for (size_t i = 0; i < 8; i++) {
				ThreadSP thread = NEW_THREAD_SP([&]() {
					for (size_t j = 0; j < 1000000; j++) {
						std::string value = "THREAD_TEST";
						if (j % 2 == 0) {
							MariaDBManager::Instance()->enqueSP("TEST_INSERT_SP", value);
						}
						else {
							MariaDBManager::Instance()->exeSP("TEST_INSERT_SP", value);
						}
					}
					});
				threadList.push_back(thread);
			}

			for (auto thread : threadList) {
				thread->join();
			}
		}

		TEST_METHOD(QueryParsingTest) {
			std::string query = "delete from TestTable where name = ? and uid = ?";
			std::string value1 = "test";
			int value2 = 15;

			std::vector<std::any> args;
			args.push_back(std::any(value1));
			args.push_back(std::any(value2));

			auto split = [&](std::string input, char delimiter)->std::vector<std::string> {
				std::vector<std::string> answer;
				std::stringstream ss(input);
				std::string temp;

				while (getline(ss, temp, delimiter)) {
					answer.push_back(temp);
				}

				return answer;
			};

			auto parsing = [&](std::string origin, std::vector<std::any> temp)->std::string {
				auto queryParts = split(origin, '?');

				if (queryParts.size() != temp.size()) {
					if (temp.size() == 1 && queryParts.size() == 2) {}
					else {
						std::string error = "illegal query bind, query: " + origin + ", argsCount: " + std::to_string(args.size());
						throw std::exception(error.c_str());
					}
				}

				std::string query;
				for (size_t i = 0; i < queryParts.size(); i++) {
					query += queryParts[i];

					if (temp.size() <= i) { continue; }

					query += "'";
					auto arg = temp[i];
					if (auto castByte = arg._Cast<BYTE>()) {
						query += std::to_string(*castByte);
					}
					else if (auto castShort = arg._Cast<short>()) {
						query += std::to_string(*castShort);
					}
					else if (auto castInt = arg._Cast<int>()) {
						query += std::to_string(*castInt);
					}
					else if (auto castString = arg._Cast<std::string>()) {
						query += *castString;
					}
					else if (auto castChar = arg._Cast<const char*>()) {
						query += *castChar;
					}
					query += "'";
				}

				return query;
			};

			auto resultQuery = parsing(query, args);

			Assert::IsTrue(strcmp(resultQuery.c_str(), "delete from TestTable where name = test and uid = 15") == 0);
		}

		TEST_METHOD(RedisHashTest) {
			RedisManager::Instance()->init("118.67.134.160", 6379, "1231013a");

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
			RedisManager::Instance()->init("118.67.134.160", 6379, "1231013a");

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
			RedisManager::Instance()->init("118.67.134.160", 6379, "1231013a");

			RedisManager::Instance()->sadd(0, "test1", "hello");
			RedisManager::Instance()->sadd(0, "test2", "hello");
			RedisManager::Instance()->del(0, StringVector{ "test1", "test2" });
		}
	};
}