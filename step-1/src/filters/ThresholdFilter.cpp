/**
 * @file ThresholdFilter.cpp
 */

#include "ThresholdFilter.h"
#include "GrayscaleFilter.h"
#include "Exceptions.h"

namespace ip {

ThresholdFilter::ThresholdFilter(int threshold) : m_threshold(threshold) {
    if (threshold < 0 || threshold > 255) {
        throw FilterError("Threshold must be in [0, 255]");
    }
}

void ThresholdFilter::apply(ImageBuffer& image) const {
    GrayscaleFilter gray;
    gray.apply(image);
    const std::size_t pixels = static_cast<std::size_t>(image.width()) * image.height();
    std::uint8_t* data = image.data();
    for (std::size_t i = 0; i < pixels; ++i) {
        std::uint8_t val = data[i * 3];
        std::uint8_t out = (val >= m_threshold) ? 255 : 0;
        data[i * 3 + 0] = out;
        data[i * 3 + 1] = out;
        data[i * 3 + 2] = out;
    }
}

} // namespace ip
