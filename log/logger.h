#pragma once

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#include "spdlog/spdlog.h"

class Log
{
 public:
	Log() = delete;
	Log(const Log &) = delete;
	Log &operator=(const Log &) = delete;

	static void Init();

	static void End();

	static spdlog::logger *GetLoggerInstance()
	{
		assert(sLoggerInstance && "Logger instance is null,maybe you have not execute Log::Init().");
		return sLoggerInstance.get();
	}

 private:
	static std::shared_ptr<spdlog::logger> sLoggerInstance;
};

#define LOG_T(...) SPDLOG_LOGGER_TRACE(Log::GetLoggerInstance(), __VA_ARGS__, __FILE__, __LINE__)
#define LOG_D(...) SPDLOG_LOGGER_DEBUG(Log::GetLoggerInstance(), __VA_ARGS__, __FILE__, __LINE__)
#define LOG_I(...) SPDLOG_LOGGER_INFO(Log::GetLoggerInstance(), __VA_ARGS__)
#define LOG_W(...) SPDLOG_LOGGER_WARN(Log::GetLoggerInstance(), __VA_ARGS__, __FILE__, __LINE__)
#define LOG_E(...) SPDLOG_LOGGER_ERROR(Log::GetLoggerInstance(), __VA_ARGS__, __FILE__, __LINE__)


