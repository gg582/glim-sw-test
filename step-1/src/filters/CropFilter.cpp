/**
 * @file CropFilter.cpp
 */

#include "CropFilter.h"
#include "Exceptions.h"

#include <cstring>

namespace ip {

CropFilter::CropFilter(int x, int y, int w, int h)
    : m_x(x), m_y(y), m_w(w), m_h(h)
{
    if (w <= 0 || h <= 0) {
        throw FilterError("Crop dimensions must be positive");
    }
}

void CropFilter::apply(ImageBuffer& image) const {
    const int srcW = image.width();
    const int srcH = image.height();
    if (m_x < 0 || m_y < 0 || m_x + m_w > srcW || m_y + m_h > srcH) {
        throw FilterError("Crop region out of bounds");
    }
    ImageBuffer result(m_w, m_h);
    for (int y = 0; y < m_h; ++y) {
        const std::uint8_t* srcRow = image.rowPtr(m_y + y) + m_x * 3;
        std::uint8_t* dstRow = result.rowPtr(y);
        std::memcpy(dstRow, srcRow, result.rowStride());
    }
    image = std::move(result);
}

} // namespace ip
