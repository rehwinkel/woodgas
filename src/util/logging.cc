#include "logging.h"

#include <iostream>
#include <fstream>
#include <ctime>
#include <chrono>

using namespace logging;

Logger::Logger() : log_stream(std::cout) {}

Logger::Logger(std::ostream &file) : log_stream(file) {}

void Logger::set_log_level(LogLevel level) { this->current_level = level; }

void Logger::log_time_string() {
    time_t rawtime;
    struct tm *timeinfo;
    char buffer[80];
    std::time(&rawtime);
    timeinfo = std::localtime(&rawtime);
    std::strftime(buffer, sizeof(buffer), "%H:%M:%S", timeinfo);
    std::string str(buffer);
    std::string ms = std::to_string(std::clock() % 1000);
    this->log_stream << "[" << str << "." << std::string(3 - ms.size(), '0')
                     << ms << "]";
}

void Logger::log(std::string message, LogLevel level) {
    if (this->current_level <= level) {
        this->log_time_string();
        this->log_stream << " ";

        switch (level) {
            case DEBUG:
                this->log_stream << "[DEBUG]\033[0;32m ";
                break;
            case INFO:
                this->log_stream << "[INFO] ";
                break;
            case WARN:
                this->log_stream << "[WARN]\033[0;33m ";
                break;
            case ERROR:
                this->log_stream << "[ERROR]\033[0;31m ";
                break;
        }

        this->log_stream << message << "\033[0m" << std::endl;
    }
}

void Logger::debug(std::string message) { this->log(message, LogLevel::DEBUG); }

void Logger::info(std::string message) { this->log(message, LogLevel::INFO); }

void Logger::warn(std::string message) { this->log(message, LogLevel::WARN); }

void Logger::error(std::string message) { this->log(message, LogLevel::ERROR); }