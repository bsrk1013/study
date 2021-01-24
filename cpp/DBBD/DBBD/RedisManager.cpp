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
void DBBD::RedisManager::hset(
	const std::string& key, const std::string& field, const std::string& value)
{
	innerHset(key, field, value);
}

void DBBD::RedisManager::hset(
	const std::string& key, const std::string& field, const int& value)
{
	innerHset(key, field, std::to_string(value));
}

void DBBD::RedisManager::innerHset(
	const std::string& key, const std::string& field, const std::string& value)
{
	auto redis = getConn();
	redis->conn->hset(key, field, value);
	redis->conn->commit();
	putConn(redis);
}

cpp_redis::reply DBBD::RedisManager::innerHget(
	const std::string& key, const std::string& field)
{
	auto redis = getConn();
	auto result = redis->conn->hget(key, field);
	redis->conn->commit();
	putConn(redis);
	return result.get();
}
#pragma endregion

DBBD::RedisSP DBBD::RedisManager::getConn()
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
		if (elapsed.count() >= 1000 * 60 * 60) {
			redis->conn->disconnect();
			iter = redisSet.erase(iter);
		}
		else {
			iter++;
		}
	}
}