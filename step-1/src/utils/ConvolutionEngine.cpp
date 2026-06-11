/**
 * @file ConvolutionEngine.cpp
 */

#include "ConvolutionEngine.h"
#include "Logger.h"
#include "Exceptions.h"

#include <algorithm>
#include <cmath>
#include <thread>
#include <vector>

#if defined(__AVX2__)
#include <immintrin.h>
#endif

namespace ip {

namespace {

void toPlanar(const ImageBuffer& src,
              std::vector<int32_t>& r,
              std::vector<int32_t>& g,
              std::vector<int32_t>& b)
{
    const int w = src.width();
    const int h = src.height();
    const std::size_t pixels = static_cast<std::size_t>(w) * h;
    r.resize(pixels);
    g.resize(pixels);
    b.resize(pixels);
    const std::uint8_t* data = src.data();
    for (std::size_t i = 0; i < pixels; ++i) {
        b[i] = data[i * 3 + 0];
        g[i] = data[i * 3 + 1];
        r[i] = data[i * 3 + 2];
    }
}

void fromPlanar(const std::vector<int32_t>& r,
                const std::vector<int32_t>& g,
                const std::vector<int32_t>& b,
                ImageBuffer& dst)
{
    const int w = dst.width();
    const int h = dst.height();
    const std::size_t pixels = static_cast<std::size_t>(w) * h;
    std::uint8_t* data = dst.data();
    for (std::size_t i = 0; i < pixels; ++i) {
        data[i * 3 + 0] = static_cast<std::uint8_t>(std::clamp(b[i], 0, 255));
        data[i * 3 + 1] = static_cast<std::uint8_t>(std::clamp(g[i], 0, 255));
        data[i * 3 + 2] = static_cast<std::uint8_t>(std::clamp(r[i], 0, 255));
    }
}

void convolvePlanarScalar(const int32_t* src, int32_t* dst,
                          int width, int height,
                          const int kernel[9], int divisor,
                          int startY, int endY)
{
    if (divisor == 0) divisor = 1;
    for (int y = startY; y < endY; ++y) {
        if (y == 0 || y == height - 1) continue;
        for (int x = 1; x < width - 1; ++x) {
            int sum = 0;
            for (int ky = -1; ky <= 1; ++ky) {
                int yy = y + ky;
                for (int kx = -1; kx <= 1; ++kx) {
                    int xx = x + kx;
                    sum += src[yy * width + xx] * kernel[(ky + 1) * 3 + (kx + 1)];
                }
            }
            int val = sum / divisor;
            if (val < 0) val = 0;
            if (val > 255) val = 255;
            dst[y * width + x] = val;
        }
    }
}

#if defined(__AVX2__)
void convolvePlanarSIMD(const int32_t* src, int32_t* dst,
                        int width, int height,
                        const int kernel[9], int divisor,
                        int startY, int endY)
{
    if (divisor == 0) divisor = 1;
    const float invDiv = 1.0f / static_cast<float>(divisor);
    const __m256 vInvDiv = _mm256_set1_ps(invDiv);

    for (int y = startY; y < endY; ++y) {
        if (y == 0 || y == height - 1) continue;
        int x = 1;
        const int xLimit = width - 1 - 8;
        for (; x <= xLimit; x += 8) {
            __m256 sum = _mm256_setzero_ps();
            for (int ky = -1; ky <= 1; ++ky) {
                int yy = y + ky;
                const int32_t* rowPtr = src + yy * width;
                for (int kx = -1; kx <= 1; ++kx) {
                    int ki = (ky + 1) * 3 + (kx + 1);
                    const int32_t* ptr = rowPtr + x + kx;
                    __m256i vals = _mm256_loadu_si256(
                        reinterpret_cast<const __m256i*>(ptr));
                    __m256 fvals = _mm256_cvtepi32_ps(vals);
                    __m256 coeff = _mm256_set1_ps(static_cast<float>(kernel[ki]));
                    sum = _mm256_add_ps(sum, _mm256_mul_ps(fvals, coeff));
                }
            }
            sum = _mm256_mul_ps(sum, vInvDiv);
            __m256i result = _mm256_cvttps_epi32(sum);
            __m256i zero = _mm256_setzero_si256();
            __m256i maxv = _mm256_set1_epi32(255);
            result = _mm256_max_epi32(zero, _mm256_min_epi32(maxv, result));
            _mm256_storeu_si256(
                reinterpret_cast<__m256i*>(dst + y * width + x), result);
        }
        // Scalar tail
        for (; x < width - 1; ++x) {
            int sum = 0;
            for (int ky = -1; ky <= 1; ++ky) {
                int yy = y + ky;
                for (int kx = -1; kx <= 1; ++kx) {
                    int xx = x + kx;
                    sum += src[yy * width + xx] * kernel[(ky + 1) * 3 + (kx + 1)];
                }
            }
            int val = sum / divisor;
            if (val < 0) val = 0;
            if (val > 255) val = 255;
            dst[y * width + x] = val;
        }
    }
}
#endif

} // anonymous namespace

void ConvolutionEngine::apply(ImageBuffer& image,
                              const std::vector<std::vector<int>>& kernel,
                              int divisor,
                              int threadCount)
{
    if (kernel.size() != 3 || kernel[0].size() != 3 ||
        kernel[1].size() != 3 || kernel[2].size() != 3) {
        throw FilterError("ConvolutionEngine: only 3x3 kernels are supported");
    }

    const int width = image.width();
    const int height = image.height();
    if (width < 3 || height < 3) {
        Logger::instance().log("ConvolutionEngine: image too small for 3x3 kernel");
        return;
    }

    int k[9];
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            k[i * 3 + j] = kernel[i][j];

    std::vector<int32_t> r, g, b;
    toPlanar(image, r, g, b);

    std::vector<int32_t> rOut(r.size()), gOut(g.size()), bOut(b.size());

    if (threadCount <= 0) {
        threadCount = static_cast<int>(std::thread::hardware_concurrency());
        if (threadCount <= 0) threadCount = 1;
    }

    auto worker = [&](std::vector<int32_t>& src, std::vector<int32_t>& dst,
                      int startY, int endY) {
#if defined(__AVX2__)
        convolvePlanarSIMD(src.data(), dst.data(), width, height, k, divisor, startY, endY);
#else
        convolvePlanarScalar(src.data(), dst.data(), width, height, k, divisor, startY, endY);
#endif
    };

    std::vector<std::thread> threads;
    const int rowsPerThread = height / threadCount;
    for (int t = 0; t < threadCount; ++t) {
        int startY = t * rowsPerThread;
        int endY = (t == threadCount - 1) ? height : (startY + rowsPerThread);
        if (startY == 0) startY = 1;
        if (endY == height) endY = height - 1;
        if (startY >= endY) continue;
        threads.emplace_back([&, startY, endY]() {
            worker(r, rOut, startY, endY);
            worker(g, gOut, startY, endY);
            worker(b, bOut, startY, endY);
        });
    }

    for (auto& t : threads) {
        if (t.joinable()) t.join();
    }

    fromPlanar(rOut, gOut, bOut, image);
    Logger::instance().log("ConvolutionEngine: applied 3x3 kernel with " +
                           std::to_string(threadCount) + " threads");
}

} // namespace ip
