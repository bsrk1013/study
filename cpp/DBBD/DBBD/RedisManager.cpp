#include "RedisManager.h"

namespace DBBD
{
	void RedisManager::init(const std::string& address, const short& port, const short& maxConnCount)
	{
		this->address = address;
		this->port = port;
		this->maxConnCount = maxConnCount;

		for (size_t i = 0; i < maxConnCount; i++) {
			auto redis = createRedis();
			{
				std::lock_guard<std::mutex> lock(lockObject);
				redisSet.insert(redis);
			}
		}
	}

#pragma region HASH
	bool RedisManager::hexists(const short& db, const std::string& key, const std::string& field)
	{
		auto redis = getConn(db);
		auto result = redis->conn->hexists(key, field);
		redis->conn->commit();
		result.wait();
		putConn(redis);
		return result.get().as_integer() > 0;
	}

	bool RedisManager::hdel(const short& db, const std::string& key, const std::string& field)
	{
		std::vector<std::string> fields;
		fields.push_back(field);
		return hdel(db, key, fields);
	}

	bool RedisManager::hdel(const short& db, const std::string& key, const std::vector<std::string>& fields)
	{
		auto redis = getConn(db);
		auto result = redis->conn->hdel(key, fields);
		redis->conn->commit();
		result.wait();
		putConn(redis);
		return result.get().as_integer() > 0;
	}

	bool RedisManager::hset(const short& db, const std::string& key, const std::string& field, const int& value)
	{
		return hset(db, key, field, std::to_string(value));
	}

	bool RedisManager::hset(const short& db, const std::string& key, const std::string& field, const std::string& value)
	{
		auto redis = getConn(db);
		auto result = redis->conn->hset(key, field, value);
		redis->conn->commit();
		result.wait();
		putConn(redis);
		return result.get().as_integer() > 0;
	}

	std::vector<std::string> RedisManager::hkeys(const short& db, const std::string& key)
	{
		auto redis = getConn(db);
		auto result = redis->conn->hkeys(key);
		redis->conn->commit();
		result.wait();
		putConn(redis);

		std::vector<std::string> vec;

		auto replyArray = result.get();
		auto keyArray = replyArray.as_array();
		for (auto reply : keyArray) {
			auto key = reply.as_string();
			vec.push_back(key);
		}

		return vec;
	}

	int RedisManager::hincrby(const short& db, const std::string& key, const std::string& field, const int& incr)
	{
		auto redis = getConn(db);
		auto result = redis->conn->hincrby(key, field, incr);
		redis->conn->commit();
		result.wait();
		putConn(redis);
		return result.get().as_integer();
	}
#pragma endregion

#pragma region SETS
	bool RedisManager::sadd(const short& db, const std::string& key, const int& member)
	{
		return sadd(db, key, std::to_string(member));
	}

	bool RedisManager::sadd(const short& db, const std::string& key, const std::string& member)
	{
		StringVector vec;
		vec.push_back(member);
		return sadd(db, key, vec);
	}

	bool RedisManager::sadd(const short& db, const std::string& key, const std::vector<int>& members)
	{
		StringVector vec;
		for (auto member : members) {
			vec.push_back(std::to_string(member));
		}
		return sadd(db, key, vec);
	}

	bool RedisManager::sadd(const short& db, const std::string& key, const StringVector& members)
	{
		auto redis = getConn(db);
		auto result = redis->conn->sadd(key, members);
		redis->conn->commit();
		result.wait();
		putConn(redis);
		return result.get().as_integer() > 0;
	}

	int RedisManager::scard(const short& db, const std::string& key)
	{
		auto redis = getConn(db);
		auto result = redis->conn->scard(key);
		redis->conn->commit();
		result.wait();
		putConn(redis);

		return result.get().as_integer();
	}

	bool RedisManager::sismember(const short& db, const std::string& key, const int& member)
	{
		return sismember(db, key, std::to_string(member));
	}

	bool RedisManager::sismember(const short& db, const std::string& key, const std::string& member)
	{
		auto redis = getConn(db);
		auto result = redis->conn->sismember(key, member);
		redis->conn->commit();
		result.wait();
		putConn(redis);
		return result.get().as_integer() > 0;
	}

	bool RedisManager::smove(const short& db, const std::string& sourceKey, const std::string& destKey, const int& member)
	{
		return smove(db, sourceKey, destKey, std::to_string(member));
	}

	bool RedisManager::smove(const short& db, const std::string& sourceKey, const std::string& destKey, const std::string& member)
	{
		auto redis = getConn(db);
		auto result = redis->conn->smove(sourceKey, destKey, member);
		redis->conn->commit();
		result.wait();
		putConn(redis);
		return result.get().as_integer() > 0;
	}
#pragma endregion

#pragma region SORTED_SET
	bool RedisManager::zadd(const short& db, const std::string& key, const std::string& member, const int& score, const StringVector& opts)
	{
		std::multimap<std::string, std::string> sets;
		sets.insert(std::pair<std::string, std::string>(std::to_string(score), member));
		return zadd(db, key, sets, opts);
	}

	bool RedisManager::zadd(const short& db, const std::string& key, const int& member, const int& score, const StringVector& opts)
	{
		std::multimap<std::string, std::string> sets;
		sets.insert(std::pair<std::string, std::string>(std::to_string(score), std::to_string(member)));
		return zadd(db, key, sets, opts);
	}

	bool RedisManager::zadd(const short& db, const std::string& key, const std::multimap<int, int> sets, const StringVector& opts)
	{
		std::multimap<std::string, std::string> newSets;
		for (auto pair : sets) {
			newSets.insert(std::pair<std::string, std::string>(std::to_string(pair.first), std::to_string(pair.second)));
		}
		return zadd(db, key, newSets, opts);
	}

	bool RedisManager::zadd(const short& db, const std::string& key, const std::multimap<std::string, std::string> sets, const StringVector& opts)
	{
		auto redis = getConn(db);
		auto result = redis->conn->zadd(key, opts, sets);
		redis->conn->commit();
		result.wait();
		putConn(redis);
		return result.get().as_integer() > 0;
	}

	int RedisManager::zscore(const short& db, const std::string& key, const int& member)
	{
		return zscore(db, key, std::to_string(member));
	}

	int RedisManager::zscore(const short& db, const std::string& key, const std::string& member)
	{
		auto redis = getConn(db);
		auto result = redis->conn->zscore(key, member);
		redis->conn->commit();
		result.wait();
		putConn(redis);

		auto reply = result.get();
		if (reply.is_null()) {
			return -1;
		}

		auto strScore = reply.as_string();
		return std::stoi(strScore);
	}

	int RedisManager::zincrby(const short& db, const std::string& key, const int& member, const int& incr)
	{
		return zincrby(db, key, std::to_string(member), incr);
	}

	int RedisManager::zincrby(const short& db, const std::string& key, const std::string& member, const int& incr)
	{
		auto redis = getConn(db);
		auto result = redis->conn->zincrby(key, std::to_string(incr), member);
		redis->conn->commit();
		result.wait();
		putConn(redis);
		auto strIncrScore = result.get().as_string();
		return std::stoi(strIncrScore);
	}

	int RedisManager::zrank(const short& db, const std::string& key, const int& member, const bool& isReverse)
	{
		return zrank(db, key, std::to_string(member), isReverse);
	}

	int RedisManager::zrank(const short& db, const std::string& key, const std::string& member, const bool& isReverse)
	{
		auto redis = getConn(db);
		auto result = isReverse ? redis->conn->zrevrank(key, member) : redis->conn->zrank(key, member);
		redis->conn->commit();
		result.wait();
		putConn(redis);

		auto reply = result.get();
		if (reply.is_null()) {
			return -1;
		}

		return reply.as_integer();
	}

	std::vector<cpp_redis::reply> RedisManager::innerZrange(const short& db, const std::string& key, const int& start, const int& stop, const bool& withscore, const bool& isReverse)
	{
		auto redis = getConn(db);
		auto result = isReverse ? redis->conn->zrevrange(key, std::to_string(start), std::to_string(stop), withscore)
			: redis->conn->zrange(key, std::to_string(start), std::to_string(stop), withscore);
		redis->conn->commit();
		result.wait();
		putConn(redis);

		auto replyArray = result.get().as_array();
		return replyArray;
	}

	std::vector<cpp_redis::reply> RedisManager::innerZragebyscore(const short& db, const std::string& key, const int& min, const int& max, const bool& withscore, const bool& isReverse)
	{
		auto redis = getConn(db);
		auto result = isReverse ? redis->conn->zrevrangebyscore(key, std::to_string(min), std::to_string(max), withscore)
			: redis->conn->zrangebyscore(key, std::to_string(min), std::to_string(max), withscore);
		redis->conn->commit();
		result.wait();
		putConn(redis);

		auto replyArray = result.get().as_array();
		return replyArray;
	}
#pragma endregion

#pragma region EXPIRE
	void RedisManager::expire(const short& db, const std::string& key, const size_t& seconds)
	{
		auto redis = getConn(db);
		redis->conn->expire(key, seconds);
		redis->conn->commit();
		putConn(redis);
	}

	void RedisManager::expireat(const short& db, const std::string& key, std::chrono::system_clock::time_point at)
	{
		auto duration = at.time_since_epoch() / 10000000;
		expireat(db, key, duration.count());
	}

	void RedisManager::expireat(const short& db, const std::string& key, const int& timestamp)
	{
		auto redis = getConn(db);
		redis->conn->expireat(key, timestamp);
		redis->conn->commit();
		putConn(redis);
	}
#pragma endregion

#pragma region BASE
	RedisSP RedisManager::getConn(const short& db)
	{
		RedisSP redis;
		{
			std::lock_guard<std::mutex> lock(lockObject);
			for (auto tempRedis : redisSet) {
				redis = tempRedis;
				break;
			}

			redisSet.erase(redis);
			redis->usedTime = std::chrono::system_clock::now();
		}

		if (!redis) {
			redis = createRedis();
		}

		refreshRedis();

		redis->conn->select(db);

		return redis;
	}

	void RedisManager::putConn(RedisSP redis)
	{
		std::lock_guard<std::mutex> lock(lockObject);
		redisSet.insert(redis);
	}

	RedisSP RedisManager::createRedis()
	{
		std::shared_ptr<cpp_redis::client> conn = std::make_shared<cpp_redis::client>();
		conn->connect(address, port);
		RedisSP redis = std::make_shared<RedisConnInfo>();
		redis->conn = conn;
		redis->usedTime = std::chrono::system_clock::now();
		return redis;
	}

	void RedisManager::refreshRedis()
	{
		auto now = std::chrono::system_clock::now();
		std::lock_guard<std::mutex> lock(lockObject);
		for (auto iter = redisSet.begin(); iter != redisSet.end();) {
			auto redis = *iter;
			auto elapsed = now - redis->usedTime;
			auto elapsedSeconds = std::chrono::duration_cast<std::chrono::seconds>(elapsed);
			if (elapsedSeconds.count() >= 60 * 30) {
				redis->conn->disconnect();
				iter = redisSet.erase(iter);
			}
			else {
				iter++;
			}
		}
	}
#pragma endregion
}