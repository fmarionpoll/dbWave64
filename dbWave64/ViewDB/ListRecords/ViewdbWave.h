#pragma once

#include "ViewDB/ViewDbTable.h"
#include "DataListCtrl.h"
#include "ViewDB/ListRecords/DisplaySettings.h"

struct ViewdbWaveState {
	boolean primed = false;
	DisplayMode display_mode;
	BOOL b_display_file_name;
	BOOL b_filter_dat;
	BOOL b_set_time_span;
	float t_first;
	float t_last;
	BOOL b_set_mv_span;
	float mv_span;
	BOOL b_all_classes;
	int spike_class;
};

class ViewdbWave : public ViewDbTable
{
protected:
	DECLARE_DYNCREATE(ViewdbWave)
	ViewdbWave();
	~ViewdbWave() override;

public:
	enum { IDD = IDD_VIEWDBWAVE };
	void delete_records();

protected:
	void DoDataExchange(CDataExchange* p_dx) override;
	void OnInitialUpdate() override;
	void OnActivateView(BOOL b_activate, CView* p_activate_view, CView* p_deactive_view) override;
	void OnUpdate(CView* p_sender, LPARAM l_hint, CObject* p_hint) override;

	void OnSize(UINT n_type, int cx, int cy);

	afx_msg void on_record_page_up();
	afx_msg void on_record_page_down();

	afx_msg void on_hdn_end_track_list_ctrl(NMHDR* p_nmhdr, LRESULT* p_result);
	afx_msg void on_item_activate_list_ctrl(NMHDR* p_nmhdr, LRESULT* p_result);
	afx_msg void on_dbl_clk_list_ctrl(NMHDR* p_nmhdr, LRESULT* p_result);

	afx_msg void on_bn_clicked_data();
	afx_msg void on_bn_clicked_display_spikes();
	afx_msg void on_bn_clicked_display_nothing();
	afx_msg void on_en_change_time_first();
	afx_msg void on_en_change_time_last();
	afx_msg void on_en_change_amplitude_span();
	afx_msg void on_en_change_spike_class();
	afx_msg void on_bn_clicked_check_filename();
	afx_msg void on_click_median_filter();
	afx_msg void on_bn_clicked_check2();
	afx_msg void on_bn_clicked_check1();
	afx_msg void on_bn_clicked_radio_all_classes();
	afx_msg void on_bn_clicked_radio_one_class();
	afx_msg void on_lvn_column_click_list_ctrl(NMHDR* p_nmhdr, LRESULT* p_result);
	afx_msg LRESULT on_my_message(WPARAM w_param, LPARAM l_param);

protected:
	DECLARE_MESSAGE_MAP()
	void save_controls_state();
	void restore_controls_state();
	void set_display_mode(DisplayMode mode);
	void update_controls();

private:
	DataListCtrl m_list_ctrl_;
	// cached UI values
	float m_time_first_  {0.f};
	float m_time_last_   {0.f};
	float m_amplitude_span_ {0.f};
	int m_spike_class_ {0};
	CUIntArray m_columns_width_;
	static ViewdbWaveState s_view_state_;
};


