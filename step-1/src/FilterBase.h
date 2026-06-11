#pragma once

/**
 * @file FilterBase.h
 * @brief Abstract base class for all image filters (Strategy Pattern).
 */

#include "ImageBuffer.h"

#include <string>

namespace ip {

class FilterBase {
public:
    virtual ~FilterBase() = default;

    /// Apply the filter to the image in-place.
    virtual void apply(ImageBuffer& image) const = 0;

    /// Human-readable name of the filter.
    virtual std::string name() const = 0;
};

} // namespace ip
