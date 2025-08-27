#pragma once

enum class DisplayMode
{
	None = 0,
	Data = 1,
	Spikes = 2
};

enum class SpikePlotMode
{
	AllClasses = 0,
	OneClass = 1
};

enum class DataTransform
{
	None = 0,
	MedianFilter = 13
};

struct DisplaySettings
{
	int image_width { 400 };
	int image_height { 50 };
	DataTransform data_transform { DataTransform::None };
	DisplayMode display_mode { DisplayMode::Data };
	SpikePlotMode spike_plot_mode { SpikePlotMode::AllClasses };
	int selected_class { 0 };
	float t_first { 0.f };
	float t_last { 0.f };
	float mv_span { 0.f };
	boolean b_set_time_span { FALSE };
	boolean b_set_mv_span { FALSE };
	boolean b_display_file_name { FALSE };
};


