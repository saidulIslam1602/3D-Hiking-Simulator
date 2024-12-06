// log.h

#ifndef LOG_H
#define LOG_H

#include <string>
#include <fstream>
#include <mutex>
#include <ctime>


class Logger {
public:
    
    Logger(const std::string& filename);
    ~Logger();
    void log(const std::string& message);

private:
    std::ofstream logFile; ///< Output file stream for logging.
    std::mutex logMutex;   ///< Mutex for thread-safe logging.
    std::string getCurrentTime() const;
};

#endif // LOG_H
