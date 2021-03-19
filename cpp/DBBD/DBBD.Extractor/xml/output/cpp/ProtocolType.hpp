#pragma once
#include "DBBD/Cell.h"
#include "DBBD/Request.h"
#include "DBBD/Response.h"
#include "DBBD/Common.hpp"
#include "ProtocolType.hpp"

#include <map>
#include <string>

namespace Base {
	enum Value {
		ServerConnectReq = 1001,
		ServerConnectResp = 1002,
		PingCheckReq = 1003,
		PingCheckResp = 1004,
	};
	std::map<Value, std::wstring> stringMap = {
		{ Value::ServerConnectReq, L"ServerConnectReq" },
		{ Value::ServerConnectResp, L"ServerConnectResp" },
		{ Value::PingCheckReq, L"PingCheckReq" },
		{ Value::PingCheckResp, L"PingCheckResp" },
	};
	std::wstring Get(Value value) {
		auto iter = Base::stringMap.find(value);
		if (iter == Base::stringMap.end()) {
			return "";
		}
		return iter->second;
	}
}
#include <map>
#include <string>

namespace Relay {
	enum Value {
		RelayNoti = 2001,
	};
	std::map<Value, std::wstring> stringMap = {
		{ Value::RelayNoti, L"RelayNoti" },
	};
	std::wstring Get(Value value) {
		auto iter = Relay::stringMap.find(value);
		if (iter == Relay::stringMap.end()) {
			return "";
		}
		return iter->second;
	}
}
