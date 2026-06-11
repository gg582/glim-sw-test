/**
 * @file Logger.cpp
 */

#include "Logger.h"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace ip {

Logger::~Logger() {
    close();
}

Logger& Logger::instance() {
    static Logger inst;
    return inst;
}

void Logger::open(const std::string& path) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_opened) m_file.close();
    m_file.open(path, std::ios::out | std::ios::trunc);
    m_opened = m_file.is_open();
}

void Logger::close() {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_opened) {
        m_file.close();
        m_opened = false;
    }
}

void Logger::log(const std::string& message) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!m_opened) return;
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::tm tmBuf{};
    std::tm* tmPtr = std::localtime(&time);
    if (tmPtr) {
        tmBuf = *tmPtr;
    }
    m_file << std::put_time(&tmBuf, "[%Y-%m-%d %H:%M:%S] ") << message << "\n";
    m_file.flush();
}

} // namespace ip
