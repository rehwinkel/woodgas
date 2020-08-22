// header for the logging API

#pragma once

#include <ostream>

namespace logging {
    enum LogLevel { DEBUG, INFO, WARN, ERROR };

    class Logger {
        std::ostream &out_stream;
        std::ostream void_stream;
        LogLevel current_level;
        void log_time_string();

       public:
        Logger();                    // log to console
        Logger(std::ostream &file);  // log to file
        void set_log_level(LogLevel level);
        void log(std::string message, LogLevel level);
        std::ostream &log_stream(LogLevel level);
        void debug(std::string message);
        void info(std::string message);
        void warn(std::string message);
        void error(std::string message);
        std::ostream &debug_stream();
        std::ostream &info_stream();
        std::ostream &warn_stream();
        std::ostream &error_stream();
    };
}