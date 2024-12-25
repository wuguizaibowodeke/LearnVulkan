#include "logger.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/async.h"

std::shared_ptr<spdlog::logger> Log::sLoggerInstance{};

void Log::Init()
{
	sLoggerInstance = spdlog::stderr_color_mt<spdlog::async_factory>("Logger:");
	sLoggerInstance->set_level(spdlog::level::trace);
	//sLoggerInstance->set_pattern("[%H:%M:%S %z] [%n] [%^---%L---%$] [thread %t] %v");
	sLoggerInstance->set_pattern("%n [%^----%L----%$] %v. [%s:%#]");
}