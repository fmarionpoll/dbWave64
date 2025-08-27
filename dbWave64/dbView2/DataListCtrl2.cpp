#include "StdAfx.h"

#include "DataListCtrl2.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

int DataListCtrl2::m_column_width_[] = {
	1,
	10, 300, 15, 30,
	30, 50, 40, 40,
	40, 40
};

CString DataListCtrl2::m_column_headers_[] = {
	__T(""),
	__T("#"), __T("data"), __T("insect ID"), __T("sensillum"),
	__T("stim1"), __T("conc1"), __T("stim2"), __T("conc2"),
	__T("spikes"), __T("flag")
};

int DataListCtrl2::m_column_format_[] = {
	LVCFMT_LEFT,
	LVCFMT_CENTER, LVCFMT_CENTER, LVCFMT_CENTER, LVCFMT_CENTER,
	LVCFMT_CENTER, LVCFMT_CENTER, LVCFMT_CENTER, LVCFMT_CENTER,
	LVCFMT_CENTER, LVCFMT_CENTER
};

BEGIN_MESSAGE_MAP(DataListCtrl2, CListCtrl)
	ON_WM_VSCROLL()
	ON_NOTIFY_REFLECT(LVN_GETDISPINFO, on_get_display_info)
	ON_WM_DESTROY()
    ON_NOTIFY(HDN_ENDTRACK, 0, on_hdn_endtrack)
END_MESSAGE_MAP()

DataListCtrl2::DataListCtrl2()
	: cache_(nullptr)
{
}

DataListCtrl2::~DataListCtrl2()
{
	if (cache_ != nullptr)
		delete cache_;
	if (p_empty_bitmap_ != nullptr)
		delete p_empty_bitmap_;
	SAFE_DELETE(provider_)
	SAFE_DELETE(data_renderer_)
	SAFE_DELETE(spike_renderer_)
}

void DataListCtrl2::init(IDbWaveDataProvider* provider,
	const DisplaySettings& settings,
	IDataRenderer* dataRenderer,
	ISpikeRenderer* spikeRenderer)
{
	provider_ = provider;
	settings_ = settings;
	data_renderer_ = dataRenderer;
	spike_renderer_ = spikeRenderer;
	cache_ = new RowCache(provider_);

	init_columns(nullptr);
}

void DataListCtrl2::init_columns(CUIntArray* width_columns)
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

void DataListCtrl2::save_columns_width() const
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

void DataListCtrl2::OnDestroy()
{
	save_columns_width();
}

void DataListCtrl2::on_hdn_endtrack(NMHDR* p_nmhdr, LRESULT* p_result)
{
	const auto p_hdr = reinterpret_cast<LPNMHEADER>(p_nmhdr);
	if (p_hdr->iItem == CTRL2_COL_CURVE)
		resize_signal_column(p_hdr->pitem->cxy);
	*p_result = 0;
}

void DataListCtrl2::build_empty_bitmap(const boolean force_update)
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

void DataListCtrl2::setVisibleRange(int first, int last)
{
	if (cache_ == nullptr)
		return;
	cache_->setVisibleRange(first, last);
	const int size = cache_->getSize();
	image_list_.SetImageCount(size);
	update_images();
}

void DataListCtrl2::update_images()
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

void DataListCtrl2::on_get_display_info(NMHDR* p_nmhdr, LRESULT* p_result)
{
	auto* display_info = reinterpret_cast<LV_DISPINFO*>(p_nmhdr);
	LV_ITEM* item = &(display_info)->item;
	*p_result = 0;

	const int item_index = item->iItem;
	const int first_index = cache_ ? cache_->getFirstIndex() : 0;
	const int i_cache_index = item_index - first_index;
	if (cache_ == nullptr || i_cache_index < 0 || i_cache_index >= cache_->getSize())
		return;

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

void DataListCtrl2::OnVScroll(const UINT n_sb_code, const UINT n_pos, CScrollBar* p_scroll_bar)
{
	switch (n_sb_code)
	{
	case SB_LINEUP:
		SendMessage(WM_VSCROLL, SB_LINEUP, NULL);
		break;
	case SB_LINEDOWN:
		SendMessage(WM_VSCROLL, SB_LINEDOWN, NULL);
		break;
	default:
		CListCtrl::OnVScroll(n_sb_code, n_pos, p_scroll_bar);
		break;
	}
}

void DataListCtrl2::refresh_display()
{
	build_empty_bitmap(TRUE);
	update_images();
	Invalidate();
	UpdateWindow();
}

void DataListCtrl2::resize_signal_column(const int n_pixels)
{
	m_column_width_[CTRL2_COL_CURVE] = n_pixels;
	image_list_.DeleteImageList();
	settings_.image_width = n_pixels;
	image_list_.Create(settings_.image_width, settings_.image_height, ILC_COLOR4, 10, 10);
	SetImageList(&image_list_, LVSIL_SMALL);
	image_list_.SetImageCount(cache_ ? cache_->getSize() : 0);
	refresh_display();
}

void DataListCtrl2::fit_columns_to_size(const int n_pixels)
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


