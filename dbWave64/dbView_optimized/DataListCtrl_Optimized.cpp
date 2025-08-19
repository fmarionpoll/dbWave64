#include "StdAfx.h"
#include "DataListCtrl_Optimized.h"
#include "DataListCtrl_Configuration.h"
#include "DataListCtrl_Row_Optimized.h"
#include "DataListCtrl_SupportingClasses.h"
#include "DataListCtrl_Optimized_Infos.h"
#include <future>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// Static constant definitions
const std::chrono::milliseconds DataListCtrlConstants::SCROLL_THROTTLE_TIME{50};

// Column configuration arrays (matching original DataListCtrl)
int g_column_width[DLC_N_COLUMNS] = {
    1,
    10, 300, 15, 30,
    30, 50, 40, 40,
    40, 40
};

CString g_column_headers_[DLC_N_COLUMNS] = {
    _T(""),
    _T("#"), _T("data"), _T("insect ID"), _T("sensillum"),
    _T("stim1"), _T("conc1"), _T("stim2"), _T("conc2"),
    _T("spikes"), _T("flag")
};

int g_column_format_[DLC_N_COLUMNS] = {
    LVCFMT_LEFT,
    LVCFMT_CENTER, LVCFMT_CENTER, LVCFMT_CENTER, LVCFMT_CENTER,
    LVCFMT_CENTER, LVCFMT_CENTER, LVCFMT_CENTER, LVCFMT_CENTER,
    LVCFMT_CENTER, LVCFMT_CENTER
};

int g_column_index_[DLC_N_COLUMNS] = {
    0,
    DLC_COLUMN_INDEX, DLC_COLUMN_CURVE, DLC_COLUMN_INSECT, DLC_COLUMN_SENSI,
    DLC_COLUMN_STIM1, DLC_COLUMN_CONC1, DLC_COLUMN_STIM2, DLC_COLUMN_CONC2,
    DLC_COLUMN_NBSPK, DLC_COLUMN_FLAG
};

// Exception implementation - using definition from DataListCtrl_SupportingClasses.h

// Cache implementation - using definition from DataListCtrl_SupportingClasses.h

// Main DataListCtrl_Optimized implementation
// Note: IMPLEMENT_DYNAMIC removed due to MFC compatibility issues
// If dynamic class support is needed, use DECLARE_DYNCREATE/IMPLEMENT_DYNCREATE instead

DataListCtrl_Optimized::DataListCtrl_Optimized()
    : m_cache(std::make_unique<DataListCtrlCache>())
    , m_infos(nullptr)
    , m_performanceMonitoringEnabled(true)
    , m_initialized(false)
    , m_cachingEnabled(true)
    , m_threadSafe(true)
    , m_parentWindow(nullptr)
    , m_updateCancelled(false)
{
}

DataListCtrl_Optimized::~DataListCtrl_Optimized()
{
    CancelAsyncUpdate();
    SaveColumnWidths(); // Save column widths on destruction
}



DataListCtrl_Optimized::DataListCtrl_Optimized(DataListCtrl_Optimized&& other) noexcept
    : m_rows(std::move(other.m_rows))
    , m_cache(std::move(other.m_cache))
    , m_config(std::move(other.m_config))
    , m_infos(other.m_infos)
    , m_performanceMetrics(std::move(other.m_performanceMetrics))
    , m_performanceMonitoringEnabled(other.m_performanceMonitoringEnabled)
    , m_lastScrollTime(std::move(other.m_lastScrollTime))
    , m_initialized(other.m_initialized)
    , m_cachingEnabled(other.m_cachingEnabled)
    , m_threadSafe(other.m_threadSafe)
    , m_parentWindow(other.m_parentWindow)
    , m_imageList(std::move(other.m_imageList))
    , m_emptyBitmap(std::move(other.m_emptyBitmap))
    , m_asyncUpdateFuture(std::move(other.m_asyncUpdateFuture))
    , m_updateCancelled(other.m_updateCancelled.load())
{
    other.m_infos = nullptr;
    other.m_parentWindow = nullptr;
    other.m_initialized = false;
    other.m_cachingEnabled = true;
    other.m_threadSafe = true;
    other.m_updateCancelled = false;
}

DataListCtrl_Optimized& DataListCtrl_Optimized::operator=(DataListCtrl_Optimized&& other) noexcept
{
    if (this != &other)
    {
        CancelAsyncUpdate();
        
        m_rows = std::move(other.m_rows);
        m_cache = std::move(other.m_cache);
        m_config = std::move(other.m_config);
        m_infos = other.m_infos;
        m_performanceMetrics = std::move(other.m_performanceMetrics);
        m_performanceMonitoringEnabled = other.m_performanceMonitoringEnabled;
        m_lastScrollTime = std::move(other.m_lastScrollTime);
        m_initialized = other.m_initialized;
        m_cachingEnabled = other.m_cachingEnabled;
        m_threadSafe = other.m_threadSafe;
        m_parentWindow = other.m_parentWindow;
        m_imageList = std::move(other.m_imageList);
        m_emptyBitmap = std::move(other.m_emptyBitmap);
        m_asyncUpdateFuture = std::move(other.m_asyncUpdateFuture);
        m_updateCancelled = other.m_updateCancelled.load();
        
        other.m_infos = nullptr;
        other.m_parentWindow = nullptr;
        other.m_initialized = false;
        other.m_cachingEnabled = true;
        other.m_threadSafe = true;
        other.m_updateCancelled = false;
    }
    return *this;
}

void DataListCtrl_Optimized::Initialize(const DataListCtrlConfiguration& config)
{
    try
    {
        // Copy configuration settings instead of using assignment operator
        m_config.GetDisplaySettings() = config.GetDisplaySettings();
        m_config.GetTimeSettings() = config.GetTimeSettings();
        m_config.GetAmplitudeSettings() = config.GetAmplitudeSettings();
        m_config.GetUISettings() = config.GetUISettings();
        m_config.GetPerformanceSettings() = config.GetPerformanceSettings();
        m_config.SetColumns(config.GetColumns());
        
        SetupDefaultConfiguration();
        InitializeColumns();
        InitializeImageList();
        CreateEmptyBitmap();
        m_initialized = true;
    }
    catch (const std::exception& e)
    {
        HandleError(DataListCtrlError::INVALID_PARAMETER, CString(e.what()));
    }
}

void DataListCtrl_Optimized::InitializeColumns()
{
    try
    {
        if (!GetSafeHwnd())
            return;

        DeleteAllItems();
        
        // Check if header control exists before accessing it
        CHeaderCtrl* pHeader = GetHeaderCtrl();
        if (pHeader && pHeader->GetSafeHwnd())
        {
            while (pHeader->GetItemCount() > 0)
                DeleteColumn(0);
        }
        
        // Add columns using the original column configuration
        for (int i = 0; i < DLC_N_COLUMNS; i++)
        {
            InsertColumn(i, g_column_headers_[i], g_column_format_[i], g_column_width[i], -1);
        }
        
        // Set image width for the curve column (column 2)
        if (m_infos)
            m_infos->image_width = g_column_width[DLC_COLUMN_CURVE];
    }
    catch (const std::exception& e)
    {
        HandleError(DataListCtrlError::INVALID_PARAMETER, CString(e.what()));
    }
}

void DataListCtrl_Optimized::SetupColumns()
{
    if (!m_initialized)
        return;
    
    try
    {
        // Check if the control has been created
        if (!GetSafeHwnd())
        {
            // Control not created yet, just return
            return;
        }
        
        // Clear existing columns
        DeleteAllItems();
        
        // Check if header control exists before accessing it
        CHeaderCtrl* pHeader = GetHeaderCtrl();
        if (pHeader && pHeader->GetSafeHwnd())
        {
            while (pHeader->GetItemCount() > 0)
            {
                DeleteColumn(0);
            }
        }
        
        // Add columns using the original column configuration
        for (int i = 0; i < DLC_N_COLUMNS; i++)
        {
            InsertColumn(i, g_column_headers_[i], g_column_format_[i], g_column_width[i], -1);
        }
        
        // Set image width for the curve column (column 2)
        if (m_infos)
        {
            m_infos->image_width = g_column_width[DLC_COLUMN_CURVE];
        }
    }
    catch (const std::exception& e)
    {
        HandleError(DataListCtrlError::INVALID_PARAMETER, CString(e.what()));
    }
}

void DataListCtrl_Optimized::SetupImageList()
{
    if (!m_initialized)
        return;
    
    try
    {
        // Check if the control has been created
        if (!GetSafeHwnd())
        {
            // Control not created yet, just return
            return;
        }
        
        const auto& displayConfig = m_config.GetDisplayConfig();
        m_imageList = std::make_unique<CImageList>();
        m_imageList->Create(displayConfig.GetImageWidth(), displayConfig.GetImageHeight(), 
                           ILC_COLOR24 | ILC_MASK, 0, 1);
        SetImageList(m_imageList.get(), LVSIL_SMALL);
    }
    catch (const std::exception& e)
    {
        HandleError(DataListCtrlError::GDI_RESOURCE_FAILED, CString(e.what()));
    }
}

void DataListCtrl_Optimized::SetParentWindow(CWnd* parent)
{
    m_parentWindow = parent;
}

void DataListCtrl_Optimized::SetRowCount(int count)
{
    try
    {
        LockIfThreadSafe();
        
        // Clear existing rows
        ClearRows();
        
        // Resize vector
        m_rows.resize(count);
        
        // Set item count for virtual list control
        if (GetSafeHwnd())
        {
            SetItemCount(count);
        }
        
        UnlockIfThreadSafe();
    }
    catch (const std::exception& e)
    {
        HandleError(DataListCtrlError::INVALID_PARAMETER, CString(e.what()));
    }
}

void DataListCtrl_Optimized::ClearRows()
{
    try
    {
        LockIfThreadSafe();
        
        m_rows.clear();
        
        // Check if the control has been created before calling DeleteAllItems
        if (GetSafeHwnd())
        {
            DeleteAllItems();
        }
        
        if (m_cache)
        {
            m_cache->Clear();
        }
        
        UnlockIfThreadSafe();
    }
    catch (const std::exception& e)
    {
        HandleError(DataListCtrlError::INVALID_PARAMETER, CString(e.what()));
    }
}

void DataListCtrl_Optimized::AddRow(std::unique_ptr<DataListCtrl_Row_Optimized> row)
{
    try
    {
        LockIfThreadSafe();
        
        int index = static_cast<int>(m_rows.size());
        m_rows.push_back(std::move(row));
        
        // Add item to list control
        if (GetSafeHwnd())
        {
            InsertItem(index, _T(""));
        }
        
        UnlockIfThreadSafe();
    }
    catch (const std::exception& e)
    {
        HandleError(DataListCtrlError::INVALID_PARAMETER, CString(e.what()));
    }
}

void DataListCtrl_Optimized::RemoveRow(int index)
{
    try
    {
        if (!IsValidIndex(index))
        {
            HandleError(DataListCtrlError::INVALID_INDEX, 
                       _T("Invalid row index: ") + CString(std::to_string(index).c_str()));
            return;
        }
        
        LockIfThreadSafe();
        
        // Remove from vector
        if (index < static_cast<int>(m_rows.size()))
        {
            m_rows.erase(m_rows.begin() + index);
        }
        
        // Remove from list control
        if (GetSafeHwnd())
        {
            DeleteItem(index);
        }
        
        // Invalidate cache
        if (m_cache)
        {
            m_cache->InvalidateCache(index);
        }
        
        UnlockIfThreadSafe();
    }
    catch (const std::exception& e)
    {
        HandleError(DataListCtrlError::INVALID_PARAMETER, CString(e.what()));
    }
}

void DataListCtrl_Optimized::UpdateRow(int index)
{
    try
    {
        if (!IsValidIndex(index))
        {
            HandleError(DataListCtrlError::INVALID_INDEX, 
                       _T("Invalid row index: ") + CString(std::to_string(index).c_str()));
            return;
        }
        
        ProcessRowUpdate(index);
    }
    catch (const std::exception& e)
    {
        HandleError(DataListCtrlError::INVALID_PARAMETER, CString(e.what()));
    }
}

void DataListCtrl_Optimized::SetDisplayMode(int mode)
{
    try
    {
        if (!IsValidDisplayMode(mode))
        {
            HandleError(DataListCtrlError::INVALID_PARAMETER, 
                       _T("Invalid display mode: ") + CString(std::to_string(mode).c_str()));
            return;
        }
        
        LockIfThreadSafe();
        
        // Update configuration
        auto& displayConfig = m_config.GetDisplayConfig();
        displayConfig.SetDisplayMode(mode);
        
        // Clear cache if mode changed
        if (m_cache)
        {
            m_cache->Clear();
        }
        
        // Refresh display
        RefreshDisplay();
        
        UnlockIfThreadSafe();
    }
    catch (const std::exception& e)
    {
        HandleError(DataListCtrlError::INVALID_PARAMETER, CString(e.what()));
    }
}

void DataListCtrl_Optimized::SetTimeSpan(float first, float last)
{
    try
    {
        LockIfThreadSafe();
        
        // Update configuration
        auto& timeConfig = m_config.GetTimeSettings();
        timeConfig.SetTimeSpan(first, last);
        
        // Clear cache
        if (m_cache)
        {
            m_cache->Clear();
        }
        
        // Refresh display
        RefreshDisplay();
        
        UnlockIfThreadSafe();
    }
    catch (const std::exception& e)
    {
        HandleError(DataListCtrlError::INVALID_PARAMETER, CString(e.what()));
    }
}

void DataListCtrl_Optimized::SetAmplitudeSpan(float span)
{
    try
    {
        LockIfThreadSafe();
        
        // Update configuration
        auto& amplitudeConfig = m_config.GetAmplitudeSettings();
        amplitudeConfig.SetMvSpan(span);
        
        // Clear cache
        if (m_cache)
        {
            m_cache->Clear();
        }
        
        // Refresh display
        RefreshDisplay();
        
        UnlockIfThreadSafe();
    }
    catch (const std::exception& e)
    {
        HandleError(DataListCtrlError::INVALID_PARAMETER, CString(e.what()));
    }
}

void DataListCtrl_Optimized::SetDisplayFileName(bool display)
{
    try
    {
        LockIfThreadSafe();
        
        // Update configuration
        auto& uiConfig = m_config.GetUISettings();
        uiConfig.SetDisplayFileName(display);
        
        // Clear cache
        if (m_cache)
        {
            m_cache->Clear();
        }
        
        // Refresh display
        RefreshDisplay();
        
        UnlockIfThreadSafe();
    }
    catch (const std::exception& e)
    {
        HandleError(DataListCtrlError::INVALID_PARAMETER, CString(e.what()));
    }
}

void DataListCtrl_Optimized::RefreshDisplay()
{
    try
    {
        if (!m_initialized)
            return;
        
        // Check if the control has been created
        if (!GetSafeHwnd())
        {
            // Control not created yet, just return
            return;
        }
        
        // Since we're no longer using async loading, always use synchronous update
        Invalidate();
        UpdateWindow();
    }
    catch (const std::exception& e)
    {
        HandleError(DataListCtrlError::UI_UPDATE_FAILED, CString(e.what()));
    }
}

void DataListCtrl_Optimized::SetConfiguration(const DataListCtrlConfiguration& config)
{
    try
    {
        LockIfThreadSafe();
        
        // Copy configuration settings instead of using assignment operator
        m_config.GetDisplaySettings() = config.GetDisplaySettings();
        m_config.GetTimeSettings() = config.GetTimeSettings();
        m_config.GetAmplitudeSettings() = config.GetAmplitudeSettings();
        m_config.GetUISettings() = config.GetUISettings();
        m_config.GetPerformanceSettings() = config.GetPerformanceSettings();
        m_config.SetColumns(config.GetColumns());
        
        // Reinitialize if already initialized
        if (m_initialized)
        {
            SetupColumns();
            SetupImageList();
        }
        
        UnlockIfThreadSafe();
    }
    catch (const std::exception& e)
    {
        HandleError(DataListCtrlError::INVALID_PARAMETER, CString(e.what()));
    }
}

void DataListCtrl_Optimized::ResetPerformanceMetrics()
{
    try
    {
        LockIfThreadSafe();
        m_performanceMetrics.Reset();
        UnlockIfThreadSafe();
    }
    catch (const std::exception& e)
    {
        HandleError(DataListCtrlError::INVALID_PARAMETER, CString(e.what()));
    }
}

void DataListCtrl_Optimized::ClearCache()
{
    try
    {
        if (m_cache)
        {
            m_cache->Clear();
        }
    }
    catch (const std::exception& e)
    {
        HandleError(DataListCtrlError::INVALID_PARAMETER, CString(e.what()));
    }
}

// Public interface methods (maintaining compatibility)
void DataListCtrl_Optimized::init_columns(std::vector<int>* width_columns)
{
    try
    {
        if (!width_columns)
        {
            HandleError(DataListCtrlError::INVALID_PARAMETER, _T("Invalid width columns parameter"));
            return;
        }
        
        LockIfThreadSafe();
        
        // Clear existing columns
        DeleteAllItems();
        while (GetHeaderCtrl()->GetItemCount() > 0)
        {
            DeleteColumn(0);
        }
        
        // Add columns with specified widths
        for (size_t i = 0; i < width_columns->size(); ++i)
        {
            CString header;
            header.Format(_T("Column %d"), i + 1);
            InsertColumn(i, header, LVCFMT_LEFT, (*width_columns)[i]);
        }
        
        UnlockIfThreadSafe();
    }
    catch (const std::exception& e)
    {
        HandleError(DataListCtrlError::INVALID_PARAMETER, CString(e.what()));
    }
}

void DataListCtrl_Optimized::set_amplitude_span(float span)
{
    SetAmplitudeSpan(span);
}

void DataListCtrl_Optimized::set_display_file_name(bool display)
{
    SetDisplayFileName(display);
}

void DataListCtrl_Optimized::set_time_intervals(float first, float last)
{
    SetTimeSpan(first, last);
}

void DataListCtrl_Optimized::set_data_transform(int transform)
{
    try
    {
        LockIfThreadSafe();
        
        // Update configuration
        auto& displayConfig = m_config.GetDisplayConfig();
        displayConfig.SetDataTransform(transform);
        
        // Clear cache
        if (m_cache)
        {
            m_cache->Clear();
        }
        
        // Refresh display
        RefreshDisplay();
        
        UnlockIfThreadSafe();
    }
    catch (const std::exception& e)
    {
        HandleError(DataListCtrlError::INVALID_PARAMETER, CString(e.what()));
    }
}

void DataListCtrl_Optimized::set_spike_plot_mode(int mode)
{
    try
    {
        LockIfThreadSafe();
        
        // Update configuration
        auto& displayConfig = m_config.GetDisplayConfig();
        displayConfig.SetSpikePlotMode(mode);
        
        // Clear cache
        if (m_cache)
        {
            m_cache->Clear();
        }
        
        // Refresh display
        RefreshDisplay();
        
        UnlockIfThreadSafe();
    }
    catch (const std::exception& e)
    {
        HandleError(DataListCtrlError::INVALID_PARAMETER, CString(e.what()));
    }
}

void DataListCtrl_Optimized::set_selected_class(int classIndex)
{
    try
    {
        LockIfThreadSafe();
        
        // Update configuration
        auto& displayConfig = m_config.GetDisplayConfig();
        displayConfig.SetSelectedClass(classIndex);
        
        // Clear cache
        if (m_cache)
        {
            m_cache->Clear();
        }
        
        // Refresh display
        RefreshDisplay();
        
        UnlockIfThreadSafe();
    }
    catch (const std::exception& e)
    {
        HandleError(DataListCtrlError::INVALID_PARAMETER, CString(e.what()));
    }
}

// Note: OnInitialUpdate() removed - this is a CView method, not appropriate for CListCtrl
// Initialization should be done in the constructor or a separate Initialize() method

void DataListCtrl_Optimized::OnDestroy()
{
    try
    {
        SaveColumnWidths(); // Save column widths before destruction
        CancelAsyncUpdate();
        CListCtrl::OnDestroy();
    }
    catch (const std::exception& e)
    {
        HandleError(DataListCtrlError::UI_UPDATE_FAILED, CString(e.what()));
    }
}

void DataListCtrl_Optimized::OnSize(UINT nType, int cx, int cy)
{
    try
    {
        CListCtrl::OnSize(nType, cx, cy);
        
        // Auto-adjust signal column width when control is resized
        if (m_initialized && cx > 0)
        {
            fit_columns_to_size(cx);
        }
    }
    catch (const std::exception& e)
    {
        HandleError(DataListCtrlError::UI_UPDATE_FAILED, CString(e.what()));
    }
}

// Message map handlers
BEGIN_MESSAGE_MAP(DataListCtrl_Optimized, CListCtrl)
    ON_WM_VSCROLL()
    ON_WM_HSCROLL()
    ON_WM_KEYUP()
    ON_WM_CHAR()
    ON_WM_KEYDOWN()
    ON_WM_DESTROY()
    ON_WM_SIZE()
    ON_NOTIFY_REFLECT(LVN_GETDISPINFO, OnGetDisplayInfo)
END_MESSAGE_MAP()

void DataListCtrl_Optimized::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    try
    {
        HandleScrollEvent(nSBCode, nPos);
        CListCtrl::OnVScroll(nSBCode, nPos, pScrollBar);
    }
    catch (const std::exception& e)
    {
        HandleError(DataListCtrlError::UI_UPDATE_FAILED, CString(e.what()));
    }
}

void DataListCtrl_Optimized::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    try
    {
        HandleScrollEvent(nSBCode, nPos);
        CListCtrl::OnHScroll(nSBCode, nPos, pScrollBar);
    }
    catch (const std::exception& e)
    {
        HandleError(DataListCtrlError::UI_UPDATE_FAILED, CString(e.what()));
    }
}

void DataListCtrl_Optimized::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    try
    {
        CListCtrl::OnKeyUp(nChar, nRepCnt, nFlags);
    }
    catch (const std::exception& e)
    {
        HandleError(DataListCtrlError::UI_UPDATE_FAILED, CString(e.what()));
    }
}

void DataListCtrl_Optimized::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    try
    {
        CListCtrl::OnChar(nChar, nRepCnt, nFlags);
    }
    catch (const std::exception& e)
    {
        HandleError(DataListCtrlError::UI_UPDATE_FAILED, CString(e.what()));
    }
}

void DataListCtrl_Optimized::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    try
    {
        CListCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
    }
    catch (const std::exception& e)
    {
        HandleError(DataListCtrlError::UI_UPDATE_FAILED, CString(e.what()));
    }
}

void DataListCtrl_Optimized::OnGetDisplayInfo(NMHDR* pNMHDR, LRESULT* pResult)
{
    try
    {
        LV_DISPINFO* pDispInfo = reinterpret_cast<LV_DISPINFO*>(pNMHDR);
        HandleDisplayInfoRequest(pDispInfo);
        *pResult = 0;
    }
    catch (const std::exception& e)
    {
        HandleError(DataListCtrlError::UI_UPDATE_FAILED, CString(e.what()));
        *pResult = 1;
    }
}

// Private implementation methods

void DataListCtrl_Optimized::InitializeImageList()
{
    if (!GetSafeHwnd())
        return;
    
    const auto& display_config = m_config.GetDisplayConfig();
    m_imageList = std::make_unique<CImageList>();
    m_imageList->Create(display_config.GetImageWidth(), display_config.GetImageHeight(), 
                       ILC_COLOR24 | ILC_MASK, 0, 1);
    SetImageList(m_imageList.get(), LVSIL_SMALL);
}

void DataListCtrl_Optimized::CreateEmptyBitmap()
{
    try
    {
        const auto& display_config = m_config.GetDisplayConfig();
        m_emptyBitmap = std::make_unique<CBitmap>();
        
        CDC dc;
        dc.CreateCompatibleDC(nullptr);
        
        m_emptyBitmap->CreateCompatibleBitmap(&dc, display_config.GetImageWidth(), display_config.GetImageHeight());
        
        CDC memDC;
        memDC.CreateCompatibleDC(&dc);
        CBitmap* pOldBitmap = memDC.SelectObject(m_emptyBitmap.get());
        
        // Fill with light grey
        CBrush brush(RGB(192, 192, 192));
        CRect rect(0, 0, display_config.GetImageWidth(), display_config.GetImageHeight());
        memDC.FillRect(&rect, &brush);
        
        memDC.SelectObject(pOldBitmap);
    }
    catch (const std::exception& e)
    {
        HandleError(DataListCtrlError::GDI_RESOURCE_FAILED, CString(e.what()));
    }
}

void DataListCtrl_Optimized::SetupDefaultConfiguration()
{
    // Set up default configuration
    DataListCtrlConfiguration::ColumnConfig col1, col2, col3, col4;
    col1.width = 50; col1.header = _T("Index"); col1.index = 0;
    col2.width = 100; col2.header = _T("Data"); col2.index = 1;
    col3.width = 100; col3.header = _T("Spikes"); col3.index = 2;
    col4.width = 150; col4.header = _T("File"); col4.index = 3;
    
    std::vector<DataListCtrlConfiguration::ColumnConfig> columns = {col1, col2, col3, col4};
    m_config.SetColumns(columns);
}

void DataListCtrl_Optimized::EnsureRowExists(int index)
{
    if (index >= static_cast<int>(m_rows.size()))
    {
        m_rows.resize(index + 1);
    }
    
    if (!m_rows[index])
    {
        m_rows[index] = std::make_unique<DataListCtrl_Row_Optimized>(index);
    }
}

void DataListCtrl_Optimized::ProcessRowUpdate(int index)
{
    try
    {
        EnsureRowExists(index);
        
        if (m_rows[index])
        {
            // Process the row update
            // This would typically involve updating the display data
            m_rows[index]->ResetDisplayProcessed();
        }
    }
    catch (const std::exception& e)
    {
        HandleError(DataListCtrlError::INVALID_PARAMETER, CString(e.what()));
    }
}

void DataListCtrl_Optimized::BatchProcessRows(int startIndex, int endIndex)
{
    try
    {
        for (int i = startIndex; i <= endIndex && i < static_cast<int>(m_rows.size()); ++i)
        {
            ProcessRowUpdate(i);
        }
    }
    catch (const std::exception& e)
    {
        HandleError(DataListCtrlError::INVALID_PARAMETER, CString(e.what()));
    }
}

void DataListCtrl_Optimized::UpdateDisplayInfo(LV_DISPINFO* pDispInfo)
{
    try
    {
        if (!pDispInfo)
            return;
        
        int index = pDispInfo->item.iItem;
        if (!IsValidIndex(index))
            return;
        
        EnsureRowExists(index);
        
        if (m_rows[index])
        {
            // Update the display info for the row
            // This would typically involve setting the item text and image
            if (pDispInfo->item.mask & LVIF_TEXT)
            {
                CString text;
                text.Format(_T("Row %d"), index);
                _tcscpy_s(pDispInfo->item.pszText, pDispInfo->item.cchTextMax, text);
            }
            
            if (pDispInfo->item.mask & LVIF_IMAGE)
            {
                pDispInfo->item.iImage = index;
            }
        }
    }
    catch (const std::exception& e)
    {
        HandleError(DataListCtrlError::UI_UPDATE_FAILED, CString(e.what()));
    }
}

void DataListCtrl_Optimized::HandleDisplayInfoRequest(LV_DISPINFO* pDispInfo)
{
    try
    {
        m_performanceMetrics.displayInfoCalls++;
        UpdateDisplayInfo(pDispInfo);
    }
    catch (const std::exception& e)
    {
        HandleError(DataListCtrlError::UI_UPDATE_FAILED, CString(e.what()));
    }
}

void DataListCtrl_Optimized::ProcessDisplayMode(int rowIndex, int displayMode)
{
    try
    {
        EnsureRowExists(rowIndex);
        
        if (m_rows[rowIndex])
        {
            // Process the display mode for the row
            // This would typically involve updating the row's display mode
        }
    }
    catch (const std::exception& e)
    {
        HandleError(DataListCtrlError::INVALID_PARAMETER, CString(e.what()));
    }
}

void DataListCtrl_Optimized::UpdateCache(int index, int displayMode)
{
    try
    {
        if (m_cache && m_cachingEnabled)
        {
            EnsureRowExists(index);
            
            if (m_rows[index])
            {
                // Create a copy of the row for caching
                auto cachedRow = std::make_unique<DataListCtrl_Row_Optimized>(*m_rows[index]);
                m_cache->SetCachedRow(index, cachedRow.get(), displayMode);
            }
        }
    }
    catch (const std::exception& e)
    {
        HandleError(DataListCtrlError::INVALID_PARAMETER, CString(e.what()));
    }
}

void DataListCtrl_Optimized::InvalidateCacheForRow(int index)
{
    try
    {
        if (m_cache)
        {
            m_cache->InvalidateCache(index);
        }
    }
    catch (const std::exception& e)
    {
        HandleError(DataListCtrlError::INVALID_PARAMETER, CString(e.what()));
    }
}

void DataListCtrl_Optimized::HandleError(DataListCtrlError error, const CString& message)
{
    LogError(message);
    throw DataListCtrlException(error, message);
}

void DataListCtrl_Optimized::LogError(const CString& message) const
{
    TRACE(_T("DataListCtrl_Optimized Error: %s\n"), message);
}

void DataListCtrl_Optimized::LogPerformance(const CString& operation, std::chrono::microseconds duration) const
{
    TRACE(_T("DataListCtrl_Optimized Performance: %s took %lld microseconds\n"), 
          operation, duration.count());
}

bool DataListCtrl_Optimized::IsValidIndex(int index) const
{
    return index >= 0 && index < static_cast<int>(m_rows.size());
}

bool DataListCtrl_Optimized::IsValidDisplayMode(int mode) const
{
    return mode >= 0 && mode <= 2; // Assuming 0-2 are valid display modes
}

void DataListCtrl_Optimized::ValidateConfiguration() const
{
    if (!m_config.ValidateConfiguration())
    {
        throw DataListCtrlException(DataListCtrlError::INVALID_PARAMETER, 
                                   _T("Invalid configuration"));
    }
}

void DataListCtrl_Optimized::StartAsyncUpdate()
{
    try
    {
        if (m_asyncUpdateFuture.valid())
        {
            // Cancel existing update
            CancelAsyncUpdate();
        }
        
        m_updateCancelled = false;
        m_asyncUpdateFuture = std::async(std::launch::async, [this]() -> void {
            this->ProcessAsyncUpdate();
        });
    }
    catch (const std::exception& e)
    {
        HandleError(DataListCtrlError::ASYNC_OPERATION_FAILED, CString(e.what()));
    }
}

void DataListCtrl_Optimized::CancelAsyncUpdate()
{
    try
    {
        m_updateCancelled = true;
        
        if (m_asyncUpdateFuture.valid())
        {
            m_asyncUpdateFuture.wait();
        }
    }
    catch (const std::exception& e)
    {
        HandleError(DataListCtrlError::ASYNC_OPERATION_FAILED, CString(e.what()));
    }
}

void DataListCtrl_Optimized::ProcessAsyncUpdate()
{
    try
    {
        // Process all visible rows
        int visibleCount = GetCountPerPage();
        int topIndex = GetTopIndex();
        
        for (int i = 0; i < visibleCount && !m_updateCancelled; ++i)
        {
            int rowIndex = topIndex + i;
            if (IsValidIndex(rowIndex))
            {
                ProcessRowUpdate(rowIndex);
            }
        }
        
        // Post message to update UI
        if (!m_updateCancelled)
        {
            PostMessage(WM_PAINT);
        }
    }
    catch (const std::exception& e)
    {
        HandleError(DataListCtrlError::ASYNC_OPERATION_FAILED, CString(e.what()));
    }
}

void DataListCtrl_Optimized::HandleScrollEvent(UINT scrollCode, UINT position)
{
    try
    {
        m_performanceMetrics.scrollEvents++;
        
        if (ShouldThrottleScroll())
        {
            return;
        }
        
        m_lastScrollTime = std::chrono::steady_clock::now();
        
        // Handle scroll event
        // This could involve preloading data for newly visible rows
    }
    catch (const std::exception& e)
    {
        HandleError(DataListCtrlError::UI_UPDATE_FAILED, CString(e.what()));
    }
}

bool DataListCtrl_Optimized::ShouldThrottleScroll() const
{
    auto now = std::chrono::steady_clock::now();
    return (now - m_lastScrollTime) < DataListCtrlConstants::SCROLL_THROTTLE_TIME;
}

void DataListCtrl_Optimized::SaveColumnWidths()
{
    try
    {
        if (!m_initialized)
            return;
            
        // Check if the control and header are still valid
        if (!GetSafeHwnd())
            return;
            
        CHeaderCtrl* pHeader = GetHeaderCtrl();
        if (!pHeader || !pHeader->GetSafeHwnd())
            return;
            
        // Save column widths to configuration
        std::vector<DataListCtrlConfiguration::ColumnConfig> columns = m_config.GetColumns();
        
        // Ensure we have enough columns
        int headerItemCount = pHeader->GetItemCount();
        if (columns.size() < static_cast<size_t>(headerItemCount))
        {
            columns.resize(headerItemCount);
        }
        
        // Save current column widths
        for (int i = 0; i < headerItemCount; ++i)
        {
            if (i < static_cast<int>(columns.size()))
            {
                columns[i].width = GetColumnWidth(i);
                // Also update the global array for compatibility
                if (i < DLC_N_COLUMNS)
                {
                    g_column_width[i] = GetColumnWidth(i);
                }
            }
        }
        
        m_config.SetColumns(columns);
        
        // Save to registry for persistence
        m_config.SaveToRegistry(_T("DataListCtrl_Optimized"));
    }
    catch (const std::exception& e)
    {
        HandleError(DataListCtrlError::INVALID_PARAMETER, CString(e.what()));
    }
}

void DataListCtrl_Optimized::LoadColumnWidths()
{
    try
    {
        if (!m_initialized)
            return;
            
        // Check if the control and header are still valid
        if (!GetSafeHwnd())
            return;
            
        CHeaderCtrl* pHeader = GetHeaderCtrl();
        if (!pHeader || !pHeader->GetSafeHwnd())
            return;
            
        // Load column widths from configuration
        const auto& columns = m_config.GetColumns();
        
        // Apply column widths to the list control
        int headerItemCount = pHeader->GetItemCount();
        for (size_t i = 0; i < columns.size() && i < static_cast<size_t>(headerItemCount); ++i)
        {
            SetColumnWidth(static_cast<int>(i), columns[i].width);
            // Also update the global array for compatibility
            if (i < DLC_N_COLUMNS)
            {
                g_column_width[i] = columns[i].width;
            }
        }
    }
    catch (const std::exception& e)
    {
        HandleError(DataListCtrlError::INVALID_PARAMETER, CString(e.what()));
    }
}

void DataListCtrl_Optimized::ApplyColumnConfiguration()
{
    try
    {
        // Load configuration from registry first
        m_config.LoadFromRegistry(_T("DataListCtrl_Optimized"));
        
        SetupColumns();
        LoadColumnWidths();
    }
    catch (const std::exception& e)
    {
        HandleError(DataListCtrlError::INVALID_PARAMETER, CString(e.what()));
    }
}

// Signal column adjustment methods (from original DataListCtrl)
void DataListCtrl_Optimized::resize_signal_column(int n_pixels)
{
    try
    {
        if (!m_initialized || !m_infos)
            return;
            
        // Check if the control is still valid
        if (!GetSafeHwnd())
            return;
            
        // Update the signal column width in the global array
        g_column_width[DLC_COLUMN_CURVE] = n_pixels;
        
        // Update the image list with new width
        if (m_imageList)
        {
            m_imageList->DeleteImageList();
        }
        
        m_infos->image_width = n_pixels;
        m_infos->image_height = m_config.GetDisplayConfig().GetImageHeight();
        
        m_imageList = std::make_unique<CImageList>();
        if (!m_imageList->Create(m_infos->image_width, m_infos->image_height, ILC_COLOR4, 10, 10))
        {
            HandleError(DataListCtrlError::GDI_RESOURCE_FAILED, _T("Failed to create image list"));
            return;
        }
        
        SetImageList(m_imageList.get(), LVSIL_SMALL);
        
        // Clear chart data for all rows to force regeneration
        for (auto& row : m_rows)
        {
            if (row)
            {
                // Clear chart data windows to force regeneration
                // This matches the original behavior
            }
        }
        
        // Update the configuration
        auto columns = m_config.GetColumns();
        if (columns.size() > DLC_COLUMN_CURVE)
        {
            columns[DLC_COLUMN_CURVE].width = n_pixels;
            m_config.SetColumns(columns);
        }
        
        // Refresh the display
        RefreshDisplay();
    }
    catch (const std::exception& e)
    {
        HandleError(DataListCtrlError::INVALID_PARAMETER, CString(e.what()));
    }
}

void DataListCtrl_Optimized::fit_columns_to_size(int n_pixels)
{
    try
    {
        if (!m_initialized)
            return;
            
        // Check if the control is still valid
        if (!GetSafeHwnd())
            return;
            
        // Calculate total width of fixed columns (excluding signal column)
        int fixed_width = 0;
        for (int i = 0; i < DLC_N_COLUMNS; ++i)
        {
            if (i != DLC_COLUMN_CURVE)
            {
                fixed_width += g_column_width[i];
            }
        }
        
        // Calculate available width for signal column
        int signal_column_width = n_pixels - fixed_width;
        
        // Ensure minimum width for signal column
        if (signal_column_width < 50)
            signal_column_width = 50;
        
        // Only adjust if the new width is different and reasonable
        if (signal_column_width != g_column_width[DLC_COLUMN_CURVE] && signal_column_width > 2)
        {
            // Update the global array first
            g_column_width[DLC_COLUMN_CURVE] = signal_column_width;
            
            // Set the column width in the list control
            SetColumnWidth(DLC_COLUMN_CURVE, signal_column_width);
            
            // Update the image list and configuration
            resize_signal_column(signal_column_width);
        }
    }
    catch (const std::exception& e)
    {
        HandleError(DataListCtrlError::INVALID_PARAMETER, CString(e.what()));
    }
}
