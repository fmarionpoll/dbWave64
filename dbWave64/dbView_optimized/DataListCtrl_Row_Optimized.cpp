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
    : m_cache(std::make_unique<DataListCtrlCache>())
{
}

DataListCtrl_Row_Optimized::DataListCtrl_Row_Optimized(int index)
    : m_index(index), m_cache(std::make_unique<DataListCtrlCache>())
{
}

DataListCtrl_Row_Optimized::DataListCtrl_Row_Optimized(const DataListCtrl_Row_Optimized& other)
    : m_index(other.m_index), m_insectId(other.m_insectId), m_recordId(other.m_recordId),
      m_comment(other.m_comment), m_dataFileName(other.m_dataFileName), m_spikeFileName(other.m_spikeFileName),
      m_sensillumName(other.m_sensillumName), m_stimulus1(other.m_stimulus1), m_concentration1(other.m_concentration1),
      m_stimulus2(other.m_stimulus2), m_concentration2(other.m_concentration2), m_nSpikes(other.m_nSpikes),
      m_flag(other.m_flag), m_date(other.m_date), m_displayProcessed(other.m_displayProcessed),
      m_lastDisplayMode(other.m_lastDisplayMode), m_init(other.m_init), m_changed(other.m_changed),
      m_version(other.m_version), m_cacheEnabled(other.m_cacheEnabled), m_performanceMetrics(other.m_performanceMetrics)
{
    // Note: Smart pointers and cache are not copied, they need to be recreated
    // They will be default-constructed (nullptr)
}

DataListCtrl_Row_Optimized& DataListCtrl_Row_Optimized::operator=(const DataListCtrl_Row_Optimized& other)
{
    if (this != &other)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        m_index = other.m_index;
        m_insectId = other.m_insectId;
        m_recordId = other.m_recordId;
        m_comment = other.m_comment;
        m_dataFileName = other.m_dataFileName;
        m_spikeFileName = other.m_spikeFileName;
        m_sensillumName = other.m_sensillumName;
        m_stimulus1 = other.m_stimulus1;
        m_concentration1 = other.m_concentration1;
        m_stimulus2 = other.m_stimulus2;
        m_concentration2 = other.m_concentration2;
        m_nSpikes = other.m_nSpikes;
        m_flag = other.m_flag;
        m_date = other.m_date;
        m_displayProcessed = other.m_displayProcessed;
        m_lastDisplayMode = other.m_lastDisplayMode;
        m_init = other.m_init;
        m_changed = other.m_changed;
        m_version = other.m_version;
        m_cacheEnabled = other.m_cacheEnabled;
        m_performanceMetrics = other.m_performanceMetrics;
        
        // Note: Smart pointers are not copied, they need to be recreated
    }
    return *this;
}

DataListCtrl_Row_Optimized::DataListCtrl_Row_Optimized(DataListCtrl_Row_Optimized&& other) noexcept
    : m_index(other.m_index), m_insectId(other.m_insectId), m_recordId(other.m_recordId),
      m_comment(std::move(other.m_comment)), m_dataFileName(std::move(other.m_dataFileName)),
      m_spikeFileName(std::move(other.m_spikeFileName)), m_sensillumName(std::move(other.m_sensillumName)),
      m_stimulus1(std::move(other.m_stimulus1)), m_concentration1(std::move(other.m_concentration1)),
      m_stimulus2(std::move(other.m_stimulus2)), m_concentration2(std::move(other.m_concentration2)),
      m_nSpikes(std::move(other.m_nSpikes)), m_flag(std::move(other.m_flag)), m_date(std::move(other.m_date)),
      m_pDataDoc(std::move(other.m_pDataDoc)), m_pChartDataWnd(std::move(other.m_pChartDataWnd)),
      m_pSpikeDoc(std::move(other.m_pSpikeDoc)), m_pChartSpikeWnd(std::move(other.m_pChartSpikeWnd)),
      m_displayProcessed(other.m_displayProcessed), m_lastDisplayMode(other.m_lastDisplayMode),
      m_init(other.m_init), m_changed(other.m_changed), m_version(other.m_version),
      m_cacheEnabled(other.m_cacheEnabled), m_performanceMetrics(other.m_performanceMetrics),
      m_cache(std::move(other.m_cache))
{
    // Reset other object
    other.m_index = 0;
    other.m_insectId = 0;
    other.m_recordId = 0;
    other.m_displayProcessed = false;
    other.m_lastDisplayMode = -1;
    other.m_init = false;
    other.m_changed = false;
    other.m_version = 0;
    other.m_cacheEnabled = true;
    other.m_performanceMetrics = PerformanceMetrics{};
}

DataListCtrl_Row_Optimized& DataListCtrl_Row_Optimized::operator=(DataListCtrl_Row_Optimized&& other) noexcept
{
    if (this != &other)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        m_index = other.m_index;
        m_insectId = other.m_insectId;
        m_recordId = other.m_recordId;
        m_comment = std::move(other.m_comment);
        m_dataFileName = std::move(other.m_dataFileName);
        m_spikeFileName = std::move(other.m_spikeFileName);
        m_sensillumName = std::move(other.m_sensillumName);
        m_stimulus1 = std::move(other.m_stimulus1);
        m_concentration1 = std::move(other.m_concentration1);
        m_stimulus2 = std::move(other.m_stimulus2);
        m_concentration2 = std::move(other.m_concentration2);
        m_nSpikes = std::move(other.m_nSpikes);
        m_flag = std::move(other.m_flag);
        m_date = std::move(other.m_date);
        m_pDataDoc = std::move(other.m_pDataDoc);
        m_pChartDataWnd = std::move(other.m_pChartDataWnd);
        m_pSpikeDoc = std::move(other.m_pSpikeDoc);
        m_pChartSpikeWnd = std::move(other.m_pChartSpikeWnd);
        m_displayProcessed = other.m_displayProcessed;
        m_lastDisplayMode = other.m_lastDisplayMode;
        m_init = other.m_init;
        m_changed = other.m_changed;
        m_version = other.m_version;
        m_cacheEnabled = other.m_cacheEnabled;
        m_performanceMetrics = other.m_performanceMetrics;
        m_cache = std::move(other.m_cache);
        
        // Reset other object
        other.m_index = 0;
        other.m_insectId = 0;
        other.m_recordId = 0;
        other.m_displayProcessed = false;
        other.m_lastDisplayMode = -1;
        other.m_init = false;
        other.m_changed = false;
        other.m_version = 0;
        other.m_cacheEnabled = true;
        other.m_performanceMetrics = PerformanceMetrics{};
    }
    return *this;
}

void DataListCtrl_Row_Optimized::AttachDatabaseRecord(CdbWaveDoc* db_wave_doc)
{
    if (!db_wave_doc)
    {
        HandleError(DataListCtrlError::INVALID_INDEX, _T("Invalid database document"));
        return;
    }
    
    try
    {
        if (db_wave_doc->db_set_current_record_position(m_index))
        {
            db_wave_doc->open_current_data_file();
            db_wave_doc->open_current_spike_file();
        }
        
        m_dataFileName = db_wave_doc->db_get_current_dat_file_name(TRUE);
        m_spikeFileName = db_wave_doc->db_get_current_spk_file_name(TRUE);
        
        const auto database = db_wave_doc->db_table;
        if (!database)
        {
            HandleError(DataListCtrlError::INVALID_INDEX, _T("Invalid database table"));
            return;
        }
        
        DB_ITEMDESC desc;
        
        // Get record ID
        database->get_record_item_value(CH_ID, &desc);
        m_recordId = desc.l_val;
        
        // Get insect ID
        database->get_record_item_value(CH_IDINSECT, &desc);
        m_insectId = desc.l_val;
        
        // Get stimulus and concentration data
        database->get_record_item_value(CH_STIM1_KEY, &desc);
        m_stimulus1 = desc.cs_val;
        
        database->get_record_item_value(CH_CONC1_KEY, &desc);
        m_concentration1 = desc.cs_val;
        
        database->get_record_item_value(CH_STIM2_KEY, &desc);
        m_stimulus2 = desc.cs_val;
        
        database->get_record_item_value(CH_CONC2_KEY, &desc);
        m_concentration2 = desc.cs_val;
        
        database->get_record_item_value(CH_SENSILLUM_KEY, &desc);
        m_sensillumName = desc.cs_val;
        
        database->get_record_item_value(CH_FLAG, &desc);
        m_flag.Format(_T("%i"), desc.l_val);
        
        // Handle spike count
        if (db_wave_doc->db_get_current_spk_file_name(TRUE).IsEmpty())
        {
            m_nSpikes.Empty();
        }
        else
        {
            database->get_record_item_value(CH_NSPIKES, &desc);
            const int n_spikes = desc.l_val;
            database->get_record_item_value(CH_NSPIKECLASSES, &desc);
            m_nSpikes.Format(_T("%i (%i classes)"), n_spikes, desc.l_val);
        }
    }
    catch (const std::exception& e)
    {
        HandleError(DataListCtrlError::FILE_OPEN_FAILED, CString(e.what()));
    }
}

void DataListCtrl_Row_Optimized::SetDisplayParameters(DataListCtrlInfos* infos, int imageIndex)
{
    if (!infos)
    {
        HandleError(DataListCtrlError::INVALID_INDEX, _T("Invalid display info"));
        return;
    }
    
    std::lock_guard<std::mutex> lock(m_mutex);
    
    // Check if display mode has changed
    if (m_lastDisplayMode != infos->display_mode)
    {
        m_displayProcessed = false;
    }
    
    // Prevent duplicate processing
    if (m_displayProcessed)
    {
        return;
    }
    
    try
    {
        m_displayProcessed = true;
        m_lastDisplayMode = infos->display_mode;
        
        // Validate image index
        const auto pdb_doc = static_cast<ViewdbWave_Optimized*>(infos->parent->GetParent())->GetDocument();
        if (pdb_doc)
        {
            const int db_record_count = pdb_doc->db_get_records_count();
            if (!IsValidIndex(imageIndex, db_record_count))
            {
                HandleError(DataListCtrlError::INVALID_INDEX, 
                           _T("Invalid image index: ") + CString(std::to_string(imageIndex).c_str()));
                return;
            }
        }
        
        // Check cache first
        if (m_cacheEnabled)
        {
            CBitmap* cachedBitmap = m_cache->GetCachedBitmap(imageIndex, infos->display_mode);
            if (cachedBitmap)
            {
                m_performanceMetrics.cacheHits++;
                infos->image_list.Replace(imageIndex, cachedBitmap, nullptr);
                return;
            }
            m_performanceMetrics.cacheMisses++;
        }
        
        // Process display mode
        ProcessDisplayMode(infos, imageIndex);
    }
    catch (const std::exception& e)
    {
        HandleError(DataListCtrlError::GDI_RESOURCE_FAILED, CString(e.what()));
    }
}

void DataListCtrl_Row_Optimized::ResetDisplayProcessed()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_displayProcessed = false;
    m_lastDisplayMode = -1;
}

void DataListCtrl_Row_Optimized::Serialize(CArchive& ar)
{
    try
    {
        if (ar.IsStoring())
        {
            SerializeStoring(ar);
        }
        else
        {
            SerializeLoading(ar);
        }
    }
    catch (const CArchiveException& e)
    {
        throw DataListCtrlException(DataListCtrlError::SERIALIZATION_FAILED, _T("Serialization failed"));
    }
}

// Dependency injection methods for testing
void DataListCtrl_Row_Optimized::SetDataDocument(std::unique_ptr<AcqDataDoc> doc)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_pDataDoc = std::move(doc);
}

void DataListCtrl_Row_Optimized::SetSpikeDocument(std::unique_ptr<CSpikeDoc> doc)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_pSpikeDoc = std::move(doc);
}

void DataListCtrl_Row_Optimized::SetChartDataWindow(std::unique_ptr<ChartData> chart)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_pChartDataWnd = std::move(chart);
}

void DataListCtrl_Row_Optimized::SetChartSpikeWindow(std::unique_ptr<ChartSpikeBar> chart)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_pChartSpikeWnd = std::move(chart);
}

void DataListCtrl_Row_Optimized::ResetPerformanceMetrics()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_performanceMetrics = PerformanceMetrics{};
}

// Private implementation methods
void DataListCtrl_Row_Optimized::ProcessDisplayMode(DataListCtrlInfos* infos, int imageIndex)
{
    switch (infos->display_mode)
    {
    case DataListCtrlConfigConstants::DISPLAY_MODE_DATA:
        DisplayDataWindow(infos, imageIndex);
        break;
    case DataListCtrlConfigConstants::DISPLAY_MODE_SPIKE:
        DisplaySpikeWindow(infos, imageIndex);
        break;
    case DataListCtrlConfigConstants::DISPLAY_MODE_EMPTY:
    default:
        DisplayEmptyWindow(infos, imageIndex);
        break;
    }
}

void DataListCtrl_Row_Optimized::DisplayDataWindow(DataListCtrlInfos* infos, int imageIndex)
{
    MeasurePerformance([&]() {
        CreateDataWindow(infos, imageIndex);
        LoadDataDocument();
        ConfigureDataWindow(infos);
        PlotToImageList(infos, imageIndex, m_pChartDataWnd.get(), m_dataFileName);
    });
}

void DataListCtrl_Row_Optimized::DisplaySpikeWindow(DataListCtrlInfos* infos, int imageIndex)
{
    MeasurePerformance([&]() {
        CreateSpikeWindow(infos, imageIndex);
        LoadSpikeDocument();
        ConfigureSpikeWindow(infos);
        PlotToImageList(infos, imageIndex, m_pChartSpikeWnd.get(), m_spikeFileName);
    });
}

void DataListCtrl_Row_Optimized::DisplayEmptyWindow(DataListCtrlInfos* infos, int imageIndex)
{
    MeasurePerformance([&]() {
        // Create empty bitmap with filename using standard MFC GDI
        CDC* pDC = infos->parent->GetDC();
        if (!pDC)
        {
            HandleError(DataListCtrlError::GDI_RESOURCE_FAILED, _T("Failed to get device context"));
            return;
        }
        
        CDC memDC;
        if (!memDC.CreateCompatibleDC(pDC))
        {
            infos->parent->ReleaseDC(pDC);
            HandleError(DataListCtrlError::GDI_RESOURCE_FAILED, _T("Failed to create compatible DC"));
            return;
        }
        
        CBitmap bitmap;
        if (!bitmap.CreateBitmap(infos->image_width, infos->image_height, pDC->GetDeviceCaps(PLANES), 4, nullptr))
        {
            infos->parent->ReleaseDC(pDC);
            HandleError(DataListCtrlError::GDI_RESOURCE_FAILED, _T("Failed to create bitmap"));
            return;
        }
        
        CBitmap* pOldBitmap = memDC.SelectObject(&bitmap);
        memDC.SetMapMode(pDC->GetMapMode());
        
        // Fill with light grey and draw filename
        CBrush brush(DataListCtrlConstants::COLOR_LIGHT_GREY);
        memDC.SelectObject(&brush);
        CPen pen(PS_SOLID, 1, DataListCtrlConstants::COLOR_BLACK);
        memDC.SelectObject(&pen);
        
        CRect rect(0, 0, infos->image_width, infos->image_height);
        memDC.Rectangle(&rect);
        
        // Draw filename
        CString filename = ExtractFilename(m_dataFileName);
        if (filename.IsEmpty())
        {
            filename = _T("No file");
        }
        TruncateFilename(filename);
        
        memDC.SetTextColor(DataListCtrlConstants::COLOR_BLACK);
        memDC.SetBkMode(TRANSPARENT);
        
        CFont font;
        font.CreateFont(DataListCtrlConstants::FONT_SIZE, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                       DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                       DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("Arial"));
        
        CFont* pOldFont = memDC.SelectObject(&font);
        
        CSize textSize = memDC.GetTextExtent(filename);
        int x = (infos->image_width - textSize.cx) / 2;
        int y = (infos->image_height - textSize.cy) / 2;
        memDC.TextOut(x, y, filename);
        
        memDC.SelectObject(pOldFont);
        
        // Replace in image list
        infos->image_list.Replace(imageIndex, &bitmap, nullptr);
        
        // Cleanup
        memDC.SelectObject(pOldBitmap);
        infos->parent->ReleaseDC(pDC);
    });
}

void DataListCtrl_Row_Optimized::CreateDataWindow(DataListCtrlInfos* infos, int imageIndex)
{
    if (!m_pChartDataWnd)
    {
        m_pChartDataWnd = std::make_unique<ChartData>();
        
        const BOOL create_result = m_pChartDataWnd->Create(_T("DATAWND"), 
            WS_CHILD, CRect(0, 0, infos->image_width, infos->image_height), 
            infos->parent, imageIndex * DataListCtrlConstants::DATA_WINDOW_ID_BASE);
            
        if (!create_result)
        {
            HandleError(DataListCtrlError::WINDOW_CREATION_FAILED, _T("Failed to create data window"));
            m_pChartDataWnd.reset();
            return;
        }
        
        m_pChartDataWnd->set_b_use_dib(FALSE);
    }
    
    m_pChartDataWnd->set_string(m_comment);
}

void DataListCtrl_Row_Optimized::LoadDataDocument()
{
    if (!m_pDataDoc)
    {
        m_pDataDoc = std::make_unique<AcqDataDoc>();
    }
    
    if (m_dataFileName.IsEmpty() || !m_pDataDoc->open_document(m_dataFileName))
    {
        if (m_pChartDataWnd)
        {
            m_pChartDataWnd->remove_all_channel_list_items();
            CString comment = _T("File name: ") + m_dataFileName;
            comment += _T(" -- data not available");
            m_pChartDataWnd->set_string(comment);
        }
        return;
    }
    
    m_pDataDoc->read_data_infos();
    m_comment = m_pDataDoc->get_wave_format()->get_comments(_T(" "));
    m_pDataDoc->acq_close_file();
}

void DataListCtrl_Row_Optimized::ConfigureDataWindow(DataListCtrlInfos* infos)
{
    if (!m_pChartDataWnd || !m_pDataDoc)
        return;
    
    if (m_nSpikes.IsEmpty())
        m_pChartDataWnd->get_scope_parameters()->cr_scope_fill = DataListCtrlConstants::COLOR_LIGHT_GREY;
    else
        m_pChartDataWnd->get_scope_parameters()->cr_scope_fill = DataListCtrlConstants::COLOR_WHITE;
    
    m_pChartDataWnd->attach_data_file(m_pDataDoc.get());
    m_pChartDataWnd->load_all_channels(infos->data_transform);
    m_pChartDataWnd->load_data_within_window(infos->b_set_time_span, infos->t_first, infos->t_last);
    m_pChartDataWnd->adjust_gain(infos->b_set_mv_span, infos->mv_span);
}

void DataListCtrl_Row_Optimized::CreateSpikeWindow(DataListCtrlInfos* infos, int imageIndex)
{
    if (!m_pChartSpikeWnd)
    {
        m_pChartSpikeWnd = std::make_unique<ChartSpikeBar>();
        
        const BOOL create_result = m_pChartSpikeWnd->Create(_T("SPKWND"), 
            WS_CHILD, CRect(0, 0, infos->image_width, infos->image_height), 
            infos->parent, m_index * DataListCtrlConstants::SPIKE_WINDOW_ID_BASE);
            
        if (!create_result)
        {
            HandleError(DataListCtrlError::WINDOW_CREATION_FAILED, _T("Failed to create spike window"));
            m_pChartSpikeWnd.reset();
            return;
        }
        
        m_pChartSpikeWnd->set_b_use_dib(FALSE);
        m_pChartSpikeWnd->set_display_all_files(false);
    }
}

void DataListCtrl_Row_Optimized::LoadSpikeDocument()
{
    if (!m_pSpikeDoc)
    {
        m_pSpikeDoc = std::make_unique<CSpikeDoc>();
    }
    
    if (m_spikeFileName.IsEmpty())
    {
        return;
    }
    
    if (!m_pSpikeDoc->OnOpenDocument(m_spikeFileName))
    {
        HandleError(DataListCtrlError::FILE_OPEN_FAILED, _T("Failed to open spike file: ") + m_spikeFileName);
    }
}

void DataListCtrl_Row_Optimized::ConfigureSpikeWindow(DataListCtrlInfos* infos)
{
    if (!m_pChartSpikeWnd || !m_pSpikeDoc)
        return;
    
    const auto p_parent0 = static_cast<ViewdbWave_Optimized*>(infos->parent->GetParent());
    const int i = p_parent0->spk_list_tab_ctrl.GetCurSel();
    const int i_tab = i < 0 ? 0 : i;
    const auto p_spk_list = m_pSpikeDoc->set_index_current_spike_list(i_tab);
    
    m_pChartSpikeWnd->set_source_data(p_spk_list, p_parent0->GetDocument());
    m_pChartSpikeWnd->set_spike_doc(m_pSpikeDoc.get());
    m_pChartSpikeWnd->set_plot_mode(infos->spike_plot_mode, infos->selected_class);
    
    long l_first = 0;
    auto l_last = m_pSpikeDoc->get_acq_size();
    if (infos->b_set_time_span)
    {
        const auto sampling_rate = m_pSpikeDoc->get_acq_rate();
        l_first = static_cast<long>(infos->t_first * sampling_rate);
        l_last = static_cast<long>(infos->t_last * sampling_rate);
    }
    m_pChartSpikeWnd->set_time_intervals(l_first, l_last);
    
    if (infos->b_set_mv_span)
    {
        const auto volts_per_bin = p_spk_list->get_acq_volts_per_bin();
        const auto y_we = static_cast<int>(infos->mv_span / 1000.f / volts_per_bin);
        const auto y_wo = p_spk_list->get_acq_bin_zero();
        m_pChartSpikeWnd->set_yw_ext_org(y_we, y_wo);
    }
}

void DataListCtrl_Row_Optimized::PlotToImageList(DataListCtrlInfos* infos, int imageIndex, 
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
        HandleError(DataListCtrlError::GDI_RESOURCE_FAILED, _T("Failed to get device context"));
        return;
    }
    
    CDC memDC;
    if (!memDC.CreateCompatibleDC(pDC))
    {
        pChart->ReleaseDC(pDC);
        HandleError(DataListCtrlError::GDI_RESOURCE_FAILED, _T("Failed to create compatible DC"));
        return;
    }
    
    CBitmap bitmap;
    if (!bitmap.CreateBitmap(client_rect.right, client_rect.bottom, pDC->GetDeviceCaps(PLANES), 4, nullptr))
    {
        pChart->ReleaseDC(pDC);
        HandleError(DataListCtrlError::GDI_RESOURCE_FAILED, _T("Failed to create bitmap"));
        return;
    }
    
    CBitmap* pOldBitmap = memDC.SelectObject(&bitmap);
    memDC.SetMapMode(pDC->GetMapMode());
    
    if (!pChart->GetSafeHwnd())
    {
        memDC.SelectObject(pOldBitmap);
        pChart->ReleaseDC(pDC);
        HandleError(DataListCtrlError::WINDOW_CREATION_FAILED, _T("Invalid window handle"));
        return;
    }
    
    pChart->plot_data_to_dc(&memDC);
    
    // Cache the bitmap if caching is enabled
    if (m_cacheEnabled)
    {
        auto cachedBitmap = std::make_unique<CBitmap>();
        cachedBitmap->Attach(bitmap.GetSafeHandle());
        m_cache->SetCachedBitmap(imageIndex, std::move(cachedBitmap), infos->display_mode);
    }
    
    infos->image_list.Replace(imageIndex, &bitmap, nullptr);
    
    // Cleanup
    memDC.SelectObject(pOldBitmap);
    pChart->ReleaseDC(pDC);
}

void DataListCtrl_Row_Optimized::PlotToImageList(DataListCtrlInfos* infos, int imageIndex, 
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
        HandleError(DataListCtrlError::GDI_RESOURCE_FAILED, _T("Failed to get device context"));
        return;
    }
    
    CDC memDC;
    if (!memDC.CreateCompatibleDC(pDC))
    {
        pChart->ReleaseDC(pDC);
        HandleError(DataListCtrlError::GDI_RESOURCE_FAILED, _T("Failed to create compatible DC"));
        return;
    }
    
    CBitmap bitmap;
    if (!bitmap.CreateBitmap(client_rect.right, client_rect.bottom, pDC->GetDeviceCaps(PLANES), 4, nullptr))
    {
        pChart->ReleaseDC(pDC);
        HandleError(DataListCtrlError::GDI_RESOURCE_FAILED, _T("Failed to create bitmap"));
        return;
    }
    
    CBitmap* pOldBitmap = memDC.SelectObject(&bitmap);
    memDC.SetMapMode(pDC->GetMapMode());
    
    if (!pChart->GetSafeHwnd())
    {
        memDC.SelectObject(pOldBitmap);
        pChart->ReleaseDC(pDC);
        HandleError(DataListCtrlError::WINDOW_CREATION_FAILED, _T("Invalid window handle"));
        return;
    }
    
    pChart->plot_data_to_dc(&memDC);
    
    // Cache the bitmap if caching is enabled
    if (m_cacheEnabled)
    {
        auto cachedBitmap = std::make_unique<CBitmap>();
        cachedBitmap->Attach(bitmap.GetSafeHandle());
        m_cache->SetCachedBitmap(imageIndex, std::move(cachedBitmap), infos->display_mode);
    }
    
    infos->image_list.Replace(imageIndex, &bitmap, nullptr);
    
    // Cleanup
    memDC.SelectObject(pOldBitmap);
    pChart->ReleaseDC(pDC);
}

void DataListCtrl_Row_Optimized::CreatePlotBitmap(DataListCtrlInfos* infos, int imageIndex,
                                                 ChartData* pChart, CBitmap& bitmap)
{
    if (!pChart)
        return;
    
    CRect client_rect;
    pChart->GetClientRect(&client_rect);
    
    CDC* pDC = pChart->GetDC();
    if (!pDC)
    {
        HandleError(DataListCtrlError::GDI_RESOURCE_FAILED, _T("Failed to get device context"));
        return;
    }
    
    if (!bitmap.CreateBitmap(client_rect.right, client_rect.bottom, pDC->GetDeviceCaps(PLANES), 4, nullptr))
    {
        pChart->ReleaseDC(pDC);
        HandleError(DataListCtrlError::GDI_RESOURCE_FAILED, _T("Failed to create bitmap"));
        return;
    }
    
    CDC memDC;
    if (!memDC.CreateCompatibleDC(pDC))
    {
        pChart->ReleaseDC(pDC);
        HandleError(DataListCtrlError::GDI_RESOURCE_FAILED, _T("Failed to create compatible DC"));
        return;
    }
    
    CBitmap* pOldBitmap = memDC.SelectObject(&bitmap);
    memDC.SetMapMode(pDC->GetMapMode());
    
    pChart->plot_data_to_dc(&memDC);
    
    memDC.SelectObject(pOldBitmap);
    pChart->ReleaseDC(pDC);
}

void DataListCtrl_Row_Optimized::CreatePlotBitmap(DataListCtrlInfos* infos, int imageIndex,
                                                 ChartSpikeBar* pChart, CBitmap& bitmap)
{
    if (!pChart)
        return;
    
    CRect client_rect;
    pChart->GetClientRect(&client_rect);
    
    CDC* pDC = pChart->GetDC();
    if (!pDC)
    {
        HandleError(DataListCtrlError::GDI_RESOURCE_FAILED, _T("Failed to get device context"));
        return;
    }
    
    if (!bitmap.CreateBitmap(client_rect.right, client_rect.bottom, pDC->GetDeviceCaps(PLANES), 4, nullptr))
    {
        pChart->ReleaseDC(pDC);
        HandleError(DataListCtrlError::GDI_RESOURCE_FAILED, _T("Failed to create bitmap"));
        return;
    }
    
    CDC memDC;
    if (!memDC.CreateCompatibleDC(pDC))
    {
        pChart->ReleaseDC(pDC);
        HandleError(DataListCtrlError::GDI_RESOURCE_FAILED, _T("Failed to create compatible DC"));
        return;
    }
    
    CBitmap* pOldBitmap = memDC.SelectObject(&bitmap);
    memDC.SetMapMode(pDC->GetMapMode());
    
    pChart->plot_data_to_dc(&memDC);
    
    memDC.SelectObject(pOldBitmap);
    pChart->ReleaseDC(pDC);
}

// Utility methods
CString DataListCtrl_Row_Optimized::ExtractFilename(const CString& fullPath) const
{
    int lastSlash = fullPath.ReverseFind('\\');
    if (lastSlash >= 0)
    {
        return fullPath.Mid(lastSlash + 1);
    }
    return fullPath;
}

void DataListCtrl_Row_Optimized::TruncateFilename(CString& filename) const
{
    if (filename.GetLength() > DataListCtrlConstants::MAX_FILENAME_LENGTH)
    {
        filename = filename.Left(DataListCtrlConstants::FILENAME_TRUNCATE_LENGTH) + _T("...");
    }
}

bool DataListCtrl_Row_Optimized::IsValidIndex(int index, int maxCount) const
{
    return index >= 0 && index < maxCount;
}

bool DataListCtrl_Row_Optimized::IsValidDisplayMode(int mode) const
{
        return mode >= DataListCtrlConfigConstants::DISPLAY_MODE_EMPTY &&
           mode <= DataListCtrlConfigConstants::DISPLAY_MODE_SPIKE;
}

void DataListCtrl_Row_Optimized::HandleError(DataListCtrlError error, const CString& message)
{
    LogError(message);
    throw DataListCtrlException(error, message);
}

void DataListCtrl_Row_Optimized::LogError(const CString& message) const
{
    TRACE(_T("DataListCtrl_Row_Optimized Error: %s\n"), message);
}

// Serialization helpers
void DataListCtrl_Row_Optimized::SerializeStoring(CArchive& ar)
{
    m_version = 2;
    ar << m_version;
    ar << m_index;
    
    SerializeStrings(ar, true);
    SerializeObjects(ar, true);
}

void DataListCtrl_Row_Optimized::SerializeLoading(CArchive& ar)
{
    ar >> m_version;
    ar >> m_index;
    
    SerializeStrings(ar, false);
    SerializeObjects(ar, false);
}

void DataListCtrl_Row_Optimized::SerializeStrings(CArchive& ar, bool isStoring)
{
    constexpr auto string_count = 8;
    
    if (isStoring)
    {
        ar << string_count;
        ar << m_comment;
        ar << m_dataFileName;
        ar << m_sensillumName;
        ar << m_stimulus1;
        ar << m_concentration1;
        ar << m_stimulus2;
        ar << m_concentration2;
        ar << m_nSpikes;
        ar << m_flag;
        ar << m_date;
    }
    else
    {
        int string_count_read;
        ar >> string_count_read;
        ar >> m_comment;
        string_count_read--;
        ar >> m_dataFileName;
        string_count_read--;
        ar >> m_sensillumName;
        string_count_read--;
        ar >> m_stimulus1;
        string_count_read--;
        ar >> m_concentration1;
        string_count_read--;
        if (m_version > 1)
        {
            ar >> m_stimulus2;
            string_count_read--;
            ar >> m_concentration2;
            string_count_read--;
        }
        ar >> m_nSpikes;
        string_count_read--;
        ar >> m_flag;
        string_count_read--;
        ar >> m_date;
        string_count_read--;
    }
}

void DataListCtrl_Row_Optimized::SerializeObjects(CArchive& ar, bool isStoring)
{
    constexpr auto object_count = 3;
    
    if (isStoring)
    {
        ar << object_count;
        if (m_pChartDataWnd)
            m_pChartDataWnd->Serialize(ar);
        if (m_pChartSpikeWnd)
            m_pChartSpikeWnd->Serialize(ar);
        ar << m_insectId;
    }
    else
    {
        int object_count_read;
        ar >> object_count_read;
        ASSERT(object_count_read >= 2);
        
        if (m_pChartDataWnd)
            m_pChartDataWnd->Serialize(ar);
        object_count_read--;
        
        if (m_pChartSpikeWnd)
            m_pChartSpikeWnd->Serialize(ar);
        object_count_read--;
        
        if (object_count_read > 0)
            ar >> m_insectId;
    }
}
