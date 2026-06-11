#pragma once

/**
 * @file CircleDialog.h
 * @brief MFC Dialog-based application for drawing a circle through 3 points.
 */

#include <afxwin.h>
#include <afxdlgs.h>
#include <vector>
#include <thread>
#include <atomic>
#include "CircleDrawer.h"
#include "resource.h"

class CircleDialog : public CDialogEx {
public:
    CircleDialog(CWnd* pParent = nullptr);
    enum { IDD = IDD_CIRCLEDIALOG };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    virtual BOOL OnInitDialog();

    afx_msg void OnPaint();
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnBnClickedReset();
    afx_msg void OnBnClickedRandomMove();
    afx_msg LRESULT OnRandomStep(WPARAM wParam, LPARAM lParam);

    DECLARE_MESSAGE_MAP()

private:
    void drawScene(CDC* pDC);
    void updateCircle();
    bool isNearPoint(const CPoint& pt, const Point2D& center, int radius) const;

    std::vector<Point2D> m_points;
    Point2D m_circleCenter{0, 0};
    double m_circleRadius = 0.0;
    bool m_hasCircle = false;

    int m_clickRadius = 10;
    int m_edgeThickness = 2;

    int m_dragIndex = -1;
    bool m_isDragging = false;

    CEdit m_editRadius;
    CEdit m_editThickness;
    CStatic m_staticCoords;

    std::thread m_randomThread;
    std::atomic<bool> m_randomRunning{false};
};
