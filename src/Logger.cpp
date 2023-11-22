#include "Logger.h"

using namespace LotusLib;

Logger::Logger()
{
	auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_st>();
	consoleSink->set_level(spdlog::level::info);
	consoleSink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] %v");

	Logger::_log = std::make_shared<spdlog::logger>("__default_logger", spdlog::sinks_init_list({ consoleSink }));
	Logger::_log->set_level(spdlog::level::trace);

	spdlog::register_logger(_log);
}

void
Logger::setLogProperties(std::string loggerName, const std::filesystem::path& logPath, spdlog::level logLevel)
{
	auto sinkFile = std::make_shared<spdlog::sinks::basic_file_sink_st>(logPath.string(), true);
	sinkFile->set_level(static_cast<spdlog::level>(logLevel));
	sinkFile->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] %v");

	auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_st>();
	consoleSink->set_level(spdlog::level::info);
	consoleSink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] %v");

	Logger::_log = std::make_shared<spdlog::logger>(loggerName, spdlog::sinks_init_list({ consoleSink, sinkFile }));
	Logger::_log->set_level(spdlog::level::trace);

	spdlog::register_logger(_log);
}

void
Logger::setLogProperties(std::string loggerName)
{
	_log = spdlog::get(loggerName);
}

Logger&
Logger::getInstance()
{
	static Logger instance;
	return instance;
}

void
Logger::info(std::string msg)
{
	_log->info(msg);
}

void
Logger::warn(std::string msg)
{
	_log->warn(msg);
}

void
Logger::error(std::string msg)
{
	_log->error(msg);
}

void
Logger::debug(std::string msg)
{
	_log->debug(msg);
}

void
Logger::trace(std::string msg)
{
	_log->trace(msg);
}
