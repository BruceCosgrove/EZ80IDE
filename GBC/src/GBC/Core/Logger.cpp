#include "gbcpch.h"
#include "Logger.h"
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace gbc
{
	Ref<spdlog::logger> Logger::coreLogger;
	Ref<spdlog::logger> Logger::clientLogger;

	void Logger::Init()
	{
		std::vector<spdlog::sink_ptr> sinks;
		sinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>())->set_pattern("%^[%T] %n: %v%$");
		sinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("GBC.log", true))->set_pattern("[%T] [%l] %n: %v");

		coreLogger = std::make_shared<spdlog::logger>("GBC", sinks.begin(), sinks.end());
		spdlog::register_logger(coreLogger);
		coreLogger->set_level(spdlog::level::trace);
		coreLogger->flush_on(spdlog::level::trace);

		clientLogger = std::make_shared<spdlog::logger>("APP", sinks.begin(), sinks.end());
		spdlog::register_logger(clientLogger);
		clientLogger->set_level(spdlog::level::trace);
		clientLogger->flush_on(spdlog::level::trace);
	}
}
