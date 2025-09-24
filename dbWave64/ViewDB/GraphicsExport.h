#pragma once

#include <functional>
#include <afxwin.h>

class CRect;

namespace GraphicsExport
{
	// Create an EMF with HIMETRIC bounds derived from pixelRect and copy it to the clipboard.
	// draw_fn must draw content within pixelRect coordinates (1:1) on the provided DC.
	BOOL CopyAsEmfToClipboard(CWnd* owner_wnd,
		const CRect& pixel_rect,
		const CString& title,
		const std::function<void(CDC* /*dc*/, const CRect& /*pixelRect*/ )>& draw_fn);

	// Render to an offscreen DIB and save as PNG. Returns TRUE on success.
	BOOL ExportToPng(CWnd* owner_wnd,
		const CRect& pixel_rect,
		const CString& file_path,
		int bg_color,
		const std::function<void(CDC* /*dc*/, const CRect& /*pixelRect*/ )>& draw_fn);
}


