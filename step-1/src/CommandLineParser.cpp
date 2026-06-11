/**
 * @file CommandLineParser.cpp
 */

#include "CommandLineParser.h"
#include "Exceptions.h"

#include <iostream>
#include <string>

namespace ip {

namespace {
    std::string nextArg(int argc, char* argv[], int& i, const std::string& flag) {
        if (i + 1 >= argc) {
            throw ArgumentError(flag + ": missing value");
        }
        return argv[++i];
    }
}

ProgramOptions CommandLineParser::parse(int argc, char* argv[]) {
    ProgramOptions options;

    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];

        if (arg == "--input" || arg == "-i") {
            options.inputPath = nextArg(argc, argv, i, arg);
        }
        else if (arg == "--output" || arg == "-o") {
            options.outputPath = nextArg(argc, argv, i, arg);
        }
        else if (arg == "--filter" || arg == "-f") {
            options.filterName = nextArg(argc, argv, i, arg);
        }
        else if (arg == "--pipeline" || arg == "-p") {
            options.pipelineString = nextArg(argc, argv, i, arg);
        }
        else if (arg == "--log" || arg == "-l") {
            options.logPath = nextArg(argc, argv, i, arg);
        }
        else if (arg == "--threads" || arg == "-t") {
            options.threadCount = std::stoi(nextArg(argc, argv, i, arg));
            if (options.threadCount < 0) {
                throw ArgumentError("--threads must be non-negative");
            }
        }
        else if (arg == "--crop") {
            options.cropRect = nextArg(argc, argv, i, arg);
        }
        else if (arg == "--resize") {
            options.resizeDim = nextArg(argc, argv, i, arg);
        }
        else if (arg == "--flip") {
            options.flipDirection = nextArg(argc, argv, i, arg);
        }
        else if (arg == "--histogram") {
            options.showHistogram = true;
        }
        else if (arg == "--help" || arg == "-h") {
            printUsage(argv[0]);
            std::exit(0);
        }
        else {
            throw ArgumentError("Unknown option: " + arg);
        }
    }

    if (options.inputPath.empty()) {
        throw ArgumentError("--input is required");
    }
    if (options.outputPath.empty()) {
        throw ArgumentError("--output is required");
    }
    if (options.filterName.empty() && options.pipelineString.empty()
        && options.cropRect.empty() && options.resizeDim.empty()
        && options.flipDirection.empty() && !options.showHistogram) {
        throw ArgumentError("At least one operation (--filter, --pipeline, --crop, --resize, --flip, --histogram) is required");
    }

    return options;
}

void CommandLineParser::printUsage(const std::string& exeName) {
    std::cout
        << "Usage:\n"
        << "  " << exeName << " --input <path> --output <path> [options]\n\n"
        << "Options:\n"
        << "  -i, --input    <path>   Input BMP file (24-bit, uncompressed)\n"
        << "  -o, --output   <path>   Output BMP file\n"
        << "  -f, --filter   <name>   Single filter (grayscale, blur, sharpen, threshold:128)\n"
        << "  -p, --pipeline <string> Filter chain (e.g., \"grayscale,blur,threshold:128\")\n"
        << "      --crop     <rect>   Crop region as x,y,w,h\n"
        << "      --resize   <dim>    Resize to w,h\n"
        << "      --flip     <dir>    horizontal or vertical\n"
        << "      --histogram         Show grayscale histogram in console\n"
        << "  -t, --threads  <n>      Number of threads (0 = auto)\n"
        << "  -l, --log      <path>   Log file path\n"
        << "  -h, --help              Show this message\n\n"
        << "Examples:\n"
        << "  " << exeName << " -i in.bmp -o out.bmp -f grayscale\n"
        << "  " << exeName << " -i in.bmp -o out.bmp -p \"grayscale,blur,threshold:128\"\n"
        << "  " << exeName << " -i in.bmp -o out.bmp --crop 10,10,100,100 --resize 50,50 -f blur\n";
}

} // namespace ip
