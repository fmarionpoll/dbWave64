#pragma once

#include "ViewDbTable.h"
#include "DataListCtrl.h"

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
	void OnSize(UINT n_type, int cx, int cy);
	void OnUpdate(CView* p_sender, LPARAM l_hint, CObject* p_hint) override;
	void OnActivateView(BOOL b_activate, CView* p_activate_view, CView* p_deactive_view) override;
	void save_controls_state();
	void restore_controls_state();
	afx_msg void on_hdn_end_track_list_ctrl(NMHDR* p_nmhdr, LRESULT* p_result);
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
	afx_msg void on_item_activate_list_ctrl(NMHDR* p_nmhdr, LRESULT* p_result);
	afx_msg void on_dbl_clk_list_ctrl(NMHDR* p_nmhdr, LRESULT* p_result);

protected:
	DECLARE_MESSAGE_MAP()

private:
	DataListCtrl m_data_list_ctrl_;
	// cached UI values
	float m_time_first_  {0.f};
	float m_time_last_   {0.f};
	float m_amplitude_span_ {0.f};
	int m_spike_class_ {0};
	CUIntArray m_columns_width_;
};


