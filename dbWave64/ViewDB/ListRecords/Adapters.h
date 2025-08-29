#pragma once

#include "DbWaveDataProvider.h"
#include "DisplaySettings.h"
#include "Renderers.h"
#include "dbWaveDoc.h"
#include "Chart/ChartData.h"
#include "Chart/ChartSpikeBar.h"


class DbWaveDocProviderAdapter : public IDbWaveDataProvider
{
public:
	explicit DbWaveDocProviderAdapter(CdbWaveDoc* doc) : doc_(doc) {}

	int get_records_count() const override
	{
		return doc_ ? doc_->db_get_records_count() : 0;
	}

	RowMeta get_row_meta(const int index) override
	{
		RowMeta m;
		if (doc_ == nullptr)
			return m;
		const int saved_index = doc_->db_get_current_record_position();
		if (!doc_->db_set_current_record_position(index))
			return m;

		m.index = index;
		m.cs_datafile_name = doc_->db_get_current_dat_file_name(TRUE);
		m.cs_spike_file_name = doc_->db_get_current_spk_file_name(TRUE);

		const auto database = doc_->db_table;
		DB_ITEMDESC desc;
		database->get_record_item_value(CH_ID, &desc);
		m.record_id = desc.l_val;
		database->get_record_item_value(CH_IDINSECT, &desc);
		m.insect_id = desc.l_val;
		database->get_record_item_value(CH_STIM1_KEY, &desc);
		m.cs_stimulus1 = desc.cs_val;
		database->get_record_item_value(CH_CONC1_KEY, &desc);
		m.cs_concentration1 = desc.cs_val;
		database->get_record_item_value(CH_STIM2_KEY, &desc);
		m.cs_stimulus2 = desc.cs_val;
		database->get_record_item_value(CH_CONC2_KEY, &desc);
		m.cs_concentration2 = desc.cs_val;
		database->get_record_item_value(CH_SENSILLUM_KEY, &desc);
		m.cs_sensillum_name = desc.cs_val;
		database->get_record_item_value(CH_FLAG, &desc);
		m.cs_flag.Format(_T("%i"), desc.l_val);

		if (m.cs_spike_file_name.IsEmpty())
			m.cs_n_spikes.Empty();
		else
		{
			database->get_record_item_value(CH_NSPIKES, &desc);
			const int n_spikes = desc.l_val;
			database->get_record_item_value(CH_NSPIKECLASSES, &desc);
			m.cs_n_spikes.Format(_T("%i (%i classes)"), n_spikes, desc.l_val);
		}
		if (saved_index >= 0)
			doc_->db_set_current_record_position(saved_index);
		return m;
	}

private:
	CdbWaveDoc* doc_ { nullptr };
};

class ChartDataRendererAdapter : public IDataRenderer
{
public:
	void renderBitmap(const DisplaySettings& settings, const RowMeta& meta, CBitmap& out_bitmap) override
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
		chart.plot_data_to_dc(&mem_dc);
		data_doc.acq_close_file();
	}
};

class ChartSpikeRendererAdapter : public ISpikeRenderer
{
public:
	explicit ChartSpikeRendererAdapter(CdbWaveDoc* db_doc) : db_doc_(db_doc) {}
	void renderBitmap(const DisplaySettings& settings, const RowMeta& meta, CBitmap& out_bitmap) override
	{
		CWindowDC screenDC(nullptr);
		CDC mem_dc;
		VERIFY(mem_dc.CreateCompatibleDC(&screenDC));
		out_bitmap.CreateBitmap(settings.image_width, settings.image_height,
			screenDC.GetDeviceCaps(PLANES), screenDC.GetDeviceCaps(BITSPIXEL), nullptr);
		mem_dc.SelectObject(&out_bitmap);
		mem_dc.FillSolidRect(0, 0, settings.image_width, settings.image_height, RGB(235, 235, 235));

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
private:
	CdbWaveDoc* db_doc_ { nullptr };
};




