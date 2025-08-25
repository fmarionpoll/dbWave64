#include "StdAfx.h"
#include "DataListCtrl_Row.h"

#include "ColorNames.h"
#include "ViewdbWave.h"
#include "CGraphImageList.h"

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
	// Clean up persistent objects (no longer used with CGraphImageList approach)
	delete p_chart_data_wnd;
	delete p_chart_spike_wnd;
	
	SAFE_DELETE(p_data_doc)
	SAFE_DELETE(p_spike_doc)
	
	// Clear string members
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
	CBitmap* pBitmap = nullptr;
	
	switch (infos->display_mode)
	{
	case 1:
		// Generate data image using CGraphImageList
		pBitmap = CGraphImageList::GenerateDataImage(
			infos->image_width, infos->image_height, 
			cs_datafile_name, *infos);
		break;
	case 2:
		// Generate spike image using CGraphImageList
		pBitmap = CGraphImageList::GenerateSpikeImage(
			infos->image_width, infos->image_height, 
			cs_spike_file_name, *infos);
		break;
	default:
		// Generate empty image using CGraphImageList
		pBitmap = CGraphImageList::GenerateEmptyImage(
			infos->image_width, infos->image_height);
		break;
	}
	
	if (pBitmap)
	{
		infos->image_list.Replace(i_image, pBitmap, nullptr);
		delete pBitmap; // Clean up the bitmap
	}
	else
	{
		// Fallback to empty bitmap if generation fails
		infos->image_list.Replace(i_image, infos->p_empty_bitmap, nullptr);
	}
}

void DataListCtrl_Row::display_data_wnd(DataListCtrlInfos* infos, const int i_image)
{
	// Create data window and data document if necessary
	if (p_chart_data_wnd == nullptr)
	{
		p_chart_data_wnd = new ChartData;
		ASSERT(p_chart_data_wnd != NULL);
		
		if (!p_chart_data_wnd->Create(_T("DATAWND"), WS_CHILD,
									CRect(0, 0, infos->image_width, infos->image_height),
									infos->parent, i_image * 100))
		{
			TRACE(_T("DataListCtrl_Row::display_data_wnd - ChartData window creation failed\n"));
			delete p_chart_data_wnd;
			p_chart_data_wnd = nullptr;
			return;
		}
		
		p_chart_data_wnd->set_b_use_dib(FALSE);
	}

	// Create data document if necessary
	if (p_data_doc == nullptr)
	{
		p_data_doc = new AcqDataDoc;
		ASSERT(p_data_doc != NULL);
	}

	if (cs_datafile_name.IsEmpty() || !p_data_doc->open_document(cs_datafile_name))
	{
		infos->image_list.Replace(i_image, infos->p_empty_bitmap, nullptr);
	}
	else
	{
		p_data_doc->read_data_infos();
		p_chart_data_wnd->attach_data_file(p_data_doc);
		p_chart_data_wnd->load_all_channels(infos->data_transform);
		p_chart_data_wnd->load_data_within_window(infos->b_set_time_span, infos->t_first, infos->t_last);
		p_chart_data_wnd->adjust_gain(infos->b_set_mv_span, infos->mv_span);
		
		p_data_doc->acq_close_file();
		plot_data(infos, i_image);
	}
}

void DataListCtrl_Row::display_spike_wnd(DataListCtrlInfos* infos, const int i_image)
{
	// Create spike window and spike document if necessary
	if (p_chart_spike_wnd == nullptr)
	{
		p_chart_spike_wnd = new ChartSpikeBar;
		ASSERT(p_chart_spike_wnd != NULL);
		
		if (!p_chart_spike_wnd->Create(_T("SPKWND"), WS_CHILD,
									CRect(0, 0, infos->image_width, infos->image_height),
									infos->parent, index * 1000))
		{
			TRACE(_T("DataListCtrl_Row::display_spike_wnd - ChartSpikeBar window creation failed\n"));
			delete p_chart_spike_wnd;
			p_chart_spike_wnd = nullptr;
			return;
		}
		
		p_chart_spike_wnd->set_b_use_dib(FALSE);
		p_chart_spike_wnd->set_display_all_files(false);
	}

	// Create spike document if necessary
	if (p_spike_doc == nullptr)
	{
		p_spike_doc = new CSpikeDoc;
		ASSERT(p_spike_doc != NULL);
	}

	if (cs_spike_file_name.IsEmpty() || !p_spike_doc->OnOpenDocument(cs_spike_file_name))
	{
		infos->image_list.Replace(i_image, infos->p_empty_bitmap, nullptr);
	}
	else
	{
		// Set up the spike chart with the persistent document
		p_chart_spike_wnd->set_spike_doc(p_spike_doc);
		p_chart_spike_wnd->set_plot_mode(infos->spike_plot_mode, infos->selected_class);

		// Set time intervals if needed
		if (infos->b_set_time_span)
		{
			long l_first = static_cast<long>(infos->t_first * p_spike_doc->get_acq_rate());
			long l_last = static_cast<long>(infos->t_last * p_spike_doc->get_acq_rate());
			p_chart_spike_wnd->set_time_intervals(l_first, l_last);
		}

		plot_spikes(infos, i_image);
	}
}

void DataListCtrl_Row::display_empty_wnd(DataListCtrlInfos* infos, const int i_image)
{
	infos->image_list.Replace(i_image, infos->p_empty_bitmap, nullptr);
}

void DataListCtrl_Row::plot_data(DataListCtrlInfos* infos, const int i_image) const
{
	if (!p_chart_data_wnd || !p_chart_data_wnd->GetSafeHwnd())
	{
		// Window is not valid, use empty bitmap
		infos->image_list.Replace(i_image, infos->p_empty_bitmap, nullptr);
		return;
	}

	p_chart_data_wnd->set_bottom_comment(infos->b_display_file_name, cs_datafile_name);

	CRect client_rect;
	p_chart_data_wnd->GetClientRect(&client_rect);
	client_rect.DeflateRect(2, 2);
	client_rect.OffsetRect(1, 1);

	CDC* p_dc = p_chart_data_wnd->GetDC();
	if (!p_dc)
	{
		infos->image_list.Replace(i_image, infos->p_empty_bitmap, nullptr);
		return;
	}

	CDC mem_dc;
	VERIFY(mem_dc.CreateCompatibleDC(p_dc));

	CBitmap bitmap_plot;
	bitmap_plot.CreateBitmap(client_rect.right, client_rect.bottom, p_dc->GetDeviceCaps(PLANES),
		p_dc->GetDeviceCaps(BITSPIXEL), nullptr);

	mem_dc.SelectObject(&bitmap_plot);
	mem_dc.SetMapMode(p_dc->GetMapMode());

	p_chart_data_wnd->plot_data_to_dc(&mem_dc);

	infos->image_list.Replace(i_image, &bitmap_plot, nullptr);
}

void DataListCtrl_Row::plot_spikes(DataListCtrlInfos* infos, const int i_image) const
{
	if (!p_chart_spike_wnd || !p_chart_spike_wnd->GetSafeHwnd())
	{
		// Window is not valid, use empty bitmap
		infos->image_list.Replace(i_image, infos->p_empty_bitmap, nullptr);
		return;
	}

	p_chart_spike_wnd->set_bottom_comment(infos->b_display_file_name, cs_spike_file_name);

	CRect client_rect;
	p_chart_spike_wnd->GetClientRect(&client_rect);
	client_rect.DeflateRect(2, 2);
	client_rect.OffsetRect(1, 1);

	CDC* p_dc = p_chart_spike_wnd->GetDC();
	if (!p_dc)
	{
		infos->image_list.Replace(i_image, infos->p_empty_bitmap, nullptr);
		return;
	}

	CDC mem_dc;
	VERIFY(mem_dc.CreateCompatibleDC(p_dc));

	CBitmap bitmap_plot;
	bitmap_plot.CreateBitmap(client_rect.right,
		client_rect.bottom,
		p_dc->GetDeviceCaps(PLANES),
		p_dc->GetDeviceCaps(BITSPIXEL),
		nullptr);
	mem_dc.SelectObject(&bitmap_plot);
	mem_dc.SetMapMode(p_dc->GetMapMode());

	p_chart_spike_wnd->plot_data_to_dc(&mem_dc);

	infos->image_list.Replace(i_image, &bitmap_plot, nullptr);
}






