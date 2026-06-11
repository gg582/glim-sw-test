#pragma once

/**
 * @file CircleDrawer.h
 * @brief Low-level pixel-based circle drawing using Bresenham's algorithm.
 *
 * High-level GDI functions such as Ellipse, FillPolygon, and DrawPolygon
 * are intentionally avoided as per the assignment requirements.
 */

#include <windows.h>

struct Point2D {
    int x;
    int y;
};

class CircleDrawer {
public:
    /// Draw a circle outline with the given thickness using SetPixel only.
    static void drawCircle(HDC hdc, int cx, int cy, int r, int thickness, COLORREF color);

    /// Draw a small filled square to represent a click point.
    static void drawFilledRect(HDC hdc, int x, int y, int halfSize, COLORREF color);

    /// Compute the unique circle passing through three non-collinear points.
    static bool computeCircleFrom3Points(const Point2D& p1,
                                          const Point2D& p2,
                                          const Point2D& p3,
                                          Point2D& center,
                                          double& radius);

private:
    static void bresenhamCircle(HDC hdc, int cx, int cy, int r, COLORREF color);
};
