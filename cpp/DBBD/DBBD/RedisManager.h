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
	using SortedSetOpts = std::vector<std::string>;
	class RedisManager : public Singleton<RedisManager>
	{
	public:
		void init(const std::string& address, const short& port, const short& maxConnCount = 8);

#pragma region HASH
		// template �Ű������� ������ std::string, int�� �����
	public:
		bool hexists(const short& db, const std::string& key, const std::string& field);
		void hdel(const short& db, const std::string& key, const std::string& field);
		void hdel(const short& db, const std::string& key, const std::vector<std::string>& fields);
		void hset(const short& db, const std::string& key, const std::string& field, const std::string& value);
		void hset(const short& db, const std::string& key, const std::string& filed, const int& value);
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

			std::vector<T> vec;
			auto resultArray = result.get().as_array();
			
			for (auto reply : resultArray) {
				if constexpr (std::is_same<std::string, T>::value) {
					vec.push_back(reply.as_string());
				}
				else if constexpr (std::is_same<int, T>::value) {
					auto result = reply.as_string();
					vec.push_back(std::stoi(result));
				}
				else {
					throw std::exception("RedisManager hget, illegal template parameter");
				}
			}

			return vec;
		}

		template <typename T>
		T hget(const short& db, const std::string& key, const std::string& field)
		{
			auto reply = innerHget(db, key, field);

			if constexpr (std::is_same<std::string, T>::value) {
				return reply.as_string();
			}
			else if constexpr (std::is_same<int, T>::value) {
				auto result = reply.as_string();
				return std::stoi(result);
				//return std::to_integer
				//return reply.as_integer();
			}
			else {
				throw std::exception("RedisManager hget, illegal template parameter");
			}
		}

		template <typename T>
		std::map<std::string, T> hgetall(const short& db, const std::string& key)
		{
			auto redis = getConn(db);
			auto result = redis->conn->hgetall(key);
			redis->conn->commit();
			result.wait();
			putConn(redis);

			std::map<std::string, T> map;
			auto resultArray = result.get().as_array();

			for (size_t i = 0; i < resultArray.size(); i += 2) {
				auto key = resultArray[i].as_string();
				auto strValue = resultArray[i + 1].as_string();

				if constexpr (std::is_same<std::string, T>::value) {
					map[key] = strValue;;
				}
				else if constexpr (std::is_same<int, T>::value) {
					map[key] = std::stoi(strValue);
				}
				else
				{
					throw std::exception("RedisManager hgetall, illegal template parameter");
				}
			}
			return map;
		}
		
	private:
		void innerHdel(const short& db, const std::string& key, const std::vector<std::string>& fields);
		void innerHset(const short& db, const std::string& key, const std::string& field, const std::string& value);
		cpp_redis::reply innerHget(const short& db, const std::string& key, const std::string& field);
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
		void zadd(const short& db, const std::string& key, const std::string& member, const int& score, const SortedSetOpts& opts = SortedSetOpts());
		void zadd(const short& db, const std::string& key, const int& member, const int& score, const SortedSetOpts& opts = SortedSetOpts());
		void zadd(const short& db, const std::string& key, const std::multimap<int, int> sets, const SortedSetOpts& opts = SortedSetOpts());
		void zadd(const short& db, const std::string& key, const std::multimap<std::string, std::string> sets, const SortedSetOpts& opts = SortedSetOpts());
		int zscore(const short& db, const std::string& key, const int& member);
		int zscore(const short& db, const std::string& key, const std::string& member);
#pragma endregion

#pragma region EXPIRE
		void expire(const short& db, const std::string& key, const size_t& seconds);
		void expireat(const short& db, const std::string& key);
#pragma endregion

	private:
		RedisSP getConn(const short& db);
		void putConn(RedisSP);
		RedisSP createRedis();
		void refreshRedis();

	private:
		bool isInit = false;
		std::string address;
		short port;
		short curCreateConnCount = 0;
		short maxConnCount;
		std::set<RedisSP> redisSet;
		std::mutex lockObject;
	};
}