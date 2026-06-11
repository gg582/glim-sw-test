#pragma once

/**
 * @file FlipFilter.h
 * @brief Horizontal or vertical image flip.
 */

#include "FilterBase.h"
#include <string>

namespace ip {

class FlipFilter : public FilterBase {
public:
    explicit FlipFilter(std::string direction);
    void apply(ImageBuffer& image) const override;
    std::string name() const override { return "Flip:" + m_direction; }

private:
    std::string m_direction;
};

} // namespace ip
