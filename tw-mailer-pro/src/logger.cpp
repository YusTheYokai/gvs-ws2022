#include "logger.h"

#include <chrono>
#include <ctime>
#include <iostream>
#include <string.h>

namespace chrono = std::chrono;

void Logger::plain(std::string message) {
    std::cout << message << std::endl;
}

void Logger::info(std::string message) {
    std::cout << time() << " - " << "[ INFO  ] " << message << std::endl;
}

void Logger::warn(std::string message) {
    std::cout << "\033[1;33m" << time() << " - " << "[WARNING] " << message << "\033[0m" << std::endl;
}

void Logger::error(std::string message) {
    std::cerr << "\033[1;31m" << time() << " - " << "[ ERROR ] " << message << "\033[0m" << std::endl;
}

void Logger::success(std::string message) {
    std::cout << "\033[1;32m" << time() << " - " << "[SUCCESS] " << message << "\033[0m" << std::endl;
}

std::string Logger::time() {
    auto now = chrono::system_clock::now();
    auto in_time_t = chrono::system_clock::to_time_t(now);
    char* time = std::ctime(&in_time_t);
    time[strlen(time) - 1] = '\0';
    return time;
}
