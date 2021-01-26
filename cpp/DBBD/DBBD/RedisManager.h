#pragma once
#include <string>
//#include <queue>
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

#pragma region HASH
		// template �Ű������� ������ std::string, int�� �����
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
			auto redis = getConn(db);
			auto result = redis->conn->hvals(key);
			redis->conn->commit();
			result.wait();
			putConn(redis);

			auto resultArray = result.get().as_array();

			std::vector<T> vec = convertVec<T>(resultArray);
			return vec;
		}
		template <typename T>
		T hget(const short& db, const std::string& key, const std::string& field)
		{
			auto redis = getConn(db);
			auto result = redis->conn->hget(key, field);
			redis->conn->commit();
			result.wait();
			putConn(redis);

			auto reply = result.get();
			return convertElem<T>(reply);
		}
		template <typename T>
		std::map<std::string, T> hgetall(const short& db, const std::string& key)
		{
			auto redis = getConn(db);
			auto result = redis->conn->hgetall(key);
			redis->conn->commit();
			result.wait();
			putConn(redis);

			auto resultArray = result.get().as_array();
			return convertMap<std::string, T>(resultArray);
		}
#pragma endregion

#pragma region SETS
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
		template <typename T>
		std::vector<T> smembers(const short& db, const std::string& key)
		{
			auto redis = getConn(db);
			auto result = redis->conn->smembers(key);
			redis->conn->commit();
			result.wait();
			putConn(redis);

			auto resultArray = result.get().as_array();
			return convertVec<T>(resultArray);
		}
#pragma endregion

#pragma region SORTED_SET
		/// <summary>
		/// sorted_set �ɼ�
		/// XX : �̹� �����ϴ� ��Ҹ� ������Ʈ �Ѵ�, ���� �߰����� �ʴ´�.
		/// NX : ��Ҹ� ������Ʈ ���� �ʰ� �׻� ���ο� ��Ҹ� �߰��Ѵ�.
		/// LT : ���ο� ������ ���� �������� ������ ������Ʈ �Ѵ�.
		/// GT : ���ο� ������ ���� �������� ������ ������Ʈ �Ѵ�.
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

#pragma region EXPIRE
	public:
		void expire(const short& db, const std::string& key, const size_t& seconds);
		void expireat(const short& db, const std::string& key, std::chrono::system_clock::time_point at);
		void expireat(const short& db, const std::string& key, const int& timestamp);
#pragma endregion

#pragma region BASE
	private:
		template <typename T>
		T convertElem(cpp_redis::reply reply)
		{
			if constexpr (std::is_same<std::string, T>::value) {
				return reply.as_string();
			}
			else if constexpr (std::is_same<int, T>::value) {
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

	private:
		RedisSP getConn(const short& db);
		void putConn(RedisSP);
		RedisSP createRedis();
		void refreshRedis();
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