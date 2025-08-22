#include "StdAfx.h"
#include "DataListCtrl_Row_Optimized.h"
#include "DataListCtrl_Configuration.h"
#include "DataListCtrl_SupportingClasses.h"
#include "DataListCtrl_Optimized_Infos.h"

#include "AcqDataDoc.h"
#include "ChartData.h"
#include "SpikeDoc.h"
#include "ChartSpikeBar.h"
#include "ViewdbWave_Optimized.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// Exception implementation - using definition from DataListCtrl_SupportingClasses.h

// GDI Resource Manager Implementation - using definition from DataListCtrl_SupportingClasses.h

// Cache Implementation - using definition from DataListCtrl_SupportingClasses.h

// Main DataListCtrl_Row_Optimized Implementation
IMPLEMENT_SERIAL(DataListCtrl_Row_Optimized, CObject, 0)

DataListCtrl_Row_Optimized::DataListCtrl_Row_Optimized()
    : m_cache_(std::make_unique<data_list_ctrl_cache>())
{
}

DataListCtrl_Row_Optimized::DataListCtrl_Row_Optimized(int index)
    : m_index_(index), m_cache_(std::make_unique<data_list_ctrl_cache>())
{
}

DataListCtrl_Row_Optimized::DataListCtrl_Row_Optimized(const DataListCtrl_Row_Optimized& other)
    : m_index_(other.m_index_), m_insect_id_(other.m_insect_id_), m_record_id_(other.m_record_id_),
      m_comment_(other.m_comment_), m_data_file_name_(other.m_data_file_name_), m_spike_file_name_(other.m_spike_file_name_),
      m_sensillum_name_(other.m_sensillum_name_), m_stimulus1_(other.m_stimulus1_), m_concentration1_(other.m_concentration1_),
      m_stimulus2_(other.m_stimulus2_), m_concentration2_(other.m_concentration2_), m_n_spikes_(other.m_n_spikes_),
      m_flag_(other.m_flag_), m_date_(other.m_date_), m_display_processed_(other.m_display_processed_),
      m_last_display_mode_(other.m_last_display_mode_), m_init_(other.m_init_), m_changed_(other.m_changed_),
      m_version_(other.m_version_), m_cache_enabled_(other.m_cache_enabled_), m_performance_metrics_(other.m_performance_metrics_)
{
    // Note: Smart pointers and cache are not copied, they need to be recreated
    // They will be default-constructed (nullptr)
}

DataListCtrl_Row_Optimized& DataListCtrl_Row_Optimized::operator=(const DataListCtrl_Row_Optimized& other)
{
    if (this != &other)
    {
        std::lock_guard<std::mutex> lock(m_mutex_);
        
        m_index_ = other.m_index_;
        m_insect_id_ = other.m_insect_id_;
        m_record_id_ = other.m_record_id_;
        m_comment_ = other.m_comment_;
        m_data_file_name_ = other.m_data_file_name_;
        m_spike_file_name_ = other.m_spike_file_name_;
        m_sensillum_name_ = other.m_sensillum_name_;
        m_stimulus1_ = other.m_stimulus1_;
        m_concentration1_ = other.m_concentration1_;
        m_stimulus2_ = other.m_stimulus2_;
        m_concentration2_ = other.m_concentration2_;
        m_n_spikes_ = other.m_n_spikes_;
        m_flag_ = other.m_flag_;
        m_date_ = other.m_date_;
        m_display_processed_ = other.m_display_processed_;
        m_last_display_mode_ = other.m_last_display_mode_;
        m_init_ = other.m_init_;
        m_changed_ = other.m_changed_;
        m_version_ = other.m_version_;
        m_cache_enabled_ = other.m_cache_enabled_;
        m_performance_metrics_ = other.m_performance_metrics_;
        
        // Note: Smart pointers are not copied, they need to be recreated
    }
    return *this;
}

DataListCtrl_Row_Optimized::DataListCtrl_Row_Optimized(DataListCtrl_Row_Optimized&& other) noexcept
    : m_index_(other.m_index_), m_insect_id_(other.m_insect_id_), m_record_id_(other.m_record_id_),
      m_comment_(std::move(other.m_comment_)), m_data_file_name_(std::move(other.m_data_file_name_)),
      m_spike_file_name_(std::move(other.m_spike_file_name_)), m_sensillum_name_(std::move(other.m_sensillum_name_)),
      m_stimulus1_(std::move(other.m_stimulus1_)), m_concentration1_(std::move(other.m_concentration1_)),
      m_stimulus2_(std::move(other.m_stimulus2_)), m_concentration2_(std::move(other.m_concentration2_)),
      m_n_spikes_(std::move(other.m_n_spikes_)), m_flag_(std::move(other.m_flag_)), m_date_(std::move(other.m_date_)),
      m_p_data_doc_(std::move(other.m_p_data_doc_)), m_p_chart_data_wnd_(std::move(other.m_p_chart_data_wnd_)),
      m_p_spike_doc_(std::move(other.m_p_spike_doc_)), m_p_chart_spike_wnd_(std::move(other.m_p_chart_spike_wnd_)),
      m_display_processed_(other.m_display_processed_), m_last_display_mode_(other.m_last_display_mode_),
      m_init_(other.m_init_), m_changed_(other.m_changed_), m_version_(other.m_version_),
      m_cache_enabled_(other.m_cache_enabled_), m_performance_metrics_(other.m_performance_metrics_),
      m_cache_(std::move(other.m_cache_))
{
    // Reset other object
    other.m_index_ = 0;
    other.m_insect_id_ = 0;
    other.m_record_id_ = 0;
    other.m_display_processed_ = false;
    other.m_last_display_mode_ = -1;
    other.m_init_ = false;
    other.m_changed_ = false;
    other.m_version_ = 0;
    other.m_cache_enabled_ = true;
    other.m_performance_metrics_ = PerformanceMetrics{};
}

DataListCtrl_Row_Optimized& DataListCtrl_Row_Optimized::operator=(DataListCtrl_Row_Optimized&& other) noexcept
{
    if (this != &other)
    {
        std::lock_guard<std::mutex> lock(m_mutex_);
        
        m_index_ = other.m_index_;
        m_insect_id_ = other.m_insect_id_;
        m_record_id_ = other.m_record_id_;
        m_comment_ = std::move(other.m_comment_);
        m_data_file_name_ = std::move(other.m_data_file_name_);
        m_spike_file_name_ = std::move(other.m_spike_file_name_);
        m_sensillum_name_ = std::move(other.m_sensillum_name_);
        m_stimulus1_ = std::move(other.m_stimulus1_);
        m_concentration1_ = std::move(other.m_concentration1_);
        m_stimulus2_ = std::move(other.m_stimulus2_);
        m_concentration2_ = std::move(other.m_concentration2_);
        m_n_spikes_ = std::move(other.m_n_spikes_);
        m_flag_ = std::move(other.m_flag_);
        m_date_ = std::move(other.m_date_);
        m_p_data_doc_ = std::move(other.m_p_data_doc_);
        m_p_chart_data_wnd_ = std::move(other.m_p_chart_data_wnd_);
        m_p_spike_doc_ = std::move(other.m_p_spike_doc_);
        m_p_chart_spike_wnd_ = std::move(other.m_p_chart_spike_wnd_);
        m_display_processed_ = other.m_display_processed_;
        m_last_display_mode_ = other.m_last_display_mode_;
        m_init_ = other.m_init_;
        m_changed_ = other.m_changed_;
        m_version_ = other.m_version_;
        m_cache_enabled_ = other.m_cache_enabled_;
        m_performance_metrics_ = other.m_performance_metrics_;
        m_cache_ = std::move(other.m_cache_);
        
        // Reset other object
        other.m_index_ = 0;
        other.m_insect_id_ = 0;
        other.m_record_id_ = 0;
        other.m_display_processed_ = false;
        other.m_last_display_mode_ = -1;
        other.m_init_ = false;
        other.m_changed_ = false;
        other.m_version_ = 0;
        other.m_cache_enabled_ = true;
        other.m_performance_metrics_ = PerformanceMetrics{};
    }
    return *this;
}

bool DataListCtrl_Row_Optimized::load_database_record(CdbWaveDoc* db_wave_doc, int index)
{
    if (!db_wave_doc)
    {
        handle_error(data_list_ctrl_error::INVALID_INDEX, _T("Invalid database document"));
        return false;
    }

    // save current record position and set record position to index
    const int current_record = db_wave_doc->OnNewDocument();
    if (!db_wave_doc->db_set_current_record_position(index))
    {
        return false;
    }

    set_index(index);
    try
    {
        if (db_wave_doc->db_set_current_record_position(index))
        {
            db_wave_doc->open_current_data_file();
            db_wave_doc->open_current_spike_file();
        }

        m_data_file_name_ = db_wave_doc->db_get_current_dat_file_name(TRUE);
        m_spike_file_name_ = db_wave_doc->db_get_current_spk_file_name(TRUE);

        const auto database = db_wave_doc->db_table;
        if (!database)
        {
            handle_error(data_list_ctrl_error::INVALID_DATABASE_TABLE, _T("Invalid database table"));
            return false;
        }

        DB_ITEMDESC desc;

        database->get_record_item_value(CH_ID, &desc);
        m_record_id_ = desc.l_val;
        database->get_record_item_value(CH_IDINSECT, &desc);
        m_insect_id_ = desc.l_val;
        database->get_record_item_value(CH_STIM1_KEY, &desc);
        m_stimulus1_ = desc.cs_val;
        database->get_record_item_value(CH_CONC1_KEY, &desc);
        m_concentration1_ = desc.cs_val;
        database->get_record_item_value(CH_STIM2_KEY, &desc);
        m_stimulus2_ = desc.cs_val;
        database->get_record_item_value(CH_CONC2_KEY, &desc);
        m_concentration2_ = desc.cs_val;
        database->get_record_item_value(CH_SENSILLUM_KEY, &desc);
        m_sensillum_name_ = desc.cs_val;
        database->get_record_item_value(CH_FLAG, &desc);
        m_flag_.Format(_T("%i"), desc.l_val);

        if (db_wave_doc->db_get_current_spk_file_name(TRUE).IsEmpty())
        {
            m_n_spikes_.Empty();
        }
        else
        {
            database->get_record_item_value(CH_NSPIKES, &desc);
            const int n_spikes = desc.l_val;
            database->get_record_item_value(CH_NSPIKECLASSES, &desc);
            m_n_spikes_.Format(_T("%i (%i classes)"), n_spikes, desc.l_val);
        }

        // restore current record position
        db_wave_doc->db_set_current_record_position(current_record);
        return true;
    }
    catch (const std::exception& e)
    {
        handle_error(data_list_ctrl_error::DATABABASE_LOAD_RECORD_FAILED, CString(e.what()));
        return false;
    }
}

void DataListCtrl_Row_Optimized::attach_database_record(CdbWaveDoc* db_wave_doc)
{
    if (!db_wave_doc)
    {
        handle_error(data_list_ctrl_error::INVALID_INDEX, _T("Invalid database document"));
        return;
    }
    
    try
    {
        if (db_wave_doc->db_set_current_record_position(m_index_))
        {
            db_wave_doc->open_current_data_file();
            db_wave_doc->open_current_spike_file();
        }
        
        m_data_file_name_ = db_wave_doc->db_get_current_dat_file_name(TRUE);
        m_spike_file_name_ = db_wave_doc->db_get_current_spk_file_name(TRUE);
        
        const auto database = db_wave_doc->db_table;
        if (!database)
        {
            handle_error(data_list_ctrl_error::INVALID_INDEX, _T("Invalid database table"));
            return;
        }
        
        DB_ITEMDESC desc;
        
        // Get record ID
        database->get_record_item_value(CH_ID, &desc);
        m_record_id_ = desc.l_val;
        
        // Get insect ID
        database->get_record_item_value(CH_IDINSECT, &desc);
        m_insect_id_ = desc.l_val;
        
        // Get stimulus and concentration data
        database->get_record_item_value(CH_STIM1_KEY, &desc);
        m_stimulus1_ = desc.cs_val;
        
        database->get_record_item_value(CH_CONC1_KEY, &desc);
        m_concentration1_ = desc.cs_val;
        
        database->get_record_item_value(CH_STIM2_KEY, &desc);
        m_stimulus2_ = desc.cs_val;
        
        database->get_record_item_value(CH_CONC2_KEY, &desc);
        m_concentration2_ = desc.cs_val;
        
        database->get_record_item_value(CH_SENSILLUM_KEY, &desc);
        m_sensillum_name_ = desc.cs_val;
        
        database->get_record_item_value(CH_FLAG, &desc);
        m_flag_.Format(_T("%i"), desc.l_val);
        
        // Handle spike count
        if (db_wave_doc->db_get_current_spk_file_name(TRUE).IsEmpty())
        {
            m_n_spikes_.Empty();
        }
        else
        {
            database->get_record_item_value(CH_NSPIKES, &desc);
            const int n_spikes = desc.l_val;
            database->get_record_item_value(CH_NSPIKECLASSES, &desc);
            m_n_spikes_.Format(_T("%i (%i classes)"), n_spikes, desc.l_val);
        }
    }
    catch (const std::exception& e)
    {
        handle_error(data_list_ctrl_error::FILE_OPEN_FAILED, CString(e.what()));
    }
}

void DataListCtrl_Row_Optimized::set_display_parameters(data_list_ctrl_infos* infos, int imageIndex)
{
    if (!infos)
    {
        handle_error(data_list_ctrl_error::INVALID_INDEX, _T("Invalid display info"));
        return;
    }
    
    std::lock_guard<std::mutex> lock(m_mutex_);
    
    // Check if display mode has changed
    if (m_last_display_mode_ != infos->display_mode)
    {
        m_display_processed_ = false;
    }
    
    // Prevent duplicate processing
    if (m_display_processed_)
    {
        return;
    }
    
    try
    {
        m_display_processed_ = true;
        m_last_display_mode_ = infos->display_mode;
        
        // Validate image index
        const auto pdb_doc = static_cast<ViewdbWave_Optimized*>(infos->parent->GetParent())->GetDocument();
        if (pdb_doc)
        {
            const int db_record_count = pdb_doc->db_get_records_count();
            if (!is_valid_index(imageIndex, db_record_count))
            {
                handle_error(data_list_ctrl_error::INVALID_INDEX, 
                           _T("Invalid image index: ") + CString(std::to_string(imageIndex).c_str()));
                return;
            }
        }
        
        // Check cache first
        if (m_cache_enabled_)
        {
            CBitmap* cachedBitmap = m_cache_->get_cached_bitmap(imageIndex, infos->display_mode);
            if (cachedBitmap)
            {
                m_performance_metrics_.cacheHits++;
                infos->image_list.Replace(imageIndex, cachedBitmap, nullptr);
                return;
            }
            m_performance_metrics_.cacheMisses++;
        }
        
        // Process display mode
        process_display_mode(infos, imageIndex);
    }
    catch (const std::exception& e)
    {
        handle_error(data_list_ctrl_error::GDI_RESOURCE_FAILED, CString(e.what()));
    }
}

void DataListCtrl_Row_Optimized::reset_display_processed()
{
    std::lock_guard<std::mutex> lock(m_mutex_);
    m_display_processed_ = false;
    m_last_display_mode_ = -1;
}

void DataListCtrl_Row_Optimized::Serialize(CArchive& ar)
{
    try
    {
        if (ar.IsStoring())
        {
            serialize_storing(ar);
        }
        else
        {
            serialize_loading(ar);
        }
    }
    catch (const CArchiveException& e)
    {
        CString archiveErrorMsg;
        e.GetErrorMessage(archiveErrorMsg.GetBuffer(256), 256);
        archiveErrorMsg.ReleaseBuffer();
        
        CString errorMessage;
        errorMessage.Format(_T("Serialization failed: %s (Error code: %d)"), archiveErrorMsg, e.m_cause);
        throw DataListCtrlException(data_list_ctrl_error::SERIALIZATION_FAILED, errorMessage);
    }
}

// Dependency injection methods for testing
void DataListCtrl_Row_Optimized::set_data_document(std::unique_ptr<AcqDataDoc> doc)
{
    std::lock_guard<std::mutex> lock(m_mutex_);
    m_p_data_doc_ = std::move(doc);
}

void DataListCtrl_Row_Optimized::set_spike_document(std::unique_ptr<CSpikeDoc> doc)
{
    std::lock_guard<std::mutex> lock(m_mutex_);
    m_p_spike_doc_ = std::move(doc);
}

void DataListCtrl_Row_Optimized::set_chart_data_window(std::unique_ptr<ChartData> chart)
{
    std::lock_guard<std::mutex> lock(m_mutex_);
    m_p_chart_data_wnd_ = std::move(chart);
}

void DataListCtrl_Row_Optimized::set_chart_spike_window(std::unique_ptr<ChartSpikeBar> chart)
{
    std::lock_guard<std::mutex> lock(m_mutex_);
    m_p_chart_spike_wnd_ = std::move(chart);
}

void DataListCtrl_Row_Optimized::reset_performance_metrics()
{
    std::lock_guard<std::mutex> lock(m_mutex_);
    m_performance_metrics_ = PerformanceMetrics{};
}

// Private implementation methods
void DataListCtrl_Row_Optimized::process_display_mode(data_list_ctrl_infos* infos, int image_index)
{
    switch (infos->display_mode)
    {
    case DataListCtrl_ConfigConstants::DISPLAY_MODE_DATA:
        display_data_window(infos, image_index);
        break;
    case DataListCtrl_ConfigConstants::DISPLAY_MODE_SPIKE:
        display_spike_window(infos, image_index);
        break;
    case DataListCtrl_ConfigConstants::DISPLAY_MODE_EMPTY:
    default:
        display_empty_window(infos, image_index);
        break;
    }
}

void DataListCtrl_Row_Optimized::display_data_window(data_list_ctrl_infos* infos, const int image_index)
{
    MeasurePerformance([&]() {
        create_data_window(infos, image_index);
        load_data_document();
        configure_data_window(infos);
        plot_to_image_list(infos, image_index, m_p_chart_data_wnd_.get(), m_data_file_name_);
    });
}

void DataListCtrl_Row_Optimized::display_spike_window(data_list_ctrl_infos* infos, const int image_index)
{
    MeasurePerformance([&]() {
        create_spike_window(infos, image_index);
        load_spike_document();
        configure_spike_window(infos);
        plot_to_image_list(infos, image_index, m_p_chart_spike_wnd_.get(), m_spike_file_name_);
    });
}

void DataListCtrl_Row_Optimized::display_empty_window(data_list_ctrl_infos* infos, const int image_index)
{
    MeasurePerformance([&]() {
        // Create empty bitmap with filename using standard MFC GDI
        CDC* pDC = infos->parent->GetDC();
        if (!pDC)
        {
            handle_error(data_list_ctrl_error::GDI_RESOURCE_FAILED, _T("Failed to get device context"));
            return;
        }
        
        CDC memDC;
        if (!memDC.CreateCompatibleDC(pDC))
        {
            infos->parent->ReleaseDC(pDC);
            handle_error(data_list_ctrl_error::GDI_RESOURCE_FAILED, _T("Failed to create compatible DC"));
            return;
        }
        
        CBitmap bitmap;
        if (!bitmap.CreateBitmap(infos->image_width, infos->image_height, pDC->GetDeviceCaps(PLANES), 4, nullptr))
        {
            infos->parent->ReleaseDC(pDC);
            handle_error(data_list_ctrl_error::GDI_RESOURCE_FAILED, _T("Failed to create bitmap"));
            return;
        }
        
        CBitmap* pOldBitmap = memDC.SelectObject(&bitmap);
        memDC.SetMapMode(pDC->GetMapMode());
        
        // Fill with light grey and draw filename
        CBrush brush(data_list_ctrl_constants::COLOR_LIGHT_GREY);
        memDC.SelectObject(&brush);
        CPen pen(PS_SOLID, 1, data_list_ctrl_constants::COLOR_BLACK);
        memDC.SelectObject(&pen);
        
        CRect rect(0, 0, infos->image_width, infos->image_height);
        memDC.Rectangle(&rect);
        
        // Draw filename
        CString filename = extract_filename(m_data_file_name_);
        if (filename.IsEmpty())
        {
            filename = _T("No file");
        }
        truncate_filename(filename);
        
        memDC.SetTextColor(data_list_ctrl_constants::COLOR_BLACK);
        memDC.SetBkMode(TRANSPARENT);
        
        CFont font;
        font.CreateFont(data_list_ctrl_constants::FONT_SIZE, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                       DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                       DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("Arial"));
        
        CFont* pOldFont = memDC.SelectObject(&font);
        
        CSize textSize = memDC.GetTextExtent(filename);
        int x = (infos->image_width - textSize.cx) / 2;
        int y = (infos->image_height - textSize.cy) / 2;
        memDC.TextOut(x, y, filename);
        
        memDC.SelectObject(pOldFont);
        
        // Replace in image list
        infos->image_list.Replace(image_index, &bitmap, nullptr);
        
        // Cleanup
        memDC.SelectObject(pOldBitmap);
        infos->parent->ReleaseDC(pDC);
    });
}

void DataListCtrl_Row_Optimized::create_data_window(data_list_ctrl_infos* infos, int image_index)
{
    if (!m_p_chart_data_wnd_)
    {
        m_p_chart_data_wnd_ = std::make_unique<ChartData>();
        
        const BOOL create_result = m_p_chart_data_wnd_->Create(_T("DATAWND"), 
            WS_CHILD, CRect(0, 0, infos->image_width, infos->image_height), 
            infos->parent, image_index * data_list_ctrl_constants::DATA_WINDOW_ID_BASE);
            
        if (!create_result)
        {
            handle_error(data_list_ctrl_error::WINDOW_CREATION_FAILED, _T("Failed to create data window"));
            m_p_chart_data_wnd_.reset();
            return;
        }
        
        m_p_chart_data_wnd_->set_b_use_dib(FALSE);
    }
    
    m_p_chart_data_wnd_->set_string(m_comment_);
}

void DataListCtrl_Row_Optimized::load_data_document()
{
    if (!m_p_data_doc_)
    {
        m_p_data_doc_ = std::make_unique<AcqDataDoc>();
    }
    
    if (m_data_file_name_.IsEmpty() || !m_p_data_doc_->open_document(m_data_file_name_))
    {
        if (m_p_chart_data_wnd_)
        {
            m_p_chart_data_wnd_->remove_all_channel_list_items();
            CString comment = _T("File name: ") + m_data_file_name_;
            comment += _T(" -- data not available");
            m_p_chart_data_wnd_->set_string(comment);
        }
        return;
    }
    
    m_p_data_doc_->read_data_infos();
    m_comment_ = m_p_data_doc_->get_wave_format()->get_comments(_T(" "));
    m_p_data_doc_->acq_close_file();
}

void DataListCtrl_Row_Optimized::configure_data_window(data_list_ctrl_infos* infos)
{
    if (!m_p_chart_data_wnd_ || !m_p_data_doc_)
        return;
    
    if (m_n_spikes_.IsEmpty())
        m_p_chart_data_wnd_->get_scope_parameters()->cr_scope_fill = data_list_ctrl_constants::COLOR_LIGHT_GREY;
    else
        m_p_chart_data_wnd_->get_scope_parameters()->cr_scope_fill = data_list_ctrl_constants::COLOR_WHITE;
    
    m_p_chart_data_wnd_->attach_data_file(m_p_data_doc_.get());
    m_p_chart_data_wnd_->load_all_channels(infos->data_transform);
    m_p_chart_data_wnd_->load_data_within_window(infos->b_set_time_span, infos->t_first, infos->t_last);
    m_p_chart_data_wnd_->adjust_gain(infos->b_set_mv_span, infos->mv_span);
}

void DataListCtrl_Row_Optimized::create_spike_window(data_list_ctrl_infos* infos, int image_index)
{
    if (!m_p_chart_spike_wnd_)
    {
        m_p_chart_spike_wnd_ = std::make_unique<ChartSpikeBar>();
        
        const BOOL create_result = m_p_chart_spike_wnd_->Create(_T("SPKWND"), 
            WS_CHILD, CRect(0, 0, infos->image_width, infos->image_height), 
            infos->parent, image_index * data_list_ctrl_constants::SPIKE_WINDOW_ID_BASE);
            
        if (!create_result)
        {
            handle_error(data_list_ctrl_error::WINDOW_CREATION_FAILED, _T("Failed to create spike window"));
            m_p_chart_spike_wnd_.reset();
            return;
        }
        
        m_p_chart_spike_wnd_->set_b_use_dib(FALSE);
        m_p_chart_spike_wnd_->set_display_all_files(false);
    }
}

void DataListCtrl_Row_Optimized::load_spike_document()
{
    if (!m_p_spike_doc_)
    {
        m_p_spike_doc_ = std::make_unique<CSpikeDoc>();
    }
    
    if (m_spike_file_name_.IsEmpty())
    {
        return;
    }
    
    if (!m_p_spike_doc_->OnOpenDocument(m_spike_file_name_))
    {
        handle_error(data_list_ctrl_error::FILE_OPEN_FAILED, _T("Failed to open spike file: ") + m_spike_file_name_);
    }
}

void DataListCtrl_Row_Optimized::configure_spike_window(data_list_ctrl_infos* infos)
{
    if (!m_p_chart_spike_wnd_ || !m_p_spike_doc_)
        return;
    
    const auto p_parent0 = static_cast<ViewdbWave_Optimized*>(infos->parent->GetParent());
    const int i = p_parent0->spk_list_tab_ctrl.GetCurSel();
    const int i_tab = i < 0 ? 0 : i;
    const auto p_spk_list = m_p_spike_doc_->set_index_current_spike_list(i_tab);
    
    m_p_chart_spike_wnd_->set_source_data(p_spk_list, p_parent0->GetDocument());
    m_p_chart_spike_wnd_->set_spike_doc(m_p_spike_doc_.get());
    m_p_chart_spike_wnd_->set_plot_mode(infos->spike_plot_mode, infos->selected_class);
    
    long l_first = 0;
    auto l_last = m_p_spike_doc_->get_acq_size();
    if (infos->b_set_time_span)
    {
        const auto sampling_rate = m_p_spike_doc_->get_acq_rate();
        l_first = static_cast<long>(infos->t_first * sampling_rate);
        l_last = static_cast<long>(infos->t_last * sampling_rate);
    }
    m_p_chart_spike_wnd_->set_time_intervals(l_first, l_last);
    
    if (infos->b_set_mv_span)
    {
        const auto volts_per_bin = p_spk_list->get_acq_volts_per_bin();
        const auto y_we = static_cast<int>(infos->mv_span / 1000.f / volts_per_bin);
        const auto y_wo = p_spk_list->get_acq_bin_zero();
        m_p_chart_spike_wnd_->set_yw_ext_org(y_we, y_wo);
    }
}

void DataListCtrl_Row_Optimized::plot_to_image_list(data_list_ctrl_infos* infos, int image_index, 
                                                ChartData* pChart, const CString& filename)
{
    if (!pChart)
        return;
    
    pChart->set_values_for_bottom_comment(infos->b_display_file_name, filename);
    
    CRect client_rect;
    pChart->GetClientRect(&client_rect);
    
    // Use standard MFC GDI operations
    CDC* pDC = pChart->GetDC();
    if (!pDC)
    {
        handle_error(data_list_ctrl_error::GDI_RESOURCE_FAILED, _T("Failed to get device context"));
        return;
    }
    
    CDC memDC;
    if (!memDC.CreateCompatibleDC(pDC))
    {
        pChart->ReleaseDC(pDC);
        handle_error(data_list_ctrl_error::GDI_RESOURCE_FAILED, _T("Failed to create compatible DC"));
        return;
    }
    
    CBitmap bitmap;
    if (!bitmap.CreateBitmap(client_rect.right, client_rect.bottom, pDC->GetDeviceCaps(PLANES), 4, nullptr))
    {
        pChart->ReleaseDC(pDC);
        handle_error(data_list_ctrl_error::GDI_RESOURCE_FAILED, _T("Failed to create bitmap"));
        return;
    }
    
    CBitmap* pOldBitmap = memDC.SelectObject(&bitmap);
    memDC.SetMapMode(pDC->GetMapMode());
    
    if (!pChart->GetSafeHwnd())
    {
        memDC.SelectObject(pOldBitmap);
        pChart->ReleaseDC(pDC);
        handle_error(data_list_ctrl_error::WINDOW_CREATION_FAILED, _T("Invalid window handle"));
        return;
    }
    
    pChart->plot_data_to_dc(&memDC);
    
    // Cache the bitmap if caching is enabled
    if (m_cache_enabled_)
    {
        auto cachedBitmap = std::make_unique<CBitmap>();
        cachedBitmap->Attach(bitmap.GetSafeHandle());
        m_cache_->set_cached_bitmap(image_index, std::move(cachedBitmap), infos->display_mode);
    }
    
    infos->image_list.Replace(image_index, &bitmap, nullptr);
    
    // Cleanup
    memDC.SelectObject(pOldBitmap);
    pChart->ReleaseDC(pDC);
}

void DataListCtrl_Row_Optimized::plot_to_image_list(data_list_ctrl_infos* infos, int image_index, 
                                                ChartSpikeBar* pChart, const CString& filename)
{
    if (!pChart)
        return;
    
    pChart->set_values_for_bottom_comment(infos->b_display_file_name, filename);
    
    CRect client_rect;
    pChart->GetClientRect(&client_rect);
    
    // Use standard MFC GDI operations
    CDC* pDC = pChart->GetDC();
    if (!pDC)
    {
        handle_error(data_list_ctrl_error::GDI_RESOURCE_FAILED, _T("Failed to get device context"));
        return;
    }
    
    CDC memDC;
    if (!memDC.CreateCompatibleDC(pDC))
    {
        pChart->ReleaseDC(pDC);
        handle_error(data_list_ctrl_error::GDI_RESOURCE_FAILED, _T("Failed to create compatible DC"));
        return;
    }
    
    CBitmap bitmap;
    if (!bitmap.CreateBitmap(client_rect.right, client_rect.bottom, pDC->GetDeviceCaps(PLANES), 4, nullptr))
    {
        pChart->ReleaseDC(pDC);
        handle_error(data_list_ctrl_error::GDI_RESOURCE_FAILED, _T("Failed to create bitmap"));
        return;
    }
    
    CBitmap* pOldBitmap = memDC.SelectObject(&bitmap);
    memDC.SetMapMode(pDC->GetMapMode());
    
    if (!pChart->GetSafeHwnd())
    {
        memDC.SelectObject(pOldBitmap);
        pChart->ReleaseDC(pDC);
        handle_error(data_list_ctrl_error::WINDOW_CREATION_FAILED, _T("Invalid window handle"));
        return;
    }
    
    pChart->plot_data_to_dc(&memDC);
    
    // Cache the bitmap if caching is enabled
    if (m_cache_enabled_)
    {
        auto cachedBitmap = std::make_unique<CBitmap>();
        cachedBitmap->Attach(bitmap.GetSafeHandle());
        m_cache_->set_cached_bitmap(image_index, std::move(cachedBitmap), infos->display_mode);
    }
    
    infos->image_list.Replace(image_index, &bitmap, nullptr);
    
    // Cleanup
    memDC.SelectObject(pOldBitmap);
    pChart->ReleaseDC(pDC);
}

void DataListCtrl_Row_Optimized::create_plot_bitmap(data_list_ctrl_infos* infos, int image_index,
                                                 ChartData* pChart, CBitmap& bitmap)
{
    if (!pChart)
        return;
    
    CRect client_rect;
    pChart->GetClientRect(&client_rect);
    
    CDC* pDC = pChart->GetDC();
    if (!pDC)
    {
        handle_error(data_list_ctrl_error::GDI_RESOURCE_FAILED, _T("Failed to get device context"));
        return;
    }
    
    if (!bitmap.CreateBitmap(client_rect.right, client_rect.bottom, pDC->GetDeviceCaps(PLANES), 4, nullptr))
    {
        pChart->ReleaseDC(pDC);
        handle_error(data_list_ctrl_error::GDI_RESOURCE_FAILED, _T("Failed to create bitmap"));
        return;
    }
    
    CDC memDC;
    if (!memDC.CreateCompatibleDC(pDC))
    {
        pChart->ReleaseDC(pDC);
        handle_error(data_list_ctrl_error::GDI_RESOURCE_FAILED, _T("Failed to create compatible DC"));
        return;
    }
    
    CBitmap* pOldBitmap = memDC.SelectObject(&bitmap);
    memDC.SetMapMode(pDC->GetMapMode());
    
    pChart->plot_data_to_dc(&memDC);
    
    memDC.SelectObject(pOldBitmap);
    pChart->ReleaseDC(pDC);
}

void DataListCtrl_Row_Optimized::create_plot_bitmap(data_list_ctrl_infos* infos, int image_index,
                                                 ChartSpikeBar* pChart, CBitmap& bitmap)
{
    if (!pChart)
        return;
    
    CRect client_rect;
    pChart->GetClientRect(&client_rect);
    
    CDC* pDC = pChart->GetDC();
    if (!pDC)
    {
        handle_error(data_list_ctrl_error::GDI_RESOURCE_FAILED, _T("Failed to get device context"));
        return;
    }
    
    if (!bitmap.CreateBitmap(client_rect.right, client_rect.bottom, pDC->GetDeviceCaps(PLANES), 4, nullptr))
    {
        pChart->ReleaseDC(pDC);
        handle_error(data_list_ctrl_error::GDI_RESOURCE_FAILED, _T("Failed to create bitmap"));
        return;
    }
    
    CDC memDC;
    if (!memDC.CreateCompatibleDC(pDC))
    {
        pChart->ReleaseDC(pDC);
        handle_error(data_list_ctrl_error::GDI_RESOURCE_FAILED, _T("Failed to create compatible DC"));
        return;
    }
    
    CBitmap* pOldBitmap = memDC.SelectObject(&bitmap);
    memDC.SetMapMode(pDC->GetMapMode());
    
    pChart->plot_data_to_dc(&memDC);
    
    memDC.SelectObject(pOldBitmap);
    pChart->ReleaseDC(pDC);
}

// Utility methods
CString DataListCtrl_Row_Optimized::extract_filename(const CString& full_path) const
{
    int lastSlash = full_path.ReverseFind('\\');
    if (lastSlash >= 0)
    {
        return full_path.Mid(lastSlash + 1);
    }
    return full_path;
}

void DataListCtrl_Row_Optimized::truncate_filename(CString& filename) const
{
    if (filename.GetLength() > data_list_ctrl_constants::MAX_FILENAME_LENGTH)
    {
        filename = filename.Left(data_list_ctrl_constants::FILENAME_TRUNCATE_LENGTH) + _T("...");
    }
}

bool DataListCtrl_Row_Optimized::is_valid_index(int index, int max_count) const
{
    return index >= 0 && index < max_count;
}

bool DataListCtrl_Row_Optimized::is_valid_display_mode(int mode) const
{
        return mode >= DataListCtrl_ConfigConstants::DISPLAY_MODE_EMPTY &&
           mode <= DataListCtrl_ConfigConstants::DISPLAY_MODE_SPIKE;
}

void DataListCtrl_Row_Optimized::handle_error(data_list_ctrl_error error, const CString& message)
{
    log_error(message);
    throw DataListCtrlException(error, message);
}

void DataListCtrl_Row_Optimized::log_error(const CString& message) const
{
    TRACE(_T("DataListCtrl_Row_Optimized Error: %s\n"), message);
}

// Serialization helpers
void DataListCtrl_Row_Optimized::serialize_storing(CArchive& ar)
{
    m_version_ = 2;
    ar << m_version_;
    ar << m_index_;
    
    serialize_strings(ar, true);
    serialize_objects(ar, true);
}

void DataListCtrl_Row_Optimized::serialize_loading(CArchive& ar)
{
    ar >> m_version_;
    ar >> m_index_;
    
    serialize_strings(ar, false);
    serialize_objects(ar, false);
}

void DataListCtrl_Row_Optimized::serialize_strings(CArchive& ar, bool isStoring)
{
    constexpr auto string_count = 8;
    
    if (isStoring)
    {
        ar << string_count;
        ar << m_comment_;
        ar << m_data_file_name_;
        ar << m_sensillum_name_;
        ar << m_stimulus1_;
        ar << m_concentration1_;
        ar << m_stimulus2_;
        ar << m_concentration2_;
        ar << m_n_spikes_;
        ar << m_flag_;
        ar << m_date_;
    }
    else
    {
        int string_count_read;
        ar >> string_count_read;
        ar >> m_comment_;
        string_count_read--;
        ar >> m_data_file_name_;
        string_count_read--;
        ar >> m_sensillum_name_;
        string_count_read--;
        ar >> m_stimulus1_;
        string_count_read--;
        ar >> m_concentration1_;
        string_count_read--;
        if (m_version_ > 1)
        {
            ar >> m_stimulus2_;
            string_count_read--;
            ar >> m_concentration2_;
            string_count_read--;
        }
        ar >> m_n_spikes_;
        string_count_read--;
        ar >> m_flag_;
        string_count_read--;
        ar >> m_date_;
        string_count_read--;
    }
}

void DataListCtrl_Row_Optimized::serialize_objects(CArchive& ar, bool isStoring)
{
    constexpr auto object_count = 3;
    
    if (isStoring)
    {
        ar << object_count;
        if (m_p_chart_data_wnd_)
            m_p_chart_data_wnd_->Serialize(ar);
        if (m_p_chart_spike_wnd_)
            m_p_chart_spike_wnd_->Serialize(ar);
        ar << m_insect_id_;
    }
    else
    {
        int object_count_read;
        ar >> object_count_read;
        ASSERT(object_count_read >= 2);
        
        if (m_p_chart_data_wnd_)
            m_p_chart_data_wnd_->Serialize(ar);
        object_count_read--;
        
        if (m_p_chart_spike_wnd_)
            m_p_chart_spike_wnd_->Serialize(ar);
        object_count_read--;
        
        if (object_count_read > 0)
            ar >> m_insect_id_;
    }
}
