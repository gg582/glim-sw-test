/**
 * @file SharpenFilter.cpp
 */

#include "SharpenFilter.h"
#include "utils/ConvolutionEngine.h"

namespace ip {

SharpenFilter::SharpenFilter(int threadCount) : m_threadCount(threadCount) {}

void SharpenFilter::apply(ImageBuffer& image) const {
    std::vector<std::vector<int>> kernel = {
        { 0, -1,  0},
        {-1,  5, -1},
        { 0, -1,  0}
    };
    ConvolutionEngine::apply(image, kernel, 1, m_threadCount);
}

} // namespace ip
