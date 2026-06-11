/**
 * @file BlurFilter.cpp
 */

#include "BlurFilter.h"
#include "utils/ConvolutionEngine.h"

namespace ip {

BlurFilter::BlurFilter(int threadCount) : m_threadCount(threadCount) {}

void BlurFilter::apply(ImageBuffer& image) const {
    std::vector<std::vector<int>> kernel = {
        {1, 1, 1},
        {1, 1, 1},
        {1, 1, 1}
    };
    ConvolutionEngine::apply(image, kernel, 9, m_threadCount);
}

} // namespace ip
