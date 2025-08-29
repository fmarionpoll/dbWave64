#pragma once

#include <afx.h>

struct RowMeta
{
	int record_id { 0 };
	int index { 0 };
	long insect_id { 0 };
	CString cs_sensillum_name {};
	CString cs_stimulus1 {};
	CString cs_concentration1 {};
	CString cs_stimulus2 {};
	CString cs_concentration2 {};
	CString cs_n_spikes {};
	CString cs_flag {};
	CString cs_datafile_name {};
	CString cs_spike_file_name {};
};

class IDbWaveDataProvider
{
public:
	virtual ~IDbWaveDataProvider() = default;
	virtual int get_records_count() const = 0;
	virtual RowMeta get_row_meta(int index) = 0;
};


