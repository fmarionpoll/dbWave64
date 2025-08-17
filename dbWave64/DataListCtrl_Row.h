#pragma once

#include "Spikedoc.h"
#include "ChartData.h"
#include "ChartSpikeBar.h"
#include "DataListCtrl_Infos.h"


class DataListCtrl_Row : public CObject
{
	DECLARE_SERIAL(DataListCtrl_Row)

	DataListCtrl_Row();
	DataListCtrl_Row(int i);
	~DataListCtrl_Row() override;

	BOOL b_changed {false};
	WORD w_version {0};
	BOOL b_init {false};
	BOOL display_processed {false};
	int last_display_mode {-1};

	int record_id{ 0 };
	int index {0};
	long insect_id {0};
	CString cs_comment{};
	CString cs_datafile_name{};
	CString cs_spike_file_name{};
	CString cs_sensillum_name{};
	CString cs_stimulus1{};
	CString cs_concentration1{};
	CString cs_stimulus2{};
	CString cs_concentration2{};
	CString cs_n_spikes{};
	CString cs_flag{};
	CString cs_date{};

	AcqDataDoc* p_data_doc  {nullptr};
	ChartData* p_chart_data_wnd = {nullptr};
	
	CSpikeDoc* p_spike_doc {nullptr};
	ChartSpikeBar* p_chart_spike_wnd {nullptr};

	DataListCtrl_Row& operator =(const DataListCtrl_Row& arg);
	void Serialize(CArchive& ar) override;

	void attach_database_record(CdbWaveDoc* db_wave_doc);
	void set_display_parameters(DataListCtrlInfos* infos, int i_image);
	void reset_display_processed();

protected:
	void display_data_wnd(DataListCtrlInfos* infos, int i_image);
	void display_spike_wnd(DataListCtrlInfos* infos, int i_image);
	void display_empty_wnd(DataListCtrlInfos* infos, int i_image);
	void print_filename_on_rectangle(const DataListCtrlInfos* infos, CDC& mem_dc);

	void plot_data(DataListCtrlInfos* infos, int i_image) const;
	void plot_spikes(DataListCtrlInfos* infos, int i_image) const;

};
