#pragma once

/**
 * @file ResizeFilter.h
 * @brief Resize image using bilinear interpolation.
 */

#include "FilterBase.h"

namespace ip {

class ResizeFilter : public FilterBase {
public:
    ResizeFilter(int newW, int newH);
    void apply(ImageBuffer& image) const override;
    std::string name() const override { return "Resize"; }

private:
    int m_newW, m_newH;
};

} // namespace ip
