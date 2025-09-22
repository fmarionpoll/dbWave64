#include "StdAfx.h"

#include "MainFrm.h"
#include "resource.h"
#include "PaneldbFilter.h"

#include "dbWave.h"
#include "dbWave_constants.h"
#include "DatabaseUtils.h"
#include "dbTableAssociated.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// the numbers here are those of m_pszTableCol - they define the order of appearance of the different parameters
int PaneldbFilter::m_no_col_[] = {
	CH_EXPERIMENT_KEY,
	CH_IDINSECT,
	CH_IDSENSILLUM,
	CH_INSECT_KEY,
	CH_SENSILLUM_KEY,
	CH_LOCATION_KEY,
	CH_STRAIN_KEY,
	CH_SEX_KEY,
	CH_OPERATOR_KEY,
	CH_STIM1_KEY,
	CH_CONC1_KEY,
	CH_REPEAT,
	CH_STIM2_KEY,
	CH_CONC2_KEY,
	CH_REPEAT2,
	CH_FLAG,
	CH_ACQDATE_DAY,
	-1
};

PaneldbFilter::PaneldbFilter()
= default;

PaneldbFilter::~PaneldbFilter()
= default;

BEGIN_MESSAGE_MAP(PaneldbFilter, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_WM_PAINT()
	ON_WM_SETFOCUS()

	ON_MESSAGE(WM_MYMESSAGE, on_my_message)

	ON_CBN_SELCHANGE(ID_RECORD_SORT, on_cbn_sel_change_category)
	ON_COMMAND(ID_RECORD_SORT, on_record_sort)
	ON_COMMAND(ID_UPDATE, on_update_tree)
	ON_COMMAND(ID_APPLY_FILTER, on_apply_filter)

	ON_COMMAND(ID_BUTTON_PREVIOUS, select_previous_combo_item)
	ON_COMMAND(ID_BUTTON_NEXT, select_next_combo_item)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_PREVIOUS, on_update_bn_update_previous)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_NEXT, on_update_bn_update_next)

	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE1, on_tvn_sel_changed_filter_tree)
	ON_NOTIFY(NM_CLICK, IDC_TREE1, on_nm_click_filter_tree)

	
END_MESSAGE_MAP()
void PaneldbFilter::refresh_tree_from_document()
{
    m_p_doc_old_ = nullptr;
    init_filter_list();
}

void PaneldbFilter::AdjustLayout()
{
	if (GetSafeHwnd() == nullptr)
		return;

	CRect rect_client;
	GetClientRect(rect_client);

	const int cy_tlb = m_wnd_tool_bar_.CalcFixedLayout(FALSE, TRUE).cy;
	m_wnd_tool_bar_.SetWindowPos(nullptr, rect_client.left, 
							rect_client.top,
							rect_client.Width(), 
							cy_tlb, SWP_NOACTIVATE | SWP_NOZORDER);
	m_wnd_filter_view_.SetWindowPos(nullptr, rect_client.left + 1, 
							rect_client.top + cy_tlb + 1,
							rect_client.Width() - 2, 
							rect_client.Height() - cy_tlb - 2, SWP_NOACTIVATE | SWP_NOZORDER);
}

int PaneldbFilter::OnCreate(const LPCREATESTRUCT lp_create_struct)
{
	if (CDockablePane::OnCreate(lp_create_struct) == -1)
		return -1;

	CRect rect_dummy;
	rect_dummy.SetRectEmpty();

	// Create view for filter items
	constexpr DWORD dw_view_style = WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS;
	if (!m_wnd_filter_view_.Create(dw_view_style, rect_dummy, this, IDC_TREE1))
		return -1;

	// Create toolbar with a combobox
	m_wnd_tool_bar_.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_EXPLORER);
	m_wnd_tool_bar_.LoadToolBar(IDR_EXPLORER, 0, 0, TRUE /* Is locked */);

	m_wnd_tool_bar_.SetPaneStyle(m_wnd_tool_bar_.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);
	m_wnd_tool_bar_.SetPaneStyle(
		m_wnd_tool_bar_.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM |
			CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));
	m_wnd_tool_bar_.ReplaceButton(ID_RECORD_SORT,
		CMFCToolBarComboBoxButton(ID_RECORD_SORT, /*GetCmdMgr()->GetCmdImage(ID_RECORD_SORT)*/ NULL, CBS_DROPDOWN));

	// All commands will be routed via this control, not via the parent frame:
	m_wnd_tool_bar_.SetOwner(this);
	m_wnd_tool_bar_.SetRouteCommandsViaFrame(FALSE);

	AdjustLayout();
	return 0;
}

void PaneldbFilter::OnSize(const UINT n_type, const int cx, const int cy)
{
	CDockablePane::OnSize(n_type, cx, cy);
	AdjustLayout();
}

void PaneldbFilter::OnContextMenu(CWnd* p_wnd, const CPoint point)
{
	const auto p_wnd_tree = static_cast<CTreeCtrl*>(&m_wnd_filter_view_);
	ASSERT_VALID(p_wnd_tree);

	if (p_wnd != p_wnd_tree)
	{
		CDockablePane::OnContextMenu(p_wnd, point);
		return;
	}

	if (point != CPoint(-1, -1))
	{
		// Select clicked item:
		auto pt_tree = point;
		p_wnd_tree->ScreenToClient(&pt_tree);

		UINT flags = 0;
		const auto h_tree_item = p_wnd_tree->HitTest(pt_tree, &flags);
		if (h_tree_item != nullptr)
		{
			p_wnd_tree->SelectItem(h_tree_item);
		}
	}

	p_wnd_tree->SetFocus();
	the_app.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EXPLORER, point.x, point.y, this, TRUE);
}

void PaneldbFilter::OnPaint()
{
	CPaintDC dc(this);
	CRect rect_tree;
	m_wnd_filter_view_.GetWindowRect(rect_tree);
	ScreenToClient(rect_tree);

	rect_tree.InflateRect(1, 1);
	dc.Draw3dRect(rect_tree, GetSysColor(COLOR_3DSHADOW), GetSysColor(COLOR_3DSHADOW));
}

void PaneldbFilter::OnSetFocus(CWnd* p_old_wnd)
{
	CDockablePane::OnSetFocus(p_old_wnd);
	m_wnd_filter_view_.SetFocus();
}

void PaneldbFilter::on_update_tree()
{
	m_p_doc_old_ = nullptr;
	init_filter_list();
}

LRESULT PaneldbFilter::on_my_message(const WPARAM w_param, const LPARAM l_param)
{
	//auto p_app = (CdbWaveApp*)AfxGetApp();
	//short low_p = LO_WORD(lParam);
	//short high_p = HI_WORD(lParam);

	switch (w_param)
	{
	case HINT_ACTIVATE_VIEW:
	    m_p_doc_ = reinterpret_cast<CdbWaveDoc*>(l_param);
	    if (m_p_doc_ != m_p_doc_old_)
	    {
	        init_filter_list();
	        //if (m_p_doc_ && m_p_doc_->db_table)
	        //{
	        //   const CString blob = m_p_doc_->db_table->settings_read(_T("filter_tree_state"), _T(""));
	        //   if (!blob.IsEmpty())
	        //      restore_tree_state_from_db();
			//   const CString where_sql = m_p_doc_->db_table->settings_read(_T("filter_sql"), _T(""));
			//   if (!where_sql.IsEmpty())
			//		apply_sql_filter(where_sql);
	        //}
	    }
	    break;

	case HINT_MDI_ACTIVATE:
		{
			const auto* p_main = static_cast<CMainFrame*>(AfxGetMainWnd());
			BOOL b_maximized;
			const auto p_child = p_main->MDIGetActive(&b_maximized);
			if (!p_child)
				return NULL;
			const auto p_document = p_child->GetActiveDocument();
			if (!p_document || !p_document->IsKindOf(RUNTIME_CLASS(CdbWaveDoc)))
				return NULL;
			m_p_doc_ = static_cast<CdbWaveDoc*>(p_document);
			init_filter_list();

			//if (m_p_doc_ && m_p_doc_->db_table)
			//{
			//	const CString blob = m_p_doc_->db_table->settings_read(_T("filter_tree_state"), _T(""));
			//	if (!blob.IsEmpty()) 
			//		restore_tree_state_from_db();
			//	const CString where_sql = m_p_doc_->db_table->settings_read(_T("filter_sql"), _T(""));
			//	if (!where_sql.IsEmpty())
			//		apply_sql_filter(where_sql);
			//}
		}
		break;

	default:
		break;
	}
	return 0L;
}

void PaneldbFilter::OnUpdate(CView* p_sender, const LPARAM l_hint, CObject* p_hint)
{
	m_p_doc_ = reinterpret_cast<CdbWaveDoc*>(p_sender);
    switch (LOWORD(l_hint))
	{
	case HINT_CLOSE_FILE_MODIFIED:
	    if (m_p_doc_ && m_p_doc_->db_table)
	    {
	        auto* p_db = m_p_doc_->db_table;
	        const CString where_sql = p_db->m_main_table_set.m_strFilter;
	        p_db->settings_write(_T("filter_sql"), where_sql);
	        const CString tree_state = serialize_tree_state();
	        p_db->settings_write(_T("filter_tree_state"), tree_state);
	    }
	    break;

	case HINT_REQUERY:
		m_p_doc_old_ = nullptr;

	case HINT_DOC_HAS_CHANGED:
	case HINT_DOC_MOVE_RECORD:
	case HINT_REPLACE_VIEW:
    default:
        init_filter_list();
		// After rebuilding from current dataset, restore tree checks once per activation
	   //     if ( m_p_doc_ && m_p_doc_->db_table)
	   //     {
	   //         const CString blob = m_p_doc_->db_table->settings_read(_T("filter_tree_state"), _T(""));
    	//if (!blob.IsEmpty()) 
		//	restore_tree_state_from_db();

	   //         const CString where_sql = m_p_doc_->db_table->settings_read(_T("filter_sql"), _T(""));
	   //         if (!where_sql.IsEmpty())
	   //             apply_sql_filter(where_sql);
	   //     }
        break;
	}
}

void PaneldbFilter::fill_combo_with_categories(const CdbTable* p_db) const
{
    const auto p_combo = m_wnd_tool_bar_.get_combo();
    ASSERT(ID_RECORD_SORT == m_wnd_tool_bar_.GetItemID(3));
    if (p_combo == nullptr)
        return;

    if (p_combo->GetCount() <= 0)
    {
        for (auto i = 0; i < N_TABLE_COLUMNS; i++)
            p_combo->AddSortedItem(CdbTable::m_column_properties[i].description, i);
    }
    // Map stored header_name to description text in combo
    CString sortHeader = p_db->m_main_table_set.m_strSort;
    CString sortDesc;
    for (int i = 0; i < N_TABLE_COLUMNS; ++i)
    {
        if (CdbTable::m_column_properties[i].header_name.CompareNoCase(sortHeader) == 0)
        {
            sortDesc = CdbTable::m_column_properties[i].description;
            break;
        }
    }
    if (sortDesc.IsEmpty())
        sortDesc = CdbTable::m_column_properties[CH_ACQDATE].description;

    for (int i = 0; i < p_combo->GetCount(); ++i)
    {
        CString item = p_combo->GetItem(i);
        if (item.CompareNoCase(sortDesc) == 0)
        {
            p_combo->SelectItem(sortDesc);
            break;
        }
    }

}

DB_ITEMDESC* PaneldbFilter::create_tree_category(CdbTable* p_db, const int i) 
{
	const auto i_col = m_no_col_[i];
	m_h_tree_item_[i] = m_wnd_filter_view_.InsertItem(CdbTable::m_column_properties[i_col].description, TVI_ROOT);
	m_wnd_filter_view_.SetItemData(m_h_tree_item_[i], m_no_col_[i]);
	const auto p_desc = p_db->get_record_item_descriptor(i_col);
	// collect data (array of unique descriptors)
	switch (p_desc->data_code_number)
	{
	case FIELD_IND_TEXT:
	case FIELD_IND_FILEPATH:
		populate_item_from_linked_table(p_desc);
		break;
	case FIELD_LONG:
		populate_item_from_table_long(p_desc);
		break;
	case FIELD_DATE_YMD:
		populate_item_from_table_with_date(p_desc);
		break;
	default:
		break;
	}

	return p_desc;
}

HTREEITEM PaneldbFilter::create_tree_subitem_element(const DB_ITEMDESC* p_desc, const int i, const int j)
{
	const HTREEITEM h_tree_item = m_wnd_filter_view_.InsertItem(p_desc->cs_elements_array.GetAt(j), m_h_tree_item_[i]);
	TVCS_CHECKSTATE b_check = TVCS_CHECKED;
	const auto cs_element_j = p_desc->cs_elements_array.GetAt(j);
	if (p_desc->b_array_filter)
	{
		b_check = TVCS_UNCHECKED;
		for (auto k = 0; k < p_desc->cs_array_filter.GetSize(); k++)
		{
			if (cs_element_j.CompareNoCase(p_desc->cs_array_filter.GetAt(k)) == 0)
			{
				b_check = TVCS_CHECKED;
				break;
			}
		}
	}
	else if (p_desc->b_single_filter)
	{
		if (cs_element_j.CompareNoCase(p_desc->cs_param_single_filter) != 0)
			b_check = TVCS_UNCHECKED;
	}
	m_wnd_filter_view_.set_check(h_tree_item, b_check);
	return h_tree_item;
}

void PaneldbFilter::create_tree_subitem(const DB_ITEMDESC* p_desc, const int i)
{
	// create subitems
	auto i_sum = 0;
	auto n_items = 0;
	HTREEITEM h_tree_item = nullptr;
	TVCS_CHECKSTATE b_check = TVCS_UNCHECKED;
	for (auto j = 0; j < p_desc->cs_elements_array.GetSize(); j++)
	{
		h_tree_item = create_tree_subitem_element(p_desc, i, j);
		b_check = m_wnd_filter_view_.get_check(h_tree_item);
		i_sum += b_check; // count number of positive checks (no check=0, check = 1)
		n_items++;
	}

	// trick needed here because if the first item is checked and not the others,
	// then the parent stays in the initial state
	// namely "checked" (because at that moment it did not have other children)
	if (i_sum == 1 && h_tree_item != nullptr)
	{
		m_wnd_filter_view_.set_check(h_tree_item, TVCS_CHECKED);
		m_wnd_filter_view_.set_check(h_tree_item, b_check);
	}
	if (i_sum < n_items)
		m_wnd_filter_view_.Expand(m_h_tree_item_[i], TVE_EXPAND);
}

void PaneldbFilter::init_filter_list()
{
	if (m_p_doc_old_ == m_p_doc_)
		return;

	m_p_doc_old_ = m_p_doc_;

	const auto p_db = m_p_doc_->db_table;
	ASSERT(p_db);

	// get all categories into combo
	fill_combo_with_categories(p_db);

	// fill items of the tree
	if (p_db->m_main_table_set.IsBOF() && p_db->m_main_table_set.IsEOF())
		return;

	m_wnd_filter_view_.LockWindowUpdate();
	m_wnd_filter_view_.DeleteAllItems();

	p_db->m_main_table_set.build_and_sort_key_arrays();
	auto i = 0;
	while (m_no_col_[i] > 0)
	{
		const auto* p_desc = create_tree_category(p_db, i);
		create_tree_subitem(p_desc, i);
		i++;
	}
	m_wnd_filter_view_.UnlockWindowUpdate();
}

void PaneldbFilter::populate_item_from_table_long(DB_ITEMDESC* p_desc) const
{
	const auto p_set = &m_p_doc_->db_table->m_main_table_set;
	const auto cs_col_head = p_desc->column_name;
	const auto array_size = p_desc->li_array.GetSize();
	if (p_desc->b_array_filter)
	{
		return;
	}
	if (p_desc->b_single_filter)
	{
		p_desc->cs_param_single_filter.Format(_T("%i"), p_desc->l_param_single_filter);
	}
	else
	{
		CString str;
		CString cs;
		p_desc->cs_elements_array.RemoveAll();
		for (auto i = 0; i < array_size; i++)
		{
			const auto i_id = p_desc->li_array.GetAt(i);
			// add string only if found into p_main_table_set...
			str.Format(_T("%s=%li"), (LPCTSTR)cs_col_head, i_id);
			const auto flag = p_set->FindFirst(str);
			if (flag != 0)
			{
				cs.Format(_T("%i"), i_id);
				p_desc->cs_elements_array.Add(cs);
				if (p_desc->b_single_filter && p_desc->l_param_single_filter != i_id)
				{
					p_desc->cs_param_single_filter.Format(_T("%i"), i_id);
				}
			}
		}
	}
}

void PaneldbFilter::populate_item_from_linked_table(DB_ITEMDESC* p_desc) const
{
	auto str2 = p_desc->column_name;
	ASSERT(!str2.IsEmpty());

	auto p_linked_set = p_desc->p_linked_set;
	auto p_set = &m_p_doc_->db_table->m_main_table_set;
	if (p_desc->b_array_filter)
		return;

	if (p_desc->b_single_filter)
	{
		p_desc->cs_param_single_filter = p_linked_set->get_string_from_key(p_desc->l_param_single_filter);
	}
	else
	{
		// loop over the whole content of the attached table
		p_desc->cs_elements_array.RemoveAll();
		p_desc->li_array.RemoveAll();
		if (p_linked_set->IsOpen() && !p_linked_set->IsBOF())
		{
			CString cs;
			COleVariant var_value0, var_value1;
			p_linked_set->MoveFirst();
			while (!p_linked_set->IsEOF())
			{
			// Use bound fields directly instead of GetFieldValue to avoid 32/64-bit BSTR interpretation issues
			// The bound field approach works correctly in both 32-bit and 64-bit applications
			CString string_value;
			long id_value = 0;
			
			// Cast to the specific recordset type to access bound fields
			CdbTableAssociated* p_linked_table = dynamic_cast<CdbTableAssociated*>(p_linked_set);
			if (p_linked_table)
			{
				// Use bound fields directly - this is the same approach used in get_string_from_key
				string_value = p_linked_table->m_cs;
				id_value = p_linked_table->m_id;
			}
			else
			{
				// Fallback to GetFieldValue if dynamic cast fails
				p_linked_set->GetFieldValue(0, var_value0);
				p_linked_set->GetFieldValue(1, var_value1);
				string_value = CDatabaseUtils::safe_get_string_from_variant(var_value0);
				id_value = var_value1.lVal;
			}
			
			// add string only if found into p_main_table_set...
			cs.Format(_T("%s=%li"), (LPCTSTR)p_desc->column_name, id_value);
			const auto flag = p_set->FindFirst(cs);
			if (flag != 0)
			{
				insert_alphabetic(string_value, p_desc->cs_elements_array);
				p_desc->li_array.Add(id_value);
			}
				p_linked_set->MoveNext();
			}
		}
	}
}

void PaneldbFilter::populate_item_from_table_with_date(DB_ITEMDESC* p_desc) const
{
	CString cs; // to construct date
	const auto cs_column_head = p_desc->column_name;
	const auto p_main_table_set = &m_p_doc_->db_table->m_main_table_set;
	const auto array_size = p_main_table_set->m_desc[CH_ACQDATE_DAY].ti_array.GetSize();

	if (p_desc->b_array_filter)
	{
		return;
	}
	if (p_desc->b_single_filter)
	{
		cs = p_desc->date_time_param_single_filter.Format(VAR_DATEVALUEONLY);
		p_desc->cs_param_single_filter = cs;
	}
	else
	{
		CString str;
		p_desc->cs_elements_array.RemoveAll();
		for (auto i = 0; i < array_size; i++)
		{
			auto &o_time = p_main_table_set->m_desc[CH_ACQDATE_DAY].ti_array.GetAt(i);
			cs = o_time.Format(_T("%m/%d/%y")); // filter needs to be constructed as month-day-year
			str.Format(_T("%s=#%s#"), (LPCTSTR)cs_column_head, (LPCTSTR)cs);
			const auto flag = p_main_table_set->FindFirst(str);
			if (flag != 0) // add string only if found into p_main_table_set...
			{
				cs = o_time.Format(VAR_DATEVALUEONLY);
				p_desc->cs_elements_array.Add(cs);
			}
		}
	}
}

void PaneldbFilter::insert_alphabetic(const CString& cs, CStringArray& cs_array)
{
	auto k = 0;
	for (auto i = 0; i < cs_array.GetSize(); i++, k++)
	{
		const auto& cs_comp = cs_array.GetAt(k);
		const auto j = cs.CompareNoCase(cs_comp);
		if (j < 0)
			break;
	}
	cs_array.InsertAt(k, cs);
}

void PaneldbFilter::build_filter_item_indirection_from_tree(DB_ITEMDESC* p_desc, const HTREEITEM start_item) const
{
	auto i = 0;
	for (auto item = start_item; item != nullptr; item = m_wnd_filter_view_.GetNextItem(item, TVGN_NEXT), i++)
	{
		const auto state = m_wnd_filter_view_.get_check(item);
		if (state == TVCS_CHECKED)
		{
			auto cs = m_wnd_filter_view_.GetItemText(item);
			for (auto j = 0; j < p_desc->li_array.GetSize(); j++)
			{
				const auto li = p_desc->li_array.GetAt(j);
				auto str = p_desc->p_linked_set->get_string_from_key(li);
				if (str == cs)
				{
					p_desc->l_param_filter_array.Add(li);
					p_desc->cs_array_filter.Add(cs);
					break;
				}
			}
		}
	}
}

void PaneldbFilter::build_filter_item_long_from_tree(DB_ITEMDESC* p_desc, const HTREEITEM start_item) const
{
	auto i = 0;
	for (auto item = start_item; item != nullptr; item = m_wnd_filter_view_.GetNextItem(item, TVGN_NEXT), i++)
	{
		const auto state = m_wnd_filter_view_.get_check(item);
		if (state == TVCS_CHECKED)
		{
			auto cs = m_wnd_filter_view_.GetItemText(item);
			const auto li = p_desc->li_array.GetAt(i);
			p_desc->l_param_filter_array.Add(li);
			p_desc->cs_array_filter.Add(cs);
		}
	}
}

void PaneldbFilter::build_filter_item_date_from_tree(DB_ITEMDESC* p_desc, const HTREEITEM start_item) const
{
	auto i = 0;
	for (auto item = start_item; item != nullptr; item = m_wnd_filter_view_.GetNextItem(item, TVGN_NEXT), i++)
	{
		const auto state = m_wnd_filter_view_.get_check(item);
		if (state == TVCS_CHECKED)
		{
			auto cs_filter_checked = m_wnd_filter_view_.GetItemText(item);
			COleDateTime o_time;
			o_time.ParseDateTime(cs_filter_checked);
			p_desc->data_time_array_filter.Add(o_time);
			p_desc->cs_array_filter.Add(cs_filter_checked);
		}
	}
}

void PaneldbFilter::on_apply_filter()
{
	if (!m_p_doc_)
		return;
	const auto p_db = m_p_doc_->db_table;

	auto i = 0;
    while (m_no_col_[i] > 0)
    {
        const auto h_parent = m_h_tree_item_[i];
        i++;

        const int i_col = static_cast<int>(m_wnd_filter_view_.GetItemData(h_parent));
        const auto p_desc = p_db->get_record_item_descriptor(i_col);

        // Rebuild filter strictly from UI state
        p_desc->b_single_filter = FALSE;
        p_desc->b_array_filter = FALSE;
        p_desc->l_param_filter_array.RemoveAll();
        p_desc->cs_array_filter.RemoveAll();
        p_desc->data_time_array_filter.RemoveAll();

        const auto first_child = m_wnd_filter_view_.GetNextItem(h_parent, TVGN_CHILD);
        int total_children = 0;
        int checked_children = 0;
        int child_index = 0;
        for (HTREEITEM h = first_child; h != nullptr; h = m_wnd_filter_view_.GetNextItem(h, TVGN_NEXT), ++child_index)
        {
            total_children++;
            const auto st = m_wnd_filter_view_.get_check(h);
            if (st != TVCS_CHECKED)
                continue;
            checked_children++;
            CString itemText = m_wnd_filter_view_.GetItemText(h);
            switch (p_desc->data_code_number)
            {
            case FIELD_IND_TEXT:
            case FIELD_IND_FILEPATH:
            {
                // map display string back to key
                long li = -1;
                if (p_desc->p_linked_set && p_desc->p_linked_set->get_key_from_string(itemText, li))
                {
                    p_desc->l_param_filter_array.Add(li);
                    p_desc->cs_array_filter.Add(itemText);
                }
                break;
            }
            case FIELD_LONG:
            {
                // Use corresponding id from li_array by index
                if (child_index < p_desc->li_array.GetSize())
                {
                    const long li = p_desc->li_array.GetAt(child_index);
                    p_desc->l_param_filter_array.Add(li);
                    p_desc->cs_array_filter.Add(itemText);
                }
                break;
            }
            case FIELD_DATE_YMD:
            {
                COleDateTime dt; dt.ParseDateTime(itemText);
                p_desc->data_time_array_filter.Add(dt);
                p_desc->cs_array_filter.Add(itemText);
                break;
            }
            default:
                ASSERT(false);
                break;
            }
        }

        // Decide filter activation:
        // - 0 checked => no filter
        // - all checked => no filter (equivalent to full set)
        // - some checked => enable array filter
        if (checked_children > 0 && checked_children < total_children)
        {
            // Ensure arrays contain entries for this descriptor type
            const bool has_long = (p_desc->data_code_number == FIELD_DATE_YMD)
                ? (p_desc->data_time_array_filter.GetSize() > 0)
                : (p_desc->l_param_filter_array.GetSize() > 0);
            if (has_long)
                p_desc->b_array_filter = TRUE;
        }
    }

	// update recordset and tell other views...
	p_db->m_main_table_set.build_filters();
	p_db->m_main_table_set.refresh_query();
	// Persist SQL filter and full tree selection state in per-database settings
	CString where_sql = p_db->m_main_table_set.m_strFilter;
	p_db->settings_write(_T("filter_sql"), where_sql);
	CString tree_state = serialize_tree_state();
	p_db->settings_write(_T("filter_tree_state"), tree_state);
	m_p_doc_->update_all_views_db_wave(nullptr, HINT_REQUERY, nullptr);
}

static CString TrimBrackets(const CString& col)
{
    CString s = col;
    s.Trim();
    if (!s.IsEmpty() && s[0] == '[' && s[s.GetLength()-1] == ']')
        s = s.Mid(1, s.GetLength()-2);
    return s;
}

CString PaneldbFilter::serialize_tree_state() const
{
    CString out;
    const auto* p_db = m_p_doc_->db_table;
    for (int i = 0; m_no_col_[i] > 0; ++i)
    {
        const int col = m_no_col_[i];
        const auto* d = &p_db->m_main_table_set.m_desc[col];
        CString line; line += d->column_name; line += _T("|");
        if (d->b_single_filter && !d->cs_param_single_filter.IsEmpty())
        {
            line += _T("1|"); line += d->cs_param_single_filter;
        }
        else if (d->b_array_filter)
        {
            line += _T("2|");
            for (int k = 0; k < d->cs_array_filter.GetSize(); ++k)
            {
                if (k) line += _T(",");
                line += d->cs_array_filter.GetAt(k);
            }
        }
        else
        {
            continue;
        }
        out += line; out += _T("\n");
    }
    return out;
}

void PaneldbFilter::restore_tree_state_from_db()
{
    if (!m_p_doc_ || !m_p_doc_->db_table) 
		return;
    auto* p_db = m_p_doc_->db_table;
    const CString blob = p_db->settings_read(_T("filter_tree_state"), _T(""));
    if (blob.IsEmpty()) 
		return;
    const CString saved = p_db->m_main_table_set.m_strFilter;
    // Ensure we rebuild UI from the full dataset
    p_db->m_main_table_set.m_strFilter.Empty(); p_db->m_main_table_set.refresh_query();
    m_p_doc_old_ = nullptr; // force rebuild from full dataset
    init_filter_list();
    for (int i = 0; i < p_db->m_main_table_set.m_nFields; ++i)
    {
        auto* d = &p_db->m_main_table_set.m_desc[i];
        d->b_single_filter = FALSE; d->b_array_filter = FALSE;
        d->l_param_single_filter = 0; d->l_param_filter_array.RemoveAll();
        d->cs_array_filter.RemoveAll(); d->data_time_array_filter.RemoveAll();
    }
    int start = 0;
    while (start < blob.GetLength())
    {
        int end = blob.Find(_T('\n'), start);
        CString line = (end >= 0) ? blob.Mid(start, end - start) : blob.Mid(start);
        if (end < 0) start = blob.GetLength(); else start = end + 1;
        if (line.IsEmpty()) continue;
        int p1 = line.Find(_T('|')); if (p1 < 0) continue;
        int p2 = line.Find(_T('|'), p1 + 1); if (p2 < 0) continue;
        CString col = line.Left(p1);
        int type = _ttoi(line.Mid(p1 + 1, p2 - p1 - 1));
        CString values = line.Mid(p2 + 1);
        int idx = p_db->m_main_table_set.get_column_index(col);
        if (idx < 0) continue;
        auto* d = &p_db->m_main_table_set.m_desc[idx];
        if (type == 1) { d->b_single_filter = TRUE; d->cs_param_single_filter = values; }
        else if (type == 2)
        {
            d->b_array_filter = TRUE; CString v = values;
            while (!v.IsEmpty())
            {
                int c = v.Find(_T(',')); CString tok = (c >= 0) ? v.Left(c) : v; if (c >= 0) v = v.Mid(c + 1); else v.Empty();
                tok.Trim(); if (!tok.IsEmpty()) d->cs_array_filter.Add(tok);
            }
            // Guard: if no values found, disable array filter
            if (d->cs_array_filter.GetSize() == 0)
                d->b_array_filter = FALSE;
        }
    }
    for (int i = 0; m_no_col_[i] > 0; ++i)
    {
        const int col = m_no_col_[i];
        const auto* d = &p_db->m_main_table_set.m_desc[col];
        const HTREEITEM h_parent = m_h_tree_item_[i];
        const auto startH = m_wnd_filter_view_.GetNextItem(h_parent, TVGN_CHILD);
        for (HTREEITEM h = startH; h != nullptr; h = m_wnd_filter_view_.GetNextItem(h, TVGN_NEXT))
        {
            CString itemText = m_wnd_filter_view_.GetItemText(h);
            TVCS_CHECKSTATE st = TVCS_UNCHECKED;
            if (d->b_single_filter && !d->cs_param_single_filter.IsEmpty())
            {
                if (itemText.CompareNoCase(d->cs_param_single_filter) == 0) st = TVCS_CHECKED;
            }
            else if (d->b_array_filter)
            {
                for (int k = 0; k < d->cs_array_filter.GetSize(); ++k)
                {
                    if (itemText.CompareNoCase(d->cs_array_filter.GetAt(k)) == 0) { st = TVCS_CHECKED; break; }
                }
            }
            m_wnd_filter_view_.set_check(h, st);
        }
    }
    // Restore previous dataset filter (if any) after UI reflects checks
    p_db->m_main_table_set.m_strFilter = saved; p_db->m_main_table_set.refresh_query();
}

void PaneldbFilter::apply_sql_filter(const CString& where_clause)
{
    if ((!m_p_doc_ || !m_p_doc_->db_table) && GetSafeHwnd())
    {
        // Try to fetch active document if panel was not initialized yet
        const auto* p_main = static_cast<CMainFrame*>(AfxGetMainWnd());
        if (p_main)
        {
            BOOL bMax;
        	auto* pChild = p_main->MDIGetActive(&bMax);
            if (pChild)
            {
                auto* pDoc = pChild->GetActiveDocument();
                if (pDoc && pDoc->IsKindOf(RUNTIME_CLASS(CdbWaveDoc)))
                    m_p_doc_ = static_cast<CdbWaveDoc*>(pDoc);
            }
        }
    }
    if (!m_p_doc_ || !m_p_doc_->db_table || where_clause.IsEmpty()) return;
    auto* p_db = m_p_doc_->db_table;

    // Build UI from full dataset so all options are present
    const CString saved_sql = p_db->m_main_table_set.m_strFilter;
    p_db->m_main_table_set.m_strFilter.Empty();
    p_db->m_main_table_set.refresh_query();
    // Reset filter flags BEFORE building arrays/tree so all choices are computed
    for (int i = 0; i < p_db->m_main_table_set.m_nFields; ++i)
    {
        auto* d = &p_db->m_main_table_set.m_desc[i];
        d->b_single_filter = FALSE; d->b_array_filter = FALSE;
        d->l_param_single_filter = 0; d->l_param_filter_array.RemoveAll();
        d->cs_array_filter.RemoveAll(); d->data_time_array_filter.RemoveAll();
    }
    m_p_doc_old_ = nullptr; // force rebuild from full dataset
    init_filter_list();

    // Very simple parser: split by AND, support [col]=value and [col] IN (..)
    CString where = where_clause; where.Trim();
    CString rest = where;
    while (!rest.IsEmpty())
    {
        int and_pos = rest.Find(_T(" AND "));
        CString term = (and_pos >= 0) ? rest.Left(and_pos) : rest;
        if (and_pos >= 0) rest = rest.Mid(and_pos + 5); else rest.Empty();
        term.Trim(); if (term.IsEmpty()) continue;

        // Handle IN list
        int in_pos = term.Find(_T(" IN "));
        if (in_pos > 0)
        {
            CString col = TrimBrackets(term.Left(in_pos));
            int lpar = term.Find('('); int rpar = term.ReverseFind(')');
            if (lpar > in_pos && rpar > lpar)
            {
                CString list = term.Mid(lpar+1, rpar-lpar-1);
                // split on comma
                int idx = p_db->m_main_table_set.get_column_index(col);
                auto* d = &p_db->m_main_table_set.m_desc[idx];
                d->b_array_filter = TRUE; d->l_param_filter_array.RemoveAll(); d->cs_array_filter.RemoveAll();
                while (!list.IsEmpty())
                {
                    list.Trim();
                    int comma = list.Find(',');
                    CString tok = (comma >= 0) ? list.Left(comma) : list;
                    if (comma >= 0) list = list.Mid(comma+1); else list.Empty();
                    tok.Trim();
                    if (!tok.IsEmpty())
                    {
                        if (tok[0] == '#')
                        {
                            // date literal
                            tok.Trim(_T("#"));
                            COleDateTime dt; dt.ParseDateTime(tok);
                            d->data_time_array_filter.Add(dt);
                            d->cs_array_filter.Add(dt.Format(VAR_DATEVALUEONLY));
                        }
                        else
                        {
                            // strip optional quotes
                            tok.Trim(_T("'\""));
                            long val = _ttol(tok);
                            d->l_param_filter_array.Add(val);
                            if (d->data_code_number == FIELD_IND_TEXT || d->data_code_number == FIELD_IND_FILEPATH)
                            {
                                CString disp = (d->p_linked_set) ? d->p_linked_set->get_string_from_key(val) : tok;
                                if (disp.IsEmpty()) disp = tok;
                                d->cs_array_filter.Add(disp);
                            }
                            else
                            {
                                d->cs_array_filter.Add(tok);
                            }
                        }
                    }
                }
            }
            continue;
        }

        // Handle equality
        int eq = term.Find('=');
        if (eq > 0)
        {
            CString col = TrimBrackets(term.Left(eq));
            CString val = term.Mid(eq+1); val.Trim();
            int idx = p_db->m_main_table_set.get_column_index(col);
            auto* d = &p_db->m_main_table_set.m_desc[idx];
            d->b_single_filter = TRUE;
            if (!val.IsEmpty() && val[0] == '#')
            {
                val.Trim(_T("#"));
                COleDateTime dt; dt.ParseDateTime(val);
                d->date_time_param_single_filter = dt;
                d->cs_param_single_filter = dt.Format(VAR_DATEVALUEONLY);
            }
            else
            {
                val.Trim(_T("'\""));
                long id_val = _ttol(val);
                d->l_param_single_filter = id_val;
                if (d->data_code_number == FIELD_IND_TEXT || d->data_code_number == FIELD_IND_FILEPATH)
                {
                    CString disp = (d->p_linked_set) ? d->p_linked_set->get_string_from_key(id_val) : val;
                    d->cs_param_single_filter = disp.IsEmpty() ? val : disp;
                }
                else
                {
                    d->cs_param_single_filter = val;
                }
            }
        }
    }

    // Reflect flags in the tree (check marks) without collapsing options
    // Iterate categories and children, set check state according to flags
    for (int i = 0; m_no_col_[i] > 0; ++i)
    {
        const int col = m_no_col_[i];
        const auto* d = &p_db->m_main_table_set.m_desc[col];
        const HTREEITEM h_parent = m_h_tree_item_[i];
        const auto start = m_wnd_filter_view_.GetNextItem(h_parent, TVGN_CHILD);
        for (HTREEITEM h = start; h != nullptr; h = m_wnd_filter_view_.GetNextItem(h, TVGN_NEXT))
        {
            CString itemText = m_wnd_filter_view_.GetItemText(h);
            TVCS_CHECKSTATE st = TVCS_UNCHECKED;
            if (d->b_single_filter && !d->cs_param_single_filter.IsEmpty())
            {
                // For indirection columns, cs_param_single_filter is a display string; compare directly
                if (itemText.CompareNoCase(d->cs_param_single_filter) == 0) st = TVCS_CHECKED;
            }
            else if (d->b_array_filter)
            {
                for (int k = 0; k < d->cs_array_filter.GetSize(); ++k)
                {
                    if (itemText.CompareNoCase(d->cs_array_filter.GetAt(k)) == 0) { st = TVCS_CHECKED; break; }
                }
            }
            m_wnd_filter_view_.set_check(h, st);
        }
    }

    // Finally apply the SQL again so the dataset is filtered
    p_db->m_main_table_set.m_strFilter = where_clause;
    p_db->m_main_table_set.refresh_query();
    // Persist tree selection as well, but only if something is actually selected
    CString tree_state = serialize_tree_state();
    if (!tree_state.IsEmpty())
        p_db->settings_write(_T("filter_tree_state"), tree_state);
    m_p_doc_->update_all_views_db_wave(nullptr, HINT_REQUERY, nullptr);
}

CString PaneldbFilter::build_sql_filter() const
{
    const auto* p_set = &m_p_doc_->db_table->m_main_table_set;
    return p_set->m_strFilter; // use generated SQL directly
}

//void PaneldbFilter::on_sort_records()
//{
//	const auto p_database = m_p_doc_->db_table;
//	ASSERT(p_database);
//	const auto p_combo = static_cast<CMFCToolBarComboBoxButton*>(m_wnd_tool_bar_.GetButton(3));
//	ASSERT(ID_RECORD_SORT == m_wnd_tool_bar_.GetItemID(3));
//
//	const auto i_sel = p_combo->GetCurSel();
//	ASSERT(i_sel != CB_ERR);
//	const int i = p_combo->GetItemData(i_sel);
//	p_database->m_main_table_set.m_strSort = CdbTable::m_column_properties[i].header_name;
//
//	p_database->m_main_table_set.refresh_query();
//	m_p_doc_->update_all_views_db_wave(nullptr, HINT_REQUERY, nullptr);
//}

void PaneldbFilter::on_tvn_sel_changed_filter_tree(NMHDR* p_nmhdr, LRESULT* p_result)
{
	NM_TREEVIEW* p_nm_tree_view = (NM_TREEVIEW*) p_nmhdr;
	auto* p_new_item = &p_nm_tree_view->itemNew;
//	TRACE("changed filter \n");
}

void PaneldbFilter::on_nm_click_filter_tree(NMHDR* p_nmhdr, LRESULT* p_result)
{
	//const auto i_cur_sel = spk_list_tab_ctrl.GetCurSel();
	//SendMessage(WM_MYMESSAGE, HINT_VIEW_TAB_CHANGE, MAKELPARAM(i_cur_sel, 0));
	//*p_result = 0;
	//TRACE("click filter \n");
}

void  PaneldbFilter::on_cbn_sel_change_category()
{
	TRACE("change category from combobox \n");
}

void PaneldbFilter::on_update_bn_update_previous(CCmdUI* p_cmd_ui)
{
	//p_cmd_ui->Enable(m_b_changed_property_);
	p_cmd_ui->Enable(TRUE);
}

void PaneldbFilter::on_update_bn_update_next(CCmdUI* p_cmd_ui)
{
	//p_cmd_ui->Enable(m_b_changed_property_);
	p_cmd_ui->Enable(TRUE);
}

void PaneldbFilter::select_previous_combo_item()
{
	TRACE("got to previous");
	select_next_filter_item(false);
}

void PaneldbFilter::select_next_combo_item()
{
	TRACE("go to next");
	select_next_filter_item(true);
}

void PaneldbFilter::select_next_filter_item(boolean b_next)
{
	// get combo selected item
	const int index = m_wnd_tool_bar_.get_combo()->GetCurSel();
	const CString description = m_wnd_tool_bar_.get_combo()->GetItem(index);
	const int column_index = static_cast<int>(m_wnd_tool_bar_.get_combo()->GetItemData(index));
	// select corresponding tree item
	auto h_item = select_tree_item(column_index);
	const auto p_tree = static_cast<CTreeCtrl*>(&m_wnd_filter_view_);
	ASSERT_VALID(p_tree);
	p_tree->SelectItem(h_item);
	select_next(b_next);
}

HTREEITEM PaneldbFilter::select_tree_item(const int col_requested)
{
	auto i = 0;
	HTREEITEM item_found = nullptr;
	const auto p_db = m_p_doc_->db_table;
	const auto p_desc = p_db->get_record_item_descriptor(col_requested);

	while (m_no_col_[i] > 0)
	{
		const auto h_parent = m_h_tree_item_[i];
		const int i_col = static_cast<int>(m_wnd_filter_view_.GetItemData(h_parent));
		if (i_col == col_requested)
		{
			item_found = h_parent;
			break;
		}
		i++;
	}
	return item_found;
}


void PaneldbFilter::select_next(const boolean b_next)
{
	const auto p_tree = static_cast<CTreeCtrl*>(&m_wnd_filter_view_);
	ASSERT_VALID(p_tree);

	auto h_item = p_tree->GetSelectedItem();
	if (!p_tree->ItemHasChildren(h_item))
		h_item = p_tree->GetParentItem(h_item);
	if (h_item == nullptr)
		return;

	auto count = 0;
	HTREEITEM h_last_selected = nullptr;
	auto n_selected = 0;
	auto h_kid = p_tree->GetChildItem(h_item);
	do
	{
		const auto state = static_cast<CQuadStateTree*>(p_tree)->get_check(h_kid);
		if (state == TVCS_CHECKED)
		{
			h_last_selected = h_kid;
			n_selected++;
		}
		count++;
	} while ((h_kid = p_tree->GetNextSiblingItem(h_kid)));

	// if single selection select next item on the list and deselect current; update
	if (n_selected == 1)
	{
		HTREEITEM h_next;
		if (b_next)
			h_next = p_tree->GetNextSiblingItem(h_last_selected);
		else
			h_next = p_tree->GetPrevSiblingItem(h_last_selected);
		if (h_next == nullptr)
			return;
		static_cast<CQuadStateTree*>(p_tree)->set_check(h_next, TVCS_CHECKED);
		static_cast<CQuadStateTree*>(p_tree)->set_check(h_last_selected, TVCS_UNCHECKED);
		on_apply_filter();
	}
}

void PaneldbFilter::on_record_sort()
{
	const auto p_database = m_p_doc_->db_table;
	ASSERT(p_database);
	const auto p_combo = static_cast<CMFCToolBarComboBoxButton*>(m_wnd_tool_bar_.GetButton(3));
	ASSERT(ID_RECORD_SORT == m_wnd_tool_bar_.GetItemID(3));

	const auto i_sel = p_combo->GetCurSel();
	ASSERT(i_sel != CB_ERR);
	const int i = p_combo->GetItemData(i_sel);
	//p_database->m_main_table_set.m_strSort = CdbTable::m_column_properties[i].header_name;

	//p_database->m_main_table_set.refresh_query();
	//m_p_doc_->update_all_views_db_wave(nullptr, HINT_REQUERY, nullptr);
}
