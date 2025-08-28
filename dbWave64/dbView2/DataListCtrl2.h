#pragma once

#include <afxcview.h>
#include "DisplaySettings.h"
#include "DbWaveDataProvider.h"
#include "RowCache.h"
#include "Renderers.h"

#define CTRL2_COL_INDEX 1
#define CTRL2_COL_CURVE (CTRL2_COL_INDEX+1)
#define CTRL2_COL_INSECT (CTRL2_COL_INDEX+2)
#define CTRL2_COL_SENSI (CTRL2_COL_INDEX+3)
#define CTRL2_COL_STIM1 (CTRL2_COL_INDEX+4)
#define CTRL2_COL_CONC1 (CTRL2_COL_INDEX+5)
#define CTRL2_COL_STIM2 (CTRL2_COL_INDEX+6)
#define CTRL2_COL_CONC2 (CTRL2_COL_INDEX+7)
#define CTRL2_COL_NBSPK (CTRL2_COL_INDEX+8)
#define CTRL2_COL_FLAG (CTRL2_COL_INDEX+9)
#define N_COLUMNS2 11

class DataListCtrl2 : public CListCtrl
{
public:
	DataListCtrl2();
	~DataListCtrl2() override;

	void init(IDbWaveDataProvider* provider,
		const DisplaySettings& settings,
		IDataRenderer* dataRenderer,
		ISpikeRenderer* spikeRenderer);
	void init_columns(CUIntArray* width_columns = nullptr);
	void setVisibleRange(int first, int last);
	void refresh_display();
	void fit_columns_to_size(int n_pixels);
	void resize_signal_column(int n_pixels);

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void on_get_display_info(NMHDR* p_nmhdr, LRESULT* p_result);
	afx_msg void OnVScroll(UINT n_sb_code, UINT n_pos, CScrollBar* p_scroll_bar);
	afx_msg void OnDestroy();

private:
	void build_empty_bitmap(boolean force_update = FALSE);
	void update_images();
	void save_columns_width() const;

private:
	static int m_column_width_[N_COLUMNS2];
	static CString m_column_headers_[N_COLUMNS2];
	static int m_column_format_[N_COLUMNS2];

	CUIntArray* m_width_columns_ { nullptr };
	CImageList image_list_;
	CBitmap* p_empty_bitmap_ { nullptr };
	IDbWaveDataProvider* provider_ { nullptr };
	RowCache* cache_ { nullptr };
	DisplaySettings settings_ {};
	IDataRenderer* data_renderer_ { nullptr };
	ISpikeRenderer* spike_renderer_ { nullptr };
};


