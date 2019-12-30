#pragma once
//在  spdlog.h   之前定义，才有效
#ifndef SPDLOG_TRACE_ON
#define SPDLOG_TRACE_ON
#endif

#ifndef SPDLOG_DEBUG_ON
#define SPDLOG_DEBUG_ON
#endif
#include "spdlog/spdlog.h"
#include "spdlog/async.h" //support for async logging
#include "spdlog/sinks/daily_file_sink.h"
#include "spdlog/fmt/bin_to_hex.h"
#include<memory>
using namespace spdlog;
class Logger
{
public:
	static Logger& GetInstance() {
		static Logger m_instance;
		return m_instance;
	}
	auto GetLogger() { return nml_logger; }
private:
	Logger()
	{
		nml_logger = daily_logger_mt("Logger", "Log/Logger.log", 0, 0);

		// 设置日志记录级别
#ifdef _DEBUG
		nml_logger->set_level(spdlog::level::info);
		nml_logger->flush_on(spdlog::level::info);
#else
		nml_logger->set_level(spdlog::level::err);
		nml_logger->flush_on(spdlog::level::err);
#endif
		//spdlog::flush_every(std::chrono::seconds(3));
	}
	~Logger() {
		spdlog::drop_all();
	}
	Logger(const Logger&) = delete;
	Logger& operator=(const Logger&) = delete;

private:
	std::shared_ptr<spdlog::logger> nml_logger;
};

#define LTrace(msg,...) Logger::GetInstance().GetLogger()->trace(suffix(msg),__VA_ARGS__)
#define LDebug(...) Logger::GetInstance().GetLogger()->debug(__VA_ARGS__)
#define LInfo(...) Logger::GetInstance().GetLogger()->info(__VA_ARGS__)
#define LWarn(...) Logger::GetInstance().GetLogger()->warn(__VA_ARGS__)
#define LError(...) Logger::GetInstance().GetLogger()->error(__VA_ARGS__)
#define LCritical(...) Logger::GetInstance().GetLogger()->critical(__VA_ARGS__)

#define criticalif(b, ...)                        \
    do {                                       \
        if ((b)) {                             \
           Logger::GetInstance().GetLogger()->critical(__VA_ARGS__); \
        }                                      \
    } while (0)

