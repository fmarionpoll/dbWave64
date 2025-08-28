#include "StdAfx.h"

#include "DataListCtrl.h"

#include "ViewDbTable.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

int DataListCtrl::m_column_width_[] = {
	1,
	10, 300, 15, 30,
	30, 50, 40, 40,
	40, 40
};

CString DataListCtrl::m_column_headers_[] = {
	__T(""),
	__T("#"), __T("data"), __T("insect ID"), __T("sensillum"),
	__T("stim1"), __T("conc1"), __T("stim2"), __T("conc2"),
	__T("spikes"), __T("flag")
};

int DataListCtrl::m_column_format_[] = {
	LVCFMT_LEFT,
	LVCFMT_CENTER, LVCFMT_CENTER, LVCFMT_CENTER, LVCFMT_CENTER,
	LVCFMT_CENTER, LVCFMT_CENTER, LVCFMT_CENTER, LVCFMT_CENTER,
	LVCFMT_CENTER, LVCFMT_CENTER
};

BEGIN_MESSAGE_MAP(DataListCtrl, CListCtrl)
	ON_WM_VSCROLL()
	ON_WM_KEYUP()
	ON_NOTIFY_REFLECT(LVN_GETDISPINFO, on_get_display_info)
	ON_WM_DESTROY()
END_MESSAGE_MAP()

DataListCtrl::DataListCtrl()
	: cache_(nullptr)
{
}

DataListCtrl::~DataListCtrl()
{
	if (cache_ != nullptr)
		delete cache_;
	if (p_empty_bitmap_ != nullptr)
		delete p_empty_bitmap_;
	SAFE_DELETE(provider_)
	SAFE_DELETE(data_renderer_)
	SAFE_DELETE(spike_renderer_)
}

void DataListCtrl::init(IDbWaveDataProvider* provider,
	const DisplaySettings& settings,
	IDataRenderer* dataRenderer,
	ISpikeRenderer* spikeRenderer,
	CUIntArray* width_columns)
{
	provider_ = provider;
	settings_ = settings;
	data_renderer_ = dataRenderer;
	spike_renderer_ = spikeRenderer;
	cache_ = new RowCache(provider_);

	init_columns(width_columns);
}

void DataListCtrl::init_columns(CUIntArray* width_columns)
{
	if (width_columns != nullptr)
	{
		m_width_columns_ = width_columns;
		const auto n_columns = width_columns->GetSize();
		if (n_columns < N_COLUMNS2)
			width_columns->SetSize(N_COLUMNS2);
		for (auto i = 0; i < n_columns; i++)
			m_column_width_[i] = static_cast<int>(width_columns->GetAt(i));
	}

	for (auto i = 0; i < N_COLUMNS2; i++)
	{
		InsertColumn(i, m_column_headers_[i], m_column_format_[i], m_column_width_[i], -1);
	}

	image_list_.Create(settings_.image_width, settings_.image_height, ILC_COLOR4, 10, 10);
	SetImageList(&image_list_, LVSIL_SMALL);
}

void DataListCtrl::save_columns_width() const
{
	if (m_width_columns_ != nullptr)
	{
		const auto n_columns_stored = m_width_columns_->GetSize();
		if (n_columns_stored != N_COLUMNS2)
			m_width_columns_->SetSize(N_COLUMNS2);
		for (auto i = 0; i < N_COLUMNS2; i++)
			m_width_columns_->SetAt(i, GetColumnWidth(i));
	}
}

void DataListCtrl::OnDestroy()
{
	save_columns_width();
}

void DataListCtrl::build_empty_bitmap(const boolean force_update)
{
	if (p_empty_bitmap_ != nullptr && !force_update)
		return;

	if (p_empty_bitmap_ != nullptr)
	{ delete p_empty_bitmap_; p_empty_bitmap_ = nullptr; }

	p_empty_bitmap_ = new CBitmap;
	CWindowDC dc(this);
	CDC mem_dc;
	VERIFY(mem_dc.CreateCompatibleDC(&dc));

	p_empty_bitmap_->CreateBitmap(settings_.image_width, settings_.image_height,
		dc.GetDeviceCaps(PLANES),
		dc.GetDeviceCaps(BITSPIXEL), nullptr);
	mem_dc.SelectObject(p_empty_bitmap_);
	mem_dc.SetMapMode(dc.GetMapMode());

	CBrush brush(RGB(200,200,200));
	mem_dc.SelectObject(&brush);
	CPen pen;
	pen.CreatePen(PS_SOLID, 1, RGB(0,0,0));
	mem_dc.SelectObject(&pen);
	const auto rect_data = CRect(1, 0, settings_.image_width, settings_.image_height);
	mem_dc.Rectangle(&rect_data);
}

void DataListCtrl::setVisibleRange(int first, int last)
{
	if (cache_ == nullptr)
		return;
	RowCacheUpdatePlan plan{};
	cache_->setVisibleRange(first, last, &plan);
	const int size = cache_->getSize();
	image_list_.SetImageCount(size);
	update_images();
}

void DataListCtrl::update_images()
{
	build_empty_bitmap(FALSE);
	const int size = cache_->getSize();
	for (int i = 0; i < size; ++i)
	{
		const RowMeta& meta = cache_->at(i);
		CBitmap bmp;
		switch (settings_.display_mode)
		{
		case DisplayMode::Data:
			if (data_renderer_)
			{
				data_renderer_->renderBitmap(settings_, meta, bmp);
			}
			break;
		case DisplayMode::Spikes:
			if (spike_renderer_)
			{
				spike_renderer_->renderBitmap(settings_, meta, bmp);
			}
			break;
		default:
			break;
		}
		if (bmp.m_hObject == nullptr && p_empty_bitmap_ != nullptr)
		{
			image_list_.Replace(i, p_empty_bitmap_, nullptr);
		}
		else
		{
			image_list_.Replace(i, &bmp, nullptr);
		}
	}
}

void DataListCtrl::on_get_display_info(NMHDR* p_nmhdr, LRESULT* p_result)
{
	auto* display_info = reinterpret_cast<LV_DISPINFO*>(p_nmhdr);
	LV_ITEM* item = &(display_info)->item;
	*p_result = 0;

	const int item_index = item->iItem;

	int first_cache = 0;
	int last_cache = -1;
	if (cache_ && cache_->getSize() > 0)
	{
		first_cache = cache_->getFirstIndex();
		last_cache = first_cache + cache_->getSize() - 1;
	}

	if (cache_)
	{
		if (item_index < first_cache)
		{
			int first = item_index;
			int last = first + GetCountPerPage() - 1;
			setVisibleRange(first, last);
		}
		else if (item_index > last_cache)
		{
			int last = item_index;
			int first = last - GetCountPerPage() + 1;
			setVisibleRange(first, last);
		}
		else if (cache_->getSize() == 0)
		{
			setVisibleRange(0, GetCountPerPage() - 1);
		}
	}

	if (cache_ == nullptr || cache_->getSize() == 0)
		return;

	const int first_index = cache_->getFirstIndex();
	int i_cache_index = item_index - first_index;
	if (i_cache_index < 0) i_cache_index = 0;
	if (i_cache_index >= cache_->getSize()) i_cache_index = cache_->getSize() - 1;

	const RowMeta& row = cache_->at(i_cache_index);

	if (item->mask & LVIF_TEXT)
	{
		CString cs;
		switch (item->iSubItem)
		{
		case CTRL2_COL_INDEX: cs.Format(_T("%i"), row.index); break;
		case CTRL2_COL_CURVE: break;
		case CTRL2_COL_INSECT: cs.Format(_T("%i"), row.insect_id); break;
		case CTRL2_COL_SENSI: cs = row.cs_sensillum_name; break;
		case CTRL2_COL_STIM1: cs = row.cs_stimulus1; break;
		case CTRL2_COL_CONC1: cs = row.cs_concentration1; break;
		case CTRL2_COL_STIM2: cs = row.cs_stimulus2; break;
		case CTRL2_COL_CONC2: cs = row.cs_concentration2; break;
		case CTRL2_COL_NBSPK: cs = row.cs_n_spikes; break;
		case CTRL2_COL_FLAG: cs = row.cs_flag; break;
		default: break;
		}
		if (!cs.IsEmpty())
			lstrcpy(item->pszText, cs);
	}

	if (item->mask & LVIF_IMAGE && item->iSubItem == CTRL2_COL_CURVE)
		item->iImage = i_cache_index;
}

void DataListCtrl::OnVScroll(const UINT n_sb_code, const UINT n_pos, CScrollBar* p_scroll_bar)
{
	switch (n_sb_code)
	{
	case SB_LINEUP:
		static_cast<ViewDbTable*>(GetParent())->OnMove(ID_RECORD_PREV);
		break;
	case SB_LINEDOWN:
		static_cast<ViewDbTable*>(GetParent())->OnMove(ID_RECORD_NEXT);
		break;
	case SB_PAGEUP:
		static_cast<ViewDbTable*>(GetParent())->OnMove(ID_RECORD_PAGE_UP);
		break;
	case SB_PAGEDOWN:
		static_cast<ViewDbTable*>(GetParent())->OnMove(ID_RECORD_PAGE_DOWN);
		break;
	default:
		CListCtrl::OnVScroll(n_sb_code, n_pos, p_scroll_bar);
		break;
	}
}

void DataListCtrl::OnKeyUp(UINT n_char, UINT n_rep_cnt, UINT n_flags)
{
	switch (n_char)
	{
	case VK_PRIOR:
		SendMessage(WM_VSCROLL, SB_PAGEUP, NULL);
		break;
	case VK_NEXT:
		SendMessage(WM_VSCROLL, SB_PAGEDOWN, NULL);
		break;
	case VK_UP:
		static_cast<ViewDbTable*>(GetParent())->OnMove(ID_RECORD_PREV);
		break;
	case VK_DOWN:
		static_cast<ViewDbTable*>(GetParent())->OnMove(ID_RECORD_NEXT);
		break;
	default:
		CListCtrl::OnKeyUp(n_char, n_rep_cnt, n_flags);
		break;
	}
}

void DataListCtrl::refresh_display()
{
	build_empty_bitmap(TRUE);
	update_images();
	Invalidate();
	UpdateWindow();
}

void DataListCtrl::resize_signal_column(const int n_pixels)
{
	m_column_width_[CTRL2_COL_CURVE] = n_pixels;
	image_list_.DeleteImageList();
	settings_.image_width = n_pixels;
	image_list_.Create(settings_.image_width, settings_.image_height, ILC_COLOR4, 10, 10);
	SetImageList(&image_list_, LVSIL_SMALL);
	image_list_.SetImageCount(cache_ ? cache_->getSize() : 0);
	refresh_display();
}

void DataListCtrl::fit_columns_to_size(const int n_pixels)
{
	int fixed_width = 0;
	for (const auto i : m_column_width_)
	{
		fixed_width += i;
	}
	fixed_width -= m_column_width_[CTRL2_COL_CURVE];
	const auto signal_column_width = n_pixels - fixed_width;
	if (signal_column_width != m_column_width_[CTRL2_COL_CURVE] && signal_column_width > 2)
	{
		SetColumnWidth(CTRL2_COL_CURVE, signal_column_width);
		resize_signal_column(signal_column_width);
	}
}


