#include "LotusLibLogger.h"

using namespace LotusLib;

void
Logger::setLogProperties(spdlog::level::level_enum consoleLogLevel)
{
	if (spdlog::get("LotusLib"))
		spdlog::drop("LotusLib");

	if (spdlog::get("__default_logger"))
		spdlog::drop("__default_logger");

	auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_st>();
	consoleSink->set_level(consoleLogLevel);
	consoleSink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] %v");

	auto newLogger = std::make_shared<spdlog::logger>("LotusLib", spdlog::sinks_init_list({ consoleSink }));
	newLogger->set_level(spdlog::level::trace);

	spdlog::register_logger(newLogger);

	_logWrapper = LoggerWrapper(newLogger);
}

void
Logger::setLogProperties(const std::filesystem::path& logPath, spdlog::level::level_enum logPathLevel, spdlog::level::level_enum consoleLogLevel)
{
	if (spdlog::get("LotusLib"))
		spdlog::drop("LotusLib");

	if (spdlog::get("__default_logger"))
		spdlog::drop("__default_logger");
	
	auto sinkFile = std::make_shared<spdlog::sinks::basic_file_sink_st>(logPath.string(), true);
	sinkFile->set_level(logPathLevel);
	sinkFile->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] %v");

	auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_st>();
	consoleSink->set_level(consoleLogLevel);
	consoleSink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] %v");

	auto newLogger = std::make_shared<spdlog::logger>("LotusLib", spdlog::sinks_init_list({ consoleSink, sinkFile }));
	newLogger->set_level(spdlog::level::trace);

	spdlog::register_logger(newLogger);

	_logWrapper = LoggerWrapper(newLogger);
}

void
Logger::error(const std::string& msg)
{
	_logWrapper.log->error(msg);
}

void
Logger::warn(const std::string& msg)
{
	_logWrapper.log->warn(msg);
}

void
Logger::info(const std::string& msg)
{
	_logWrapper.log->info(msg);
}

void
Logger::debug(const std::string& msg)
{
	_logWrapper.log->debug(msg);
}

void
Logger::trace(const std::string& msg)
{
	_logWrapper.log->trace(msg);
}

Logger::LoggerWrapper::LoggerWrapper()
{
	auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_st>();
	consoleSink->set_level(m_logLevel);
	consoleSink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] %v");

	log = std::make_shared<spdlog::logger>("__default_logger", spdlog::sinks_init_list({ consoleSink }));
	log->set_level(m_logLevel);

	spdlog::register_logger(log);
}

Logger::LoggerWrapper::LoggerWrapper(std::shared_ptr<spdlog::logger> logger)
{
	log = logger;
}