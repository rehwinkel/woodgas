#include "logging.h"

#include <iostream>
#include <fstream>
#include <ctime>
#include <chrono>

using namespace logging;

Logger::Logger() : log_stream(std::cout) {}

Logger::Logger(std::ostream &file) : log_stream(file) {}

void Logger::set_log_level(LogLevel level) { this->current_level = level; }

void Logger::log(std::string message, LogLevel level) {
    if (this->current_level <= level) {
        time_t rawtime;
        struct tm *timeinfo;
        char buffer[80];

        std::time(&rawtime);
        timeinfo = std::localtime(&rawtime);

        std::strftime(buffer, sizeof(buffer), "[%H:%M:%S", timeinfo);
        std::string str(buffer);

        int ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                     std::chrono::time_point_cast<std::chrono::milliseconds>(
                         std::chrono::system_clock::now())
                         .time_since_epoch())
                     .count() %
                 1000;

        std::string ms_string = std::to_string(ms);
        std::string dest =
            std::string(3 - ms_string.size(), '0').append(ms_string);

        this->log_stream << str << "." << dest << "] ";

        switch (level) {
            case DEBUG:
                this->log_stream << "[DEBUG] "
                                 << "\033[0;32m" << message << "\033[0m";
                break;
            case INFO:
                this->log_stream << "[INFO]  " << message;
                break;
            case WARN:
                this->log_stream << "[WARN]  "
                                 << "\033[0;33m" << message << "\033[0m";
                break;
            case ERROR:
                this->log_stream << "[ERROR] "
                                 << "\033[0;31m" << message << "\033[0m";
                break;
        }

        this->log_stream << std::endl;
    }
}

void Logger::debug(std::string message) { this->log(message, LogLevel::DEBUG); }

void Logger::info(std::string message) { this->log(message, LogLevel::INFO); }

void Logger::warn(std::string message) { this->log(message, LogLevel::WARN); }

void Logger::error(std::string message) { this->log(message, LogLevel::ERROR); }