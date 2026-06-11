#pragma once

/**
 * @file Exceptions.h
 * @brief Application-specific exception hierarchy.
 */

#include <stdexcept>
#include <string>

namespace ip {

class BmpParseError : public std::runtime_error {
public:
    explicit BmpParseError(const std::string& msg)
        : std::runtime_error("[BMP] " + msg) {}
};

class FilterError : public std::runtime_error {
public:
    explicit FilterError(const std::string& msg)
        : std::runtime_error("[Filter] " + msg) {}
};

class ArgumentError : public std::runtime_error {
public:
    explicit ArgumentError(const std::string& msg)
        : std::runtime_error("[Argument] " + msg) {}
};

class PipelineError : public std::runtime_error {
public:
    explicit PipelineError(const std::string& msg)
        : std::runtime_error("[Pipeline] " + msg) {}
};

} // namespace ip
