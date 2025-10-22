#include "StdAfx.h"
#include "ExportOptions.h"
#include "dbWave.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(ExportOptionsWindow, CWnd)
    ON_WM_CREATE()
    ON_WM_DESTROY()
END_MESSAGE_MAP()

ExportOptionsWindow::ExportOptionsWindow() = default;
ExportOptionsWindow::~ExportOptionsWindow() = default;

BOOL ExportOptionsWindow::Show(CWnd* p_parent, options_print* p_opts)
{
    auto* wnd = new ExportOptionsWindow();
    if (!wnd->Create(p_parent, p_opts))
    {
        delete wnd;
        return FALSE;
    }
    return TRUE;
}

BOOL ExportOptionsWindow::Create(CWnd* p_parent, options_print* p_opts)
{
    p_opts_ = p_opts;
    CString cls = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW, ::LoadCursor(nullptr, IDC_ARROW), (HBRUSH)(COLOR_WINDOW+1), nullptr);
    CRect rc(0, 0, 380, 240);
    BOOL ok = CWnd::CreateEx(WS_EX_DLGMODALFRAME, cls, _T("Export Options"), WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_VISIBLE, rc, p_parent, 0);
    if (!ok) return FALSE;
    CenterWindow(p_parent);
    return TRUE;
}

int ExportOptionsWindow::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CWnd::OnCreate(lpCreateStruct) == -1)
        return -1;

    const int x1 = 12; const int y1 = 12; const int w1 = 120; const int h1 = 20; const int gap = 8;
    CStatic* labW = new CStatic(); labW->Create(_T("Width (px):"), WS_CHILD|WS_VISIBLE, CRect(x1, y1, x1+w1, y1+h1), this);
    CStatic* labH = new CStatic(); labH->Create(_T("Height (px):"), WS_CHILD|WS_VISIBLE, CRect(x1, y1+h1+gap, x1+w1, y1+h1+gap+h1), this);

    edit_w_.Create(WS_CHILD|WS_VISIBLE|WS_BORDER|ES_NUMBER, CRect(x1+w1+6, y1, x1+w1+6+80, y1+h1), this, 2001);
    edit_h_.Create(WS_CHILD|WS_VISIBLE|WS_BORDER|ES_NUMBER, CRect(x1+w1+6, y1+h1+gap, x1+w1+6+80, y1+h1+gap+h1), this, 2002);

    chk_frame_.Create(_T("Frame"), WS_CHILD|WS_VISIBLE|BS_AUTOCHECKBOX, CRect(x1, y1+2*(h1+gap)+2, x1+120, y1+2*(h1+gap)+2+h1), this, 2011);
    chk_clip_.Create(_T("Clip to rect"), WS_CHILD|WS_VISIBLE|BS_AUTOCHECKBOX, CRect(x1+130, y1+2*(h1+gap)+2, x1+260, y1+2*(h1+gap)+2+h1), this, 2012);
    chk_timebar_.Create(_T("Time bar"), WS_CHILD|WS_VISIBLE|BS_AUTOCHECKBOX, CRect(x1, y1+3*(h1+gap)+2, x1+120, y1+3*(h1+gap)+2+h1), this, 2013);
    chk_voltbar_.Create(_T("Voltage bar"), WS_CHILD|WS_VISIBLE|BS_AUTOCHECKBOX, CRect(x1+130, y1+3*(h1+gap)+2, x1+260, y1+3*(h1+gap)+2+h1), this, 2014);
    chk_preview_.Create(_T("Preview before copy"), WS_CHILD|WS_VISIBLE|BS_AUTOCHECKBOX, CRect(x1, y1+4*(h1+gap)+2, x1+200, y1+4*(h1+gap)+2+h1), this, 2015);

    btn_ok_.Create(_T("OK"), WS_CHILD|WS_VISIBLE|BS_DEFPUSHBUTTON, CRect(200, 190, 260, 214), this, IDOK);
    btn_cancel_.Create(_T("Cancel"), WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON, CRect(270, 190, 330, 214), this, IDCANCEL);

    // Set initial values
    CString s; s.Format(_T("%d"), p_opts_->horizontal_resolution); edit_w_.SetWindowText(s);
    s.Format(_T("%d"), p_opts_->vertical_resolution); edit_h_.SetWindowText(s);
    chk_frame_.SetCheck(p_opts_->b_frame_rect);
    chk_clip_.SetCheck(p_opts_->b_clip_rect);
    chk_timebar_.SetCheck(p_opts_->b_timescale_bar);
    chk_voltbar_.SetCheck(p_opts_->b_voltage_scale_bar);
    chk_preview_.SetCheck(p_opts_->b_preview_before_copy);

    btn_ok_.SetButtonStyle(BS_DEFPUSHBUTTON);
    btn_cancel_.SetButtonStyle(BS_PUSHBUTTON);

    return 0;
}

void ExportOptionsWindow::OnOk()
{
    // Validate and store
    CString sW, sH; edit_w_.GetWindowText(sW); edit_h_.GetWindowText(sH);
    int w = _ttoi(sW); int h = _ttoi(sH);
    if (w <= 0 || h <= 0) { AfxMessageBox(_T("Width/Height must be positive."), MB_OK|MB_ICONERROR); return; }
    p_opts_->horizontal_resolution = w;
    p_opts_->vertical_resolution = h;
    p_opts_->b_frame_rect = (chk_frame_.GetCheck() == BST_CHECKED);
    p_opts_->b_clip_rect = (chk_clip_.GetCheck() == BST_CHECKED);
    p_opts_->b_timescale_bar = (chk_timebar_.GetCheck() == BST_CHECKED);
    p_opts_->b_voltage_scale_bar = (chk_voltbar_.GetCheck() == BST_CHECKED);
    p_opts_->b_preview_before_copy = (chk_preview_.GetCheck() == BST_CHECKED);
    DestroyWindow();
}

void ExportOptionsWindow::OnCancelClicked()
{
    DestroyWindow();
}

void ExportOptionsWindow::OnDestroy()
{
    CWnd::OnDestroy();
}








