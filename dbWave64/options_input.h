#pragma once
#include "AcqWaveChanArray.h"
#include "AcqWaveFormat.h"

/* for OLDRV_SETCHANNELTYPE         */
#define OLx_CHNT_SINGLEENDED         0
#define OLx_CHNT_DIFFERENTIAL        1

/* for Encoding property            */
#define OLx_ENC_BINARY               0
#define OLx_ENC_2SCOMP                1

/* for trigger property             */
#define OLx_TRG_SOFT                 0
#define OLx_TRG_EXTERN               1
#define OLx_TRG_THRESHPOS            2
#define OLx_TRG_THRESHNEG            3
#define OLx_TRG_ANALOGEVENT          4
#define OLx_TRG_DIGITALEVENT         5
#define OLx_TRG_TIMEREVENT           6
#define OLx_TRG_EXTRA                7

/* for ClockSource property         */
#define OLx_CLK_INTERNAL             0
#define OLx_CLK_EXTERNAL             1
#define OLx_CLK_EXTRA                2


class options_input final : public CObject
{
	DECLARE_SERIAL(options_input)
	options_input();
	~options_input() override;
	options_input& operator =(const options_input& arg);
	void Serialize(CArchive& ar) override;
	int serialize_all_string_arrays(CArchive& ar, int& n);

public:
	BOOL b_changed{false};
	WORD w_version{2};

	CString cs_basename{};
	CString cs_pathname{};
	int experiment_number{0};
	int insect_number{0};
	int i_under_sample{1};
	BOOL b_audible_sound{0};
	BOOL b_channel_type{OLx_CHNT_DIFFERENTIAL};

	CStringArray cs_a_stimulus{};
	CStringArray cs_a_concentration{};
	CStringArray cs_a_stimulus2{};
	CStringArray cs_a_concentration2{};
	CStringArray cs_a_insect{};
	CStringArray cs_a_location{};
	CStringArray cs_a_sensillum{};
	CStringArray cs_a_strain{};
	CStringArray cs_a_operator_name{};
	CStringArray cs_a_sex{};
	CStringArray cs_a_experiment{};

	int ics_a_stimulus{0};
	int ics_a_concentration{0};
	int ics_a_stimulus2{0};
	int ics_a_concentration2{0};
	int ics_a_insect{0};
	int ics_a_location{0};
	int ics_a_sensillum{0};
	int ics_a_strain{0};
	int ics_a_operator_name{0};
	int ics_a_sex{0};
	int ics_a_repeat{0};
	int ics_a_repeat2{0};
	int ics_a_experiment{0};
	int i_zoom_cur_sel{0};

	CWaveFormat wave_format{};
	CWaveChanArray chan_array{};
	float sweep_duration{2.f};
	float duration_to_acquire{ 2. };

private:
	int serialize_one_string_array(CArchive& ar, CStringArray& string_array, int& n);
	int serialize_all_int(CArchive& ar, int& n);
	int serialize_one_int(CArchive& ar, int& value, int& n);
};
