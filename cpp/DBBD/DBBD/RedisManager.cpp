#include "RedisManager.h"

void DBBD::RedisManager::init(const std::string& address, const short& port, const short& maxConnCount)
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
bool DBBD::RedisManager::hexists(const short& db, const std::string& key, const std::string& field)
{
	auto redis = getConn(db);
	auto result = redis->conn->hexists(key, field);
	redis->conn->commit();
	result.wait();
	putConn(redis);
	return result.get().as_integer() > 0;
}

void DBBD::RedisManager::hdel(const short& db, const std::string& key, const std::string& field)
{
	std::vector<std::string> fields;
	fields.push_back(field);
	innerHdel(db, key, fields);
}

void DBBD::RedisManager::hdel(const short& db, const std::string& key, const std::vector<std::string>& fields)
{
	innerHdel(db, key, fields);
}

void DBBD::RedisManager::hset(const short& db, const std::string& key, const std::string& field, const std::string& value)
{
	innerHset(db, key, field, value);
}

void DBBD::RedisManager::hset(const short& db, const std::string& key, const std::string& field, const int& value)
{
	innerHset(db, key, field, std::to_string(value));
}

std::vector<std::string> DBBD::RedisManager::hkeys(const short& db, const std::string& key)
{
	auto redis = getConn(db);
	auto result = redis->conn->hkeys(key);
	redis->conn->commit();
	result.wait();
	putConn(redis);

	std::vector<std::string> vec;
	auto keyArray = result.get().as_array();
	for (auto reply : keyArray) {
		auto key = reply.as_string();
		vec.push_back(key);
	}

	return vec;
}

int DBBD::RedisManager::hincrby(const short& db, const std::string& key, const std::string& field, const int& incr)
{
	auto redis = getConn(db);
	auto result = redis->conn->hincrby(key, field, incr);
	redis->conn->commit();
	result.wait();
	putConn(redis);
	return result.get().as_integer();
}

void DBBD::RedisManager::innerHdel(const short& db, const std::string& key, const std::vector<std::string>& fields)
{
	auto redis = getConn(db);
	auto result = redis->conn->hdel(key, fields);
	redis->conn->commit();
	result.wait();
	putConn(redis);
}

void DBBD::RedisManager::innerHset(const short& db, const std::string& key, const std::string& field, const std::string& value)
{
	auto redis = getConn(db);
	auto result = redis->conn->hset(key, field, value);
	redis->conn->commit();
	result.wait();
	putConn(redis);
}

cpp_redis::reply DBBD::RedisManager::innerHget(const short& db, const std::string& key, const std::string& field)
{
	auto redis = getConn(db);
	auto result = redis->conn->hget(key, field);
	redis->conn->commit();
	result.wait();
	putConn(redis);
	return result.get();
}
#pragma endregion

#pragma region SORTED_SET
void DBBD::RedisManager::zadd(const short& db, const std::string& key, const std::string& member, const int& score, const SortedSetOpts& opts)
{
	std::multimap<std::string, std::string> sets;
	sets.insert(std::pair<std::string, std::string>(std::to_string(score), member));
	zadd(db, key, sets, opts);
}

void DBBD::RedisManager::zadd(const short& db, const std::string& key, const int& member, const int& score, const SortedSetOpts& opts)
{
	std::multimap<std::string, std::string> sets;
	sets.insert(std::pair<std::string, std::string>(std::to_string(score), std::to_string(member)));
	zadd(db, key, sets, opts);
}

void DBBD::RedisManager::zadd(const short& db, const std::string& key, const std::multimap<int, int> sets, const SortedSetOpts& opts)
{
	std::multimap<std::string, std::string> newSets;
	for (auto pair : sets) {
		newSets.insert(std::pair<std::string, std::string>(std::to_string(pair.first), std::to_string(pair.second)));
	}
	zadd(db, key, newSets, opts);
}

void DBBD::RedisManager::zadd(const short& db, const std::string& key, const std::multimap<std::string, std::string> sets, const SortedSetOpts& opts)
{
	auto redis = getConn(db);
	auto result = redis->conn->zadd(key, opts, sets);
	redis->conn->commit();
	result.wait();
	putConn(redis);
}

int DBBD::RedisManager::zscore(const short& db, const std::string& key, const int& member)
{
	return zscore(db, key, std::to_string(member));
}

int DBBD::RedisManager::zscore(const short& db, const std::string& key, const std::string& member)
{
	auto redis = getConn(db);
	auto result = redis->conn->zscore(key, member);
	redis->conn->commit();
	result.wait();
	putConn(redis);
	auto strScore = result.get().as_string();
	return std::stoi(strScore);
}
#pragma endregion

#pragma region EXPIRE
#pragma endregion

#pragma region BASE
DBBD::RedisSP DBBD::RedisManager::getConn(const short& db)
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

void DBBD::RedisManager::putConn(DBBD::RedisSP redis)
{
	std::lock_guard<std::mutex> lock(lockObject);
	redisSet.insert(redis);
}

DBBD::RedisSP DBBD::RedisManager::createRedis()
{
	std::shared_ptr<cpp_redis::client> conn = std::make_shared<cpp_redis::client>();
	conn->connect(address, port);
	RedisSP redis = std::make_shared<RedisConnInfo>();
	redis->conn = conn;
	redis->usedTime = std::chrono::system_clock::now();
	return redis;
}

void DBBD::RedisManager::refreshRedis()
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