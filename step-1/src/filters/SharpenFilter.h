#pragma once

/**
 * @file SharpenFilter.h
 * @brief 3x3 sharpening kernel using the convolution engine.
 */

#include "FilterBase.h"

namespace ip {

class SharpenFilter : public FilterBase {
public:
    explicit SharpenFilter(int threadCount = 0);
    void apply(ImageBuffer& image) const override;
    std::string name() const override { return "Sharpen"; }

private:
    int m_threadCount;
};

} // namespace ip
