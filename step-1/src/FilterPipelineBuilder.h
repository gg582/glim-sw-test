#pragma once

/**
 * @file FilterPipelineBuilder.h
 * @brief Builder pattern for constructing a sequential filter pipeline.
 */

#include "FilterBase.h"

#include <memory>
#include <vector>

namespace ip {

class FilterPipelineBuilder {
public:
    FilterPipelineBuilder& add(std::unique_ptr<FilterBase> filter);

    void applyAll(ImageBuffer& image) const;

    std::size_t size() const noexcept { return m_filters.size(); }

private:
    std::vector<std::unique_ptr<FilterBase>> m_filters;
};

} // namespace ip
