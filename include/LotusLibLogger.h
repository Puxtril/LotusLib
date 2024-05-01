#pragma once

#include <filesystem>

#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace LotusLib
{
	// Must call LotusLib::Logger::setLogProperties to log to file, otherwise 
	//  logging will only stream to stdout
	class Logger
	{
		Logger();
		Logger(Logger&);
		void operator=(Logger&);

	public:
		// This should only be called once
		static void setLogProperties(spdlog::level::level_enum consoleLogLevel);
		static void setLogProperties(const std::filesystem::path& logPath, spdlog::level::level_enum logPathLevel, spdlog::level::level_enum consoleLogLevel);

		static void error(const std::string& msg);
		static void warn(const std::string& msg);
		static void info(const std::string& msg);
		static void debug(const std::string& msg);
		static void trace(const std::string& msg);

	private:
		// Handle static initilization of logging data
		class LoggerWrapper
		{
		public:
		#ifdef NDEBUG
			inline constexpr static spdlog::level::level_enum m_logLevel = spdlog::level::warn;
		#else
			inline constexpr static spdlog::level::level_enum m_logLevel = spdlog::level::trace;
		#endif

			inline static std::shared_ptr<spdlog::logger> log;
			
			LoggerWrapper();
			LoggerWrapper(std::shared_ptr<spdlog::logger> logger);
		};

	protected:
		inline static LoggerWrapper _logWrapper;
	};

	static inline void logError(const std::string& msg) { LotusLib::Logger::error(msg); }
	static inline void logWarn(const std::string& msg) { LotusLib::Logger::warn(msg); }
	static inline void logInfo(const std::string& msg) { LotusLib::Logger::info(msg); }
	static inline void logDebug(const std::string& msg) { LotusLib::Logger::debug(msg); }
	static inline void logTrace(const std::string& msg) { LotusLib::Logger::trace(msg); }
}
