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

class DataListCtrl : public CListCtrl
{
public:
	DataListCtrl();
	~DataListCtrl() override;

	void init(IDbWaveDataProvider* provider,
		const DisplaySettings& settings,
		IDataRenderer* dataRenderer,
		ISpikeRenderer* spikeRenderer,
		CUIntArray* width_columns = nullptr);
	void init_columns(CUIntArray* width_columns = nullptr);

	// DisplaySettings setters
	void set_amplitude_span(const float mv_span_new) { settings_.mv_span = mv_span_new; }
	void set_display_file_name(const boolean flag) { settings_.b_display_file_name = flag; }
	void set_time_intervals(const float t_first_new, const float t_last_new) { settings_.t_first = t_first_new; settings_.t_last = t_last_new; }
	void set_timespan_adjust_mode(const boolean flag) { settings_.b_set_time_span = flag; }
	void set_amplitude_adjust_mode(const boolean flag) { settings_.b_set_mv_span = flag; }
	void set_display_mode(const DisplayMode mode) { settings_.display_mode = mode; }
	void set_transform_mode(const DataTransform mode) { settings_.data_transform = mode; }
	void set_spike_plot_mode(const SpikePlotMode mode, const int i_class) { settings_.spike_plot_mode = mode; settings_.selected_class = i_class; }
	void set_visible_range(int first, int last);

	float get_amplitude_span() const { return settings_.mv_span; }
	boolean get_display_file_name() const { return settings_.b_display_file_name; }
	boolean get_timespan_adjust_mode() const { return settings_.b_set_time_span; }
	boolean get_amplitude_adjust_mode() const { return settings_.b_set_mv_span; }
	DisplayMode get_display_mode() const { return settings_.display_mode; }
	DataTransform get_transform_mode() const { return settings_.data_transform; }
	SpikePlotMode get_spike_plot_mode() const { return settings_.spike_plot_mode; }


	void refresh_display();
	void fit_columns_to_size(int n_pixels);
	void resize_signal_column(int n_pixels);

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void on_get_display_info(NMHDR* p_nmhdr, LRESULT* p_result);
	afx_msg void OnVScroll(UINT n_sb_code, UINT n_pos, CScrollBar* p_scroll_bar);
	afx_msg void OnKeyUp(UINT n_char, UINT n_rep_cnt, UINT n_flags);
	afx_msg void OnDestroy();

private:
	void build_empty_bitmap(boolean force_update = FALSE);
	void update_images();
	void save_columns_width() const;

private:
	static int m_column_width_[N_COLUMNS2];
	static CString m_column_headers_[N_COLUMNS2];
	static int m_column_format_[N_COLUMNS2];

	CUIntArray* m_width_columns_{ nullptr };
	CImageList image_list_;
	CBitmap* p_empty_bitmap_{ nullptr };
	IDbWaveDataProvider* provider_{ nullptr };
	RowCache* cache_{ nullptr };
	DisplaySettings settings_{};
	IDataRenderer* data_renderer_{ nullptr };
	ISpikeRenderer* spike_renderer_{ nullptr };
};


