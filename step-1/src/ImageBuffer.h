#pragma once

/**
 * @file ImageBuffer.h
 * @brief Container class for 24-bit BGR pixel data.
 */

#include <cstdint>
#include <vector>

namespace ip {

/**
 * @brief RAII container for 24-bit BGR image pixel data.
 *
 * Pixel data is stored in row-major, top-down order without row padding.
 * The 4-byte alignment required by BMP is handled by BmpParser.
 */
class ImageBuffer {
public:
    static constexpr int CHANNELS = 3; ///< B, G, R

    ImageBuffer() = default;
    ImageBuffer(int width, int height);

    ImageBuffer(const ImageBuffer&) = default;
    ImageBuffer(ImageBuffer&&) noexcept = default;
    ImageBuffer& operator=(const ImageBuffer&) = default;
    ImageBuffer& operator=(ImageBuffer&&) noexcept = default;
    ~ImageBuffer() = default;

    int width() const noexcept { return m_width; }
    int height() const noexcept { return m_height; }
    int rowStride() const noexcept { return m_width * CHANNELS; }
    std::size_t dataSize() const noexcept { return m_data.size(); }
    bool empty() const noexcept { return m_data.empty(); }

    std::uint8_t* data() noexcept { return m_data.data(); }
    const std::uint8_t* data() const noexcept { return m_data.data(); }

    std::uint8_t* rowPtr(int y);
    const std::uint8_t* rowPtr(int y) const;

    std::uint8_t* pixelPtr(int x, int y);
    const std::uint8_t* pixelPtr(int x, int y) const;

    ImageBuffer cloneDimensions() const;

private:
    int m_width = 0;
    int m_height = 0;
    std::vector<std::uint8_t> m_data;
};

} // namespace ip
