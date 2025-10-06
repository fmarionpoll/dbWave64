#include "StdAfx.h"
#include "options_view_data.h"

#include "dbWave.h"


IMPLEMENT_SERIAL(options_view_data, CObject, 0 /* schema number*/)

options_view_data::options_view_data()
= default;

options_view_data::~options_view_data()
= default;

options_view_data& options_view_data::operator =(const options_view_data& arg)
{
	if (this != &arg)
	{
		b_all_channels = arg.b_all_channels;
		b_complete_record = arg.b_complete_record;
		b_multiple_rows = arg.b_multiple_rows;
		b_split_curves = arg.b_split_curves;
		b_maximize_gain = arg.b_maximize_gain;
		b_center_curves = arg.b_center_curves;
		b_keep_for_each_file = arg.b_keep_for_each_file;
		b_display_old_detect_p = arg.b_display_old_detect_p;
		b_acq_comments = arg.b_acq_comments;
		b_acq_date = arg.b_acq_date;
		b_acq_time = arg.b_acq_time;
		b_file_size = arg.b_file_size;
		b_acq_channel_comment = arg.b_acq_channel_comment;
		b_acq_channel_setting = arg.b_acq_channel_setting;
		b_units = arg.b_units;
		b_contours = arg.b_contours;
		b_graphics = arg.b_graphics;
		b_data_base_columns = arg.b_data_base_columns;
		b_to_excel = arg.b_to_excel;
		hz_resolution = arg.hz_resolution;
		vt_resolution = arg.vt_resolution;
		unit_mode = arg.unit_mode;
		view_data = arg.view_data;
		view_spk_detect_filtered = arg.view_spk_detect_filtered;
		view_spk_detect_data = arg.view_spk_detect_data;
		view_spk_detect_spk = arg.view_spk_detect_spk;
		view_spk_detect_bars = arg.view_spk_detect_bars;
		view_ad_continuous = arg.view_ad_continuous;
		spk_view_data = arg.spk_view_data;
		spk_view_spk = arg.spk_view_spk;
		spk_view_bars = arg.spk_view_bars;
		spk_sort1_spk = arg.spk_sort1_spk;
		spk_sort1_parameters = arg.spk_sort1_parameters;
		spk_sort1_bars = arg.spk_sort1_bars;
		spk_sort1_hist = arg.spk_sort1_hist;
		db_view_data = arg.db_view_data;
		b_filter_dat = arg.b_filter_dat;
		db_view_bars = arg.db_view_bars;
		db_view_shape = arg.db_view_shape;
		n_filter_index = arg.n_filter_index;
		b_detect_while_browse = arg.b_detect_while_browse;
		b_goto_record_id = arg.b_goto_record_id;
		col0_width = arg.col0_width;
		row0_height = arg.row0_height;
		b_col0_visible = arg.b_col0_visible;
		b_row0_visible = arg.b_row0_visible;
		i_row_height = arg.i_row_height;
		t_first = arg.t_first;
		t_last = arg.t_last;
		mv_span = arg.mv_span;
		display_mode = arg.display_mode;
		b_display_file_name = arg.b_display_file_name;
		b_set_time_span = arg.b_set_time_span;
		b_set_mv_span = arg.b_set_mv_span;
		b_display_all_classes = arg.b_display_all_classes;
		spike_class = arg.spike_class;
	}
	return *this;
}

void options_view_data::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		m_w_version = 6;
		ar << m_w_version;

		// how data are displayed from file to file
		WORD wBrowseFlags;
		WORD w_2_power = 1;
		wBrowseFlags = b_all_channels * w_2_power; //0
		w_2_power += w_2_power;
		wBrowseFlags += b_complete_record * w_2_power; //2
		w_2_power += w_2_power;
		wBrowseFlags += b_multiple_rows * w_2_power; //4
		w_2_power += w_2_power;
		wBrowseFlags += b_split_curves * w_2_power; //8
		w_2_power += w_2_power;
		wBrowseFlags += b_maximize_gain * w_2_power; //16
		w_2_power += w_2_power;
		wBrowseFlags += b_center_curves * w_2_power; //32
		w_2_power += w_2_power;
		wBrowseFlags += b_keep_for_each_file * w_2_power; //64
		w_2_power += w_2_power;
		wBrowseFlags += b_display_old_detect_p * w_2_power; //128
		w_2_power += w_2_power;
		wBrowseFlags += b_filter_dat * w_2_power; //256
		w_2_power += w_2_power;
		wBrowseFlags += b_detect_while_browse * w_2_power; //512
		w_2_power += w_2_power;
		wBrowseFlags += b_goto_record_id * w_2_power; //1024
		w_2_power += w_2_power;
		wBrowseFlags += b_display_file_name * w_2_power; //2048
		w_2_power += w_2_power;
		wBrowseFlags += b_set_time_span * w_2_power; //4096
		w_2_power += w_2_power;
		wBrowseFlags += b_set_mv_span * w_2_power; //8192
		w_2_power += w_2_power;
		wBrowseFlags += b_display_all_classes * w_2_power; //16384
		ar << wBrowseFlags;

		// export infos from data files
		WORD wInfoFlags;
		w_2_power = 1;
		wInfoFlags = b_acq_comments * w_2_power; // 0
		w_2_power += w_2_power;
		wInfoFlags += b_acq_date * w_2_power; // 2
		w_2_power += w_2_power;
		wInfoFlags += b_acq_time * w_2_power; // 4
		w_2_power += w_2_power;
		wInfoFlags += b_file_size * w_2_power; // 8
		w_2_power += w_2_power;
		wInfoFlags += b_acq_channel_comment * w_2_power; // 16
		w_2_power += w_2_power;
		wInfoFlags += b_acq_channel_setting * w_2_power; // 32
		w_2_power += w_2_power;
		wInfoFlags += b_units * w_2_power; // 64
		w_2_power += w_2_power;
		wInfoFlags += b_contours * w_2_power; // 128
		w_2_power += w_2_power;
		wInfoFlags += b_graphics * w_2_power; // 256
		w_2_power += w_2_power;
		wInfoFlags += b_to_excel * w_2_power; // 512
		w_2_power += w_2_power;
		wInfoFlags += b_data_base_columns * w_2_power; // 1024
		ar << wInfoFlags;

		// export int(s) WORD = nb of items
		ar << static_cast<WORD>(11);
		ar << hz_resolution; //1
		ar << vt_resolution; //2
		ar << unit_mode; //3
		ar << n_filter_index; //6
		ar << col0_width; //7
		ar << row0_height; //8
		ar << b_col0_visible; //9
		ar << b_row0_visible; //10
		ar << i_row_height; //11
		ar << display_mode; //12
		ar << spike_class; //13

		ar << i_column_width.GetSize();
		for (int ii = 0; ii < i_column_width.GetSize(); ii++)
			ar << i_column_width[ii];

		ar << 3; // nb floats
		ar << t_first;
		ar << t_last;
		ar << mv_span;

		n_scope_items = 16;
		ar << n_scope_items;
		view_data.Serialize(ar); //1
		view_spk_detect_data.Serialize(ar); //2
		view_spk_detect_spk.Serialize(ar); //3
		view_spk_detect_bars.Serialize(ar); //4
		view_ad_continuous.Serialize(ar); //5
		spk_view_data.Serialize(ar); //6
		spk_view_spk.Serialize(ar); //7
		spk_view_bars.Serialize(ar); //8
		spk_sort1_spk.Serialize(ar); //9
		spk_sort1_parameters.Serialize(ar); //10
		spk_sort1_hist.Serialize(ar); //11
		db_view_data.Serialize(ar); //12
		db_view_bars.Serialize(ar); //13
		db_view_shape.Serialize(ar); //14
		spk_sort1_bars.Serialize(ar); //15
		view_spk_detect_filtered.Serialize(ar); //16
	}
	else
	{
		WORD version;
		ar >> version;
		if (version < 6) {
			read_version_before_6(ar, version); // version not supported
			return;
		}

		// how data are displayed from file to file
		WORD wBrowseFlags;
		WORD wMult = 1;
		ar >> wBrowseFlags;
		
		b_all_channels = ((wBrowseFlags & wMult) > 0);
		wMult += wMult;
		b_complete_record = ((wBrowseFlags & wMult) > 0);
		wMult += wMult;
		b_multiple_rows = ((wBrowseFlags & wMult) > 0);
		wMult += wMult;
		b_split_curves = ((wBrowseFlags & wMult) > 0);
		wMult += wMult;
		b_maximize_gain = ((wBrowseFlags & wMult) > 0);
		wMult += wMult;
		b_center_curves = ((wBrowseFlags & wMult) > 0);
		wMult += wMult;
		b_keep_for_each_file = ((wBrowseFlags & wMult) > 0);
		wMult += wMult;
		b_display_old_detect_p = ((wBrowseFlags & wMult) > 0);
		wMult += wMult;
		b_filter_dat = ((wBrowseFlags & wMult) > 0);
		wMult += wMult;
		b_detect_while_browse = ((wBrowseFlags & wMult) > 0);
		wMult += wMult;
		b_goto_record_id = ((wBrowseFlags & wMult) > 0);
		wMult += wMult;
		b_display_file_name = ((wBrowseFlags & wMult) > 0);
		wMult += wMult;
		b_set_time_span = ((wBrowseFlags & wMult) > 0);
		wMult += wMult;
		b_set_mv_span = ((wBrowseFlags & wMult) > 0);
		wMult += wMult;
		b_display_all_classes = ((wBrowseFlags & wMult) > 0);

		// export textual infos from data files
		WORD wInfoFlags;
		ar >> wInfoFlags;
		wMult = 1;
		b_acq_comments = ((wInfoFlags & wMult) > 0);
		wMult += wMult;
		b_acq_date = ((wInfoFlags & wMult) > 0);
		wMult += wMult;
		b_acq_time = ((wInfoFlags & wMult) > 0);
		wMult += wMult;
		b_file_size = ((wInfoFlags & wMult) > 0);
		wMult += wMult;
		b_acq_channel_comment = ((wInfoFlags & wMult) > 0);
		wMult += wMult;
		b_acq_channel_setting = ((wInfoFlags & wMult) > 0);
		wMult += wMult;
		b_units = ((wInfoFlags & wMult) > 0);
		wMult += wMult;
		b_contours = ((wInfoFlags & wMult) > 0);
		wMult += wMult;
		b_graphics = ((wInfoFlags & wMult) > 0);
		wMult += wMult;
		b_to_excel = ((wInfoFlags & wMult) > 0);
		wMult += wMult;
		b_data_base_columns = ((wInfoFlags & wMult) > 0);

		WORD n_int_items;
		ar >> n_int_items;
		int nb_int_items = n_int_items;
		ar >> hz_resolution;
		nb_int_items--; //1
		ar >> vt_resolution;
		nb_int_items--; //2
		ar >> unit_mode;
		nb_int_items--; //3
		ar >> n_filter_index;
		nb_int_items--; //6
		ar >> col0_width;
		nb_int_items--; //7
		ar >> row0_height;
		nb_int_items--; //8
		ar >> b_col0_visible;
		nb_int_items--; //9
		ar >> b_row0_visible;
		nb_int_items--; //10
		ar >> i_row_height;
		nb_int_items--; //11
		ar >> display_mode;
		nb_int_items--; //12
		if (nb_int_items > 0) ar >> spike_class;
		nb_int_items--;

		ar >> nb_int_items;
		i_column_width.SetSize(nb_int_items);
		for (auto ii = 0; ii < nb_int_items; ii++)
			ar >> i_column_width[ii];
		int n_float_items;
		ar >> n_float_items;
		ar >> t_first;
		n_float_items--;
		ar >> t_last;
		n_float_items--;
		ar >> mv_span;
		n_float_items--;

		ar >> n_scope_items;
		if (n_scope_items > 0)
		{
			view_data.Serialize(ar);
			n_scope_items--;
			view_spk_detect_data.Serialize(ar);
			n_scope_items--;
			view_spk_detect_spk.Serialize(ar);
			n_scope_items--;
			view_spk_detect_bars.Serialize(ar);
			n_scope_items--;
			view_ad_continuous.Serialize(ar);
			n_scope_items--;
			spk_view_data.Serialize(ar);
			n_scope_items--;
			spk_view_spk.Serialize(ar);
			n_scope_items--;
			spk_view_bars.Serialize(ar);
			n_scope_items--;
			spk_sort1_spk.Serialize(ar);
			n_scope_items--;
			spk_sort1_parameters.Serialize(ar);
			n_scope_items--;
			spk_sort1_hist.Serialize(ar);
			n_scope_items--;
			if (n_scope_items > 0)
			{
				db_view_data.Serialize(ar);
				n_scope_items--;
			}
			if (n_scope_items > 0)
			{
				db_view_bars.Serialize(ar);
				n_scope_items--;
			}
			if (n_scope_items > 0)
			{
				db_view_shape.Serialize(ar);
				n_scope_items--;
			}
			if (n_scope_items > 0)
			{
				spk_sort1_bars.Serialize(ar);
				n_scope_items--;
			}
			if (n_scope_items > 0)
			{
				view_spk_detect_filtered.Serialize(ar);
				n_scope_items--;
			}
			ASSERT(n_scope_items == 0);
		}
	}
}

void options_view_data::read_version_before_6(CArchive& ar, WORD version)
{
	const auto options_print_data = &(static_cast<CdbWaveApp*>(AfxGetApp())->options_print_data);
	// print options
	WORD wPrintFlags, wMult;
	ar >> wPrintFlags;
	wMult = 1;
	options_print_data->b_acq_comment = ((wPrintFlags & wMult) > 0);
	wMult += wMult;
	options_print_data->b_acq_date_time = ((wPrintFlags & wMult) > 0);
	wMult += wMult;
	options_print_data->b_channel_comment = ((wPrintFlags & wMult) > 0);
	wMult += wMult;
	options_print_data->b_channel_settings = ((wPrintFlags & wMult) > 0);
	wMult += wMult;
	options_print_data->b_doc_name = ((wPrintFlags & wMult) > 0);
	wMult += wMult;
	options_print_data->b_frame_rect = ((wPrintFlags & wMult) > 0);
	wMult += wMult;
	options_print_data->b_clip_rect = ((wPrintFlags & wMult) > 0);
	wMult += wMult;
	options_print_data->b_timescale_bar = ((wPrintFlags & wMult) > 0);
	wMult += wMult;
	options_print_data->b_voltage_scale_bar = ((wPrintFlags & wMult) > 0);
	wMult += wMult;
	options_print_data->b_print_selection = ((wPrintFlags & wMult) > 0);
	wMult += wMult;
	options_print_data->b_print_spk_bars = ((wPrintFlags & wMult) > 0);
	wMult += wMult;
	options_print_data->b_filter_data_source = ((wPrintFlags & wMult) > 0);

	// print parameters
	WORD w1;
	ar >> w1;
	options_print_data->horizontal_resolution = w1;
	ar >> w1;
	options_print_data->vertical_resolution = w1;
	ar >> w1;
	options_print_data->height_doc = w1;
	ar >> w1;
	options_print_data->width_doc = w1;
	ar >> w1;
	options_print_data->left_page_margin = w1;
	ar >> w1;
	options_print_data->bottom_page_margin = w1;
	ar >> w1;
	options_print_data->right_page_margin = w1;
	ar >> w1;
	options_print_data->top_page_margin = w1;
	ar >> w1;
	options_print_data->height_separator = w1;
	ar >> w1;
	options_print_data->text_separator = w1;
	ar >> w1;
	options_print_data->font_size = w1;

	// how data are displayed from file to file
	WORD wBrowseFlags;
	ar >> wBrowseFlags;
	wMult = 1;
	b_all_channels = ((wBrowseFlags & wMult) > 0);
	wMult += wMult;
	b_complete_record = ((wBrowseFlags & wMult) > 0);
	wMult += wMult;
	b_multiple_rows = ((wBrowseFlags & wMult) > 0);
	wMult += wMult;
	b_split_curves = ((wBrowseFlags & wMult) > 0);
	wMult += wMult;
	b_maximize_gain = ((wBrowseFlags & wMult) > 0);
	wMult += wMult;
	b_center_curves = ((wBrowseFlags & wMult) > 0);
	wMult += wMult;
	b_keep_for_each_file = ((wBrowseFlags & wMult) > 0);
	wMult += wMult;
	b_display_old_detect_p = ((wBrowseFlags & wMult) > 0);
	wMult += wMult;
	b_filter_dat = ((wBrowseFlags & wMult) > 0);
	wMult += wMult;
	b_detect_while_browse = ((wBrowseFlags & wMult) > 0);
	wMult += wMult;
	b_goto_record_id = ((wBrowseFlags & wMult) > 0);
	wMult += wMult;
	b_display_file_name = ((wBrowseFlags & wMult) > 0);
	wMult += wMult;
	b_set_time_span = ((wBrowseFlags & wMult) > 0);
	wMult += wMult;
	b_set_mv_span = ((wBrowseFlags & wMult) > 0);
	wMult += wMult;
	b_display_all_classes = ((wBrowseFlags & wMult) > 0);

	// export textual infos from data files
	WORD wInfoFlags;
	ar >> wInfoFlags;
	wMult = 1;
	b_acq_comments = ((wInfoFlags & wMult) > 0);
	wMult += wMult;
	b_acq_date = ((wInfoFlags & wMult) > 0);
	wMult += wMult;
	b_acq_time = ((wInfoFlags & wMult) > 0);
	wMult += wMult;
	b_file_size = ((wInfoFlags & wMult) > 0);
	wMult += wMult;
	b_acq_channel_comment = ((wInfoFlags & wMult) > 0);
	wMult += wMult;
	b_acq_channel_setting = ((wInfoFlags & wMult) > 0);
	wMult += wMult;
	b_units = ((wInfoFlags & wMult) > 0);
	wMult += wMult;
	b_contours = ((wInfoFlags & wMult) > 0);
	wMult += wMult;
	b_graphics = ((wInfoFlags & wMult) > 0);
	wMult += wMult;
	b_to_excel = ((wInfoFlags & wMult) > 0);
	wMult += wMult;
	b_data_base_columns = ((wInfoFlags & wMult) > 0);

	if (version > 4)
	{
		WORD n_int_items;
		ar >> n_int_items;
		int nb_int_items = n_int_items;
		ar >> hz_resolution;
		nb_int_items--; //1
		ar >> vt_resolution;
		nb_int_items--; //2
		ar >> unit_mode;
		nb_int_items--; //3
		ar >> options_print_data->spike_height;
		nb_int_items--; //4
		ar >> options_print_data->spike_width;
		nb_int_items--; //5
		ar >> n_filter_index;
		nb_int_items--; //6
		ar >> col0_width;
		nb_int_items--; //7
		ar >> row0_height;
		nb_int_items--; //8
		ar >> b_col0_visible;
		nb_int_items--; //9
		ar >> b_row0_visible;
		nb_int_items--; //10
		ar >> i_row_height;
		nb_int_items--; //11
		ar >> display_mode;
		nb_int_items--; //12
		if (nb_int_items > 0) ar >> spike_class;
		nb_int_items--;

		ar >> nb_int_items;
		i_column_width.SetSize(nb_int_items);
		for (auto ii = 0; ii < nb_int_items; ii++)
			ar >> i_column_width[ii];
		int n_float_items;
		ar >> n_float_items;
		ar >> t_first;
		n_float_items--;
		ar >> t_last;
		n_float_items--;
		ar >> mv_span;
		n_float_items--;

		ar >> n_scope_items;
		if (n_scope_items > 0)
		{
			view_data.Serialize(ar);
			n_scope_items--;
			view_spk_detect_data.Serialize(ar);
			n_scope_items--;
			view_spk_detect_spk.Serialize(ar);
			n_scope_items--;
			view_spk_detect_bars.Serialize(ar);
			n_scope_items--;
			view_ad_continuous.Serialize(ar);
			n_scope_items--;
			spk_view_data.Serialize(ar);
			n_scope_items--;
			spk_view_spk.Serialize(ar);
			n_scope_items--;
			spk_view_bars.Serialize(ar);
			n_scope_items--;
			spk_sort1_spk.Serialize(ar);
			n_scope_items--;
			spk_sort1_parameters.Serialize(ar);
			n_scope_items--;
			spk_sort1_hist.Serialize(ar);
			n_scope_items--;
			if (n_scope_items > 0)
			{
				db_view_data.Serialize(ar);
				n_scope_items--;
			}
			if (n_scope_items > 0)
			{
				db_view_bars.Serialize(ar);
				n_scope_items--;
			}
			if (n_scope_items > 0)
			{
				db_view_shape.Serialize(ar);
				n_scope_items--;
			}
			if (n_scope_items > 0)
			{
				spk_sort1_bars.Serialize(ar);
				n_scope_items--;
			}
			if (n_scope_items > 0)
			{
				view_spk_detect_filtered.Serialize(ar);
				n_scope_items--;
			}
			ASSERT(n_scope_items == 0);
		}
	}
	else if (version >= 2)
	{
		WORD ww1;
		ar >> ww1;
		int nb_int_items = ww1;
		if (nb_int_items > 0) ar >> hz_resolution;
		nb_int_items--;
		if (nb_int_items > 0)
		{
			ar >> vt_resolution;
			nb_int_items--;
			ar >> unit_mode;
			nb_int_items--;
		}
		if (nb_int_items > 0)
		{
			ar >> options_print_data->spike_height;
			nb_int_items--;
			ar >> options_print_data->spike_width;
			nb_int_items--;
		}
		if (nb_int_items > 0)
			ar >> n_filter_index;
		nb_int_items--;
		if (nb_int_items > 0)
		{
			for (auto ii = 0; ii < 8; ii++)
			{
				ar >> i_column_width[ii];
				nb_int_items--;
			}
		}
		if (nb_int_items > 0)
		{
			ar >> col0_width;
			nb_int_items--;
			ar >> row0_height;
			nb_int_items--;
			ar >> b_col0_visible;
			nb_int_items--;
			ar >> b_row0_visible;
			nb_int_items--;
		}

		ar >> n_scope_items;
		if (n_scope_items > 0)
		{
			view_data.Serialize(ar);
			n_scope_items--;
			view_spk_detect_data.Serialize(ar);
			n_scope_items--;
			view_spk_detect_spk.Serialize(ar);
			n_scope_items--;
			view_spk_detect_bars.Serialize(ar);
			n_scope_items--;
			view_ad_continuous.Serialize(ar);
			n_scope_items--;
			spk_view_data.Serialize(ar);
			n_scope_items--;
			spk_view_spk.Serialize(ar);
			n_scope_items--;
			spk_view_bars.Serialize(ar);
			n_scope_items--;
			spk_sort1_spk.Serialize(ar);
			n_scope_items--;
			spk_sort1_parameters.Serialize(ar);
			n_scope_items--;
			spk_sort1_hist.Serialize(ar);
			n_scope_items--;
			if (n_scope_items > 0)
			{
				db_view_data.Serialize(ar);
				n_scope_items--;
			}
			if (n_scope_items > 0)
			{
				db_view_bars.Serialize(ar);
				n_scope_items--;
			}
			if (n_scope_items > 0)
			{
				db_view_shape.Serialize(ar);
				n_scope_items--;
			}
			if (n_scope_items > 0)
			{
				spk_sort1_bars.Serialize(ar);
				n_scope_items--;
			}
			if (n_scope_items > 0)
			{
				view_spk_detect_filtered.Serialize(ar);
				n_scope_items--;
			}
			ASSERT(n_scope_items == 0);
		}
	}
}

