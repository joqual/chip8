#ifndef OWL_HPP
#define OWL_HPP

#include <chrono>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>

namespace owl
{

	enum class Level
	{
		DEBUG = 0,
		INFO,
		WARNING,
		ERROR,
		CRITICAL
	};

	class Logger
	{
	public:
		// Singleton
		static Logger& get()
		{
			static Logger instance;
			return instance;
		}

		void set_enabled(bool enable)
		{
			enabled = enable;
		}

		void set_max_level(Level level)
		{
			max_level = level;
		}

		void log(Level level, const std::string& message)
		{
			if (!enabled || level < max_level)
			{
				return;
			}

			std::lock_guard<std::mutex> lock(mtx);
			if (!file.is_open())
			{
				file.open("owl.log", std::ios::app);
			}

			if (file)
			{
				file << "[" << get_timestamp() << "] "
					<< "[" << to_string(level) << "] " << message << std::endl;
			}
		}

	private:
		Logger() : enabled(true), max_level(Level::INFO) {};

		~Logger()
		{
			if (file.is_open())
			{
				file.close();
			}
		}

		bool enabled;
		Level max_level;
		std::ofstream file;
		std::mutex mtx;

		std::string get_timestamp() {
			auto now = std::chrono::system_clock::now();
			auto in_time_t = std::chrono::system_clock::to_time_t(now);
			std::tm bt{}; // Buffer to hold the time safely

#if defined(_MSC_VER)
			localtime_s(&bt, &in_time_t);
#else
			localtime_r(&in_time_t, &bt);
#endif

			std::stringstream ss;
			ss << std::put_time(&bt, "%Y-%m-%d %X");
			return ss.str();
		}

		const char* to_string(Level level)
		{
			switch (level)
			{
			case Level::DEBUG:
				return "DEBUG";
			case Level::INFO:
				return "INFO";
			case Level::WARNING:
				return "WARNING";
			case Level::ERROR:
				return "ERROR";
			case Level::CRITICAL:
				return "CRITICAL";
			default:
				return "UNKNOWN";
			}
		}
	};
} // namespace owl


// Convenient macros
#define OWL_DEBUG(msg) owl::Logger::get().log(owl::Level::DEBUG, msg)
#define OWL_INFO(msg) owl::Logger::get().log(owl::Level::INFO, msg)
#define OWL_WARN(msg) owl::Logger::get().log(owl::Level::WARNING, msg)
#define OWL_ERROR(msg) owl::Logger::get().log(owl::Level::ERROR, msg)
#define OWL_CRITICAL(msg) owl::Logger::get().log(owl::Level::CRITICAL, msg)


#endif