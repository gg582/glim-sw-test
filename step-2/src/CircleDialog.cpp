/**
 * @file CircleDialog.cpp
 */

#include "CircleDialog.h"

#include <sstream>
#include <random>
#include <chrono>

#define WM_RANDOM_STEP (WM_USER + 1)

BEGIN_MESSAGE_MAP(CircleDialog, CDialogEx)
    ON_WM_PAINT()
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_MOUSEMOVE()
    ON_BN_CLICKED(IDC_BUTTON_RESET, &CircleDialog::OnBnClickedReset)
    ON_BN_CLICKED(IDC_BUTTON_RANDOM, &CircleDialog::OnBnClickedRandomMove)
    ON_MESSAGE(WM_RANDOM_STEP, &CircleDialog::OnRandomStep)
END_MESSAGE_MAP()

CircleDialog::CircleDialog(CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_CIRCLEDIALOG, pParent)
{
}

void CircleDialog::DoDataExchange(CDataExchange* pDX) {
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_EDIT_RADIUS, m_editRadius);
    DDX_Control(pDX, IDC_EDIT_THICKNESS, m_editThickness);
    DDX_Control(pDX, IDC_STATIC_COORDS, m_staticCoords);
}

BOOL CircleDialog::OnInitDialog() {
    CDialogEx::OnInitDialog();
    SetWindowText(_T("Circle through 3 Points"));

    m_editRadius.SetWindowText(_T("10"));
    m_editThickness.SetWindowText(_T("2"));
    return TRUE;
}

void CircleDialog::OnPaint() {
    CPaintDC dc(this);
    drawScene(&dc);
}

void CircleDialog::drawScene(CDC* pDC) {
    CString strR, strT;
    m_editRadius.GetWindowText(strR);
    m_editThickness.GetWindowText(strT);
    m_clickRadius = _ttoi(strR);
    if (m_clickRadius <= 0) m_clickRadius = 10;
    m_edgeThickness = _ttoi(strT);
    if (m_edgeThickness <= 0) m_edgeThickness = 2;

    CRect rect;
    GetClientRect(&rect);
    int drawW = rect.Width() - 20;
    int drawH = rect.Height() - 100;

    // Clear drawing area
    pDC->FillSolidRect(10, 60, drawW, drawH, RGB(255, 255, 255));

    // Draw click points
    for (size_t i = 0; i < m_points.size(); ++i) {
        CircleDrawer::drawFilledRect(pDC->GetSafeHdc(), m_points[i].x, m_points[i].y, m_clickRadius, RGB(255, 0, 0));

        CString coord;
        coord.Format(_T("P%d(%d,%d)"), static_cast<int>(i) + 1, m_points[i].x, m_points[i].y);
        pDC->SetTextColor(RGB(0, 0, 0));
        pDC->SetBkMode(TRANSPARENT);
        pDC->TextOut(m_points[i].x + m_clickRadius + 4, m_points[i].y - 6, coord);
    }

    // Draw circumcircle
    if (m_hasCircle) {
        int r = static_cast<int>(std::round(m_circleRadius));
        CircleDrawer::drawCircle(pDC->GetSafeHdc(), m_circleCenter.x, m_circleCenter.y,
                                 r, m_edgeThickness, RGB(0, 0, 255));
    }

    // Update coordinate summary
    CString summary;
    if (m_points.size() >= 3) {
        summary.Format(_T("Points: (%d,%d) (%d,%d) (%d,%d)  Center: (%d,%d)  R:%.1f"),
                       m_points[0].x, m_points[0].y,
                       m_points[1].x, m_points[1].y,
                       m_points[2].x, m_points[2].y,
                       m_circleCenter.x, m_circleCenter.y, m_circleRadius);
    } else {
        summary.Format(_T("Click %d more point(s)..."), 3 - static_cast<int>(m_points.size()));
    }
    m_staticCoords.SetWindowText(summary);
}

void CircleDialog::OnLButtonDown(UINT nFlags, CPoint point) {
    CRect drawArea;
    GetClientRect(&drawArea);
    drawArea.DeflateRect(10, 60, 10, 10);

    if (!drawArea.PtInRect(point)) {
        CDialogEx::OnLButtonDown(nFlags, point);
        return;
    }

    if (m_points.size() < 3) {
        m_points.push_back({point.x, point.y});
        if (m_points.size() == 3) {
            updateCircle();
        }
        Invalidate(FALSE);
    } else {
        for (int i = 0; i < 3; ++i) {
            if (isNearPoint(point, m_points[i], m_clickRadius + 8)) {
                m_dragIndex = i;
                m_isDragging = true;
                SetCapture();
                break;
            }
        }
    }
    CDialogEx::OnLButtonDown(nFlags, point);
}

void CircleDialog::OnLButtonUp(UINT nFlags, CPoint point) {
    if (m_isDragging) {
        m_isDragging = false;
        m_dragIndex = -1;
        ReleaseCapture();
    }
    CDialogEx::OnLButtonUp(nFlags, point);
}

void CircleDialog::OnMouseMove(UINT nFlags, CPoint point) {
    if (m_isDragging && m_dragIndex >= 0 && m_dragIndex < 3) {
        m_points[m_dragIndex].x = point.x;
        m_points[m_dragIndex].y = point.y;
        updateCircle();
        Invalidate(FALSE);
    }
    CDialogEx::OnMouseMove(nFlags, point);
}

void CircleDialog::updateCircle() {
    if (m_points.size() == 3) {
        m_hasCircle = CircleDrawer::computeCircleFrom3Points(
            m_points[0], m_points[1], m_points[2], m_circleCenter, m_circleRadius);
    }
}

bool CircleDialog::isNearPoint(const CPoint& pt, const Point2D& center, int radius) const {
    int dx = pt.x - center.x;
    int dy = pt.y - center.y;
    return (dx * dx + dy * dy) <= radius * radius;
}

void CircleDialog::OnBnClickedReset() {
    m_points.clear();
    m_hasCircle = false;
    m_circleRadius = 0.0;
    m_dragIndex = -1;
    m_isDragging = false;
    Invalidate(FALSE);
}

void CircleDialog::OnBnClickedRandomMove() {
    if (m_randomRunning.load() || m_points.size() < 3) return;

    m_randomRunning = true;
    m_randomThread = std::thread([this]() {
        std::random_device rd;
        std::mt19937 gen(rd());
        CRect rect;
        GetClientRect(&rect);
        int minX = 20;
        int minY = 70;
        int maxX = rect.Width() - 20;
        int maxY = rect.Height() - 20;
        if (maxX <= minX) maxX = minX + 100;
        if (maxY <= minY) maxY = minY + 100;

        std::uniform_int_distribution<> distX(minX, maxX);
        std::uniform_int_distribution<> distY(minY, maxY);

        for (int step = 0; step < 10; ++step) {
            for (int i = 0; i < 3; ++i) {
                m_points[i].x = distX(gen);
                m_points[i].y = distY(gen);
            }
            PostMessage(WM_RANDOM_STEP, 0, 0);
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
        m_randomRunning = false;
    });
    m_randomThread.detach();
}

LRESULT CircleDialog::OnRandomStep(WPARAM /*wParam*/, LPARAM /*lParam*/) {
    updateCircle();
    Invalidate(FALSE);
    return 0;
}
