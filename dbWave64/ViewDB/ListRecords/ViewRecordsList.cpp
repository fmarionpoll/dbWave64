#include "StdAfx.h"
#include "ViewRecordsList.h"

#include <algorithm>
#include "dbWave.h"
#include "resource.h"
#include "Adapters.h"
#include "ChildFrame.h"
#include "MainFrm.h"
#include "RegistryManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(ViewRecordsList, ViewDbTable)

// Static session-level state for this view
ViewRecordsListState ViewRecordsList::s_view_state_ {};

BEGIN_MESSAGE_MAP(ViewRecordsList, ViewDbTable)
	ON_COMMAND(ID_RECORD_PAGE_UP, &ViewRecordsList::on_record_page_up)
	ON_COMMAND(ID_RECORD_PAGE_DOWN, &ViewRecordsList::on_record_page_down)

	ON_WM_SIZE()

	ON_BN_CLICKED(IDC_DISPLAY_DATA, &ViewRecordsList::on_bn_clicked_display_data)
	ON_BN_CLICKED(IDC_DISPLAY_SPIKES, &ViewRecordsList::on_bn_clicked_display_spikes)
	ON_BN_CLICKED(IDC_DISPLAY_NOTHING, &ViewRecordsList::on_bn_clicked_display_nothing)

	ON_EN_CHANGE(IDC_TIMEFIRST, &ViewRecordsList::on_en_change_time_first)
	ON_EN_CHANGE(IDC_TIMELAST, &ViewRecordsList::on_en_change_time_last)
	ON_EN_CHANGE(IDC_AMPLITUDESPAN, &ViewRecordsList::on_en_change_amplitude_span)
	ON_EN_CHANGE(IDC_SPIKECLASS, &ViewRecordsList::on_en_change_spike_class)
	ON_BN_CLICKED(IDC_CHECKFILENAME, &ViewRecordsList::on_bn_clicked_check_filename)
	ON_BN_CLICKED(IDC_FILTERCHECK, &ViewRecordsList::on_click_median_filter)
	ON_BN_CLICKED(IDC_CHECK2, &ViewRecordsList::on_bn_clicked_check2)
	ON_BN_CLICKED(IDC_CHECK1, &ViewRecordsList::on_bn_clicked_check1)
	ON_BN_CLICKED(IDC_RADIOALLCLASSES, &ViewRecordsList::on_bn_clicked_radio_all_classes)
	ON_BN_CLICKED(IDC_RADIOONECLASS, &ViewRecordsList::on_bn_clicked_radio_one_class)

	ON_MESSAGE(WM_MYMESSAGE, &ViewRecordsList::on_my_message)

	ON_NOTIFY(HDN_ENDTRACK, 0, &ViewRecordsList::on_hdn_end_track_list_ctrl)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LISTCTRL, &ViewRecordsList::on_lvn_column_click_list_ctrl)
    ON_NOTIFY(LVN_ITEMACTIVATE, IDC_LISTCTRL, &ViewRecordsList::on_item_activate_list_ctrl)
    ON_NOTIFY(LVN_ITEMCHANGED, IDC_LISTCTRL, &ViewRecordsList::on_item_changed_list_ctrl)
	ON_NOTIFY(NM_DBLCLK, IDC_LISTCTRL, &ViewRecordsList::on_dbl_clk_list_ctrl)

END_MESSAGE_MAP()

LRESULT ViewRecordsList::on_my_message(const WPARAM w_param, const LPARAM l_param)
{
	const int threshold = LOWORD(l_param);
	if (w_param == HINT_VIEW_TAB_HAS_CHANGED)
	{
		GetDocument()->get_current_spike_file()->set_index_current_spike_list(threshold);
		m_list_ctrl_.refresh_display();
	}
	return 0L;
}

ViewRecordsList::ViewRecordsList() : ViewDbTable(IDD)
{
}

ViewRecordsList::~ViewRecordsList() = default;

void ViewRecordsList::DoDataExchange(CDataExchange* p_dx)
{
	ViewDbTable::DoDataExchange(p_dx);
	DDX_Text(p_dx, IDC_TIMEFIRST, m_time_first_);
	DDX_Text(p_dx, IDC_TIMELAST, m_time_last_);
	DDX_Text(p_dx, IDC_AMPLITUDESPAN, m_amplitude_span_);
	DDX_Text(p_dx, IDC_SPIKECLASS, m_spike_class_);
}

void ViewRecordsList::OnInitialUpdate()
{
	ViewDbTable::OnInitialUpdate();

	VERIFY(m_list_ctrl_.SubclassDlgItem(IDC_LISTCTRL, this));
    {
        const DWORD ex0 = m_list_ctrl_.GetExtendedStyle();
        DWORD ex = ex0 | LVS_EX_FULLROWSELECT | LVS_EX_SUBITEMIMAGES;
        ex &= ~LVS_EX_ONECLICKACTIVATE; // avoid hot-track activation side effects
        m_list_ctrl_.SetExtendedStyle(ex);
    }

	// initialize document and db table pointer (same as original view)
	const auto db_wave_doc = GetDocument();
	p_db_table_main = &db_wave_doc->db_table->m_main_table_set;

	// make the list control stretch with the parent view
	stretch_.attach_parent(this);
	stretch_.new_prop(IDC_LISTCTRL, XLEQ_XREQ, YTEQ_YBEQ);
	b_init_ = TRUE;

	// load persisted column widths if present
	m_columns_width_.SetSize(N_COLUMNS2);
	bool have_widths = false;
	for (int i = 0; i < N_COLUMNS2; ++i)
	{
		CString key; key.Format(_T("col_%d"), i);
		const int w = ReadRegistryInt(_T("ViewListRecords\\DataListCtrl"), key, -1);
		if (w > 0) { m_columns_width_.SetAt(i, static_cast<UINT>(w)); have_widths = true; }
	}

	DisplaySettings settings;
	settings.image_width = 300;
	settings.image_height = 50;
	settings.display_mode = DisplayMode::Data;

	// Wire provider to current document and minimal renderers
	auto* provider = new DbWaveDocProviderAdapter(db_wave_doc);
	auto* data_renderer = new ChartDataRendererAdapter();
	auto* spike_renderer = new ChartSpikeRendererAdapter(db_wave_doc);
	m_list_ctrl_.init(provider, settings, data_renderer, spike_renderer, have_widths ? &m_columns_width_ : nullptr);

	const int n_records = db_wave_doc->db_get_records_count();
	m_list_ctrl_.SetItemCountEx(n_records);
	const int per_page = m_list_ctrl_.GetCountPerPage();
    int current_index = db_wave_doc->db_get_current_record_position();
	current_index = std::max(current_index, 0);
	if (current_index >= 0 && n_records > 0)
	{
		const int page = std::max(m_list_ctrl_.GetCountPerPage(), 1);
		int first = 0; // start at the top for initial load for predictability
		int last = first + page - 1;
		if (last >= n_records)
		{
			last = n_records - 1;
			first = std::max(last - page + 1, 0);
		}
        m_list_ctrl_.set_visible_range(first, last);
        if (current_index >= 0 && current_index < m_list_ctrl_.GetItemCount())
        {
            suppress_selection_events_ = true;
            m_list_ctrl_.SetItemState(current_index, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
            m_list_ctrl_.EnsureVisible(current_index, FALSE);
            suppress_selection_events_ = false;
        }
	}
	else
	{
		m_list_ctrl_.set_visible_range(0, per_page - 1);
	}

	// initialize controls state similar to ViewListRecords
	if (s_view_state_.primed)
	{
		// Apply session state immediately to avoid flashing defaults
		restore_controls_state();
	}
	else
	{
		CheckDlgButton(IDC_CHECKFILENAME, BST_UNCHECKED);
		CheckDlgButton(IDC_CHECK1, BST_UNCHECKED);
		GetDlgItem(IDC_TIMEFIRST)->EnableWindow(FALSE);
		GetDlgItem(IDC_TIMELAST)->EnableWindow(FALSE);
		CheckDlgButton(IDC_CHECK2, BST_UNCHECKED);
		GetDlgItem(IDC_AMPLITUDESPAN)->EnableWindow(FALSE);

		// default to data
		on_bn_clicked_display_data();
	}
}

void ViewRecordsList::OnUpdate(CView* p_sender, const LPARAM l_hint, CObject* p_hint)
{
	if (!b_init_)
		return;

	switch (LOWORD(l_hint))
	{
	case HINT_GET_SELECTED_RECORDS:
	{
		const auto p_document = GetDocument();
		p_document->selected_records.RemoveAll();
		const int selected_count = static_cast<int>(m_list_ctrl_.GetSelectedCount());
		if (selected_count > 0)
		{
			p_document->selected_records.SetSize(selected_count);
			int n_item = -1;
			for (int i = 0; i < selected_count; i++)
			{
				n_item = m_list_ctrl_.GetNextItem(n_item, LVNI_SELECTED);
				ASSERT(n_item != -1);
				p_document->selected_records.SetAt(i, n_item);
			}
		}
	}
	break;

	case HINT_SET_SELECTED_RECORDS:
	{
		const auto p_document = GetDocument();
		const UINT u_selected_count = p_document->selected_records.GetSize();

		int item = -1;
		item = m_list_ctrl_.GetNextItem(item, LVNI_SELECTED);
		while (item != -1)
		{
			m_list_ctrl_.SetItemState(item, 0, LVIS_SELECTED);
			item = m_list_ctrl_.GetNextItem(item, LVNI_SELECTED);
		}

		if (u_selected_count > 0)
		{
			for (UINT i = 0; i < u_selected_count; i++)
			{
				item = static_cast<int>(p_document->selected_records.GetAt(static_cast<int>(i)));
				m_list_ctrl_.SetItemState(item, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
			}
		}
		m_list_ctrl_.EnsureVisible(item, FALSE);
	}
	break;

	case HINT_REPLACE_VIEW:
		m_list_ctrl_.set_visible_range(0, m_list_ctrl_.GetCountPerPage() - 1);
		update_controls();
		break;

	case HINT_REQUERY:
		fill_list_box();
	case HINT_DOC_HAS_CHANGED:
		m_list_ctrl_.set_visible_range(-1, -1);
	case HINT_DOC_MOVE_RECORD:
	default:
		update_controls();
		break;
	}
}

void ViewRecordsList::fill_list_box()
{
	m_list_ctrl_.DeleteAllItems();
	const int n_records = GetDocument()->db_get_records_count();
	m_list_ctrl_.SetItemCountEx(n_records);
}

void ViewRecordsList::update_controls()
{
	const auto db_wave_doc = GetDocument();
	const int i_file = db_wave_doc->db_get_current_record_position();

	m_list_ctrl_.set_current_selection(i_file);
	m_list_ctrl_.EnsureVisible(i_file, FALSE);

	//if (m_options_view_data_->display_mode == 2)
	//{
	//	CSpikeDoc* p_spk_doc = GetDocument()->open_current_spike_file();
	//	if (p_spk_doc != nullptr)
	//	{
	//		const auto spk_list_size = p_spk_doc->get_spike_list_size();
	//		if (spk_list_tab_ctrl.GetItemCount() < spk_list_size)
	//			spk_list_tab_ctrl.init_ctrl_tab_from_spike_doc(p_spk_doc);
	//	}
	//}

	//pdb_doc->SetModifiedFlag(true);
	//pdb_doc->UpdateAllViews(this, HINT_DOC_MOVE_RECORD, nullptr);
	db_wave_doc->update_all_views_db_wave(this, HINT_DOC_MOVE_RECORD, nullptr);

	//POSITION pos = pdb_doc->GetFirstViewPosition();
	//int n_views = 0;
	//while (pos != NULL)
	//{
	//	CView* pView = pdb_doc->GetNextView(pos);
	//	CMainFrame* frame = (CMainFrame*)pView->GetParentFrame();
	//	frame->OnUpdate(this, HINT_DOC_MOVE_RECORD, nullptr);

	//	//pView->UpdateWindow();
	//	n_views++;
	//}

}

void ViewRecordsList::delete_records()
{
	// save index current file
	const auto current_index = std::max<long>( GetDocument()->db_get_current_record_position() - 1, 0);

	// loop on C_data_list_ctrl to delete all selected items
	const auto pdb_doc = GetDocument();
	auto pos = m_list_ctrl_.GetFirstSelectedItemPosition();
	if (pos == nullptr)
	{
		AfxMessageBox(_T("No item selected: delete operation failed"));
		return;
	}
	// assume no one else accesses to the database at the same time
	auto n_files_to_delete = 0;
	while (pos)
	{
		const auto n_item = m_list_ctrl_.GetNextSelectedItem(pos);
		if (pdb_doc->db_set_current_record_position(n_item - n_files_to_delete))
			pdb_doc->db_delete_current_record();
		n_files_to_delete++;
	}

	BOOL result = pdb_doc->db_set_current_record_position(current_index);
	pdb_doc->UpdateAllViews(nullptr, HINT_REQUERY, nullptr);
}

void ViewRecordsList::on_item_activate_list_ctrl(NMHDR* p_nmhdr, LRESULT* p_result)
{
	const auto p_item_activate = reinterpret_cast<NMITEMACTIVATE*>(p_nmhdr);
	if (p_item_activate->iItem >= 0)
		GetDocument()->db_set_current_record_position(p_item_activate->iItem);
	GetDocument()->UpdateAllViews(nullptr, HINT_DOC_MOVE_RECORD, nullptr);
	*p_result = 0;
}

void ViewRecordsList::on_item_changed_list_ctrl(NMHDR* p_nmhdr, LRESULT* p_result)
{
    auto* p_nmlv = reinterpret_cast<LPNMLISTVIEW>(p_nmhdr);
    *p_result = 0;
    if ((p_nmlv->uChanged & LVIF_STATE) == 0)
        return;
    if (suppress_selection_events_)
        return;
    const UINT was_selected = (p_nmlv->uOldState & LVIS_SELECTED);
    const UINT is_selected = (p_nmlv->uNewState & LVIS_SELECTED);
    const UINT is_focused  = (p_nmlv->uNewState & LVIS_FOCUSED);
    // Only react to the final focused-selected item to avoid transient or cache-driven state changes
    if (!was_selected && is_selected && is_focused)
    {
        int index = m_list_ctrl_.GetNextItem(-1, LVNI_SELECTED | LVNI_FOCUSED);
        if (index < 0)
            index = p_nmlv->iItem;
        if (index >= 0)
        {
            // Update doc selection first
            GetDocument()->db_set_current_record_position(index);
            // Align cache viewport around the selection for stability
            const int total = m_list_ctrl_.GetItemCount();
            if (total > 0)
            {
                const int page = std::max(m_list_ctrl_.GetCountPerPage(), 1);
                const int first = std::max(0, std::min(index, std::max(total - page, 0)));
                const int last  = std::min(first + page - 1, std::max(total - 1, 0));
                m_list_ctrl_.set_visible_range(first, last);
            }
            // Notify
            GetDocument()->UpdateAllViews(nullptr, HINT_DOC_MOVE_RECORD, nullptr);
        }
    }
}

void ViewRecordsList::on_dbl_clk_list_ctrl(NMHDR* p_nmhdr, LRESULT* p_result)
{
	*p_result = 0;
	// quit the current view and switch to spike detection view
	GetParent()->PostMessage(WM_COMMAND, static_cast<WPARAM>(ID_VIEW_SPIKE_DETECTION), static_cast<LPARAM>(NULL));
}

void ViewRecordsList::on_record_page_up()
{
	m_list_ctrl_.SendMessage(WM_VSCROLL, SB_PAGEUP, NULL);
}

void ViewRecordsList::on_record_page_down()
{
	m_list_ctrl_.SendMessage(WM_VSCROLL, SB_PAGEDOWN, NULL);
}

void ViewRecordsList::OnActivateView(const BOOL b_activate, CView* p_activate_view, CView* p_deactive_view)
{
	auto* p_mainframe = static_cast<CMainFrame*>(AfxGetMainWnd());
	if (b_activate)
	{
        restore_controls_state();

		// make sure the secondary toolbar is not visible (none is defined for the database)
		if (p_mainframe->m_p_second_tool_bar != nullptr)
			p_mainframe->ShowPane(p_mainframe->m_p_second_tool_bar, FALSE, FALSE, TRUE);
		// load status
		m_nStatus = static_cast<CChildFrame*>(p_mainframe->MDIGetActive())->m_n_status;
		p_mainframe->PostMessageW(WM_MYMESSAGE, HINT_ACTIVATE_VIEW, reinterpret_cast<LPARAM>(p_activate_view->GetDocument()));
	}
	else
	{
		save_controls_state();
		if (p_activate_view != nullptr)
			static_cast<CChildFrame*>(p_mainframe->MDIGetActive())->m_n_status = m_nStatus;
	}
	ViewDbTable::OnActivateView(b_activate, p_activate_view, p_deactive_view);
}

void ViewRecordsList::save_controls_state()
{
	UpdateData(TRUE);
	ViewRecordsListState state;
	state.primed = true;
	state.display_mode = m_list_ctrl_.get_display_mode();
	state.b_display_file_name = m_list_ctrl_.get_display_file_name();
	state.b_filter_dat = m_list_ctrl_.get_transform_mode() == DataTransform::MedianFilter;
	state.b_set_time_span = m_list_ctrl_.get_timespan_adjust_mode();
	state.t_first = m_time_first_;
	state.t_last = m_time_last_;
	state.b_set_mv_span = m_list_ctrl_.get_amplitude_adjust_mode();
	state.mv_span = m_amplitude_span_;
	state.b_all_classes = m_list_ctrl_.get_spike_plot_mode() == SpikePlotMode::AllClasses;
	state.spike_class = m_spike_class_;
	s_view_state_ = state;
}

void ViewRecordsList::restore_controls_state()
{
	const ViewRecordsListState state = s_view_state_;
	if (state.primed)
	{
		// display file name
		const BOOL show = state.b_display_file_name;
		CheckDlgButton(IDC_CHECKFILENAME, show ? BST_CHECKED : BST_UNCHECKED);
		m_list_ctrl_.set_display_file_name(static_cast<boolean>(show));

		// filter
		const BOOL b_filter_dat = state.b_filter_dat;
		CheckDlgButton(IDC_FILTERCHECK, b_filter_dat ? BST_CHECKED : BST_UNCHECKED);
		m_list_ctrl_.set_transform_mode(b_filter_dat ? DataTransform::MedianFilter : DataTransform::None);

		// time span
		const BOOL b_set_time_span = state.b_set_time_span;
		CheckDlgButton(IDC_CHECK1, b_set_time_span ? BST_CHECKED : BST_UNCHECKED);
		GetDlgItem(IDC_TIMEFIRST)->EnableWindow(b_set_time_span);
		GetDlgItem(IDC_TIMELAST)->EnableWindow(b_set_time_span);
		m_time_first_ = state.t_first;
		m_time_last_ = state.t_last;
		m_list_ctrl_.set_timespan_adjust_mode(static_cast<boolean>(b_set_time_span));
		m_list_ctrl_.set_time_intervals(m_time_first_, m_time_last_);

		// amplitude span
		const BOOL b_set_mv_span = state.b_set_mv_span;
		CheckDlgButton(IDC_CHECK2, b_set_mv_span ? BST_CHECKED : BST_UNCHECKED);
		GetDlgItem(IDC_AMPLITUDESPAN)->EnableWindow(b_set_mv_span);
		m_list_ctrl_.set_amplitude_adjust_mode(static_cast<boolean>(b_set_mv_span));
		m_amplitude_span_ = state.mv_span;
		m_list_ctrl_.set_amplitude_span(m_amplitude_span_);

		// enable/disable controls according to display mode
		m_spike_class_ = state.spike_class;
		if (state.display_mode == DisplayMode::Data)
			on_bn_clicked_display_data();
		else if (state.display_mode == DisplayMode::Spikes)
			on_bn_clicked_display_spikes();
		else
			on_bn_clicked_display_nothing();

		// update buttons for spike plot mode
		if (state.b_all_classes)
		{
			CheckDlgButton(IDC_RADIOALLCLASSES, BST_CHECKED);
			m_list_ctrl_.set_spike_plot_mode(SpikePlotMode::AllClasses, m_spike_class_);
		}
		else {
			CheckDlgButton(IDC_RADIOONECLASS, BST_CHECKED);
			m_list_ctrl_.set_spike_plot_mode(SpikePlotMode::OneClass, m_spike_class_);
			
		}

		UpdateData(FALSE);
	}
}

void ViewRecordsList::OnSize(const UINT n_type, const int cx, const int cy)
{
	ViewDbTable::OnSize(n_type, cx, cy);
	if (!b_init_)
		return;
	stretch_.resize_controls(n_type, cx, cy);
	if (IsWindow(m_list_ctrl_.m_hWnd))
	{
		CRect rect;
		m_list_ctrl_.GetClientRect(&rect);
		m_list_ctrl_.fit_columns_to_size(rect.Width());
	}
}

void ViewRecordsList::on_hdn_end_track_list_ctrl(NMHDR* p_nmhdr, LRESULT* p_result)
{
	const auto p_hdr = reinterpret_cast<LPNMHEADER>(p_nmhdr);
	if (p_hdr->iItem == CTRL2_COL_CURVE)
		m_list_ctrl_.resize_signal_column(p_hdr->pitem->cxy);
	*p_result = 0;

	// Save all column widths
	for (int i = 0; i < N_COLUMNS2; ++i)
	{
		CString key; key.Format(_T("col_%d"), i);
		WriteRegistryInt(_T("ViewListRecords\\DataListCtrl"), key, m_list_ctrl_.GetColumnWidth(i));
	}
}

void ViewRecordsList::set_display_mode(const DisplayMode mode)
{
	m_list_ctrl_.set_display_mode(mode);
	m_list_ctrl_.refresh_display();
	int iID = IDC_DISPLAY_DATA;
	if (mode == DisplayMode::Spikes)
		iID = IDC_DISPLAY_SPIKES;
	else if (mode == DisplayMode::None)
		iID = IDC_DISPLAY_NOTHING;
	check_display_button(iID);

	GetDlgItem(IDC_FILTERCHECK)->EnableWindow(mode == DisplayMode::Data ? TRUE: FALSE);
	GetDlgItem(IDC_RADIOALLCLASSES)->EnableWindow(mode == DisplayMode::Spikes? TRUE : FALSE);
	GetDlgItem(IDC_RADIOONECLASS)->EnableWindow(mode == DisplayMode::Spikes ? TRUE : FALSE);
	GetDlgItem(IDC_SPIKECLASS)->EnableWindow(mode == DisplayMode::Spikes ? TRUE : FALSE);
}

void ViewRecordsList::check_display_button(int iID)
{
	static_cast<CButton*>(GetDlgItem(iID))->SetCheck(BST_CHECKED);
}

void ViewRecordsList::on_bn_clicked_display_data()
{
	set_display_mode(DisplayMode::Data);
	const BOOL b_filter_dat = static_cast<CButton*>(GetDlgItem(IDC_FILTERCHECK))->GetCheck();
	m_list_ctrl_.set_transform_mode(b_filter_dat ? DataTransform::MedianFilter : DataTransform::None);
}

void ViewRecordsList::on_bn_clicked_display_spikes()
{
	set_display_mode(DisplayMode::Spikes);
	const BOOL all_classes = static_cast<CButton*>(GetDlgItem(IDC_RADIOALLCLASSES))->GetCheck();
	m_list_ctrl_.set_spike_plot_mode(all_classes ? SpikePlotMode::AllClasses : SpikePlotMode::OneClass, m_spike_class_);
}

void ViewRecordsList::on_bn_clicked_display_nothing()
{
	set_display_mode(DisplayMode::None);
}

void ViewRecordsList::on_en_change_time_first()
{
	CString tmp; GetDlgItem(IDC_TIMEFIRST)->GetWindowText(tmp); m_time_first_ = static_cast<float>(_tstof(tmp));
	m_list_ctrl_.set_time_intervals(m_time_first_, m_time_last_);
	m_list_ctrl_.refresh_display();
}

void ViewRecordsList::on_en_change_time_last()
{
	CString tmp; GetDlgItem(IDC_TIMELAST)->GetWindowText(tmp); m_time_last_ = static_cast<float>(_tstof(tmp));
	m_list_ctrl_.set_time_intervals(m_time_first_, m_time_last_);
	m_list_ctrl_.refresh_display();
}

void ViewRecordsList::on_en_change_amplitude_span()
{
	CString tmp; GetDlgItem(IDC_AMPLITUDESPAN)->GetWindowText(tmp); m_amplitude_span_ = static_cast<float>(_tstof(tmp));
	m_list_ctrl_.set_amplitude_span(m_amplitude_span_);
	m_list_ctrl_.refresh_display();
}

void ViewRecordsList::on_en_change_spike_class()
{
	CString tmp; GetDlgItem(IDC_SPIKECLASS)->GetWindowText(tmp); m_spike_class_ = _tstoi(tmp);
	m_list_ctrl_.set_spike_plot_mode(SpikePlotMode::OneClass, m_spike_class_);
	m_list_ctrl_.refresh_display();
}

void ViewRecordsList::on_bn_clicked_check_filename()
{
	const BOOL show = (IsDlgButtonChecked(IDC_CHECKFILENAME) == BST_CHECKED);
	m_list_ctrl_.set_display_file_name(static_cast<boolean>(show));
	m_list_ctrl_.refresh_display();
}

void ViewRecordsList::on_click_median_filter()
{
	const BOOL on = static_cast<CButton*>(GetDlgItem(IDC_FILTERCHECK))->GetCheck();
	m_list_ctrl_.set_transform_mode(on ? DataTransform::MedianFilter : DataTransform::None);
	m_list_ctrl_.refresh_display();
}

void ViewRecordsList::on_bn_clicked_check2()
{
	const BOOL on = (IsDlgButtonChecked(IDC_CHECK2) == BST_CHECKED);
	m_list_ctrl_.set_amplitude_adjust_mode(static_cast<boolean>(on));
	GetDlgItem(IDC_AMPLITUDESPAN)->EnableWindow(on);
	m_list_ctrl_.refresh_display();
}

void ViewRecordsList::on_bn_clicked_check1()
{
	const BOOL on = (IsDlgButtonChecked(IDC_CHECK1) == BST_CHECKED);
	m_list_ctrl_.set_timespan_adjust_mode(static_cast<boolean>(on));
	GetDlgItem(IDC_TIMEFIRST)->EnableWindow(on);
	GetDlgItem(IDC_TIMELAST)->EnableWindow(on);
	m_list_ctrl_.refresh_display();
}

void ViewRecordsList::on_bn_clicked_radio_all_classes()
{
	m_list_ctrl_.set_spike_plot_mode(SpikePlotMode::AllClasses, m_spike_class_);
	m_list_ctrl_.refresh_display();
}

void ViewRecordsList::on_bn_clicked_radio_one_class()
{
	m_list_ctrl_.set_spike_plot_mode(SpikePlotMode::OneClass, m_spike_class_);
	m_list_ctrl_.refresh_display();
}

void ViewRecordsList::on_lvn_column_click_list_ctrl(NMHDR* p_nmhdr, LRESULT* p_result)
{
	const auto p_nmlv = reinterpret_cast<LPNMLISTVIEW>(p_nmhdr);
	CString cs;
	const auto pdb_doc = GetDocument();
	switch (p_nmlv->iSubItem)
	{
	case CTRL2_COL_CURVE: cs = pdb_doc->db_table->m_main_table_set.m_desc[CH_DATALEN].column_name; break;
	case CTRL2_COL_INDEX: cs = pdb_doc->db_table->m_main_table_set.m_desc[CH_ID].column_name; break;
	case CTRL2_COL_SENSI: cs = pdb_doc->db_table->m_main_table_set.m_desc[CH_SENSILLUM_KEY].column_name; break;
	case CTRL2_COL_STIM1: cs = pdb_doc->db_table->m_main_table_set.m_desc[CH_STIM1_KEY].column_name; break;
	case CTRL2_COL_CONC1: cs = pdb_doc->db_table->m_main_table_set.m_desc[CH_CONC1_KEY].column_name; break;
	case CTRL2_COL_STIM2: cs = pdb_doc->db_table->m_main_table_set.m_desc[CH_STIM2_KEY].column_name; break;
	case CTRL2_COL_CONC2: cs = pdb_doc->db_table->m_main_table_set.m_desc[CH_CONC2_KEY].column_name; break;
	case CTRL2_COL_NBSPK: cs = pdb_doc->db_table->m_main_table_set.m_desc[CH_NSPIKES].column_name; break;
	case CTRL2_COL_FLAG: cs = pdb_doc->db_table->m_main_table_set.m_desc[CH_FLAG].column_name; break;
	default: break;
	}
	p_db_table_main->m_strSort = cs;
	p_db_table_main->Requery();
	GetDocument()->UpdateAllViews(nullptr, HINT_REQUERY, nullptr);
	*p_result = 0;
}


