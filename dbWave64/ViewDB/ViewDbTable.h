#pragma once
#include <afxdao.h>
#pragma warning(disable : 4995)
#include <memory>

#include "SpikeListTabCtrl.h"
#include "dbTableMain.h"
#include "options_print.h"
#include "StretchControls.h"
#include "Chart/ChartWnd.h"
#include "ViewDB/TableDataService.h"
#include "ViewDB/TablePrintRenderer.h"
#include "ViewDB/GraphicsExport.h"

constexpr auto b_restore = 0;
constexpr auto b_save = 1;
class CdbWaveDoc;

class ViewDbTable : public CDaoRecordView
{
	DECLARE_DYNAMIC(ViewDbTable)

protected:
	ViewDbTable(LPCTSTR lpsz_template_name);
	ViewDbTable(UINT n_id_template);
	~ViewDbTable() override;

public:
	CdbTableMain* p_db_table_main{ nullptr };
	boolean m_auto_detect { false };
	boolean m_auto_increment {false};

	// Data service abstraction 
	std::unique_ptr<ITableDataService> data_service {};
	std::unique_ptr<ITablePrintRenderer> print_renderer {};

	// Framework default print helper (exposes base call for renderer)
	void framework_default_print(CDC* p_dc, CPrintInfo* p_info);

	CdbWaveDoc* GetDocument();
	CDaoRecordset* OnGetRecordset() override;
	BOOL OnMove(UINT n_id_move_command) override;
	void OnDraw(CDC* p_dc) override;
	BOOL PreCreateWindow(CREATESTRUCT& cs) override;

	CSpikeDoc* p_spk_doc { nullptr };
	SpikeList* p_spk_list{ nullptr };
	void save_current_spk_file();
	void increment_spike_flag();
	SpikeListTabCtrl spk_list_tab_ctrl {};

#ifdef _DEBUG
	void AssertValid() const override;
#ifndef _WIN32_WCE
	void Dump(CDumpContext& dc) const override;
#endif
#endif
	// print view
protected:
	CRect pixels_margin_;				
	int file_0_ {0};
	long l_first_0_{ 0 };
	long l_last0_{ 0 };
	int pixels_count_0_{ 0 };
	int files_count_{ 0 };
	int n_rows_per_page_{ 0 };	// USER: nb files/page
	long l_print_first_{ 0 };	
	long l_print_len_{ 0 };		// nb pts per line
	long print_first_{ 0 };
	long print_last_{ 0 };
	BOOL is_printing_ { false };
	CRect rect_data_;
	CRect rect_spike_;

	// printer parameters
	TEXTMETRIC text_metric_ {}; 
	LOGFONT log_font_ {}; 
	CFont* p_old_font_ { nullptr };
	CFont font_print_; 
	CRect print_rect_;

	BOOL OnPreparePrinting(CPrintInfo* p_info) override;
	void OnBeginPrinting(CDC* p_dc, CPrintInfo* pInfo) override;
	void OnEndPrinting(CDC* p_dc, CPrintInfo* pInfo) override;
	void OnPrint(CDC* p_dc, CPrintInfo* pInfo) override;
	void OnActivateView(BOOL b_activate, CView* p_activate_view, CView* p_deactive_view) override;

	// parameters for OnSize
	CStretchControl stretch_ {};
	BOOL b_init_ {false};

public:
	// Export helpers (EMF/PNG)
	BOOL copy_as_emf_to_clipboard(const CString& title);
	BOOL export_to_png(const CString& file_path, int bg_color = RGB(255,255,255));
	virtual void render_for_export(CDC* p_dc);
	// Compute desired export pixel bounds (width/height). Default: view client rect
	virtual CRect compute_export_bounds();
	virtual bool can_export_view() const { return true; }
	virtual bool can_export_png() const { return true; }
	virtual void serialize_windows_state(BOOL save, int tab_index = -1);

	// Anisotropic export mapping helpers (1:1 logical mapping to export bounds)
	int begin_anisotropic_export(CDC* p_dc, const CRect& export_bounds) const;
	void end_anisotropic_export(CDC* p_dc, int saved_dc) const;

	// Shared helper: render one EMF region (content -> axes -> scale bar)
	void render_region(CDC* p_dc, ChartWnd& chart, const CRect& full_rect, const options_print* opts) const;

	// Text helpers (device-independent in MM_ANISOTROPIC)
	void draw_text_block(CDC* p_dc, const CRect& device_rect, int point_size, const CString& text, UINT draw_text_flags, LPCTSTR font_face = _T("Arial")) const;
	static int get_line_height_for_point_size(CDC* p_dc, int point_size, LPCTSTR font_face = _T("Arial"));
	static int calc_draw_text_height(CDC* p_dc, int point_size, const CString& text, int max_width, UINT draw_text_flags, LPCTSTR font_face = _T("Arial"));

protected:
	afx_msg void OnSize(UINT n_type, int cx, int cy);
	afx_msg void on_nm_click_tab1(NMHDR* p_nmhdr, LRESULT* p_result);
	afx_msg void on_tcn_sel_change_tab1(NMHDR* p_nmhdr, LRESULT* p_result);
	afx_msg void OnExportViewToClipboard();
	afx_msg void OnUpdateExportViewToClipBoard(CCmdUI* pCmdUI);
	afx_msg void OnExportViewAsPng();
	afx_msg void OnUpdateExportViewAsPng(CCmdUI* pCmdUI);

	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in dataView.cpp
inline CdbWaveDoc* ViewDbTable::GetDocument()
{
	return (CdbWaveDoc*)m_pDocument;
}
#endif
