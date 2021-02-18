#pragma once
#include "Singleton.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "Common.hpp"
#include <any>

namespace DBBD
{
	enum LogLevel {
		Debug = 0,
		Info,
		Warning,
		Error,
	};

	class Log: public Singleton<Log>
	{
	public:
		void init(std::string name);
		virtual void release() override;

	public:
		void log(const LogLevel& level, const std::string& fileName, const long& line, const std::string& msg);
		void debug(const std::string& msg);
		void info(const std::string& msg);
		void warning(const std::string& msg);
		void error(const std::string& msg);

	public:
		template<typename ... Args>
		void log(const LogLevel& level, const std::string& fileName, const long& line, const std::string& msg, const Args&... args)
		{
			std::vector<std::any> argVec = { args... };
			auto resultMsg = parse(msg, argVec);
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

	private:
		void writeLog(LogLevel level, const std::string& msg);
		std::shared_ptr<spdlog::logger> getFileLogger();
		std::string parse(const std::string& msg, const std::vector<std::any>& argVec);

	private:
		std::string name;
		std::shared_ptr<spdlog::logger> consoleLogger;
		std::shared_ptr<spdlog::logger> fileLogger;
		int lastHour = 0;
	};

#define LOG_DEBUG(msg) DBBD::Log::Instance()->log(DBBD::LogLevel::Debug, __FILE__, __LINE__, msg)
#define LOG_INFO(msg) DBBD::Log::Instance()->log(DBBD::LogLevel::Info, __FILE__, __LINE__, msg)
#define LOG_DEBUG(msg, ...) DBBD::Log::Instance()->log(DBBD::LogLevel::Debug, __FILE__, __LINE__, msg, ##__VA_ARGS__)
}