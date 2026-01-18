#include "LotusLib/Logger.h"

using namespace LotusLib;

void
Logger::setLogProperties(spdlog::level::level_enum level)
{
	_logWrapper.log->set_level(level);
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

	log = std::make_shared<spdlog::logger>("LotusLib", spdlog::sinks_init_list({ consoleSink }));
	log->set_level(m_logLevel);

	spdlog::register_logger(log);
}

Logger::LoggerWrapper::LoggerWrapper(std::shared_ptr<spdlog::logger> logger)
{
	log = logger;
}