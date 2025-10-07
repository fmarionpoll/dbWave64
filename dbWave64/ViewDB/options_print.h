#pragma once


class options_print final : public CObject
{
	DECLARE_SERIAL(options_print)
	options_print();
	~options_print() override;
	options_print& operator =(const options_print& arg);
	void Serialize(CArchive& ar) override;
	boolean b_changed{ false }; // flag set TRUE if contents has changed
	WORD m_w_version{ 1 }; // version number

	// print options
	boolean b_acq_comment{ true }; // print data acquisition comments
	boolean b_acq_date_time{ true }; // data acquisition date and time
	boolean b_channel_comment{ true }; // individual data acq channel description
	boolean b_channel_settings{ true }; // amplifier's settings (CyberAmp conventions)
	boolean b_doc_name{ true }; // file name
	boolean b_frame_rect{ false }; // draw a rectangle frame around the data
	boolean b_clip_rect{ true }; // clip data to the rectangle
	boolean b_timescale_bar{ true }; // display timescale bar
	boolean b_voltage_scale_bar{ true }; // display voltage scale bar
	boolean b_print_selection{ true }; // print only selection or whole series
	boolean b_print_spk_bars{ true }; // for spike view, print bars or not
	boolean b_filter_data_source{ true }; // display source data filtered
	boolean b_contours{ true };
	boolean b_center_line{ true };

	// print parameters
	int horizontal_resolution{ 1 }; // horizontal printer resolution
	int vertical_resolution{ 1 }; // vertical printer resolution
	int height_doc{ 300 }; // height of one document
	int width_doc{ 1500 }; // width of one document
	int left_page_margin{ 100 }; // page left margin (pixels)
	int bottom_page_margin{ 40 }; // bottom
	int right_page_margin{ 40 }; // right
	int top_page_margin{ 100 }; // top
	int height_separator{ 40 }; // vertical space between consecutive displays
	int text_separator{ 40 }; // space between left margin of display and text
	int font_size{ 40 }; // font size (pixels)
	int spike_height{ 0 }; //
	int spike_width{ 0 }; //
	int line_height{ 0 };
	int export_resolution_scale{ 4 }; // EMF export: multiply logical horizontal resolution
};
