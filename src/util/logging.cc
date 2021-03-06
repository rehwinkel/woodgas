#include "logging.h"

#include <iostream>
#include <fstream>
#include <ctime>
#include <chrono>

using namespace logging;

Logger::Logger()
    : out_stream(std::cout), void_stream(0), current_level(LogLevel::DEBUG) {}

Logger::Logger(std::ostream &file)
    : out_stream(file), void_stream(0), current_level(LogLevel::DEBUG) {}

void Logger::set_log_level(LogLevel level) { this->current_level = level; }

void Logger::log_time_string() {
    time_t rawtime;
    struct tm *timeinfo;
    char buffer[80];
    std::time(&rawtime);
    timeinfo = std::localtime(&rawtime);
    std::strftime(buffer, sizeof(buffer), "%H:%M:%S", timeinfo);
    this->out_stream << "[" << buffer << "]";
}

void Logger::log_level_color(LogLevel level) {
    switch (level) {
        case DEBUG:
            this->out_stream << "\033[0;32m";
            break;
        case INFO:
            this->out_stream << "";
            break;
        case WARN:
            this->out_stream << "\033[0;33m";
            break;
        case ERROR:
            this->out_stream << "\033[0;31m";
            break;
    }
}

void Logger::log_level_name(LogLevel level) {
    switch (level) {
        case DEBUG:
            this->out_stream << "[DEBUG]";
            break;
        case INFO:
            this->out_stream << "[INFO]";
            break;
        case WARN:
            this->out_stream << "[WARN]";
            break;
        case ERROR:
            this->out_stream << "[ERROR]";
            break;
    }
}

void Logger::log(std::string message, LogLevel level) {
    if (this->current_level <= level) {
        this->log_level_color(level);
        this->log_time_string();
        this->out_stream << " ";
        this->log_level_name(level);

        this->out_stream << " " << message << "\033[0m" << std::endl;
    }
}

std::ostream &Logger::log_stream(LogLevel level) {
    if (this->current_level <= level) {
        this->log_level_color(level);
        this->log_time_string();
        this->out_stream << " ";
        this->log_level_name(level);
        this->out_stream << " ";

        return this->out_stream;
    }
    return this->void_stream;
}

void Logger::debug(std::string message) { this->log(message, LogLevel::DEBUG); }

void Logger::info(std::string message) { this->log(message, LogLevel::INFO); }

void Logger::warn(std::string message) { this->log(message, LogLevel::WARN); }

void Logger::error(std::string message) { this->log(message, LogLevel::ERROR); }

std::ostream &Logger::debug_stream() {
    return this->log_stream(LogLevel::DEBUG);
}

std::ostream &Logger::info_stream() { return this->log_stream(LogLevel::INFO); }

std::ostream &Logger::warn_stream() { return this->log_stream(LogLevel::WARN); }

std::ostream &Logger::error_stream() {
    return this->log_stream(LogLevel::ERROR);
}