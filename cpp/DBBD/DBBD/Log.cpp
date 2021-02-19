#include "Log.h"
#include "Cell.h"
#include "spdlog/cfg/env.h"
#include "spdlog/sinks/hourly_file_sink.h"
#include "spdlog/async.h"

namespace DBBD
{
	void Log::init(std::string name)
	{
		this->name = name;
		spdlog::cfg::load_env_levels();
		consoleLogger = spdlog::stdout_color_mt("Console");
		std::string fileName = "logs/" + name + ".log";
		fileLogger = spdlog::hourly_logger_mt<spdlog::async_factory>(name, fileName);
	}

	void Log::release()
	{
	}

	void Log::log(const LogLevel& level, const std::string& fileName, const long& line, const std::string& msg)
	{
		auto fileNameVec = strSplit(fileName, '\\');
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
		consoleLogger->set_level(spdlog::level::debug);
		fileLogger->set_level(spdlog::level::debug);
		writeLog(LogLevel::Debug, msg);
	}

	void Log::info(const std::string& msg)
	{ 
		consoleLogger->set_level(spdlog::level::info);
		fileLogger->set_level(spdlog::level::info);
		writeLog(LogLevel::Info, msg);
	}

	void Log::warning(const std::string& msg)
	{ 
		consoleLogger->set_level(spdlog::level::warn);
		fileLogger->set_level(spdlog::level::warn);
		writeLog(LogLevel::Warning, msg);
	}
	void Log::error(const std::string& msg)
	{
		consoleLogger->set_level(spdlog::level::err);
		fileLogger->set_level(spdlog::level::err);
		writeLog(LogLevel::Error, msg);
	}

	void Log::writeLog(LogLevel level, const std::string& msg)
	{
		//spdlog::stderr_color_mt("stderr");
		switch (level) {
		case LogLevel::Debug: consoleLogger->debug(msg); break;
		case LogLevel::Info: consoleLogger->info(msg); fileLogger->info(msg); break;
		case LogLevel::Warning: consoleLogger->warn(msg); fileLogger->warn(msg);  break;
		case LogLevel::Error: consoleLogger->error(msg); fileLogger->error(msg); break;
		}

		fileLogger->flush();

		if (telegramBot) {

		}
	}
}