#pragma once
#include <string>
#include <set>
#include <mutex>
#include <type_traits>
#include "Singleton.h"
#include "Define.h"
#include "cpp_redis/core/client.hpp"

namespace DBBD
{
	class RedisConnInfo
	{
	public:
		std::chrono::system_clock::time_point usedTime;
		std::shared_ptr<cpp_redis::client> conn;
	};

	using RedisSP = std::shared_ptr<RedisConnInfo>;

	struct RedisOrder
	{
		bool operator()(RedisSP r1, RedisSP r2) const {
			if (r1->usedTime > r2->usedTime) {
				return true;
			}
			else {
				return false;
			}
		}
	};

	class RedisManager : public Singleton<RedisManager>
	{
	public:
		void init(const std::string& address, const short& port, const short& maxConnCount = 8);

#pragma region KEYS
	public:
		bool del(const short& db, const std::string& key);
		bool del(const short& db, const StringVector& keys);
		bool exists(const short& db, const std::string& key);
		bool exists(const short& db, const StringVector& keys);
		bool rename(const short& db, const std::string& sourceKey, const std::string& destKey);
		bool expire(const short& db, const std::string& key, const int& seconds);
		bool expireat(const short& db, const std::string& key, std::chrono::system_clock::time_point at);
		bool expireat(const short& db, const std::string& key, const int& timestamp);
#pragma endregion

#pragma region HASH
		// template 매개변수로 무조건 std::string, int를 써야함
	public:
		bool hexists(const short& db, const std::string& key, const std::string& field);
		bool hdel(const short& db, const std::string& key, const std::string& field);
		bool hdel(const short& db, const std::string& key, const std::vector<std::string>& fields);
		bool hset(const short& db, const std::string& key, const std::string& filed, const int& value);
		bool hset(const short& db, const std::string& key, const std::string& field, const std::string& value);
		std::vector<std::string> hkeys(const short& db, const std::string& key);
		int hincrby(const short& db, const std::string& key, const std::string& field, const int& incr);
		template <typename T>
		std::vector<T> hvals(const short& db, const std::string& key)
		{
			auto reply = execute("hvals", db, key);
			auto resultArray = reply.as_array();
			return convertVec<T>(resultArray);
		}
		template <typename T>
		T hget(const short& db, const std::string& key, const std::string& field)
		{
			auto reply = execute("hget", db, key, field);
			return convertElem<T>(reply);
		}
		template <typename T>
		std::map<std::string, T> hgetall(const short& db, const std::string& key)
		{
			auto reply = execute("hgetall", db, key);
			auto resultArray = reply.as_array();
			return convertMap<std::string, T>(resultArray);
		}
#pragma endregion

#pragma region SET
	public:
		bool sadd(const short& db, const std::string& key, const int& member);
		bool sadd(const short& db, const std::string& key, const std::string& member);
		bool sadd(const short& db, const std::string& key, const std::vector<int>& members);
		bool sadd(const short& db, const std::string& key, const StringVector& members);
		int scard(const short& db, const std::string& key);
		bool sismember(const short& db, const std::string& key, const int& member);
		bool sismember(const short& db, const std::string& key, const std::string& member);
		bool smove(const short& db, const std::string& sourceKey, const std::string& destKey, const int& member);
		bool smove(const short& db, const std::string& sourceKey, const std::string& destKey, const std::string& member);
		bool srem(const short& db, const std::string& key, const int& member);
		bool srem(const short& db, const std::string& key, const std::string& member);
		bool srem(const short& db, const std::string& key, const std::vector<int>& members);
		bool srem(const short& db, const std::string& key, const StringVector& members);
		template <typename T>
		std::vector<T> smembers(const short& db, const std::string& key)
		{
			auto reply = execute("smembers", db, key);
			auto resultArray = reply.as_array();
			return convertVec<T>(resultArray);
		}
		template <typename T>
		T srandmember(const short& db, const std::string& key)
		{
			auto vec = srandmember(db, key, 1);
			return vec[0];
		}
		template <typename T>
		std::vector<T> srandmember(const short& db, const std::string& key, const int& count)
		{
			auto reply = execute("srandmember", db, key, count);
			auto resultArray = reply.as_array();
			return convertVec<T>(resultArray);
		}
#pragma endregion

#pragma region SORTED_SET
		/// <summary>
		/// sorted_set 옵션
		/// XX : 이미 존재하는 요소를 업데이트 한다, 절대 추가하지 않는다.
		/// NX : 요소를 업데이트 하지 않고 항상 새로운 요소를 추가한다.
		/// LT : 새로운 점수가 기존 점수보다 낮을때 업데이트 한다.
		/// GT : 새로운 점수가 기존 점수보다 높을때 업데이트 한다.
		/// </summary>
	public:
		bool zadd(const short& db, const std::string& key, const std::string& member, const int& score, const StringVector& opts = StringVector());
		bool zadd(const short& db, const std::string& key, const int& member, const int& score, const StringVector& opts = StringVector());
		bool zadd(const short& db, const std::string& key, const std::multimap<int, int> sets, const StringVector& opts = StringVector());
		bool zadd(const short& db, const std::string& key, const std::multimap<std::string, std::string> sets, const StringVector& opts = StringVector());
		int zscore(const short& db, const std::string& key, const int& member);
		int zscore(const short& db, const std::string& key, const std::string& member);
		int zincrby(const short& db, const std::string& key, const int& member, const int& incr);
		int zincrby(const short& db, const std::string& key, const std::string& member, const int& incr);
		int zrank(const short& db, const std::string& key, const int& member, const bool& isReverse = false);
		int zrank(const short& db, const std::string& key, const std::string& member, const bool& isReverse = false);
		template <typename T>
		std::vector<T> zrange(const short& db, const std::string& key, const int& start, const int& stop, const bool& isReverse = false)
		{
			auto replyArray = innerZrange(db, key, start, stop, false, isReverse);
			return convertVec<T>(replyArray);
		}
		template <typename T>
		std::map<T, int> zrangeWithscore(const short& db, const std::string& key, const int& start, const int& stop, const bool& isReverse = false)
		{
			auto replyArray = innerZrange(db, key, start, stop, true, isReverse);
			return convertMap<T, int>(replyArray);
		}
		template <typename T>
		std::vector<T> zrangebyscore(const short& db, const std::string& key, const int& min, const int& max, const bool& isReverse = false)
		{
			auto replyArray = innerZragebyscore(db, key, min, max, false, isReverse);
			return convertVec<T>(replyArray);
		}
		template <typename T>
		std::map<T, int> zrangebyscoreWithScore(const short& db, const std::string& key, const int& min, const int& max, const bool& isReverse = false)
		{
			auto replyArray = innerZragebyscore(db, key, min, max, true, isReverse);
			return convertMap<T, int>(replyArray);
		}

	private:
		std::vector<cpp_redis::reply> innerZrange(const short& db, const std::string& key, const int& start, const int& stop, const bool& withscore, const bool& isReverse);
		std::vector<cpp_redis::reply> innerZragebyscore(const short& db, const std::string& key, const int& min, const int& max, const bool& withscore, const bool& isReverse);
#pragma endregion

#pragma region STRING
	public:
		std::string get(const short& db, const std::string& key);
		bool set(const short& db, const std::string& key, const std::string& value);
		std::string getset(const short& db, const std::string& key, const std::string& value);
		bool mset(const short& db, const std::vector<std::pair<std::string, std::string>> keyvalues);
		std::map<std::string, std::string> mget(const short& db, StringVector keys);
		int incr(const short& db, const std::string& key);
		int incrby(const short& db, const std::string& key, const int& incr);
		int decr(const short& db, const std::string& key);
		int decrby(const short& db, const std::string& key, const int& decr);
		int strlen(const short& db, const std::string& key);
#pragma endregion

#pragma region BASE
	private:
		RedisSP getConn(const short& db);
		void putConn(RedisSP);
		RedisSP createRedis();
		void refreshRedis();

		template <typename  Arg1>
		cpp_redis::reply execute(const std::string& command, const short& db, Arg1 arg1)
		{
			return execute(command, db, arg1, nullptr, nullptr, nullptr);
		}
		template <typename  Arg1, typename Arg2>
		cpp_redis::reply execute(const std::string& command, const short& db, Arg1 arg1, Arg2 arg2)
		{
			return execute(command, db, arg1, arg2, nullptr, nullptr);
		}
		template <typename  Arg1, typename Arg2, typename Arg3>
		cpp_redis::reply execute(const std::string& command, const short& db, Arg1 arg1, Arg2 arg2, Arg3 arg3)
		{
			return execute(command, db, arg1, arg2, arg3, nullptr);
		}
		template <typename Arg1, typename Arg2, typename Arg3, typename Arg4>
		cpp_redis::reply execute(const std::string& command, const short& db, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4)
		{
			auto redis = getConn(db);

			StringVector vec;
			vec.push_back(command);

			auto convert = [&]<typename T>(T arg) {
				if constexpr (std::is_same<nullptr_t, T>::value) {
					return;
				}

				if constexpr (std::is_base_of<StringVector, T>::value) {
					vec.insert(std::end(vec), std::begin(arg), std::end(arg));
				}
				else if constexpr (std::is_base_of<std::multimap<std::string, std::string>, T>::value
					|| std::is_base_of<std::vector<std::pair<std::string, std::string>>, T>::value) {
					for (auto pair : arg) {
						vec.push_back(pair.first);
						vec.push_back(pair.second);
					}
				}
				else if constexpr (std::is_same<std::string, T>::value) {
					if (arg.empty()) { return; }
					vec.push_back(arg);
				}
				else if constexpr (std::is_same<int, T>::value) {
					vec.push_back(std::to_string(arg));
				}
			};

			convert(arg1);
			convert(arg2);
			convert(arg3);
			convert(arg4);

			auto result = redis->conn->send(vec);
			redis->conn->commit();
			result.wait();
			auto reply = result.get();
			putConn(redis);
			return reply;
		}

		template <typename T>
		T convertElem(cpp_redis::reply reply)
		{
			if constexpr (std::is_same<std::string, T>::value) {
				if (reply.is_null()) {
					return "";
				}
				return reply.as_string();
			}
			else if constexpr (std::is_same<int, T>::value) {
				if (reply.is_null()) {
					return -1;
				}
				auto result = reply.as_string();
				return std::stoi(result);
			}
			else {
				throw std::exception("RedisManager hget, illegal template parameter");
			}
		}

		template <typename T>
		std::vector<T> convertVec(std::vector<cpp_redis::reply> replyArray)
		{
			std::vector<T> vec;
			for (auto reply : replyArray) {
				vec.push_back(convertElem<T>(reply));
			}
			return vec;
		}

		template <typename Key, typename Value>
		std::map<Key, Value> convertMap(std::vector<cpp_redis::reply> replyArray)
		{
			std::map<Key, Value> map;
			for (size_t i = 0; i < replyArray.size(); i += 2) {
				auto keyReply = replyArray[i];
				auto valueReply = replyArray[i + 1];

				map[convertElem<Key>(keyReply)] = convertElem<Value>(valueReply);
			}
			return map;
		}
#pragma endregion

	private:
		bool isInit = false;
		std::string address;
		short port;
		short curCreateConnCount = 0;
		short maxConnCount;
		std::set<RedisSP, RedisOrder> redisSet;
		std::mutex lockObject;
	};
}