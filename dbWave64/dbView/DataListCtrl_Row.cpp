#include "StdAfx.h"
#include "DataListCtrl_Row.h"
#include "DataListCtrl.h"
#include "CGraphImageList.h"

#include "ColorNames.h"
#include "ViewdbWave.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


IMPLEMENT_SERIAL(DataListCtrl_Row, CObject, 0)

DataListCtrl_Row::DataListCtrl_Row()
= default;

DataListCtrl_Row::DataListCtrl_Row(const int i)
{
	index = i;
}

DataListCtrl_Row::~DataListCtrl_Row()
{
	delete p_chart_data_wnd;
	delete p_chart_spike_wnd;
	
	SAFE_DELETE(p_data_doc)
	SAFE_DELETE(p_spike_doc)
	cs_comment.Empty();
	cs_datafile_name.Empty();
	cs_spike_file_name.Empty();
	cs_sensillum_name.Empty();
	cs_stimulus1.Empty();
	cs_concentration1.Empty();
	cs_stimulus2.Empty();
	cs_concentration2.Empty();
	cs_n_spikes.Empty();
	cs_flag.Empty();
	cs_date.Empty();
}

DataListCtrl_Row& DataListCtrl_Row::operator =(const DataListCtrl_Row& arg)
{
	if (this != &arg)
	{
		b_init = arg.b_init;
		index = arg.index;
		insect_id = arg.insect_id;
		cs_comment = arg.cs_comment;
		cs_datafile_name = arg.cs_datafile_name;
		cs_spike_file_name = arg.cs_spike_file_name;
		cs_sensillum_name = arg.cs_sensillum_name;
		cs_stimulus1 = arg.cs_stimulus1;
		cs_concentration1 = arg.cs_concentration1;
		cs_stimulus2 = arg.cs_stimulus2;
		cs_concentration2 = arg.cs_concentration2;
		cs_n_spikes = arg.cs_n_spikes;
		cs_flag = arg.cs_flag;
		cs_date = arg.cs_date;
		p_chart_data_wnd = arg.p_chart_data_wnd;
		p_chart_spike_wnd = arg.p_chart_spike_wnd;
		p_data_doc = arg.p_data_doc;
		p_spike_doc = arg.p_spike_doc;
	}
	return *this;
}

void DataListCtrl_Row::Serialize(CArchive& ar)
{
	// not serialized:
	// AcqDataDoc*	p_data_Doc;
	// CSpikeDoc*	p_spike_Doc;

	if (ar.IsStoring())
	{
		w_version = 2;
		ar << w_version;
		ar << index;

		constexpr auto string_count = 8;
		ar << string_count;
		ar << cs_comment;
		ar << cs_datafile_name;
		ar << cs_sensillum_name;
		ar << cs_stimulus1;
		ar << cs_concentration1;
		ar << cs_stimulus2;
		ar << cs_concentration2;
		ar << cs_n_spikes;
		ar << cs_flag;
		ar << cs_date;

		constexpr auto object_count = 3;
		ar << object_count;
		p_chart_data_wnd->Serialize(ar);
		p_chart_spike_wnd->Serialize(ar);
		ar << insect_id;
	}
	else
	{
		ar >> w_version;
		ar >> index;

		int string_count;
		ar >> string_count;
		ar >> cs_comment;
		string_count--;
		ar >> cs_datafile_name;
		string_count--;
		ar >> cs_sensillum_name;
		string_count--;
		ar >> cs_stimulus1;
		string_count--;
		ar >> cs_concentration1;
		string_count--;
		if (w_version > 1)
		{
			ar >> cs_stimulus2;
			string_count--;
			ar >> cs_concentration2;
			string_count--;
		}
		ar >> cs_n_spikes;
		string_count--;
		ar >> cs_flag;
		string_count--;
		ar >> cs_date;
		string_count--;

		int object_count;
		ar >> object_count;
		ASSERT(object_count >= 2);
		p_chart_data_wnd->Serialize(ar);
		object_count--;
		p_chart_spike_wnd->Serialize(ar);;
		object_count--;
		if (object_count > 0) 
			ar >> insect_id;
	}
}

void DataListCtrl_Row::attach_database_record(CdbWaveDoc* db_wave_doc)
{
	if (db_wave_doc->db_set_current_record_position(index))
	{
		db_wave_doc->open_current_data_file();
		db_wave_doc->open_current_spike_file();
	}
	cs_datafile_name = db_wave_doc->db_get_current_dat_file_name(TRUE);
	cs_spike_file_name = db_wave_doc->db_get_current_spk_file_name(TRUE);
	const auto database = db_wave_doc->db_table;

	DB_ITEMDESC desc;
	database->get_record_item_value(CH_ID, &desc);
	record_id = desc.l_val;
	database->get_record_item_value(CH_IDINSECT, &desc);
	insect_id = desc.l_val;
	database->get_record_item_value(CH_STIM1_KEY, &desc);
	cs_stimulus1 = desc.cs_val;
	database->get_record_item_value(CH_CONC1_KEY, &desc);
	cs_concentration1 = desc.cs_val;
	database->get_record_item_value(CH_STIM2_KEY, &desc);
	cs_stimulus2 = desc.cs_val;
	database->get_record_item_value(CH_CONC2_KEY, &desc);
	cs_concentration2 = desc.cs_val;
	database->get_record_item_value(CH_SENSILLUM_KEY, &desc);
	cs_sensillum_name = desc.cs_val;
	database->get_record_item_value(CH_FLAG, &desc);
	cs_flag.Format(_T("%i"), desc.l_val);

	// column: number of spikes = verify that spike file is defined, if yes, load nb spikes
	if (db_wave_doc->db_get_current_spk_file_name(TRUE).IsEmpty())
		cs_n_spikes.Empty();
	else
	{
		database->get_record_item_value(CH_NSPIKES, &desc);
		const int n_spikes = desc.l_val;
		database->get_record_item_value(CH_NSPIKECLASSES, &desc);
		cs_n_spikes.Format(_T("%i (%i classes)"), n_spikes, desc.l_val);
	}
}

void DataListCtrl_Row::set_display_parameters(DataListCtrlInfos* infos, const int i_image)
{
	// Use CGraphImageList for character set isolated image generation
	switch (infos->display_mode)
	{
	case 1: // Data
		{
			CBitmap* pBitmap = CGraphImageList::GenerateDataImage(
				infos->image_width, infos->image_height,
				cs_datafile_name, *infos);
			infos->image_list.Replace(i_image, pBitmap, nullptr);
			delete pBitmap;
		}
		break;
	case 2: // Spike
		{
			CBitmap* pBitmap = CGraphImageList::GenerateSpikeImage(
				infos->image_width, infos->image_height,
				cs_spike_file_name, *infos);
			infos->image_list.Replace(i_image, pBitmap, nullptr);
			delete pBitmap;
		}
		break;
	default: // Empty
		{
			CBitmap* pBitmap = CGraphImageList::GenerateEmptyImage(
				infos->image_width, infos->image_height);
			infos->image_list.Replace(i_image, pBitmap, nullptr);
			delete pBitmap;
		}
		break;
	}
}

// Old display_data_wnd method removed - replaced by CGraphImageList approach

// Old plot_data method removed - replaced by CGraphImageList approach

// Old display_spike_wnd method removed - replaced by CGraphImageList approach

// Old plot_spikes method removed - replaced by CGraphImageList approach

// Old display_empty_wnd method removed - replaced by CGraphImageList approach
