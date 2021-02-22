#pragma once
#include <iostream>
#include <sstream>
#include <vector>
#include <any>
#include <chrono>
#include <locale>
#include <Windows.h>
#include <atlconv.h>
#include "Cell.h"
#include "boost/format.hpp"

namespace DBBD {
	static std::wstring strconv(const std::string& _src)
	{
		USES_CONVERSION;
		return std::wstring(A2W(_src.c_str()));
	};

	static std::string strconv(const std::wstring& _src)
	{
		USES_CONVERSION;
		return std::string(W2A(_src.c_str()));
	};

	template <typename ... Args>
	static std::string strFormat(const std::string& str, const Args&... args) {
		std::vector<std::any> argVec = { args... };

		std::vector<std::string> strArray;
		std::string temp;
		char prev = ' ';
		int literalCount = 0;
		for (auto c : str) {
			if (c == '{') {
				prev = c;
				continue;
			}
			if (prev == '{' && c == '}') {
				strArray.push_back(temp);
				temp = "";
				prev = c;
				literalCount++;
				continue;
			}

			temp += c;
		}
		strArray.push_back(temp);

		if (literalCount != argVec.size()) {
			throw std::exception("invalid log argument size...");
		}

		std::string resultStr = "";
		for (size_t i = 0; i < strArray.size(); i++) {
			resultStr += strArray[i];

			if (i < literalCount) {
				auto arg = argVec[i];
				if (arg._Cast<std::string>()) { resultStr += *arg._Cast<std::string>(); }
				else if (arg._Cast<char>()) { resultStr += std::to_string(*arg._Cast<char>()); }
				else if (arg._Cast <byte>()) { resultStr += std::to_string(*arg._Cast<byte>()); }
				else if (arg._Cast<int>()) { resultStr += std::to_string(*arg._Cast<int>()); }
				else if (arg._Cast<unsigned int>()) { resultStr += std::to_string(*arg._Cast<unsigned int>()); }
				else if (arg._Cast<short>()) { resultStr += std::to_string(*arg._Cast<short>()); }
				else if (arg._Cast<size_t>()) { resultStr += std::to_string(*arg._Cast<size_t>()); }
				else if (arg._Cast<float>()) { resultStr += std::to_string(*arg._Cast<float>()); }
				//else if (arg._Cast<Cell>()) { resultStr += *arg._Cast<Cell>().toString(); }
			}
		}

		return resultStr;
	}

	static std::vector<std::string> strSplit(std::string input, char delimiter) {
		std::vector<std::string> answer;
		std::stringstream ss(input);
		std::string temp;

		while (getline(ss, temp, delimiter)) {
			answer.push_back(temp);
		}

		return answer;
	}

	static std::string toAnsiString(const std::wstring& src, UINT codePage)
	{
		std::string strReturn;

		const int nLength = ::WideCharToMultiByte(codePage, 0UL, src.c_str(), -1, nullptr, 0, nullptr, nullptr);

		if (nLength < 0)
			return strReturn;

		strReturn.resize(nLength);
		const int nResult = ::WideCharToMultiByte(codePage, 0UL, src.c_str(), -1, &strReturn[0], nLength, nullptr, nullptr);

		(void)nResult;
		return strReturn;
	}

	static std::wstring toWideString(const std::string& src, UINT codePage)
	{
		std::wstring strReturn;

		const int nLength = ::MultiByteToWideChar(codePage, 0UL, src.c_str(), -1, nullptr, 0);

		if (nLength < 0)
			return strReturn;

		strReturn.resize(nLength);

		const int nResult = ::MultiByteToWideChar(codePage, 0UL, src.c_str(), -1, &strReturn[0], nLength);

		(void)nResult;

		return std::move(strReturn);
	}

	static std::string utfStringToUrlString(const std::string& src)
	{
		std::stringstream stm;

		for(char ch : src)
		{
			if (ch == 0)	// 가장 마지막 널문자는 변환하지 않는다.
				break;

			stm << boost::format("%%%X") % (ch & 0xff);
		}

		return stm.str();
	}

	static std::string toUrlString(const std::wstring& src)
	{
		return utfStringToUrlString(toAnsiString(src, CP_UTF8));
	}

	static std::string toUrlString(const std::string& src)
	{
		return toUrlString(toWideString(src, CP_ACP));
	}

	static std::string getNowString() {
		auto now = std::chrono::system_clock::now();
		time_t nowTime = std::chrono::system_clock::to_time_t(now);
		tm tm;
		localtime_s(&tm, &nowTime);

		std::string nowStr = strFormat("{}-{}-{} {}:{}:{}",
			(tm.tm_year + 1900), (tm.tm_mon + 1), tm.tm_mday,
			tm.tm_hour, tm.tm_min, tm.tm_sec);

		return nowStr;
	}
}