/**
 * @file main.cpp
 * @brief Entry point for the MFC Dialog-based Circle application.
 */

#include <afxwin.h>
#include "CircleDialog.h"

class CCircleApp : public CWinApp {
public:
    virtual BOOL InitInstance() {
        CWinApp::InitInstance();
        CircleDialog dlg;
        m_pMainWnd = &dlg;
        dlg.DoModal();
        return FALSE;
    }
};

CCircleApp theApp;
