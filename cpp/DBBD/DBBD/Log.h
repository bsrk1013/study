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
		Log* init(std::string name);
		Log* usingTelegramBot(std::string token, int chatId);
		virtual void release() override;

	public:
		void log(const LogLevel& level, const std::string& fileName, const long& line, const std::string& msg);
		template<typename ... Args>
		void log(const LogLevel& level, const std::string& fileName, const long& line, const std::string& msg, const Args&... args)
		{
			log(level, fileName, line, strFormat(msg, args...));
		}

	private:
		void debug(const std::string& msg);
		void info(const std::string& msg);
		void warning(const std::string& msg);
		void error(const std::string& msg);

	private:
		void writeLog(LogLevel level, const std::string& msg);
		std::shared_ptr<spdlog::logger> getFileLogger(LogLevel level);

	private:
		std::string name;
		bool telegramBot = false;
		std::string telegramSendURL;
		std::shared_ptr<spdlog::logger> consoleLogger;
		std::shared_ptr<spdlog::logger> fileLogger;
		int lastHour = 0;
	};

#define LOG_DEBUG(msg, ...) DBBD::Log::Instance()->log(DBBD::LogLevel::Debug, __FILE__, __LINE__, msg, ##__VA_ARGS__)
#define LOG_INFO(msg, ...) DBBD::Log::Instance()->log(DBBD::LogLevel::Info, __FILE__, __LINE__, msg, ##__VA_ARGS__)
#define LOG_WARN(msg, ...) DBBD::Log::Instance()->log(DBBD::LogLevel::Warning, __FILE__, __LINE__, msg, ##__VA_ARGS__)
#define LOG_ERROR(msg, ...) DBBD::Log::Instance()->log(DBBD::LogLevel::Error, __FILE__, __LINE__, msg, ##__VA_ARGS__)
}