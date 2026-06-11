#pragma once

/**
 * @file CommandLineParser.h
 * @brief Command-line argument parsing.
 */

#include <string>
#include <vector>

namespace ip {

struct ProgramOptions {
    std::string inputPath;
    std::string outputPath;
    std::string filterName;
    std::string pipelineString;
    std::string logPath;
    std::string cropRect;
    std::string resizeDim;
    std::string flipDirection;
    bool showHistogram = false;
    int threadCount = 0; // 0 = auto
};

class CommandLineParser {
public:
    CommandLineParser() = delete;

    static ProgramOptions parse(int argc, char* argv[]);
    static void printUsage(const std::string& exeName);
};

} // namespace ip
