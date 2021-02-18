#include "RedisManager.h"

namespace DBBD
{
	void RedisManager::init(const std::string& address, const short& port,
		const std::string& psw, const short& maxConnCount)
	{
		this->address = address;
		this->port = port;
		this->psw = psw;
		this->maxConnCount = maxConnCount;

		for (size_t i = 0; i < maxConnCount; i++) {
			auto redis = createInfo();
			{
				//std::lock_guard<std::mutex> lock(lockObject);
				infoSet.insert(redis);
			}
		}
		/*for (size_t i = 0; i < maxConnCount; i++) {
			auto redis = createRedis();
			{
				std::lock_guard<std::mutex> lock(lockObject);
				redisSet.insert(redis);
			}
		}*/

		isInit = true;
		std::cout << "RedisManager init, ip: " << address << ", port: " << port << std::endl;
	}

	void RedisManager::release()
	{
		for (auto info : infoSet) {
			closeInfoInternal(info);
		}

		infoSet.clear();
	}

#pragma region KEYS
	bool RedisManager::del(const short& db, const std::string& key)
	{
		return del(db, StringVector{ key });
	}

	bool RedisManager::del(const short& db, const StringVector & keys)
	{
		auto reply = execute("del", db, keys);
		if (reply.is_error()) {
			return false;
		}
		return reply.as_integer() == keys.size();
	}

	bool RedisManager::exists(const short& db, const std::string& key)
	{
		return exists(db, StringVector{ key });
	}

	bool RedisManager::exists(const short& db, const StringVector& keys)
	{
		auto reply = execute("exists", db, keys);
		if (reply.is_error()) {
			return false;
		}
		return reply.as_integer() == keys.size();
	}

	bool RedisManager::rename(const short& db, const std::string& sourceKey, const std::string& destKey)
	{
		auto reply = execute("rename", db, sourceKey, destKey);
		auto error = reply.as_string();
		return strcmp(error.c_str(), "OK") == 0;
	}

	bool RedisManager::expire(const short& db, const std::string& key, const int& seconds)
	{
		auto reply = execute("expire", db, key, seconds);
		auto error = reply.as_string();
		return strcmp(error.c_str(), "OK") == 0;
	}

	bool RedisManager::expireat(const short& db, const std::string& key, std::chrono::system_clock::time_point at)
	{
		auto duration = at.time_since_epoch() / 10000000;
		return expireat(db, key, duration.count());
	}

	bool RedisManager::expireat(const short& db, const std::string& key, const int& timestamp)
	{
		auto reply = execute("expireat", db, key, timestamp);
		return reply.as_integer() > 0;
	}
#pragma endregion

#pragma region HASH
	bool RedisManager::hexists(const short& db, const std::string& key, const std::string& field)
	{
		auto reply = execute("hexists", db, key, field);
		if (reply.is_error()) { return false; }
		return reply.as_integer() > 0;
	}

	bool RedisManager::hdel(const short& db, const std::string& key, const std::string& field)
	{
		return hdel(db, key, StringVector{ field });
	}

	bool RedisManager::hdel(const short& db, const std::string& key, const std::vector<std::string>& fields)
	{
		auto reply = execute("hdel", db, key, fields);
		if (reply.is_error()) { return false; }
		return reply.as_integer() == fields.size();
	}

	bool RedisManager::hset(const short& db, const std::string& key, const std::string& field, const int& value)
	{
		return hset(db, key, field, std::to_string(value));
	}

	bool RedisManager::hset(const short& db, const std::string& key, const std::string& field, const std::string& value)
	{
		auto reply = execute("hset", db, key, field, value);
		if (reply.is_error()) { return false; }
		return reply.as_integer() > 0;
	}

	std::vector<std::string> RedisManager::hkeys(const short& db, const std::string& key)
	{
		std::vector<std::string> vec;
		auto replyArray = execute("hkeys", db, key);
		auto keyArray = replyArray.as_array();
		for (auto reply : keyArray) {
			auto key = convertElem<std::string>(reply);
			vec.push_back(key);
		}
		return vec;
	}

	int RedisManager::hincrby(const short& db, const std::string& key, const std::string& field, const int& incr)
	{
		auto reply = execute("hincrby", db, key, field, incr);
		if (reply.is_error()) { return 0; }
		return reply.as_integer();
	}
#pragma endregion

#pragma region SET
	bool RedisManager::sadd(const short& db, const std::string& key, const int& member)
	{
		return sadd(db, key, std::to_string(member));
	}

	bool RedisManager::sadd(const short& db, const std::string& key, const std::string& member)
	{
		return sadd(db, key, StringVector{ member });
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
		auto reply = execute("sadd", db, key, members);
		if (reply.is_error()) { return false; }
		return reply.as_integer() == members.size();
	}

	int RedisManager::scard(const short& db, const std::string& key)
	{
		auto reply = execute("scard", db, key);
		if (reply.is_error()) { return 0; }
		return reply.as_integer();
	}

	bool RedisManager::sismember(const short& db, const std::string& key, const int& member)
	{
		return sismember(db, key, std::to_string(member));
	}

	bool RedisManager::sismember(const short& db, const std::string& key, const std::string& member)
	{
		auto reply = execute("sismember", db, key, member);
		if (reply.is_error()) { return false; }
		return reply.as_integer() > 0;
	}

	bool RedisManager::smove(const short& db, const std::string& sourceKey, const std::string& destKey, const int& member)
	{
		return smove(db, sourceKey, destKey, std::to_string(member));
	}

	bool RedisManager::smove(const short& db, const std::string& sourceKey, const std::string& destKey, const std::string& member)
	{
		auto reply = execute("smove", db, sourceKey, destKey, member);
		if (reply.is_error()) { return false; }
		return reply.as_integer() > 0;
	}

	bool RedisManager::srem(const short& db, const std::string& key, const int& member)
	{
		return srem(db, key, std::to_string(member));
	}

	bool RedisManager::srem(const short& db, const std::string& key, const std::string& member)
	{
		return srem(db, key, StringVector{ member });
	}

	bool RedisManager::srem(const short& db, const std::string& key, const std::vector<int>& members)
	{
		StringVector vec;
		for (auto member : members) {
			vec.push_back(std::to_string(member));
		}
		return srem(db, key, vec);
	}

	bool RedisManager::srem(const short& db, const std::string& key, const StringVector& members)
	{
		auto reply = execute("srem", db, key, members);
		if (reply.is_error()) { return false; }
		return reply.is_integer() == members.size();
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
		auto reply = execute("zadd", db, key, opts, sets);
		if (reply.is_error()) { return false; }
		return reply.as_integer() == sets.size();
		/*auto redis = getConn(db);
		auto result = redis->conn->zadd(key, opts, sets);
		redis->conn->commit();
		result.wait();
		putConn(redis);
		return result.get().as_integer() == sets.size();*/
	}

	int RedisManager::zscore(const short& db, const std::string& key, const int& member)
	{
		return zscore(db, key, std::to_string(member));
	}

	int RedisManager::zscore(const short& db, const std::string& key, const std::string& member)
	{
		auto reply = execute("zscore", db, key, member);
		if (reply.is_null()) { return -1; }
		auto strScore = reply.as_string();
		return std::stoi(strScore);
	}

	int RedisManager::zincrby(const short& db, const std::string& key, const int& member, const int& incr)
	{
		return zincrby(db, key, std::to_string(member), incr);
	}

	int RedisManager::zincrby(const short& db, const std::string& key, const std::string& member, const int& incr)
	{
		auto reply = execute("zincrby", db, key, incr, member);
		if (reply.is_error()) { return 0; }
		auto strIncrScore = reply.as_string();
		return std::stoi(strIncrScore);
	}

	int RedisManager::zrank(const short& db, const std::string& key, const int& member, const bool& isReverse)
	{
		return zrank(db, key, std::to_string(member), isReverse);
	}

	int RedisManager::zrank(const short& db, const std::string& key, const std::string& member, const bool& isReverse)
	{
		std::string command = isReverse ? "zrevrank" : "zrank";
		auto reply = execute(command, db, key, member);
		if (reply.is_null()) { return -1; }
		return reply.as_integer();
	}

	std::vector<cpp_redis::reply> RedisManager::innerZrange(const short& db, const std::string& key, const int& start, const int& stop, const bool& withscore, const bool& isReverse)
	{
		std::string command = isReverse ? "zrevrange" : "zrange";
		std::string strWithscore = withscore ? "withscores" : "";
		auto reply = execute(command, db, key, start, stop, strWithscore);
		auto replyArray = reply.as_array();
		return replyArray;
	}

	std::vector<cpp_redis::reply> RedisManager::innerZragebyscore(const short& db, const std::string& key, const int& min, const int& max, const bool& withscore, const bool& isReverse)
	{
		std::string command = isReverse ? "zrevrangebyscore" : "zrangebyscore";
		std::string strWithscore = withscore ? "withscores" : "";
		auto reply = execute(command, db, key, min, max, strWithscore);
		auto replyArray = reply.as_array();
		return replyArray;
	}
#pragma endregion

#pragma region STRING
	bool RedisManager::set(const short& db, const std::string& key, const std::string& value)
	{
		auto reply = execute("set", db, key, value);
		auto error = reply.as_string();
		return strcmp(error.c_str(), "OK") == 0;
	}

	std::string RedisManager::get(const short& db, const std::string& key)
	{
		auto reply = execute("get", db, key);
		return convertElem<std::string>(reply);
	}

	std::string RedisManager::getset(const short& db, const std::string& key, const std::string& value)
	{
		auto reply = execute("getset", db, key, value);
		return convertElem<std::string>(reply);
	}

	bool RedisManager::mset(const short& db, const std::vector<std::pair<std::string, std::string>> keyvalues)
	{
		auto reply = execute("mset", db, keyvalues);
		auto error = reply.as_string();
		return strcmp(error.c_str(), "OK") == 0;
	}

	std::map<std::string, std::string> RedisManager::mget(const short& db, StringVector keys)
	{
		std::map<std::string, std::string> map;
		auto reply = execute("mget", db, keys);
		auto resultArray = reply.as_array();
		for (size_t i = 0; i < keys.size(); i++) {
			std::string value;
			if (resultArray[i].is_null()) {
				value = "";
			}
			else {
				value = resultArray[i].as_string();
			}
			map[keys[i]] = value;
		}
		return map;
	}

	int RedisManager::incr(const short& db, const std::string& key)
	{
		return incrby(db, key, 1);
	}

	int RedisManager::incrby(const short& db, const std::string& key, const int& incr)
	{
		auto reply = execute("incrby", db, key, incr);
		return reply.as_integer();
	}

	int RedisManager::decr(const short& db, const std::string& key)
	{
		return decrby(db, key, 1);
	}

	int RedisManager::decrby(const short& db, const std::string& key, const int& decr)
	{
		auto reply = execute("devrby", db, key, decr);
		return reply.as_integer();
	}

	int RedisManager::strlen(const short& db, const std::string& key)
	{
		auto reply = execute("strlen", db, key);
		return reply.as_integer();
	}
#pragma endregion

#pragma region BASE
	RedisSP RedisManager::getInfo(const short& db)
	{
		auto info = DBBaseManager::getInfo();
		info->conn->select(db);
		return info;
	}

	RedisSP RedisManager::createInfo()
	{
		std::shared_ptr<cpp_redis::client> conn = std::make_shared<cpp_redis::client>();
		conn->connect(address, port);
		conn->auth(psw);
		RedisSP redis = std::make_shared<RedisConnInfo>();
		redis->conn = conn;
		redis->usedTime = std::chrono::system_clock::now();
		return redis;
	}

	void RedisManager::closeInfoInternal(RedisSP info)
	{
		info->conn->disconnect(true);
	}
#pragma endregion
}