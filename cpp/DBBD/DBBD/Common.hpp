#pragma once
#include <iostream>
#include <sstream>
#include <vector>
#include <Windows.h>
#include <atlconv.h>

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

	 static std::vector<std::string> dbbdsplit(std::string input, char delimiter) {
		 std::vector<std::string> answer;
		 std::stringstream ss(input);
		 std::string temp;

		 while (getline(ss, temp, delimiter)) {
			 answer.push_back(temp);
		 }

		 return answer;
	 }
}