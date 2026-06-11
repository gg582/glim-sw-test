/**
 * @file GrayscaleFilter.cpp
 */

#include "GrayscaleFilter.h"

namespace ip {

void GrayscaleFilter::apply(ImageBuffer& image) const {
    const int w = image.width();
    const int h = image.height();
    std::uint8_t* data = image.data();
    const std::size_t pixels = static_cast<std::size_t>(w) * h;
    for (std::size_t i = 0; i < pixels; ++i) {
        std::uint8_t b = data[i * 3 + 0];
        std::uint8_t g = data[i * 3 + 1];
        std::uint8_t r = data[i * 3 + 2];
        // ITU-R BT.601 fixed-point: (77*R + 150*G + 29*B) >> 8
        std::uint8_t gray = static_cast<std::uint8_t>(
            (77 * r + 150 * g + 29 * b) >> 8);
        data[i * 3 + 0] = gray;
        data[i * 3 + 1] = gray;
        data[i * 3 + 2] = gray;
    }
}

} // namespace ip
