#include "StdAfx.h"

#include "dbWave_constants.h"
#include "ViewDbTable.h"

#include "dbWaveDoc.h"

IMPLEMENT_DYNAMIC(ViewDbTable, CDaoRecordView)

ViewDbTable::ViewDbTable(LPCTSTR lpsz_template_name)
	: CDaoRecordView(lpsz_template_name)
{
	m_bEnableActiveAccessibility = FALSE;
}

ViewDbTable::ViewDbTable(UINT n_id_template)
	: CDaoRecordView(n_id_template)
{
	m_bEnableActiveAccessibility = FALSE;
}

ViewDbTable::~ViewDbTable()
= default;

BEGIN_MESSAGE_MAP(ViewDbTable, CDaoRecordView)
	ON_NOTIFY(NM_CLICK, IDC_TAB1, &ViewDbTable::on_nm_click_tab1)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &ViewDbTable::on_tcn_sel_change_tab1)

END_MESSAGE_MAP()

//  drawing
void ViewDbTable::OnDraw(CDC* p_dc)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

// diagnostics
#ifdef _DEBUG
void ViewDbTable::AssertValid() const
{
	CDaoRecordView::AssertValid();
}

CdbWaveDoc* ViewDbTable::GetDocument()
{
	return static_cast<CdbWaveDoc*>(m_pDocument);
}

void ViewDbTable::Dump(CDumpContext& dc) const
{
	CDaoRecordView::Dump(dc);
}

#endif //_DEBUG

BOOL ViewDbTable::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying the CREATESTRUCT cs
	return CDaoRecordView::PreCreateWindow(cs);
}

CDaoRecordset* ViewDbTable::OnGetRecordset()
{
	return GetDocument()->db_get_recordset();
}

void ViewDbTable::OnSize(UINT n_type, int cx, int cy)
{
	if (b_init_)
	{
		switch (n_type)
		{
		case SIZE_MAXIMIZED:
		case SIZE_RESTORED:
			if (cx <= 0 || cy <= 0)
				break;
			stretch_.resize_controls(n_type, cx, cy);
			break;
		default:
			break;
		}
	}
	CDaoRecordView::OnSize(n_type, cx, cy);
}

BOOL ViewDbTable::OnMove(const UINT n_id_move_command)
{
	const auto flag = CDaoRecordView::OnMove(n_id_move_command);
	auto p_document = GetDocument();
	if (m_auto_detect && p_document->db_get_current_spk_file_name(TRUE).IsEmpty())
	{
		GetParent()->PostMessage(WM_COMMAND, ID_VIEW_SPIKE_DETECTION, NULL);
	}
	p_document->update_all_views_db_wave(nullptr, HINT_DOC_MOVE_RECORD, nullptr);
	return flag;
}

void ViewDbTable::OnActivateView(BOOL b_activate, CView* p_activate_view, CView* p_deactive_view)
{
	if (b_activate)
	{
		AfxGetMainWnd()->PostMessage(WM_MYMESSAGE, HINT_ACTIVATE_VIEW,
		                             reinterpret_cast<LPARAM>(p_activate_view->GetDocument()));
	}
	else
	{
	}
	CDaoRecordView::OnActivateView(b_activate, p_activate_view, p_deactive_view);
}

BOOL ViewDbTable::OnPreparePrinting(CPrintInfo* p_info)
{
	if (!DoPreparePrinting(p_info))
		return FALSE;

	if (!COleDocObjectItem::OnPreparePrinting(this, p_info))
		return FALSE;

	return TRUE;
}

void ViewDbTable::OnBeginPrinting(CDC* /*p_dc*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void ViewDbTable::OnEndPrinting(CDC* /*p_dc*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

void ViewDbTable::OnPrint(CDC* p_dc, CPrintInfo* p_info)
{
	// TODO: add customized printing code here
	if (p_info->m_bDocObject)
		COleDocObjectItem::OnPrint(this, p_info, TRUE);
	else
		CView::OnPrint(p_dc, p_info);
}

void ViewDbTable::save_current_spk_file()
{
	const auto p_doc = GetDocument();
	const long current_position = p_doc->db_get_current_record_position();
	const long record_id = p_doc->db_get_current_record_id();

	if (p_spk_doc != nullptr && p_spk_doc->IsModified())
	{
		auto current_list = 0;
		if (spk_list_tab_ctrl.m_hWnd != nullptr) current_list = spk_list_tab_ctrl.GetCurSel();
		p_spk_list = p_spk_doc->set_index_current_spike_list(current_list);
		if (p_spk_list != nullptr && !p_spk_list->is_class_list_valid())
			p_spk_list->update_class_list();

		const auto spk_file_name = p_doc->db_set_current_spike_file_name();
		p_spk_doc->OnSaveDocument(spk_file_name);
		p_spk_doc->SetModifiedFlag(FALSE);

		auto n_spike_classes = 1;
		const auto n_spikes = (p_spk_list != nullptr) ? p_spk_list->get_spikes_count() : 0;
		if (n_spikes > 0)
		{
			n_spike_classes = p_spk_list->get_classes_count();
			if (!p_spk_list->is_class_list_valid()) 
				n_spike_classes = p_spk_list->update_class_list();				
		}
		p_doc->set_db_n_spikes(n_spikes);
		p_doc->set_db_n_spike_classes(n_spike_classes);
	}

	BOOL success = p_doc->db_move_to_id(record_id);
}

void ViewDbTable::increment_spike_flag()
{
	if (p_spk_doc != nullptr && p_spk_doc->IsModified())
	{
		const auto p_doc = GetDocument();
		// change flag is button is checked
		int flag = p_doc->db_get_current_record_flag();
		flag++;
		p_doc->db_set_current_record_flag(flag);
	}
}

void ViewDbTable::on_nm_click_tab1(NMHDR* p_nmhdr, LRESULT* p_result)
{
	const auto i_cur_sel = spk_list_tab_ctrl.GetCurSel();
	SendMessage(WM_MYMESSAGE, HINT_VIEW_TAB_CHANGE, MAKELPARAM(i_cur_sel, 0));
	*p_result = 0;
}

void ViewDbTable::on_tcn_sel_change_tab1(NMHDR* p_nmhdr, LRESULT* p_result)
{
	const auto i_cur_sel = spk_list_tab_ctrl.GetCurSel();
	PostMessage(WM_MYMESSAGE, HINT_VIEW_TAB_HAS_CHANGED, MAKELPARAM(i_cur_sel, 0));
	*p_result = 0;
}

// Default no-op renderer; derived classes should override when using copy_as_emf_to_clipboard
void ViewDbTable::render_for_export(CDC* /*p_dc*/, const CRect& /*pixel_rect*/)
{
}

void ViewDbTable::draw_text_block(CDC* p_dc, const CRect& device_rect, const int point_size, const CString& text, const UINT draw_text_flags, const LPCTSTR font_face) const
{
	const int saved = p_dc->SaveDC();

	// MM_ANISOTROPIC with 1:1 mapping inside device_rect
	p_dc->SetMapMode(MM_ANISOTROPIC);
	p_dc->SetWindowExt(device_rect.Width(), device_rect.Height());
	p_dc->SetViewportOrg(device_rect.left, device_rect.top);
	p_dc->SetViewportExt(device_rect.Width(), device_rect.Height());

	// Create font at requested point size
	LOGFONT lf{};
	lf.lfHeight = -MulDiv(point_size, p_dc->GetDeviceCaps(LOGPIXELSY), 72);
	_tcsncpy_s(lf.lfFaceName, font_face, LF_FACESIZE - 1);
	CFont font; font.CreateFontIndirect(&lf);
	const auto p_old_font = p_dc->SelectObject(&font);

	// Logical rect 0..W, 0..H maps to device_rect
	CRect r(0, 0, device_rect.Width(), device_rect.Height());
	p_dc->SetBkMode(TRANSPARENT);
	p_dc->DrawText(text, text.GetLength(), &r, draw_text_flags);

	if (p_old_font) p_dc->SelectObject(p_old_font);
	p_dc->RestoreDC(saved);
}

// Common EMF creation and clipboard copy
BOOL ViewDbTable::copy_as_emf_to_clipboard(const CRect& pixel_rect, const CString& title)
{
    // Create metafile DC with HIMETRIC bounds
    CMetaFileDC meta_dc;
    const auto p_ref_dc = GetDC();
    if (p_ref_dc == nullptr)
        return FALSE;
    const int dpi_x = p_ref_dc->GetDeviceCaps(LOGPIXELSX);
    const int dpi_y = p_ref_dc->GetDeviceCaps(LOGPIXELSY);
    const CRect himetric_bounds(0, 0,
        MulDiv(pixel_rect.Width(), 2540, dpi_x),
        MulDiv(pixel_rect.Height(), 2540, dpi_y));

    CString meta_title = _T("dbWave\0") + title;
    meta_title += _T("\0\0");
    const auto h_created = meta_dc.CreateEnhanced(p_ref_dc, nullptr, &himetric_bounds, meta_title);
    ASSERT(h_created != NULL);
    if (h_created == NULL)
    {
        ReleaseDC(p_ref_dc);
        return FALSE;
    }

    // Avoid SetAttribDC: derive text metrics explicitly via helper functions where needed

    // Let the derived view render its content
    render_for_export(&meta_dc, pixel_rect);

    // Cleanup and copy to clipboard
    ReleaseDC(p_ref_dc);
    const auto h_emf = meta_dc.CloseEnhanced();
    if (h_emf == nullptr)
        return FALSE;

    if (OpenClipboard())
    {
        EmptyClipboard();
        SetClipboardData(CF_ENHMETAFILE, h_emf);
        CloseClipboard();
        return TRUE;
    }
    // Clipboard busy; delete handle to avoid leak
    DeleteEnhMetaFile(h_emf);
    return FALSE;
}

int ViewDbTable::get_line_height_for_point_size(CDC* p_dc, const int point_size, const LPCTSTR font_face) const
{
    LOGFONT lf{};
    lf.lfHeight = -MulDiv(point_size, p_dc->GetDeviceCaps(LOGPIXELSY), 72);
    _tcsncpy_s(lf.lfFaceName, font_face, LF_FACESIZE - 1);
    CFont font; font.CreateFontIndirect(&lf);
    const auto p_old_font = p_dc->SelectObject(&font);
    TEXTMETRIC tm{};
    p_dc->GetTextMetrics(&tm);
    if (p_old_font) p_dc->SelectObject(p_old_font);
    return tm.tmHeight + tm.tmExternalLeading;
}

int ViewDbTable::calc_draw_text_height(CDC* p_dc, const int point_size, const CString& text, const int max_width, const UINT draw_text_flags, const LPCTSTR font_face) const
{
    LOGFONT lf{};
    lf.lfHeight = -MulDiv(point_size, p_dc->GetDeviceCaps(LOGPIXELSY), 72);
    _tcsncpy_s(lf.lfFaceName, font_face, LF_FACESIZE - 1);
    CFont font; font.CreateFontIndirect(&lf);
    const auto p_old_font = p_dc->SelectObject(&font);
    CRect rc(0, 0, max_width, 100000);
    p_dc->DrawText(text, rc, draw_text_flags | DT_CALCRECT);
    if (p_old_font) p_dc->SelectObject(p_old_font);
    return rc.Height();
}

// Export to PNG using an offscreen DIB and the same render_for_export()
// Returns TRUE on success. Path can be absolute or relative.
// usage example:
// CRect rect(0,0,w,h);
// export_to_png(rect, L"C:\\temp\\export.png");

BOOL ViewDbTable::export_to_png(const CRect& pixel_rect, const CString& file_path, const int bg_color)
{
    // Create a DIB section for offscreen rendering
    BITMAPINFO bmi{};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = pixel_rect.Width();
    bmi.bmiHeader.biHeight = -pixel_rect.Height(); // top-down DIB
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

    // Clear background
    const CRect r(0, 0, pixel_rect.Width(), pixel_rect.Height());
    HBRUSH h_br = CreateSolidBrush(bg_color);
    FillRect(mem_dc.GetSafeHdc(), &r, h_br);
    DeleteObject(h_br);

    // Set 1:1 mapping within the render area
    mem_dc.SetMapMode(MM_ANISOTROPIC);
    mem_dc.SetWindowExt(pixel_rect.Width(), pixel_rect.Height());
    mem_dc.SetViewportOrg(0, 0);
    mem_dc.SetViewportExt(pixel_rect.Width(), pixel_rect.Height());

    // Render
    render_for_export(&mem_dc, CRect(0, 0, pixel_rect.Width(), pixel_rect.Height()));

    // Save to PNG using GDI+
    BOOL result = FALSE;
    ULONG_PTR gdiplus_token = 0;
    Gdiplus::GdiplusStartupInput gdiplus_startup_input;
    if (Gdiplus::Ok == Gdiplus::GdiplusStartup(&gdiplus_token, &gdiplus_startup_input, nullptr))
    {
        Gdiplus::Bitmap bitmap((HBITMAP)h_dib, (HPALETTE)nullptr);

        // Get PNG encoder CLSID
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

    // Cleanup
    mem_dc.SelectObject(h_old);
    DeleteObject(h_dib);
    ::ReleaseDC(nullptr, h_screen);
    return result;
}