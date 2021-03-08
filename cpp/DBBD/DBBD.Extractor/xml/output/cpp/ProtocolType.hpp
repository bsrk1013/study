#pragma once
#include "DBBD/Cell.h"
#include "DBBD/Request.h"
#include "DBBD/Response.h"
#include "DBBD/Common.hpp"
#include "ProtocolType.hpp"

#include <map>
#include <string>

namespace ProtocolType {
	enum Value {
		PingCheckReq = 1001,
		PingCheckResp = 1002,
	};
	std::map<Value, std::wstring> stringMap = {
		{ Value::PingCheckReq, L"PingCheckReq" },
		{ Value::PingCheckResp, L"PingCheckResp" },
	};
	std::wstring Get(Value value) {
		auto iter = ProtocolType::stringMap.find(value);
		if (iter == ProtocolType::stringMap.end()) {
			return "";
		}
		return iter->second;
	}
}
