#include "StdAfx.h"
#include "GraphicsExport.h"

using DrawFn = std::function<void(CDC*, const CRect&)>;

static BOOL GetScreenDpi(CWnd* owner, int& dpi_x, int& dpi_y)
{
	const auto p_ref_dc = owner ? owner->GetDC() : nullptr;
	if (!p_ref_dc) return FALSE;
	dpi_x = p_ref_dc->GetDeviceCaps(LOGPIXELSX);
	dpi_y = p_ref_dc->GetDeviceCaps(LOGPIXELSY);
	if (owner) owner->ReleaseDC(p_ref_dc);
	return TRUE;
}

BOOL GraphicsExport::CopyAsEmfToClipboard(CWnd* owner_wnd,
	const CRect& pixel_rect,
	const CString& title,
	const DrawFn& draw_fn)
{
    // Use a reference device context (CDC*) so the metafile inherits mapping from a real device
    CDC* p_ref_dc = nullptr;
    CDC screen_dc;
    bool used_screen_dc = false;
    if (owner_wnd)
        p_ref_dc = owner_wnd->GetDC();
    if (!p_ref_dc)
    {
        HDC h_screen = ::GetDC(nullptr);
        screen_dc.Attach(h_screen);
        p_ref_dc = &screen_dc;
        used_screen_dc = true;
    }

    const int dpi_x = p_ref_dc->GetDeviceCaps(LOGPIXELSX);
    const int dpi_y = p_ref_dc->GetDeviceCaps(LOGPIXELSY);

    CMetaFileDC meta_dc;
    CRect himetric_bounds(0, 0,
        MulDiv(pixel_rect.Width(), 2540, dpi_x),
        MulDiv(pixel_rect.Height(), 2540, dpi_y));

    CString meta_title = _T("dbWave\0") + title;
    meta_title += _T("\0\0");
    const auto h_created = meta_dc.CreateEnhanced(p_ref_dc, nullptr, &himetric_bounds, meta_title);
	if (h_created == NULL)
    {
        if (used_screen_dc) ::ReleaseDC(nullptr, screen_dc.Detach());
        else if (owner_wnd && p_ref_dc) owner_wnd->ReleaseDC(p_ref_dc);
        return FALSE;
    }

    // Provide attribute DC so GetDeviceCaps and similar queries behave predictably during EMF recording
    meta_dc.SetAttribDC(p_ref_dc->GetSafeHdc());

    if (draw_fn)
        draw_fn(&meta_dc, pixel_rect);

    const auto h_emf = meta_dc.CloseEnhanced();
    if (h_emf == nullptr)
    {
        if (used_screen_dc) ::ReleaseDC(nullptr, screen_dc.Detach());
        else if (owner_wnd && p_ref_dc) owner_wnd->ReleaseDC(p_ref_dc);
        return FALSE;
    }

	if (OpenClipboard(owner_wnd ? owner_wnd->GetSafeHwnd() : nullptr))
	{
		EmptyClipboard();
		SetClipboardData(CF_ENHMETAFILE, h_emf);
		CloseClipboard();
        if (used_screen_dc) ::ReleaseDC(nullptr, screen_dc.Detach());
        else if (owner_wnd && p_ref_dc) owner_wnd->ReleaseDC(p_ref_dc);
		return TRUE;
	}
	// Clipboard busy; delete handle to avoid leak
	DeleteEnhMetaFile(h_emf);
    if (used_screen_dc) ::ReleaseDC(nullptr, screen_dc.Detach());
    else if (owner_wnd && p_ref_dc) owner_wnd->ReleaseDC(p_ref_dc);
	return FALSE;
}

BOOL GraphicsExport::ExportToPng(CWnd* /*owner_wnd*/,
	const CRect& pixel_rect,
	const CString& file_path,
	const int bg_color,
	const DrawFn& draw_fn)
{
	BITMAPINFO bmi{};
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = pixel_rect.Width();
	bmi.bmiHeader.biHeight = -pixel_rect.Height();
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;

	void* p_bits = nullptr;
	HDC h_screen = ::GetDC(nullptr);
	HBITMAP h_dib = CreateDIBSection(h_screen, &bmi, DIB_RGB_COLORS, &p_bits, nullptr, 0);
	if (!h_dib) { ::ReleaseDC(nullptr, h_screen); return FALSE; }

	CDC mem_dc;
	mem_dc.CreateCompatibleDC(nullptr);
	const auto h_old = mem_dc.SelectObject(h_dib);

	CRect r(0, 0, pixel_rect.Width(), pixel_rect.Height());
	HBRUSH h_br = CreateSolidBrush(bg_color);
	FillRect(mem_dc.GetSafeHdc(), &r, h_br);
	DeleteObject(h_br);

	mem_dc.SetMapMode(MM_ANISOTROPIC);
	mem_dc.SetWindowExt(pixel_rect.Width(), pixel_rect.Height());
	mem_dc.SetViewportOrg(0, 0);
	mem_dc.SetViewportExt(pixel_rect.Width(), pixel_rect.Height());

	if (draw_fn)
		draw_fn(&mem_dc, CRect(0, 0, pixel_rect.Width(), pixel_rect.Height()));

	BOOL result = FALSE;
	ULONG_PTR gdiplus_token = 0;
	Gdiplus::GdiplusStartupInput gdiplus_startup_input;
	if (Gdiplus::Ok == Gdiplus::GdiplusStartup(&gdiplus_token, &gdiplus_startup_input, nullptr))
	{
		Gdiplus::Bitmap bitmap((HBITMAP)h_dib, (HPALETTE)nullptr);
		UINT num = 0, size = 0;
		Gdiplus::GetImageEncodersSize(&num, &size);
		if (size > 0)
		{
			auto p_codecs = (Gdiplus::ImageCodecInfo*)malloc(size);
			if (p_codecs && Gdiplus::Ok == Gdiplus::GetImageEncoders(num, size, p_codecs))
			{
				CLSID png_clsid{};
				for (UINT i = 0; i < num; ++i)
				{
					if (wcscmp(p_codecs[i].MimeType, L"image/png") == 0)
					{ png_clsid = p_codecs[i].Clsid; break; }
				}
				if (!IsEqualCLSID(png_clsid, CLSID{}))
				{
					result = (Gdiplus::Ok == bitmap.Save(file_path, &png_clsid, nullptr));
				}
			}
			if (p_codecs) free(p_codecs);
		}
		Gdiplus::GdiplusShutdown(gdiplus_token);
	}

	mem_dc.SelectObject(h_old);
	DeleteObject(h_dib);
	::ReleaseDC(nullptr, h_screen);
	return result;
}


