#include "Log.h"
#include "Cell.h"

namespace DBBD
{
	void Log::init(std::string name)
	{
		this->name = name;
		consoleLogger = spdlog::stdout_color_mt("console");
		getFileLogger();
	}

	void Log::release()
	{
	}

	void Log::log(const LogLevel& level, const std::string& fileName, const long& line, const std::string& msg)
	{
		auto fileNameVec = dbbdsplit(fileName, '\\');
		std::string originFileName = fileNameVec[fileNameVec.size() - 1];
		std::string resultMsg = "[" + originFileName + "](" + std::to_string(line) + ") << " + msg;

		switch (level) {
		case LogLevel::Debug:
			debug(resultMsg);
			break;
		case LogLevel::Info:
			info(resultMsg);
			break;
		case LogLevel::Warning:
			warning(resultMsg);
			break;
		case LogLevel::Error:
			error(resultMsg);
			break;
		}
	}

	void Log::debug(const std::string& msg)
	{ 
		spdlog::set_level(spdlog::level::debug);
		writeLog(LogLevel::Debug, msg);
	}

	void Log::info(const std::string& msg)
	{ 
		spdlog::set_level(spdlog::level::info);
		writeLog(LogLevel::Info, msg);
	}

	void Log::warning(const std::string& msg)
	{ 
		spdlog::set_level(spdlog::level::warn);
		writeLog(LogLevel::Warning, msg);
	}
	void Log::error(const std::string& msg)
	{
		spdlog::set_level(spdlog::level::err);
		writeLog(LogLevel::Error, msg);
	}

	void Log::writeLog(LogLevel level, const std::string& msg)
	{
		//spdlog::stderr_color_mt("stderr");
		switch (level) {
		case LogLevel::Debug: spdlog::get("console")->debug(msg); break;
		case LogLevel::Info: spdlog::get("console")->info(msg); break;
		case LogLevel::Warning: spdlog::get("console")->warn(msg); break;
		case LogLevel::Error: spdlog::get("console")->error(msg); break;
		}
		if (level <= LogLevel::Debug) { return; }
	}

	std::shared_ptr<spdlog::logger> Log::getFileLogger()
	{
		auto now = std::chrono::system_clock::now();
		__time64_t timeNow = std::chrono::system_clock::to_time_t(now);
		tm tm;
		localtime_s(&tm, &timeNow);

		tm.tm_hour;
		if (lastHour != tm.tm_hour) {
			lastHour = tm.tm_hour;
			std::string  fileName = "..\\..\\logs\\" + tm.tm_year + tm.tm_mon + tm.tm_mday + '_' + name + '_' + std::to_string(lastHour);
			fileLogger = spdlog::basic_logger_mt("basing_logger", fileName);
		}

		return fileLogger;
	}

	std::string Log::parse(const std::string& msg, const std::vector<std::any>& argVec) {
		std::vector<std::string> msgArray;
		
		std::string temp;
		char prev = ' ';
		for (auto c : msg) {
			if (c == '{') {
				prev = c;
				continue;
			}
			if (prev == '{' && c == '}') {
				msgArray.push_back(temp);
				temp = "";
				continue;
			}

			temp += c;
		}

		if (msgArray.size() != argVec.size()) {
			throw std::exception("invalid log argument size...");
		}

		std::string resultMsg;
		for (size_t i = 0; i < msgArray.size(); i++) {
			resultMsg += msgArray[i];

			auto arg = argVec[i];
			if (arg._Cast<int>()) { resultMsg += std::to_string(*arg._Cast<int>()); }
			else if (arg._Cast<std::string>()) { resultMsg += *arg._Cast<std::string>(); }
			else if (arg._Cast<short>()) { resultMsg += std::to_string(*arg._Cast<short>()); }
			else if (arg._Cast<size_t>()) { resultMsg += std::to_string(*arg._Cast<size_t>()); }
			else if (arg._Cast<float>()) { resultMsg += std::to_string(*arg._Cast<float>()); }
			//else if (arg._Cast<Cell>()) { resultMsg += *arg._Cast<Cell>().toString(); }
		}

		return resultMsg;
	}
}