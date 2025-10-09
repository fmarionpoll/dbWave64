#include "StdAfx.h"
#include "ExportPreview.h"
#include <afxdlgs.h>
#include "ExportOptions.h"

#include "dbWave.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(ExportPreviewWindow, CWnd)
    ON_WM_CREATE()
    ON_WM_PAINT()
    ON_WM_DESTROY()
    ON_BN_CLICKED(1001, OnCopy)
    ON_BN_CLICKED(1002, OnSave)
    ON_BN_CLICKED(1003, OnCloseClicked)
    ON_BN_CLICKED(1004, OnPrint)
    ON_BN_CLICKED(1005, OnSavePng)
    ON_BN_CLICKED(1006, OnSettings)
END_MESSAGE_MAP()

ExportPreviewWindow::ExportPreviewWindow() = default;
ExportPreviewWindow::~ExportPreviewWindow() = default;

BOOL ExportPreviewWindow::Create(CWnd* p_parent, HENHMETAFILE h_emf, const CRect& pixel_bounds, const CString& title)
{
    h_emf_ = h_emf;
    pixel_bounds_ = pixel_bounds;

    CString cls = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW, ::LoadCursor(nullptr, IDC_ARROW), (HBRUSH)(COLOR_WINDOW+1), nullptr);
    CRect rc(0, 0, pixel_bounds.Width() + 40, pixel_bounds.Height() + 100);
    BOOL ok = CWnd::CreateEx(WS_EX_DLGMODALFRAME, cls, title, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_VISIBLE, rc, p_parent, 0);
    if (!ok)
        return FALSE;
    CenterWindow(p_parent);
    return TRUE;
}

BOOL ExportPreviewWindow::Show(CWnd* p_parent, HENHMETAFILE h_emf, const CRect& pixel_bounds, const CString& title)
{
    auto* wnd = new ExportPreviewWindow();
    if (!wnd->Create(p_parent, h_emf, pixel_bounds, title))
    {
        delete wnd;
        return FALSE;
    }
    return TRUE;
}

int ExportPreviewWindow::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CWnd::OnCreate(lpCreateStruct) == -1)
        return -1;

    const int btn_w = 90;
    const int btn_h = 24;
    CRect client; GetClientRect(&client);
    const int y = client.bottom - btn_h - 10;
    const int x0 = client.left + 10;

    btn_copy_.Create(_T("Copy"), WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON, CRect(x0, y, x0+btn_w, y+btn_h), this, 1001);
    btn_save_.Create(_T("Save EMF..."), WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON, CRect(x0+btn_w+6, y, x0+2*btn_w+6, y+btn_h), this, 1002);
    btn_print_.Create(_T("Print..."), WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON, CRect(x0+2*btn_w+12, y, x0+3*btn_w+12, y+btn_h), this, 1004);
    btn_png_.Create(_T("Save PNG..."), WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON, CRect(x0+3*btn_w+18, y, x0+4*btn_w+18, y+btn_h), this, 1005);
    btn_settings_.Create(_T("Settings"), WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON, CRect(x0+4*btn_w+24, y, x0+5*btn_w+24, y+btn_h), this, 1006);
    btn_close_.Create(_T("Close"), WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON, CRect(client.right-btn_w-10, y, client.right-10, y+btn_h), this, 1003);

    btn_copy_.SetFont(CFont::FromHandle((HFONT)GetStockObject(DEFAULT_GUI_FONT)));
    btn_save_.SetFont(CFont::FromHandle((HFONT)GetStockObject(DEFAULT_GUI_FONT)));
    btn_close_.SetFont(CFont::FromHandle((HFONT)GetStockObject(DEFAULT_GUI_FONT)));
    btn_print_.SetFont(CFont::FromHandle((HFONT)GetStockObject(DEFAULT_GUI_FONT)));
    btn_png_.SetFont(CFont::FromHandle((HFONT)GetStockObject(DEFAULT_GUI_FONT)));
    btn_settings_.SetFont(CFont::FromHandle((HFONT)GetStockObject(DEFAULT_GUI_FONT)));

    btn_copy_.SetWindowPos(nullptr, 0,0,0,0, SWP_NOMOVE|SWP_NOSIZE);
    btn_save_.SetWindowPos(nullptr, 0,0,0,0, SWP_NOMOVE|SWP_NOSIZE);
    btn_close_.SetWindowPos(nullptr, 0,0,0,0, SWP_NOMOVE|SWP_NOSIZE);

    // Hook button clicks
    btn_copy_.SetButtonStyle(BS_PUSHBUTTON);
    btn_save_.SetButtonStyle(BS_PUSHBUTTON);
    btn_close_.SetButtonStyle(BS_PUSHBUTTON);
    btn_print_.SetButtonStyle(BS_PUSHBUTTON);
    btn_png_.SetButtonStyle(BS_PUSHBUTTON);
    btn_settings_.SetButtonStyle(BS_PUSHBUTTON);

    btn_copy_.SetDlgCtrlID(1001);
    btn_save_.SetDlgCtrlID(1002);
    btn_close_.SetDlgCtrlID(1003);
    btn_print_.SetDlgCtrlID(1004);
    btn_png_.SetDlgCtrlID(1005);
    btn_settings_.SetDlgCtrlID(1006);

    return 0;
}

void ExportPreviewWindow::OnPaint()
{
    CPaintDC dc(this);
    CRect client; GetClientRect(&client);

    // Draw EMF centered at top area
    const int margin = 10;
    CRect preview_rect(margin, margin, client.right - margin, client.bottom - 50);
    // Maintain pixel_bounds aspect into preview_rect
    double aspect = (double)pixel_bounds_.Width() / std::max(1, pixel_bounds_.Height());
    int w = preview_rect.Width();
    int h = (int)(w / aspect);
    if (h > preview_rect.Height()) { h = preview_rect.Height(); w = (int)(h * aspect); }
    CRect dst(preview_rect.left + (preview_rect.Width()-w)/2,
              preview_rect.top + (preview_rect.Height()-h)/2,
              preview_rect.left + (preview_rect.Width()-w)/2 + w,
              preview_rect.top + (preview_rect.Height()-h)/2 + h);

    if (h_emf_)
    {
        // Play the EMF into the DC
        ::PlayEnhMetaFile(dc.GetSafeHdc(), h_emf_, &dst);
    }
}

void ExportPreviewWindow::OnDestroy()
{
    if (h_emf_)
    {
        DeleteEnhMetaFile(h_emf_);
        h_emf_ = nullptr;
    }
    CWnd::OnDestroy();
}

void ExportPreviewWindow::OnCopy()
{
    if (!h_emf_) return;
    if (CWnd::OpenClipboard())
    {
        EmptyClipboard();
        SetClipboardData(CF_ENHMETAFILE, h_emf_);
        CloseClipboard();
        h_emf_ = nullptr; // Ownership transferred
    }
}

void ExportPreviewWindow::OnSave()
{
    if (!h_emf_) return;
    CFileDialog dlg(FALSE, _T(".emf"), _T("export.emf"), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("Enhanced Metafile (*.emf)|*.emf||"), this);
    if (IDOK == dlg.DoModal())
    {
        // Save the EMF by copying its bits into a file
        // Simplest approach: Copy to clipboard-like handle then use CopyEnhMetaFile
        HENHMETAFILE copy = CopyEnhMetaFile(h_emf_, dlg.GetPathName());
        if (copy)
            DeleteEnhMetaFile(copy);
    }
}

void ExportPreviewWindow::OnPrint()
{
    if (!h_emf_) return;
    // Use standard Print dialog and request a printer DC
    CPrintDialog pd(FALSE, PD_USEDEVMODECOPIES | PD_NOPAGENUMS | PD_NOSELECTION | PD_RETURNDC, this);
    if (pd.DoModal() == IDOK)
    {
        HDC hdc = pd.GetPrinterDC();
        if (!hdc)
        {
            AfxMessageBox(_T("No printer device context available."), MB_OK | MB_ICONERROR);
            return;
        }
        // Print EMF scaled to fit page while preserving aspect
        DOCINFO di{}; di.cbSize = sizeof(DOCINFO); di.lpszDocName = _T("dbWave Export");
        if (StartDoc(hdc, &di) > 0)
        {
            if (StartPage(hdc) > 0)
            {
                const int page_w = GetDeviceCaps(hdc, HORZRES);
                const int page_h = GetDeviceCaps(hdc, VERTRES);
                const int src_w = std::max(1, pixel_bounds_.Width());
                const int src_h = std::max(1, pixel_bounds_.Height());
                // Fit preserving aspect
                int draw_w = page_w;
                int draw_h = static_cast<int>(draw_w * (static_cast<double>(src_h) / static_cast<double>(src_w)));
                if (draw_h > page_h)
                {
                    draw_h = page_h;
                    draw_w = static_cast<int>(draw_h * (static_cast<double>(src_w) / static_cast<double>(src_h)));
                }
                RECT r{};
                r.left = (page_w - draw_w) / 2;
                r.top = (page_h - draw_h) / 2;
                r.right = r.left + draw_w;
                r.bottom = r.top + draw_h;
                PlayEnhMetaFile(hdc, h_emf_, &r);
                EndPage(hdc);
            }
            EndDoc(hdc);
        }
        DeleteDC(hdc);
    }
}

void ExportPreviewWindow::OnSavePng()
{
    if (!h_emf_) return;
    // Replay the EMF into a DIB and save as PNG using GraphicsExport::ExportToPng
    CFileDialog dlg(FALSE, _T(".png"), _T("export.png"), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("PNG Image (*.png)|*.png||"), this);
    if (IDOK != dlg.DoModal())
        return;

    // Build an offscreen DIB and PlayEnhMetaFile into it, then save via GDI+ path
    const int w = std::max(1, pixel_bounds_.Width());
    const int h = std::max(1, pixel_bounds_.Height());
    BITMAPINFO bmi{};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = w;
    bmi.bmiHeader.biHeight = -h; // top-down
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    void* p_bits = nullptr;
    HDC h_screen = ::GetDC(nullptr);
    HBITMAP h_dib = CreateDIBSection(h_screen, &bmi, DIB_RGB_COLORS, &p_bits, nullptr, 0);
    if (!h_dib) { ::ReleaseDC(nullptr, h_screen); return; }

    HDC h_mem = CreateCompatibleDC(h_screen);
    HGDIOBJ old = SelectObject(h_mem, h_dib);
    RECT r{ 0,0,w,h };
    HBRUSH br = CreateSolidBrush(RGB(255,255,255));
    FillRect(h_mem, &r, br); DeleteObject(br);
    PlayEnhMetaFile(h_mem, h_emf_, &r);

    // Save to PNG (GDI+)
    BOOL saved = FALSE;
    ULONG_PTR token = 0;
    Gdiplus::GdiplusStartupInput gsi; if (Gdiplus::Ok == Gdiplus::GdiplusStartup(&token, &gsi, nullptr))
    {
        Gdiplus::Bitmap bmp(h_dib, (HPALETTE)nullptr);
        UINT num=0, size=0; Gdiplus::GetImageEncodersSize(&num, &size);
        if (size > 0)
        {
            auto p_codecs = (Gdiplus::ImageCodecInfo*)malloc(size);
            if (p_codecs && Gdiplus::Ok == Gdiplus::GetImageEncoders(num, size, p_codecs))
            {
                CLSID clsid{};
                for (UINT i=0;i<num;i++) if (wcscmp(p_codecs[i].MimeType, L"image/png")==0) { clsid = p_codecs[i].Clsid; break; }
                if (!IsEqualCLSID(clsid, CLSID{}))
                    saved = (Gdiplus::Ok == bmp.Save(dlg.GetPathName(), &clsid, nullptr));
            }
            if (p_codecs) free(p_codecs);
        }
        Gdiplus::GdiplusShutdown(token);
    }

    // Cleanup
    SelectObject(h_mem, old);
    DeleteObject(h_dib);
    DeleteDC(h_mem);
    ::ReleaseDC(nullptr, h_screen);
}

void ExportPreviewWindow::OnSettings()
{
    auto& opts = static_cast<CdbWaveApp*>(AfxGetApp())->options_print_data;
    ExportOptionsWindow::Show(this, &opts);
}

void ExportPreviewWindow::OnCloseClicked()
{
    DestroyWindow();
}
