#pragma once

/**
 * @file GrayscaleFilter.h
 * @brief Converts an image to grayscale using ITU-R BT.601 coefficients.
 */

#include "FilterBase.h"

namespace ip {

class GrayscaleFilter : public FilterBase {
public:
    void apply(ImageBuffer& image) const override;
    std::string name() const override { return "Grayscale"; }
};

} // namespace ip
