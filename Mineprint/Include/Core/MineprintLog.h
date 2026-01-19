#pragma once

// MW @gotcha: This file should only be included in the PCH, or else the Linux build will fail due to redefinition
//             fmt overloads at the end of the file.

// Disable warnings caused by fmt overloads.
// ReSharper disable CppMemberFunctionMayBeStatic

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/fmt/ostr.h" // Needed for logging some types

extern std::shared_ptr<spdlog::logger> g_MineprintLogger;

void InitLog(const char *prefPath);
void ShutdownLog();
void AddSinkToLog(const spdlog::sink_ptr &sink);

#ifndef MP_NO_LOG
	#define MP_TRACE(format, ...)                   g_MineprintLogger->trace(format "\n" __VA_OPT__(,) __VA_ARGS__)
	#define MP_INFO(format, ...)                    g_MineprintLogger->info(format "\n" __VA_OPT__(,) __VA_ARGS__)
	#define MP_INFO(format, ...)                    g_MineprintLogger->info(format "\n" __VA_OPT__(,) __VA_ARGS__)
	#define MP_WARN(format, ...)                    g_MineprintLogger->warn(format "\n" __VA_OPT__(,) __VA_ARGS__)
	#define MP_ERROR(format, ...)                   g_MineprintLogger->error(format "\n" __VA_OPT__(,) __VA_ARGS__)
	#define MP_CRITICAL(format, ...)                g_MineprintLogger->critical(format "\n" __VA_OPT__(,) __VA_ARGS__)

	#define MP_TRACE_NO_NEWLINE(format, ...)        g_MineprintLogger->trace(format __VA_OPT__(,) __VA_ARGS__)
	#define MP_INFO_NO_NEWLINE(format, ...)         g_MineprintLogger->info(format __VA_OPT__(,) __VA_ARGS__)
	#define MP_WARN_NO_NEWLINE(format, ...)         g_MineprintLogger->warn(format __VA_OPT__(,) __VA_ARGS__)
	#define MP_ERROR_NO_NEWLINE(format, ...)        g_MineprintLogger->error(format __VA_OPT__(,) __VA_ARGS__)
	#define MP_CRITICAL_NO_NEWLINE(format, ...)     g_MineprintLogger->critical(format __VA_OPT__(,) __VA_ARGS__)
#else
	#define MP_TRACE(...)      
	#define MP_INFO(...)       
	#define MP_WARN(...)       
	#define MP_ERROR(...)      
	#define MP_CRITICAL(...)

	#define MP_TRACE_NO_NEWLINE(format, ...)     
	#define MP_INFO_NO_NEWLINE(format, ...)      
	#define MP_WARN_NO_NEWLINE(format, ...)      
	#define MP_ERROR_NO_NEWLINE(format, ...)     
	#define MP_CRITICAL_NO_NEWLINE(format, ...)  
#endif
