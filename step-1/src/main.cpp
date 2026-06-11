/**
 * @file main.cpp
 * @brief Entry point for the CLI Image Processor.
 *
 * Supports single filters, pipelines, geometric transforms,
 * histogram visualization, logging, and multi-threaded SIMD convolution.
 */

#include "BmpParser.h"
#include "CommandLineParser.h"
#include "ImageBuffer.h"
#include "Exceptions.h"
#include "FilterBase.h"
#include "FilterPipelineBuilder.h"
#include "Logger.h"
#include "utils/HistogramAnalyzer.h"

#include "filters/GrayscaleFilter.h"
#include "filters/ThresholdFilter.h"
#include "filters/BlurFilter.h"
#include "filters/SharpenFilter.h"
#include "filters/FlipFilter.h"
#include "filters/CropFilter.h"
#include "filters/ResizeFilter.h"

#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>

namespace ip {

std::unique_ptr<FilterBase> createFilter(const std::string& name, int threadCount) {
    if (name == "grayscale") {
        return std::make_unique<GrayscaleFilter>();
    }
    else if (name == "blur") {
        return std::make_unique<BlurFilter>(threadCount);
    }
    else if (name == "sharpen") {
        return std::make_unique<SharpenFilter>(threadCount);
    }
    else if (name.rfind("threshold:", 0) == 0) {
        int val = std::stoi(name.substr(10));
        return std::make_unique<ThresholdFilter>(val);
    }
    else if (name.rfind("flip:", 0) == 0) {
        return std::make_unique<FlipFilter>(name.substr(5));
    }
    else if (name.rfind("crop:", 0) == 0) {
        std::string rect = name.substr(5);
        std::replace(rect.begin(), rect.end(), ',', ' ');
        std::istringstream iss(rect);
        int x, y, w, h;
        if (!(iss >> x >> y >> w >> h)) {
            throw FilterError("Invalid crop format: " + name);
        }
        return std::make_unique<CropFilter>(x, y, w, h);
    }
    else if (name.rfind("resize:", 0) == 0) {
        std::string dim = name.substr(7);
        std::replace(dim.begin(), dim.end(), ',', ' ');
        std::istringstream iss(dim);
        int w, h;
        if (!(iss >> w >> h)) {
            throw FilterError("Invalid resize format: " + name);
        }
        return std::make_unique<ResizeFilter>(w, h);
    }
    throw FilterError("Unknown filter: " + name);
}

void parsePipeline(const std::string& pipelineStr,
                   FilterPipelineBuilder& builder,
                   int threadCount)
{
    std::string token;
    std::istringstream iss(pipelineStr);
    while (std::getline(iss, token, ',')) {
        size_t start = token.find_first_not_of(" \t\r\n");
        size_t end = token.find_last_not_of(" \t\r\n");
        if (start == std::string::npos) continue;
        std::string name = token.substr(start, end - start + 1);
        if (!name.empty()) {
            builder.add(createFilter(name, threadCount));
        }
    }
}

} // namespace ip

int main(int argc, char* argv[]) {
    try {
        const ip::ProgramOptions options = ip::CommandLineParser::parse(argc, argv);

        if (!options.logPath.empty()) {
            ip::Logger::instance().open(options.logPath);
        }
        ip::Logger::instance().log("Started processing: " + options.inputPath);

        ip::ImageBuffer image = ip::BmpParser::loadFromFile(options.inputPath);
        ip::Logger::instance().log("Loaded: " + std::to_string(image.width()) +
                                   "x" + std::to_string(image.height()));

        ip::FilterPipelineBuilder pipeline;

        if (!options.cropRect.empty()) {
            std::string rect = options.cropRect;
            std::replace(rect.begin(), rect.end(), ',', ' ');
            std::istringstream iss(rect);
            int x, y, w, h;
            if (iss >> x >> y >> w >> h) {
                pipeline.add(std::make_unique<ip::CropFilter>(x, y, w, h));
            }
        }
        if (!options.resizeDim.empty()) {
            std::string dim = options.resizeDim;
            std::replace(dim.begin(), dim.end(), ',', ' ');
            std::istringstream iss(dim);
            int w, h;
            if (iss >> w >> h) {
                pipeline.add(std::make_unique<ip::ResizeFilter>(w, h));
            }
        }
        if (!options.flipDirection.empty()) {
            pipeline.add(std::make_unique<ip::FlipFilter>(options.flipDirection));
        }

        if (!options.pipelineString.empty()) {
            ip::parsePipeline(options.pipelineString, pipeline, options.threadCount);
        }
        else if (!options.filterName.empty()) {
            pipeline.add(ip::createFilter(options.filterName, options.threadCount));
        }

        pipeline.applyAll(image);

        if (options.showHistogram) {
            ip::HistogramAnalyzer::analyzeAndPrint(image);
        }

        ip::BmpParser::saveToFile(options.outputPath, image);
        ip::Logger::instance().log("Saved: " + options.outputPath);

        return 0;
    }
    catch (const ip::ArgumentError& e) {
        std::cerr << e.what() << "\n\n";
        ip::CommandLineParser::printUsage(argc > 0 ? argv[0] : "ImageProcessor");
        return 4;
    }
    catch (const ip::BmpParseError& e) {
        std::cerr << e.what() << std::endl;
        return 2;
    }
    catch (const ip::FilterError& e) {
        std::cerr << e.what() << std::endl;
        return 3;
    }
    catch (const std::exception& e) {
        std::cerr << "Unexpected error: " << e.what() << std::endl;
        return 1;
    }
}
