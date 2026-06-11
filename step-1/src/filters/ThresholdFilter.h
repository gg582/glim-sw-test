#pragma once

/**
 * @file ThresholdFilter.h
 * @brief Binarizes an image based on a given threshold.
 */

#include "FilterBase.h"

namespace ip {

class ThresholdFilter : public FilterBase {
public:
    explicit ThresholdFilter(int threshold = 128);
    void apply(ImageBuffer& image) const override;
    std::string name() const override { return "Threshold:" + std::to_string(m_threshold); }

private:
    int m_threshold;
};

} // namespace ip
