#include "Log.h"
#include "Cell.h"
#include "spdlog/cfg/env.h"
#include "spdlog/sinks/hourly_file_sink.h"
#include "spdlog/async.h"
#include <future>
#include <iostream>

namespace DBBD
{
	Log* Log::init(std::string name)
	{
		this->name = name;
		spdlog::cfg::load_env_levels();
		consoleLogger = spdlog::stdout_color_mt("Console");
		std::string fileName = "logs/" + name + ".log";
		fileLogger = spdlog::hourly_logger_mt<spdlog::async_factory>(name, fileName);
		isInit = true;
		return Log::Instance();
	}

	Log* Log::usingTelegramBot(std::string token, int chatId) {
		telegramBot = true;
		telegramToken = token;
		telegramChatId = chatId;
		telegramClient = std::make_shared<httplib::Client>("https://api.telegram.org");
		//telegramSendURL = strFormat("https://api.telegram.org/bot{}/sendmessage?chat_id={}&text=", token, chatId);

		telegramThread = NEW_THREAD_SP([&]() {
			updateTelegram();
			});

		return Log::Instance();
	}

	void Log::release()
	{
		if (telegramBot) {
			telegramClient->stop();
		}
	}

#pragma region string
	void Log::log(const LogLevel& level, const std::string& fileName, const long& line, const std::string& msg)
	{
		if (!isInit) { return; }
		auto fileNameVec = strSplit(fileName, '\\');
		std::string originFileName = fileNameVec[fileNameVec.size() - 1];
		//std::string resultMsg = "[" + originFileName + "](" + std::to_string(line) + ") << " + msg;
		std::string resultMsg = strFormat("[{}]({}) << {}", originFileName, std::to_string(line), msg);

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
		switch (level) {
		case LogLevel::Debug: consoleLogger->debug(msg); return;
		case LogLevel::Info: consoleLogger->info(msg); fileLogger->info(msg); break;
		case LogLevel::Warning: consoleLogger->warn(msg); fileLogger->warn(msg);  break;
		case LogLevel::Error: consoleLogger->error(msg); fileLogger->error(msg); break;
		}
		fileLogger->flush();

		sendTelegramBot(level, msg);
	}

	void Log::sendTelegramBot(LogLevel level, const std::string& msg)
	{
		if (!telegramBot
			|| level < LogLevel::Warning) {
			return;
		}

		std::string levelStr = level == LogLevel::Warning ? "Warn" : "Error";

		std::string telegramMsg = strFormat("[{}] [{}] [{}] {}",
			getNowString(), name, levelStr, msg);

		std::string convertMsg = toUrlString(telegramMsg);

		/*httplib::Result(httplib::Client::*gf)(const char*) = &httplib::Client::Get;
		std::async(std::launch::async, gf, telegramClient,
			strFormat("/bot{}/sendmessage?chat_id={}&text={}",
			telegramToken, telegramChatId, convertMsg).c_str());*/

		telegramMsgQueue.push(convertMsg);

		/*auto res = telegramClient->Get(strFormat("/bot{}/sendmessage?chat_id={}&text={}",
			telegramToken, telegramChatId, convertMsg).c_str());*/
	}
#pragma endregion

#pragma region wstring
	//void Log::log(const LogLevel& level, const std::string& fileName, const long& line, const std::wstring& msg)
	//{
	//	if (!isInit) { return; }
	//	auto fileNameVec = strSplit(fileName, '\\');
	//	std::string originFileName = fileNameVec[fileNameVec.size() - 1];
	//	std::wstring resultMsg = strFormat(L"[{}]({}) << {}", strconv(originFileName), std::to_wstring(line), msg);
	//	//std::wstring resultMsg = "[" + originFileName + "](" + std::to_string(line) + ") << " + msg;

	//	switch (level) {
	//	case LogLevel::Debug:
	//		debug(resultMsg);
	//		break;
	//	case LogLevel::Info:
	//		info(resultMsg);
	//		break;
	//	case LogLevel::Warning:
	//		warning(resultMsg);
	//		break;
	//	case LogLevel::Error:
	//		error(resultMsg);
	//		break;
	//	}
	//}

	//void Log::debug(const std::wstring& msg)
	//{
	//	consoleLogger->set_level(spdlog::level::debug);
	//	fileLogger->set_level(spdlog::level::debug);
	//	writeLog(LogLevel::Debug, msg);
	//}
	//void Log::info(const std::wstring& msg)
	//{
	//	consoleLogger->set_level(spdlog::level::info);
	//	fileLogger->set_level(spdlog::level::info);
	//	writeLog(LogLevel::Info, msg);
	//}
	//void Log::warning(const std::wstring& msg)
	//{
	//	consoleLogger->set_level(spdlog::level::warn);
	//	fileLogger->set_level(spdlog::level::warn);
	//	writeLog(LogLevel::Warning, msg);
	//}
	//void Log::error(const std::wstring& msg)
	//{
	//	consoleLogger->set_level(spdlog::level::err);
	//	fileLogger->set_level(spdlog::level::err);
	//	writeLog(LogLevel::Error, msg);
	//}

	//void Log::writeLog(LogLevel level, const std::wstring& msg)
	//{
	//	switch (level) {
	//	case LogLevel::Debug: consoleLogger->debug(msg); return;
	//	case LogLevel::Info: consoleLogger->info(msg); fileLogger->info(msg); break;
	//	case LogLevel::Warning: consoleLogger->warn(msg); fileLogger->warn(msg);  break;
	//	case LogLevel::Error: consoleLogger->error(msg); fileLogger->error(msg); break;
	//	}
	//	fileLogger->flush();

	//	sendTelegramBot(level, msg);
	//}

	//void Log::sendTelegramBot(LogLevel level, const std::wstring& msg)
	//{
	//	if (!telegramBot
	//		|| level < LogLevel::Warning) {
	//		return;
	//	}

	//	std::wstring levelStr = level == LogLevel::Warning ? L"Warn" : L"Error";
	//	std::wstring telegramMsg = strFormat(L"[{}] [{}] [{}] {}",
	//		getNowString(), name, levelStr, msg);

	//	std::string convertMsg = toUrlString(telegramMsg);

	//	telegramMsgQueue.push(convertMsg);
	//}
#pragma endregion

	void Log::updateTelegram() {
		while (true) {
			while (!telegramMsgQueue.empty()) {
				auto msg = telegramMsgQueue.front();
				telegramMsgQueue.pop();

				telegramClient->Get(strFormat("/bot{}/sendmessage?chat_id={}&text={}",
					telegramToken, telegramChatId, msg).c_str());
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	}
}