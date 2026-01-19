#include "mppch.h"

#include <filesystem>
#include <iomanip>
#include <spdlog/sinks/basic_file_sink.h>

std::shared_ptr<spdlog::logger> g_MineprintLogger;

void InitLog(const char* prefPath)
{
#ifdef MP_NO_LOG
	return;
#endif

	if (g_MineprintLogger)
	{
		// InitLog() is still valid if called when the log is initialised;
		// when we restart the application, we don't want to re-initialise the log
		// as that'll create a new log file.
		// Therefore, if the log is already initialised, we'll just silently return.
		return;
	}
	
	spdlog::set_pattern("%^[%T] %n: %v%$");

	std::vector<spdlog::sink_ptr> sinks;

	std::stringstream buffer;
	const std::time_t t  = std::time(nullptr);
	const std::tm     tm = *std::localtime(&t);
	buffer << std::put_time(&tm, "%d-%m-%Y_%H-%M-%S");
	// Ensure the logs folder exists inside the pref path (if it exists), as spdlog fails to create the file if the folder does not exist AND it is an absolute path.
	if (prefPath)
		std::filesystem::create_directories(fmt::format("{}Logs", prefPath));
	std::string path = fmt::format("{}Logs{}{}.txt", prefPath != nullptr ? prefPath : "",
	                               static_cast<char>(std::filesystem::path::preferred_separator), buffer.str());

	sinks.push_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>(path));
	sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());

	g_MineprintLogger = std::make_shared<spdlog::logger>("Mineprint", std::begin(sinks), std::end(sinks));
	g_MineprintLogger->set_level(spdlog::level::trace);
}

void ShutdownLog()
{
#ifdef MP_NO_LOG
	return;
#endif

	MP_ASSERT(g_MineprintLogger, "Shutting down log when it is not initialised");
	g_MineprintLogger->info("Shutting down log");
	g_MineprintLogger = nullptr;
}

void AddSinkToLog(const spdlog::sink_ptr& sink)
{
	// Don't worry - the assertion macro will check if the logger is null.
	MP_ASSERT(g_MineprintLogger, "Must initialise the logger before adding sinks to it");

	g_MineprintLogger->sinks().push_back(sink);
}
