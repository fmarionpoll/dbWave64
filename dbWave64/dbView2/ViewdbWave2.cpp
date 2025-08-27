#include "StdAfx.h"
#include "ViewdbWave2.h"
#include "dbWave.h"
#include "resource.h"
#include "Adapters.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(ViewdbWave2, ViewDbTable)

BEGIN_MESSAGE_MAP(ViewdbWave2, ViewDbTable)
	ON_WM_SIZE()
	ON_NOTIFY(HDN_ENDTRACK, 0, &ViewdbWave2::on_hdn_end_track_list_ctrl)
END_MESSAGE_MAP()

ViewdbWave2::ViewdbWave2() : ViewDbTable(IDD)
{
}

ViewdbWave2::~ViewdbWave2() = default;

void ViewdbWave2::DoDataExchange(CDataExchange* p_dx)
{
	ViewDbTable::DoDataExchange(p_dx);
}

void ViewdbWave2::OnInitialUpdate()
{
	ViewDbTable::OnInitialUpdate();

	VERIFY(m_data_list_ctrl_.SubclassDlgItem(IDC_LISTCTRL, this));

	DisplaySettings settings;
	settings.image_width = 300;
	settings.image_height = 50;
	settings.display_mode = DisplayMode::Data;

	// Wire provider to current document and minimal renderers
	auto* doc = GetDocument();
	auto* provider = new DbWaveDocProviderAdapter(doc);
	auto* data_renderer = new ChartDataRendererAdapter();
	auto* spike_renderer = new ChartSpikeRendererAdapter();
	m_data_list_ctrl_.init(provider, settings, data_renderer, spike_renderer);

	const int n_records = doc->db_get_records_count();
	m_data_list_ctrl_.SetItemCountEx(n_records);
	const int per_page = m_data_list_ctrl_.GetCountPerPage();
	m_data_list_ctrl_.setVisibleRange(0, per_page - 1);
}

void ViewdbWave2::OnSize(const UINT n_type, const int cx, const int cy)
{
	ViewDbTable::OnSize(n_type, cx, cy);
	if (IsWindow(m_data_list_ctrl_.m_hWnd))
	{
		CRect rect;
		m_data_list_ctrl_.GetClientRect(&rect);
		m_data_list_ctrl_.fit_columns_to_size(rect.Width());
	}
}

void ViewdbWave2::on_hdn_end_track_list_ctrl(NMHDR* p_nmhdr, LRESULT* p_result)
{
	const auto p_hdr = reinterpret_cast<LPNMHEADER>(p_nmhdr);
	if (p_hdr->iItem == CTRL2_COL_CURVE)
		m_data_list_ctrl_.resize_signal_column(p_hdr->pitem->cxy);
	*p_result = 0;
}


