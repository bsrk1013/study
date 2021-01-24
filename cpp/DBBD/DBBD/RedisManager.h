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
	class RedisManager : public Singleton<RedisManager>
	{
	public:
		void init(const std::string& address, const short& port, const short& maxConnCount = 8);

#pragma region HASH
	public:
		void hset(
			const std::string& key, const std::string& field, const std::string& value);
		void hset(
			const std::string& key, const std::string& filed, const int& value);
		
		// template 매개변수로 무조건 std::string, int를 써야함
		template <typename T>
		T hget(
			const std::string& key, const std::string& field) 
		{
			auto reply = innerHget(key, field);

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
		
	private:
		void innerHset(
			const std::string& key, const std::string& field, const std::string& value);
		cpp_redis::reply innerHget(
			const std::string& key, const std::string& field);
#pragma endregion

	private:
		RedisSP getConn();
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