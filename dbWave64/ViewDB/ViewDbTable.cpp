#include "StdAfx.h"

#include "dbWave_constants.h"
#include "ViewDbTable.h"

#include "dbWave.h"
#include "dbWaveDoc.h"
#include "EmfExportHelper.h"
#include "Chart/ChartWnd.h"



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
	ON_COMMAND(ID_EXPORT_VIEW_TO_CLIPBOARD, &ViewDbTable::OnExportViewToClipboard)
	ON_UPDATE_COMMAND_UI(ID_EXPORT_VIEW_TO_CLIPBOARD, &ViewDbTable::OnUpdateExportViewToClipBoard)
	ON_COMMAND(ID_EXPORT_VIEW_AS_PNG, &ViewDbTable::OnExportViewAsPng)
	ON_UPDATE_COMMAND_UI(ID_EXPORT_VIEW_TO_CLIPBOARD, &ViewDbTable::OnUpdateExportViewAsPng)
	//ON_COMMAND(ID_FILE_PRINT, ViewDbTable::OnFilePrint)
	//ON_COMMAND(ID_FILE_PRINT_DIRECT, ViewDbTable::OnFilePrint)
	//ON_COMMAND(ID_FILE_PRINT_PREVIEW, ViewDbTable::OnFilePrintPreview)
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
    if (!data_service)
    {
        // Default to Dao-backed service constructed from our document
        data_service = std::make_unique<DaoTableDataService>(GetDocument());
    }
    return data_service->get_recordset();
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
    if (!print_renderer)
        print_renderer = std::make_unique<DefaultTablePrintRenderer>();
    return print_renderer->prepare_printing(this, p_info);
}

void ViewDbTable::OnBeginPrinting(CDC* p_dc, CPrintInfo* pInfo)
{
    if (!print_renderer)
        print_renderer = std::make_unique<DefaultTablePrintRenderer>();
    print_renderer->begin_printing(this, p_dc, pInfo);
}

void ViewDbTable::OnEndPrinting(CDC* p_dc, CPrintInfo* pInfo)
{
    if (!print_renderer)
        print_renderer = std::make_unique<DefaultTablePrintRenderer>();
    print_renderer->end_printing(this, p_dc, pInfo);
}

void ViewDbTable::OnPrint(CDC* p_dc, CPrintInfo* pInfo)
{
    if (!print_renderer)
        print_renderer = std::make_unique<DefaultTablePrintRenderer>();
    print_renderer->print_page(this, p_dc, pInfo);
}

void ViewDbTable::framework_default_print(CDC* p_dc, CPrintInfo* pInfo)
{
    if (pInfo->m_bDocObject)
        COleDocObjectItem::OnPrint(this, pInfo, TRUE);
    else
        CView::OnPrint(p_dc, pInfo);
}

void ViewDbTable::render_region(CDC* p_dc, ChartWnd& chart, const CRect& full_rect, const options_print* opts) const
{
    // Compute data rect from full rect (reserve margins for scale bar)
    const CRect data_rect = EmfExportHelper::GetDataRectangle(full_rect);
    chart.export_to_emf(p_dc, &data_rect, opts);
    chart.draw_axes_to_emf(p_dc, data_rect);
    chart.draw_scale_bar_to_emf(p_dc, full_rect, nullptr);
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
void ViewDbTable::render_for_export(CDC*)
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
BOOL ViewDbTable::copy_as_emf_to_clipboard(const CString& title)
{
    // Ensure export pixel size is set BEFORE EMF is created
    auto& opts = static_cast<CdbWaveApp*>(AfxGetApp())->options_print_data;
    const CRect px = compute_export_bounds();
    opts.horizontal_resolution = std::max(1, px.Width());
    opts.vertical_resolution   = std::max(1, px.Height());
#ifdef _DEBUG
    TRACE(_T("[copy_as_emf_to_clipboard] TargetPx=(%d,%d)\n"), opts.horizontal_resolution, opts.vertical_resolution);
#endif
    return GraphicsExport::CopyAsEmfToClipboard(this, title, [this](CDC* dc) { this->render_for_export(dc); }
    );
}

int ViewDbTable::get_line_height_for_point_size(CDC* p_dc, const int point_size, const LPCTSTR font_face)
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

int ViewDbTable::calc_draw_text_height(CDC* p_dc, const int point_size, const CString& text, const int max_width, const UINT draw_text_flags, const LPCTSTR font_face)
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

BOOL ViewDbTable::export_to_png(const CString& file_path, const int bg_color)
{
    return GraphicsExport::ExportToPng(
        this,
        file_path,
        bg_color,
        [this](CDC* dc) { this->render_for_export(dc); }
    );
}

CRect ViewDbTable::compute_export_bounds()
{
	CRect client{};
	GetClientRect(&client);
	return client;
}

int ViewDbTable::begin_anisotropic_export(CDC* p_dc, const CRect& export_bounds) const
{
	const int saved = p_dc->SaveDC();
	p_dc->SetMapMode(MM_ANISOTROPIC);
	// Define logical space 0..W, 0..H mapped to export_bounds at origin
	const int w = export_bounds.Width();
	const int h = export_bounds.Height();
	p_dc->SetWindowOrg(0, 0);
	p_dc->SetWindowExt(w, h);
	p_dc->SetViewportOrg(0, 0);
	p_dc->SetViewportExt(w, h);
	// Clip to the logical export area
	p_dc->SelectClipRgn(nullptr);
	p_dc->IntersectClipRect(0, 0, w, h);
#ifdef _DEBUG
	{
		const int mm = p_dc->GetMapMode();
		const CPoint vo = p_dc->GetViewportOrg();
		const CSize ve = p_dc->GetViewportExt();
		const CPoint wo = p_dc->GetWindowOrg();
		const CSize we = p_dc->GetWindowExt();
		CRect clip_box; const int clip_type = p_dc->GetClipBox(&clip_box);
		CString msg;
		msg.Format(_T("[begin_aniso] mm=%d bounds=[%d,%d,%d,%d] WO=(%d,%d) WE=(%d,%d) VO=(%d,%d) VE=(%d,%d) CLIP=%d [%d,%d,%d,%d]\n"),
			mm, export_bounds.left, export_bounds.top, export_bounds.right, export_bounds.bottom,
			wo.x, wo.y, we.cx, we.cy, vo.x, vo.y, ve.cx, ve.cy, clip_type,
			clip_box.left, clip_box.top, clip_box.right, clip_box.bottom);
		AfxOutputDebugString(msg);
	}
#endif
	return saved;
}

void ViewDbTable::end_anisotropic_export(CDC* p_dc, const int saved_dc) const
{
	if (saved_dc)
		p_dc->RestoreDC(saved_dc);
}
void ViewDbTable::OnExportViewToClipboard()
{
	serialize_windows_state(b_save);
	copy_as_emf_to_clipboard(GetDocument()->GetTitle());
	serialize_windows_state(b_restore);
}

void ViewDbTable::serialize_windows_state(const BOOL save, int tab_index)
{
}

void ViewDbTable::OnExportViewAsPng()
{
	//if (p_options_view_data == nullptr)
	//{
	//	const auto p_app = static_cast<CdbWaveApp*>(AfxGetApp());
	//	p_options_view_data = &(p_app->options_view_data);
	//}

	//DlgCopyAs dlg;
	//dlg.m_n_abscissa = options_view_data_->hz_resolution;
	//dlg.m_n_ordinates = options_view_data_->vt_resolution;
	//dlg.b_graphics = options_view_data_->b_graphics;
	//dlg.m_i_option = options_view_data_->b_contours;
	//dlg.m_i_unit = options_view_data_->b_units;

	//// invoke dialog box
	//if (IDOK == dlg.DoModal())
	//{
	//	options_view_data_->b_graphics = dlg.b_graphics;
	//	options_view_data_->b_contours = dlg.m_i_option;
	//	options_view_data_->b_units = dlg.m_i_unit;
	//	options_view_data_->hz_resolution = dlg.m_n_abscissa;
	//	options_view_data_->vt_resolution = dlg.m_n_ordinates;
	const CString file_path = _T("c:\\temp\\export.png");
		//if (dlg.b_graphics) {
	constexpr int bg_color = 0;
	export_to_png(file_path, bg_color);
	//	}
	//}
}

void ViewDbTable::OnUpdateExportViewToClipBoard(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(can_export_view());
}

void ViewDbTable::OnUpdateExportViewAsPng(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(can_export_png());
}
