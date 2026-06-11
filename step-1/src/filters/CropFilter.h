#pragma once

/**
 * @file CropFilter.h
 * @brief Crop image to a specified rectangular region.
 */

#include "FilterBase.h"

namespace ip {

class CropFilter : public FilterBase {
public:
    CropFilter(int x, int y, int w, int h);
    void apply(ImageBuffer& image) const override;
    std::string name() const override { return "Crop"; }

private:
    int m_x, m_y, m_w, m_h;
};

} // namespace ip
