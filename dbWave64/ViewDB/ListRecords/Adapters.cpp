#include "stdafx.h"
#include "Adapters.h"

// Define the static array and its count here (single definition).
const COLORREF ChartDataRendererAdapter::color_flag[] =
{
    col_white,
    col_light_grey,
    col_yellow,  
    RGB(15, 181, 174),  // 1
    RGB(197, 198, 198), // 4
    RGB(114, 224, 106), // 7
    RGB(222, 61, 130),  // 5
    RGB(20, 122, 243),  // 8
    RGB(64, 70, 202),   // 2
    RGB(115, 38, 211),  // 9
};

const int ChartDataRendererAdapter::color_flag_count = std::size(color_flag);

void ChartDataRendererAdapter::renderBitmap(const DisplaySettings& settings, const RowMeta& meta, CBitmap& out_bitmap) 
{
	CWindowDC screen_dc(nullptr);
	CDC mem_dc;
	VERIFY(mem_dc.CreateCompatibleDC(&screen_dc));
	out_bitmap.CreateBitmap(settings.image_width, settings.image_height,
		screen_dc.GetDeviceCaps(PLANES), screen_dc.GetDeviceCaps(BITSPIXEL), nullptr);
	mem_dc.SelectObject(&out_bitmap);
	mem_dc.FillSolidRect(0, 0, settings.image_width, settings.image_height, RGB(255, 255, 255));

	ChartData chart;
	chart.set_b_use_dib(FALSE);
	chart.set_display_area_size(settings.image_width, settings.image_height);

	AcqDataDoc data_doc;
	CString dat_path = meta.cs_datafile_name;
	if (dat_path.IsEmpty() || !data_doc.open_document(dat_path))
	{
		const CString comment = _T("File name: ") + meta.cs_datafile_name + _T(" -- data not available");
		chart.set_string(comment);
		chart.plot_data_to_dc(&mem_dc);
		return;
	}

	chart.attach_data_file(&data_doc);
	chart.load_all_channels(static_cast<int>(settings.data_transform));
	chart.load_data_within_window(settings.b_set_time_span, settings.t_first, settings.t_last);
	chart.adjust_gain(settings.b_set_mv_span, settings.mv_span);
	chart.set_bottom_comment(settings.b_display_file_name, meta.cs_datafile_name);
	const CString spike_path = meta.cs_spike_file_name;

	const COLORREF frame_color = spike_path.IsEmpty() ? col_blue : col_black;
	//bk_color = meta.cs_flag == "0"? bk_color: col_light_salmon;
	chart.get_scope_parameters()->cr_scope_frame = frame_color;

	const int flag_int = _ttoi(meta.cs_flag);
	const COLORREF color = color_flag[flag_int];
	chart.get_scope_parameters()->cr_scope_fill = color;

	chart.plot_data_to_dc(&mem_dc);
	data_doc.acq_close_file();
}

void ChartSpikeRendererAdapter::renderBitmap(const DisplaySettings& settings, const RowMeta& meta, CBitmap& out_bitmap) 
{
	CWindowDC screen_dc(nullptr);
	CDC mem_dc;
	VERIFY(mem_dc.CreateCompatibleDC(&screen_dc));
	out_bitmap.CreateBitmap(settings.image_width, settings.image_height,
		screen_dc.GetDeviceCaps(PLANES), screen_dc.GetDeviceCaps(BITSPIXEL), nullptr);
	mem_dc.SelectObject(&out_bitmap);
	mem_dc.FillSolidRect(0, 0, settings.image_width, settings.image_height, col_light_grey);

	CSpikeDoc spike_doc;
	if (meta.cs_spike_file_name.IsEmpty() || !spike_doc.OnOpenDocument(meta.cs_spike_file_name))
	{
		return;
	}

	ChartSpikeBar chart;
	chart.set_b_use_dib(FALSE);
	chart.set_display_area_size(settings.image_width, settings.image_height);

	SpikeList* p_spk_list = spike_doc.set_index_current_spike_list(0);
	chart.set_source_data(p_spk_list, db_doc_);
	chart.set_spike_doc(&spike_doc);

	chart.set_plot_mode(static_cast<int>(settings.spike_plot_mode == SpikePlotMode::AllClasses ? PLOT_BLACK : PLOT_ONE_CLASS_ONLY), settings.selected_class);

	long l_first = 0;
	long l_last = spike_doc.get_acq_size();
	if (settings.b_set_time_span)
	{
		const auto sampling_rate = spike_doc.get_acq_rate();
		l_first = static_cast<long>(settings.t_first * sampling_rate);
		l_last = static_cast<long>(settings.t_last * sampling_rate);
	}
	chart.set_time_intervals(l_first, l_last);

	if (settings.b_set_mv_span)
	{
		const auto volts_per_bin = p_spk_list->get_acq_volts_per_bin();
		const auto y_we = static_cast<int>(settings.mv_span / 1000.f / volts_per_bin);
		const auto y_wo = p_spk_list->get_acq_bin_zero();
		chart.set_yw_ext_org(y_we, y_wo);
	}

	chart.set_bottom_comment(settings.b_display_file_name, meta.cs_spike_file_name);
	chart.plot_data_to_dc(&mem_dc);
}