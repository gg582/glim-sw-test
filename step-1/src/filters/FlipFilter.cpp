/**
 * @file FlipFilter.cpp
 */

#include "FlipFilter.h"
#include "Exceptions.h"

#include <algorithm>
#include <cstring>

namespace ip {

FlipFilter::FlipFilter(std::string direction) : m_direction(std::move(direction)) {
    if (m_direction != "horizontal" && m_direction != "vertical") {
        throw FilterError("Flip direction must be horizontal or vertical");
    }
}

void FlipFilter::apply(ImageBuffer& image) const {
    const int w = image.width();
    const int h = image.height();
    const int stride = image.rowStride();
    std::vector<std::uint8_t> tmp(stride);
    if (m_direction == "horizontal") {
        for (int y = 0; y < h; ++y) {
            std::uint8_t* row = image.rowPtr(y);
            for (int x = 0; x < w / 2; ++x) {
                int left = x * 3;
                int right = (w - 1 - x) * 3;
                std::swap_ranges(row + left, row + left + 3, row + right);
            }
        }
    } else {
        for (int y = 0; y < h / 2; ++y) {
            std::uint8_t* top = image.rowPtr(y);
            std::uint8_t* bottom = image.rowPtr(h - 1 - y);
            std::memcpy(tmp.data(), top, stride);
            std::memcpy(top, bottom, stride);
            std::memcpy(bottom, tmp.data(), stride);
        }
    }
}

} // namespace ip
