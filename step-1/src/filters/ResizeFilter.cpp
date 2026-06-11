/**
 * @file ResizeFilter.cpp
 */

#include "ResizeFilter.h"
#include "Exceptions.h"

#include <algorithm>
#include <cmath>

namespace ip {

ResizeFilter::ResizeFilter(int newW, int newH) : m_newW(newW), m_newH(newH) {
    if (newW <= 0 || newH <= 0) {
        throw FilterError("Resize dimensions must be positive");
    }
}

void ResizeFilter::apply(ImageBuffer& image) const {
    const int srcW = image.width();
    const int srcH = image.height();
    if (srcW == m_newW && srcH == m_newH) return;

    ImageBuffer dst(m_newW, m_newH);
    const std::uint8_t* srcData = image.data();
    std::uint8_t* dstData = dst.data();

    const float xRatio = static_cast<float>(srcW) / m_newW;
    const float yRatio = static_cast<float>(srcH) / m_newH;

    for (int y = 0; y < m_newH; ++y) {
        for (int x = 0; x < m_newW; ++x) {
            float sx = x * xRatio;
            float sy = y * yRatio;
            int x0 = static_cast<int>(sx);
            int y0 = static_cast<int>(sy);
            int x1 = std::min(x0 + 1, srcW - 1);
            int y1 = std::min(y0 + 1, srcH - 1);
            float fx = sx - x0;
            float fy = sy - y0;

            for (int c = 0; c < 3; ++c) {
                float v00 = srcData[(y0 * srcW + x0) * 3 + c];
                float v10 = srcData[(y0 * srcW + x1) * 3 + c];
                float v01 = srcData[(y1 * srcW + x0) * 3 + c];
                float v11 = srcData[(y1 * srcW + x1) * 3 + c];
                float val = (1.0f - fx) * (1.0f - fy) * v00 +
                            fx * (1.0f - fy) * v10 +
                            (1.0f - fx) * fy * v01 +
                            fx * fy * v11;
                int iv = static_cast<int>(std::round(val));
                dstData[(y * m_newW + x) * 3 + c] = static_cast<std::uint8_t>(std::clamp(iv, 0, 255));
            }
        }
    }

    image = std::move(dst);
}

} // namespace ip
