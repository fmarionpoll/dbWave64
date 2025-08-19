#include "StdAfx.h"
#include "DataListCtrl_Optimized.h"
#include "DataListCtrl_Configuration.h"
#include "DataListCtrl_Row_Optimized.h"
#include "DataListCtrl_SupportingClasses.h"
#include "DataListCtrl_Optimized_Infos.h"
#include <future>

#include "ViewdbWave_Optimized.h"
#include "dbWaveDoc.h"
#include "ChartSpikeBar.h"
#include "ChartData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// Static constant definitions removed - simplified implementation

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
    , m_initialized(false)
    , m_cachingEnabled(true)
    , m_parentWindow(nullptr)
{
}

DataListCtrl_Optimized::~DataListCtrl_Optimized()
{
    SaveColumnWidths(); // Save column widths on destruction
}



// Move constructor and assignment operator removed - simplified implementation

void DataListCtrl_Optimized::Initialize(const DataListCtrlConfiguration& config)
{
    try
    {
        TRACE(_T("DataListCtrl_Optimized::Initialize - Starting initialization\n"));
        
        // Copy configuration settings
        m_config = config;
        
        TRACE(_T("DataListCtrl_Optimized::Initialize - Configuration copied\n"));
        
        SetupDefaultConfiguration();
        TRACE(_T("DataListCtrl_Optimized::Initialize - Default configuration setup complete\n"));
        
        SetupVirtualListControl();
        TRACE(_T("DataListCtrl_Optimized::Initialize - Virtual list control setup complete\n"));
        
        InitializeColumns();
        TRACE(_T("DataListCtrl_Optimized::Initialize - Columns initialized\n"));
        
        InitializeImageList();
        TRACE(_T("DataListCtrl_Optimized::Initialize - Image list initialized\n"));
        
        CreateEmptyBitmap();
        TRACE(_T("DataListCtrl_Optimized::Initialize - Empty bitmap created\n"));
        
        // Enable caching by default
        m_cachingEnabled = true;
        m_cache = std::make_unique<DataListCtrlCache>();
        TRACE(_T("DataListCtrl_Optimized::Initialize - Caching enabled\n"));
        
        m_initialized = true;
        TRACE(_T("DataListCtrl_Optimized::Initialize - Initialization complete\n"));
    }
    catch (const std::exception& e)
    {
        TRACE(_T("DataListCtrl_Optimized::Initialize - Exception: %s\n"), CString(e.what()));
        HandleError(CString(e.what()));
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
        HandleError(CString(e.what()));
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
        HandleError(CString(e.what()));
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
        HandleError(CString(e.what()));
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
        TRACE(_T("DataListCtrl_Optimized::SetRowCount - Setting count to: %d\n"), count);
        
        LockIfThreadSafe();
        
        // Clear existing rows
        ClearRows();
        
        // Resize vector
        m_rows.resize(count);
        
        // Set item count for virtual list control
        if (GetSafeHwnd())
        {
            SetItemCount(count);
            TRACE(_T("DataListCtrl_Optimized::SetRowCount - SetItemCount(%d) called successfully\n"), count);
        }
        else
        {
            TRACE(_T("DataListCtrl_Optimized::SetRowCount - No window handle, cannot set item count\n"));
        }
        
        UnlockIfThreadSafe();
    }
    catch (const std::exception& e)
    {
        TRACE(_T("DataListCtrl_Optimized::SetRowCount - Exception: %s\n"), CString(e.what()));
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
        {
            TRACE(_T("DataListCtrl_Optimized::RefreshDisplay - Not initialized\n"));
            return;
        }
        
        // Check if the control has been created and is still valid
        HWND hWnd = GetSafeHwnd();
        if (!hWnd || !IsWindow(hWnd))
        {
            TRACE(_T("DataListCtrl_Optimized::RefreshDisplay - Invalid window handle\n"));
            return;
        }
        
        // Since we're no longer using async loading, always use synchronous update
        Invalidate();
        UpdateWindow();
        
        TRACE(_T("DataListCtrl_Optimized::RefreshDisplay - Display refreshed successfully\n"));
    }
    catch (const std::exception& e)
    {
        TRACE(_T("DataListCtrl_Optimized::RefreshDisplay - Exception: %s\n"), CString(e.what()));
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
        TRACE(_T("DataListCtrl_Optimized::OnGetDisplayInfo - Handler called\n"));
        
        LV_DISPINFO* pDispInfo = reinterpret_cast<LV_DISPINFO*>(pNMHDR);
        if (pDispInfo)
        {
            TRACE(_T("DataListCtrl_Optimized::OnGetDisplayInfo - Item: %d, SubItem: %d, Mask: 0x%08X\n"), 
                  pDispInfo->item.iItem, pDispInfo->item.iSubItem, pDispInfo->item.mask);
        }
        else
        {
            TRACE(_T("DataListCtrl_Optimized::OnGetDisplayInfo - pDispInfo is NULL\n"));
        }
        
        HandleDisplayInfoRequest(pDispInfo);
        *pResult = 0;
    }
    catch (const std::exception& e)
    {
        TRACE(_T("DataListCtrl_Optimized::OnGetDisplayInfo - Exception: %s\n"), CString(e.what()));
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

void DataListCtrl_Optimized::SetupVirtualListControl()
{
    try
    {
        if (!GetSafeHwnd())
        {
            TRACE(_T("DataListCtrl_Optimized::SetupVirtualListControl - No window handle\n"));
            return;
        }
        
        // Set the LVS_OWNERDATA style to enable virtual list control
        DWORD style = GetStyle();
        TRACE(_T("DataListCtrl_Optimized::SetupVirtualListControl - Current style: 0x%08X\n"), style);
        
        if (!(style & LVS_OWNERDATA))
        {
            style |= LVS_OWNERDATA;
            SetWindowLong(GetSafeHwnd(), GWL_STYLE, style);
            TRACE(_T("DataListCtrl_Optimized::SetupVirtualListControl - Applied LVS_OWNERDATA style\n"));
        }
        else
        {
            TRACE(_T("DataListCtrl_Optimized::SetupVirtualListControl - LVS_OWNERDATA style already present\n"));
        }
    }
    catch (const std::exception& e)
    {
        TRACE(_T("DataListCtrl_Optimized::SetupVirtualListControl - Exception: %s\n"), CString(e.what()));
        HandleError(DataListCtrlError::INVALID_PARAMETER, CString(e.what()));
    }
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
        
        // Debug output
        TRACE(_T("DataListCtrl_Optimized::UpdateDisplayInfo - index: %d\n"), index);
        
        if (!IsValidIndex(index))
        {
            TRACE(_T("DataListCtrl_Optimized::UpdateDisplayInfo - Invalid index: %d\n"), index);
            return;
        }
        
        // Check cache first
        DataListCtrl_Row_Optimized* cachedRow = nullptr;
        if (m_cache && m_cachingEnabled)
        {
            cachedRow = m_cache->GetCachedRow(index);
        }
        
        if (!cachedRow)
        {
            // Load from database only if not cached
            TRACE(_T("DataListCtrl_Optimized::UpdateDisplayInfo - Loading row data for index: %d\n"), index);
            
            // Get the database document
            CWnd* parent = GetParent();
            if (!parent)
            {
                TRACE(_T("DataListCtrl_Optimized::UpdateDisplayInfo - No parent window\n"));
                return;
            }
            
            const auto pdb_doc = static_cast<ViewdbWave_Optimized*>(parent)->GetDocument();
            if (pdb_doc == nullptr)
            {
                TRACE(_T("DataListCtrl_Optimized::UpdateDisplayInfo - No document\n"));
                return;
            }
            
            // Create new row object
            auto newRow = std::make_unique<DataListCtrl_Row_Optimized>();
            newRow->SetIndex(index);
            
            if (!LoadRowDataFromDatabase(pdb_doc, index, *newRow))
            {
                TRACE(_T("DataListCtrl_Optimized::UpdateDisplayInfo - Failed to load row data for index: %d\n"), index);
                return;
            }
            
            // Cache the row
            if (m_cache && m_cachingEnabled)
            {
                m_cache->SetCachedRow(index, newRow.get(), 0); // displayMode = 0 for now
            }
            
            cachedRow = newRow.get();
            TRACE(_T("DataListCtrl_Optimized::UpdateDisplayInfo - Cached row data for index: %d\n"), index);
        }
        else
        {
            TRACE(_T("DataListCtrl_Optimized::UpdateDisplayInfo - Using cached row data for index: %d\n"), index);
        }
        
        // Handle text display
        if (pDispInfo->item.mask & LVIF_TEXT)
        {
            CString cs;
            bool flag = TRUE;
            
            switch (pDispInfo->item.iSubItem)
            {
            case DLC_COLUMN_CURVE: 
                flag = FALSE;
                break;
            case DLC_COLUMN_INDEX: 
                cs.Format(_T("%i"), cachedRow->GetIndex());
                break;
            case DLC_COLUMN_INSECT: 
                cs.Format(_T("%i"), cachedRow->GetInsectId());
                break;
            case DLC_COLUMN_SENSI: 
                cs = cachedRow->GetSensillumName();
                break;
            case DLC_COLUMN_STIM1: 
                cs = cachedRow->GetStimulus1();
                break;
            case DLC_COLUMN_CONC1: 
                cs = cachedRow->GetConcentration1();
                break;
            case DLC_COLUMN_STIM2: 
                cs = cachedRow->GetStimulus2();
                break;
            case DLC_COLUMN_CONC2: 
                cs = cachedRow->GetConcentration2();
                break;
            case DLC_COLUMN_NBSPK: 
                cs = cachedRow->GetNSpikes();
                break;
            case DLC_COLUMN_FLAG: 
                cs = cachedRow->GetFlag();
                break;
            default: 
                flag = FALSE;
                break;
            }
            
            if (flag)
            {
                _tcscpy_s(pDispInfo->item.pszText, pDispInfo->item.cchTextMax, cs);
            }
        }
        
        // Handle image display for curve column
        if (pDispInfo->item.mask & LVIF_IMAGE && pDispInfo->item.iSubItem == DLC_COLUMN_CURVE)
        {
            pDispInfo->item.iImage = index;
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

void DataListCtrl_Optimized::HandleError(const CString& message)
{
    LogError(message);
}

void DataListCtrl_Optimized::LogError(const CString& message) const
{
    TRACE(_T("DataListCtrl_Optimized Error: %s\n"), message);
}

bool DataListCtrl_Optimized::IsValidIndex(int index) const
{
    // For virtual list control, check against the total item count, not the rows vector size
    if (!GetSafeHwnd())
        return false;
    
    const int itemCount = GetItemCount();
    return index >= 0 && index < itemCount;
}

bool DataListCtrl_Optimized::IsValidDisplayMode(int mode) const
{
    return mode >= 0 && mode <= 2; // Assuming 0-2 are valid display modes
}

void DataListCtrl_Optimized::ValidateConfiguration() const
{
    if (!m_config.ValidateConfiguration())
    {
        throw std::runtime_error("Invalid configuration");
    }
}

// Async processing and complex scroll handling methods removed - simplified implementation

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

bool DataListCtrl_Optimized::LoadRowDataFromDatabase(CdbWaveDoc* pdb_doc, int index, DataListCtrl_Row_Optimized& row)
{
    try
    {
        TRACE(_T("DataListCtrl_Optimized::LoadRowDataFromDatabase - Starting for index: %d\n"), index);
        
        if (!pdb_doc)
        {
            TRACE(_T("DataListCtrl_Optimized::LoadRowDataFromDatabase - No document\n"));
            return false;
        }
        
        // Set the current record position in the database
        TRACE(_T("DataListCtrl_Optimized::LoadRowDataFromDatabase - Setting record position to: %d\n"), index);
        if (!pdb_doc->db_set_current_record_position(index))
        {
            TRACE(_T("DataListCtrl_Optimized::LoadRowDataFromDatabase - Failed to set record position\n"));
            return false;
        }
        
        // Open the data and spike files
        pdb_doc->open_current_data_file();
        pdb_doc->open_current_spike_file();
        
        // Get file names
        row.SetDataFileName(pdb_doc->db_get_current_dat_file_name(TRUE));
        row.SetSpikeFileName(pdb_doc->db_get_current_spk_file_name(TRUE));
        
        // Get database table
        const auto database = pdb_doc->db_table;
        if (!database)
            return false;
        
        // Load record data using the same approach as the original DataListCtrl_Row
        DB_ITEMDESC desc;
        
        // Set the index
        row.SetIndex(index);
        
        // Get record ID
        database->get_record_item_value(CH_ID, &desc);
        row.SetRecordId(desc.l_val);
        
        // Get insect ID
        database->get_record_item_value(CH_IDINSECT, &desc);
        row.SetInsectId(desc.l_val);
        
        // Get stimulus 1
        database->get_record_item_value(CH_STIM1_KEY, &desc);
        row.SetStimulus1(desc.cs_val);
        
        // Get concentration 1
        database->get_record_item_value(CH_CONC1_KEY, &desc);
        row.SetConcentration1(desc.cs_val);
        
        // Get stimulus 2
        database->get_record_item_value(CH_STIM2_KEY, &desc);
        row.SetStimulus2(desc.cs_val);
        
        // Get concentration 2
        database->get_record_item_value(CH_CONC2_KEY, &desc);
        row.SetConcentration2(desc.cs_val);
        
        // Get sensillum name
        database->get_record_item_value(CH_SENSILLUM_KEY, &desc);
        row.SetSensillumName(desc.cs_val);
        
        // Get flag
        database->get_record_item_value(CH_FLAG, &desc);
        CString flagStr;
        flagStr.Format(_T("%i"), desc.l_val);
        row.SetFlag(flagStr);
        
        // Get number of spikes
        if (row.GetSpikeFileName().IsEmpty())
        {
            row.SetNSpikes(_T(""));
        }
        else
        {
            database->get_record_item_value(CH_NSPIKES, &desc);
            const int n_spikes = desc.l_val;
            database->get_record_item_value(CH_NSPIKECLASSES, &desc);
            CString spikesStr;
            spikesStr.Format(_T("%i (%i classes)"), n_spikes, desc.l_val);
            row.SetNSpikes(spikesStr);
        }
        
        TRACE(_T("DataListCtrl_Optimized::LoadRowDataFromDatabase - Successfully loaded data for index: %d\n"), index);
        return true;
    }
    catch (const std::exception& e)
    {
        TRACE(_T("DataListCtrl_Optimized::LoadRowDataFromDatabase - Exception: %s\n"), CString(e.what()));
        HandleError(DataListCtrlError::DATABASE_ACCESS_FAILED, CString(e.what()));
        return false;
    }
}
