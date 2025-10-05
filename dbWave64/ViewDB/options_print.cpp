#include "StdAfx.h"
#include "options_print.h"


IMPLEMENT_SERIAL(options_print, CObject, 0 /* schema number*/)

options_print::options_print()
= default;

options_print::~options_print()
= default;

options_print& options_print::operator =(const options_print& arg)
{
	if (this != &arg)
	{
		horizontal_resolution = arg.horizontal_resolution;
		vertical_resolution = arg.vertical_resolution;
		b_acq_comment = arg.b_acq_comment;
		b_acq_date_time = arg.b_acq_date_time;
		b_channel_comment = arg.b_channel_comment;
		b_channel_settings = arg.b_channel_settings;
		b_doc_name = arg.b_doc_name;
		b_frame_rect = arg.b_frame_rect;
		b_clip_rect = arg.b_clip_rect;
		b_timescale_bar = arg.b_timescale_bar;
		b_voltage_scale_bar = arg.b_voltage_scale_bar;
		height_doc = arg.height_doc;
		width_doc = arg.width_doc;
		left_page_margin = arg.left_page_margin;
		bottom_page_margin = arg.bottom_page_margin;
		right_page_margin = arg.right_page_margin;
		top_page_margin = arg.top_page_margin;
		height_separator = arg.height_separator;
		text_separator = arg.text_separator;
		font_size = arg.font_size;
		b_filter_data_source = arg.b_filter_data_source;
		b_print_selection = arg.b_print_selection;
		b_print_spk_bars = arg.b_print_spk_bars;
		spike_height = arg.spike_height;
		spike_width = arg.spike_width;
		b_contours = arg.b_contours;
		b_center_line = arg.b_center_line;
	}
	return *this;
}

void options_print::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		ar << m_w_version;

		// print options
		WORD w_2_power = 1;
		WORD w_print_flags = b_acq_comment * w_2_power; //0
		w_2_power += w_2_power;
		w_print_flags += b_acq_date_time * w_2_power; //2
		w_2_power += w_2_power;
		w_print_flags += b_channel_comment * w_2_power; //4
		w_2_power += w_2_power;
		w_print_flags += b_channel_settings * w_2_power; //8
		w_2_power += w_2_power;
		w_print_flags += b_doc_name * w_2_power; //16
		w_2_power += w_2_power;
		w_print_flags += b_frame_rect * w_2_power; //32
		w_2_power += w_2_power;
		w_print_flags += b_clip_rect * w_2_power; //64
		w_2_power += w_2_power;
		w_print_flags += b_timescale_bar * w_2_power; //128
		w_2_power += w_2_power;
		w_print_flags += b_voltage_scale_bar * w_2_power; //256
		w_2_power += w_2_power;
		w_print_flags += b_print_selection * w_2_power; //512
		w_2_power += w_2_power;
		w_print_flags += b_print_spk_bars * w_2_power; //1024
		w_2_power += w_2_power;
		w_print_flags += b_filter_data_source * w_2_power; //2048
		w_2_power += w_2_power;
		w_print_flags += b_contours * w_2_power; //4096
		w_2_power += w_2_power;
		w_print_flags += b_center_line * w_2_power; //8192
		ar << w_print_flags;

		// print parameters
		ar << static_cast<WORD>(horizontal_resolution);
		ar << static_cast<WORD>(vertical_resolution);
		ar << static_cast<WORD>(height_doc); // height of one document
		ar << static_cast<WORD>(width_doc); // width of one document
		ar << static_cast<WORD>(left_page_margin); // page left margin (pixels)
		ar << static_cast<WORD>(bottom_page_margin); // bottom
		ar << static_cast<WORD>(right_page_margin); // right
		ar << static_cast<WORD>(top_page_margin); // top
		ar << static_cast<WORD>(height_separator); // vertical space between consecutive displays
		ar << static_cast<WORD>(text_separator); // space between left margin of display and text
		ar << static_cast<WORD>(font_size); // font size (pixels)

		// export int(s) WORD = nb of items
		ar << static_cast<WORD>(2);
		ar << spike_height; //4
		ar << spike_width; //5

	}
	else
	{
		WORD version;
		ar >> version;

		// print options
		WORD w_print_flags;
		ar >> w_print_flags;
		WORD w_mult = 1;
		b_acq_comment = ((w_print_flags & w_mult) > 0);
		w_mult += w_mult;
		b_acq_date_time = ((w_print_flags & w_mult) > 0);
		w_mult += w_mult;
		b_channel_comment = ((w_print_flags & w_mult) > 0);
		w_mult += w_mult;
		b_channel_settings = ((w_print_flags & w_mult) > 0);
		w_mult += w_mult;
		b_doc_name = ((w_print_flags & w_mult) > 0);
		w_mult += w_mult;
		b_frame_rect = ((w_print_flags & w_mult) > 0);
		w_mult += w_mult;
		b_clip_rect = ((w_print_flags & w_mult) > 0);
		w_mult += w_mult;
		b_timescale_bar = ((w_print_flags & w_mult) > 0);
		w_mult += w_mult;
		b_voltage_scale_bar = ((w_print_flags & w_mult) > 0);
		w_mult += w_mult;
		b_print_selection = ((w_print_flags & w_mult) > 0);
		w_mult += w_mult;
		b_print_spk_bars = ((w_print_flags & w_mult) > 0);
		w_mult += w_mult;
		b_filter_data_source = ((w_print_flags & w_mult) > 0);
		w_mult += w_mult;
		b_contours = ((w_print_flags & w_mult) > 0);
		w_mult += w_mult;
		b_center_line = ((w_print_flags & w_mult) > 0);

		// print parameters
		WORD w1;
		ar >> w1;
		horizontal_resolution = w1;
		ar >> w1;
		vertical_resolution = w1;
		ar >> w1;
		height_doc = w1;
		ar >> w1;
		width_doc = w1;
		ar >> w1;
		left_page_margin = w1;
		ar >> w1;
		bottom_page_margin = w1;
		ar >> w1;
		right_page_margin = w1;
		ar >> w1;
		top_page_margin = w1;
		ar >> w1;
		height_separator = w1;
		ar >> w1;
		text_separator = w1;
		ar >> w1;
		font_size = w1;

		WORD nb_int_items;
		ar >> nb_int_items;
		ar >> spike_height;
		nb_int_items--;
		ar >> spike_width;
		nb_int_items--;
	}
}
