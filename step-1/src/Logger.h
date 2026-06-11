#pragma once

/**
 * @file Logger.h
 * @brief Thread-safe file logger (Singleton).
 */

#include <fstream>
#include <mutex>
#include <string>

namespace ip {

class Logger {
public:
    static Logger& instance();

    void open(const std::string& path);
    void close();
    void log(const std::string& message);

private:
    Logger() = default;
    ~Logger();

    std::ofstream m_file;
    std::mutex m_mutex;
    bool m_opened = false;
};

} // namespace ip
