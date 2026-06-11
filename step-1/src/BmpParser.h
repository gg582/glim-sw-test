#pragma once

/**
 * @file BmpParser.h
 * @brief Handles BMP file input and output.
 *
 * Supported format:
 *   - 24-bit color, uncompressed (BI_RGB)
 *   - bottom-up (positive height)
 *   - 4-byte row alignment
 *
 * Implemented using only the C++ standard library.
 */

#include "ImageBuffer.h"

#include <string>

namespace ip {

/**
 * @brief Static-only class responsible for BMP file I/O.
 */
class BmpParser {
public:
    BmpParser() = delete;

    /**
     * @brief Load a BMP file into an ImageBuffer.
     * @throws BmpParseError if the file cannot be opened, is unsupported, or is corrupted.
     */
    static ImageBuffer loadFromFile(const std::string& path);

    /**
     * @brief Save an ImageBuffer as a 24-bit BMP file.
     * @throws BmpParseError if writing fails.
     */
    static void saveToFile(const std::string& path, const ImageBuffer& image);

private:
    static constexpr std::uint16_t BMP_MAGIC = 0x4D42; // "BM"
    static constexpr std::uint16_t SUPPORTED_BPP = 24;
    static constexpr int ROW_ALIGNMENT = 4;

    static int paddedStride(int width);
};

} // namespace ip
