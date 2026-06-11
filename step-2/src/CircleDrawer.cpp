/**
 * @file CircleDrawer.cpp
 */

#include "CircleDrawer.h"

#include <cmath>

void CircleDrawer::drawCircle(HDC hdc, int cx, int cy, int r, int thickness, COLORREF color) {
    if (r <= 0) return;
    for (int t = 0; t < thickness; ++t) {
        bresenhamCircle(hdc, cx, cy, r + t, color);
    }
}

void CircleDrawer::bresenhamCircle(HDC hdc, int cx, int cy, int r, COLORREF color) {
    int x = 0;
    int y = r;
    int d = 3 - 2 * r;

    auto plot8 = [&](int px, int py) {
        SetPixel(hdc, cx + px, cy + py, color);
        SetPixel(hdc, cx - px, cy + py, color);
        SetPixel(hdc, cx + px, cy - py, color);
        SetPixel(hdc, cx - px, cy - py, color);
        SetPixel(hdc, cx + py, cy + px, color);
        SetPixel(hdc, cx - py, cy + px, color);
        SetPixel(hdc, cx + py, cy - px, color);
        SetPixel(hdc, cx - py, cy - px, color);
    };

    while (x <= y) {
        plot8(x, y);
        if (d < 0) {
            d = d + 4 * x + 6;
        } else {
            d = d + 4 * (x - y) + 10;
            --y;
        }
        ++x;
    }
}

void CircleDrawer::drawFilledRect(HDC hdc, int x, int y, int halfSize, COLORREF color) {
    for (int dy = -halfSize; dy <= halfSize; ++dy) {
        for (int dx = -halfSize; dx <= halfSize; ++dx) {
            SetPixel(hdc, x + dx, y + dy, color);
        }
    }
}

bool CircleDrawer::computeCircleFrom3Points(const Point2D& p1,
                                            const Point2D& p2,
                                            const Point2D& p3,
                                            Point2D& center,
                                            double& radius)
{
    double d = 2.0 * (static_cast<double>(p1.x) * (p2.y - p3.y) +
                      static_cast<double>(p2.x) * (p3.y - p1.y) +
                      static_cast<double>(p3.x) * (p1.y - p2.y));
    if (std::abs(d) < 1e-6) {
        return false; // Points are collinear
    }

    double u1 = static_cast<double>(p1.x) * p1.x + static_cast<double>(p1.y) * p1.y;
    double u2 = static_cast<double>(p2.x) * p2.x + static_cast<double>(p2.y) * p2.y;
    double u3 = static_cast<double>(p3.x) * p3.x + static_cast<double>(p3.y) * p3.y;

    double ux = (u1 * (p2.y - p3.y) + u2 * (p3.y - p1.y) + u3 * (p1.y - p2.y)) / d;
    double uy = (u1 * (p3.x - p2.x) + u2 * (p1.x - p3.x) + u3 * (p2.x - p1.x)) / d;

    center.x = static_cast<int>(std::round(ux));
    center.y = static_cast<int>(std::round(uy));
    radius = std::hypot(static_cast<double>(p1.x) - ux,
                        static_cast<double>(p1.y) - uy);
    return true;
}
