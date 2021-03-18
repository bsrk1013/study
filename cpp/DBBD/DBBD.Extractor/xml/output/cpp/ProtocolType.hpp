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
		ServerConnectReq = 1001,
		ServerConnectResp = 1002,
		RelayNoti = 1003,
	};
	std::map<Value, std::wstring> stringMap = {
		{ Value::ServerConnectReq, L"ServerConnectReq" },
		{ Value::ServerConnectResp, L"ServerConnectResp" },
		{ Value::RelayNoti, L"RelayNoti" },
	};
	std::wstring Get(Value value) {
		auto iter = ProtocolType::stringMap.find(value);
		if (iter == ProtocolType::stringMap.end()) {
			return "";
		}
		return iter->second;
	}
}
