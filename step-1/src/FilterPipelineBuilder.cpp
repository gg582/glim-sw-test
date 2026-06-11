/**
 * @file FilterPipelineBuilder.cpp
 */

#include "FilterPipelineBuilder.h"
#include "Logger.h"

namespace ip {

FilterPipelineBuilder& FilterPipelineBuilder::add(std::unique_ptr<FilterBase> filter) {
    if (filter) {
        m_filters.push_back(std::move(filter));
    }
    return *this;
}

void FilterPipelineBuilder::applyAll(ImageBuffer& image) const {
    for (const auto& filter : m_filters) {
        Logger::instance().log("Applying filter: " + filter->name());
        filter->apply(image);
    }
}

} // namespace ip
