#pragma once

#include "DbWaveDataProvider.h"
#include "DisplaySettings.h"
#include "Renderers.h"
#include "dbWaveDoc.h"
#include "ChartData.h"
#include "ChartSpikeBar.h"

class DbWaveDocProviderAdapter : public IDbWaveDataProvider
{
public:
	explicit DbWaveDocProviderAdapter(CdbWaveDoc* doc) : doc_(doc) {}

	int getRecordsCount() const override
	{
		return doc_ ? doc_->db_get_records_count() : 0;
	}

	RowMeta getRowMeta(const int index) override
	{
		RowMeta m;
		if (doc_ == nullptr)
			return m;
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
		return m;
	}

private:
	CdbWaveDoc* doc_ { nullptr };
};

class ChartDataRendererAdapter : public IDataRenderer
{
public:
	CBitmap createBitmap(const DisplaySettings& settings, const RowMeta& meta) override
	{
		CBitmap bitmap_plot;
		CWindowDC screenDC(nullptr);
		CDC mem_dc;
		VERIFY(mem_dc.CreateCompatibleDC(&screenDC));
		bitmap_plot.CreateBitmap(settings.image_width, settings.image_height,
			screenDC.GetDeviceCaps(PLANES), screenDC.GetDeviceCaps(BITSPIXEL), nullptr);
		mem_dc.SelectObject(&bitmap_plot);
		mem_dc.FillSolidRect(0, 0, settings.image_width, settings.image_height, RGB(255, 255, 255));
		mem_dc.TextOut(2, 2, meta.cs_datafile_name);
		return bitmap_plot;
	}
};

class ChartSpikeRendererAdapter : public ISpikeRenderer
{
public:
	CBitmap createBitmap(const DisplaySettings& settings, const RowMeta& meta) override
	{
		CBitmap bitmap_plot;
		CWindowDC screenDC(nullptr);
		CDC mem_dc;
		VERIFY(mem_dc.CreateCompatibleDC(&screenDC));
		bitmap_plot.CreateBitmap(settings.image_width, settings.image_height,
			screenDC.GetDeviceCaps(PLANES), screenDC.GetDeviceCaps(BITSPIXEL), nullptr);
		mem_dc.SelectObject(&bitmap_plot);
		mem_dc.FillSolidRect(0, 0, settings.image_width, settings.image_height, RGB(235, 235, 235));
		mem_dc.TextOut(2, 2, meta.cs_spike_file_name);
		return bitmap_plot;
	}
};


