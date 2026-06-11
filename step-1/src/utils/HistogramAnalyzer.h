#pragma once

/**
 * @file HistogramAnalyzer.h
 * @brief Pixel distribution histogram analysis and console visualization.
 */

#include "ImageBuffer.h"

namespace ip {

class HistogramAnalyzer {
public:
    static void analyzeAndPrint(const ImageBuffer& image);
};

} // namespace ip
