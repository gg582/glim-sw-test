/**
 * @file HistogramAnalyzer.cpp
 */

#include "HistogramAnalyzer.h"

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <vector>

namespace ip {

void HistogramAnalyzer::analyzeAndPrint(const ImageBuffer& image) {
    const int w = image.width();
    const int h = image.height();
    const std::uint8_t* data = image.data();
    std::vector<int> hist(256, 0);

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            int idx = (y * w + x) * 3;
            int gray = (data[idx] + data[idx + 1] + data[idx + 2]) / 3;
            ++hist[gray];
        }
    }

    int maxCount = *std::max_element(hist.begin(), hist.end());
    const int barWidth = 50;

    std::cout << "\n=== Grayscale Histogram ===\n";
    for (int i = 0; i < 256; i += 4) {
        int bucket = (hist[i] + hist[i + 1] + hist[i + 2] + hist[i + 3]) / 4;
        int barLen = (maxCount > 0) ? (bucket * barWidth / maxCount) : 0;
        std::cout << std::setw(3) << i << "-"
                  << std::setw(3) << (i + 3) << " |"
                  << std::string(barLen, '#')
                  << std::string(barWidth - barLen, ' ')
                  << "| " << bucket << "\n";
    }
    std::cout << "===========================\n\n";
}

} // namespace ip
