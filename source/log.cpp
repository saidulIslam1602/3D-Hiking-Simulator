// log.cpp

#include "log.h"
#include <iostream>

Logger::Logger(const std::string& filename) {
    logFile.open(filename, std::ios::out | std::ios::app);
    if (!logFile.is_open()) {
        std::cerr << "ERROR: Unable to open log file: " << filename << std::endl;
    }
}

Logger::~Logger() {
    if (logFile.is_open()) {
        logFile.close();
    }
}

void Logger::log(const std::string& message) {
    std::lock_guard<std::mutex> lock(logMutex);
    if (logFile.is_open()) {
        logFile << "[" << getCurrentTime() << "] " << message << std::endl;
    }
    std::cout << message << std::endl; // Also output to console
}

std::string Logger::getCurrentTime() const {
    std::time_t now = std::time(nullptr);
    char buf[20]; // Enough for "YYYY-MM-DD HH:MM:SS"
#ifdef _WIN32
    tm timeInfo;
    localtime_s(&timeInfo, &now); // Use localtime_s for thread safety
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &timeInfo);
#else
    tm* timeInfo = std::localtime(&now);
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", timeInfo);
#endif
    return std::string(buf);
}
