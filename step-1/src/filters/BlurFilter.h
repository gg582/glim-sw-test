#pragma once

/**
 * @file BlurFilter.h
 * @brief 3x3 box blur using the convolution engine.
 */

#include "FilterBase.h"

namespace ip {

class BlurFilter : public FilterBase {
public:
    explicit BlurFilter(int threadCount = 0);
    void apply(ImageBuffer& image) const override;
    std::string name() const override { return "Blur"; }

private:
    int m_threadCount;
};

} // namespace ip
