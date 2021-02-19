#pragma once
#include <iostream>
#include <sstream>
#include <vector>
#include <any>
#include <Windows.h>
#include <atlconv.h>
#include "Cell.h"

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
}