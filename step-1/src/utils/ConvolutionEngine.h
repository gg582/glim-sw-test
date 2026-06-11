#pragma once

/**
 * @file ConvolutionEngine.h
 * @brief 3x3 convolution engine with multi-threading and SIMD acceleration.
 */

#include "ImageBuffer.h"

#include <cstdint>
#include <vector>

namespace ip {

class ConvolutionEngine {
public:
    static void apply(ImageBuffer& image,
                      const std::vector<std::vector<int>>& kernel,
                      int divisor,
                      int threadCount);
};

} // namespace ip
