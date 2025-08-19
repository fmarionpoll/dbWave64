#include "stdafx.h"
#include "DataListCtrl_Optimized.h"
#include "DataListCtrl_SupportingClasses.h"
#include "DataListCtrl_Row_Optimized.h"
#include "DataListCtrl_Configuration.h"
#include "DataListCtrl_Optimized_Infos.h"
#include "ViewdbWave_Optimized.h"
#include "dbWaveDoc.h"
#include <algorithm>

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

// Simplified implementation - removed complex performance monitoring, thread safety, and async operations

DataListCtrl_Optimized::DataListCtrl_Optimized()
    : m_infos(nullptr)
    , m_initialized(false)
    , m_cachingEnabled(true)
    , m_parentWindow(nullptr)
{
    // Initialize with default configuration
    m_config = DataListCtrlConfiguration();
}

DataListCtrl_Optimized::~DataListCtrl_Optimized()
{
    // Clean up resources
    if (m_infos)
    {
        delete m_infos;
        m_infos = nullptr;
    }
}

void DataListCtrl_Optimized::Initialize(const DataListCtrlConfiguration& config)
{
    try
    {
        TRACE(_T("DataListCtrl_Optimized::Initialize - Starting initialization\n"));
        
        // Copy configuration settings - avoid assignment operator
        m_config.GetDisplaySettings() = config.GetDisplaySettings();
        m_config.GetTimeSettings() = config.GetTimeSettings();
        m_config.GetAmplitudeSettings() = config.GetAmplitudeSettings();
        m_config.GetUISettings() = config.GetUISettings();
        m_config.GetPerformanceSettings() = config.GetPerformanceSettings();
        m_config.SetColumns(config.GetColumns());
        
        TRACE(_T("DataListCtrl_Optimized::Initialize - Configuration copied\n"));
        
        // Setup default configuration
        SetupDefaultConfiguration();
        
        TRACE(_T("DataListCtrl_Optimized::Initialize - Default configuration setup complete\n"));
        
        // Setup virtual list control
        SetupVirtualListControl();
        
        TRACE(_T("DataListCtrl_Optimized::Initialize - Virtual list control setup complete\n"));
        
        // Initialize columns
        InitializeColumns();
        
        TRACE(_T("DataListCtrl_Optimized::Initialize - Columns initialized\n"));
        
        // Setup image list
        SetupImageList();
        
        TRACE(_T("DataListCtrl_Optimized::Initialize - Image list initialized\n"));
        
        // Create empty bitmap
        CreateEmptyBitmap();
        
        TRACE(_T("DataListCtrl_Optimized::Initialize - Empty bitmap created\n"));
        
        // Initialize cache
        m_cache = std::make_unique<DataListCtrlCache>();
        
        // Initialize m_infos
        m_infos = new DataListCtrlInfos();
        m_infos->image_width = g_column_width[DLC_COLUMN_CURVE];
        m_infos->image_height = m_config.GetDisplayConfig().GetImageHeight();
        
        m_initialized = true;
        
        TRACE(_T("DataListCtrl_Optimized::Initialize - Initialization complete\n"));
    }
    catch (const std::exception& e)
    {
        HandleError(CString(e.what()));
    }
}

void DataListCtrl_Optimized::SetRowCount(int count)
{
    try
    {
        TRACE(_T("DataListCtrl_Optimized::SetRowCount - Setting count to: %d\n"), count);
        
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
    }
    catch (const std::exception& e)
    {
        HandleError(CString(e.what()));
    }
}

void DataListCtrl_Optimized::ClearRows()
{
    try
    {
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
    }
    catch (const std::exception& e)
    {
        HandleError(CString(e.what()));
    }
}

void DataListCtrl_Optimized::AddRow(std::unique_ptr<DataListCtrl_Row_Optimized> row)
{
    try
    {
        int index = static_cast<int>(m_rows.size());
        m_rows.push_back(std::move(row));
        
        // Add item to list control
        if (GetSafeHwnd())
        {
            InsertItem(index, _T(""));
        }
    }
    catch (const std::exception& e)
    {
        HandleError(CString(e.what()));
    }
}

void DataListCtrl_Optimized::RemoveRow(int index)
{
    try
    {
        if (!IsValidIndex(index))
        {
            HandleError(_T("Invalid row index: ") + CString(std::to_string(index).c_str()));
            return;
        }
        
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
    }
    catch (const std::exception& e)
    {
        HandleError(CString(e.what()));
    }
}

void DataListCtrl_Optimized::UpdateRow(int index)
{
    try
    {
        if (!IsValidIndex(index))
        {
            HandleError(_T("Invalid row index: ") + CString(std::to_string(index).c_str()));
            return;
        }
        
        ProcessRowUpdate(index);
    }
    catch (const std::exception& e)
    {
        HandleError(CString(e.what()));
    }
}

void DataListCtrl_Optimized::SetDisplayMode(int mode)
{
    try
    {
        if (!IsValidDisplayMode(mode))
        {
            HandleError(_T("Invalid display mode: ") + CString(std::to_string(mode).c_str()));
            return;
        }
        
        // Get current display mode
        int currentMode = m_config.GetDisplayConfig().GetDisplayMode();
        
        // Only update if mode actually changed
        if (currentMode != mode)
        {
            // Update configuration
            auto& displayConfig = m_config.GetDisplayConfig();
            displayConfig.SetDisplayMode(mode);
            
            // Clear cache when display mode changes (different images needed)
            if (m_cache)
            {
                m_cache->Clear();
                TRACE(_T("DataListCtrl_Optimized::SetDisplayMode - Cache cleared for mode change: %d -> %d\n"), currentMode, mode);
            }
            
            // Refresh display to show new mode
            RefreshDisplay();
            
            TRACE(_T("DataListCtrl_Optimized::SetDisplayMode - Display mode changed to: %d\n"), mode);
        }
    }
    catch (const std::exception& e)
    {
        HandleError(CString(e.what()));
    }
}

void DataListCtrl_Optimized::SetTimeSpan(float first, float last)
{
    try
    {
        if (first >= last)
        {
            HandleError(_T("Invalid time span: first must be less than last"));
            return;
        }
        
        auto& timeConfig = m_config.GetTimeSettings();
        timeConfig.SetTimeSpan(first, last);
        
        // Clear cache
        if (m_cache)
        {
            m_cache->Clear();
        }
        
        RefreshDisplay();
    }
    catch (const std::exception& e)
    {
        HandleError(CString(e.what()));
    }
}

void DataListCtrl_Optimized::SetAmplitudeSpan(float span)
{
    try
    {
        if (span <= 0.0f)
        {
            HandleError(_T("Invalid amplitude span: must be positive"));
            return;
        }
        
        auto& amplitudeConfig = m_config.GetAmplitudeSettings();
        amplitudeConfig.SetMvSpan(span);
        
        // Clear cache
        if (m_cache)
        {
            m_cache->Clear();
        }
        
        RefreshDisplay();
    }
    catch (const std::exception& e)
    {
        HandleError(CString(e.what()));
    }
}

void DataListCtrl_Optimized::SetDisplayFileName(bool display)
{
    try
    {
        auto& uiConfig = m_config.GetUISettings();
        uiConfig.SetDisplayFileName(display);
        
        RefreshDisplay();
    }
    catch (const std::exception& e)
    {
        HandleError(CString(e.what()));
    }
}

void DataListCtrl_Optimized::RefreshDisplay()
{
    try
    {
        if (!GetSafeHwnd())
        {
            return;
        }
        
        // Get current display mode for logging
        int currentMode = m_config.GetDisplayConfig().GetDisplayMode();
        
        // Invalidate and update the display
        Invalidate();
        UpdateWindow();
        
        TRACE(_T("DataListCtrl_Optimized::RefreshDisplay - Display refreshed with mode: %d\n"), currentMode);
    }
    catch (const std::exception& e)
    {
        HandleError(CString(e.what()));
    }
}

void DataListCtrl_Optimized::SetConfiguration(const DataListCtrlConfiguration& config)
{
    try
    {
        // Copy configuration settings - avoid assignment operator
        m_config.GetDisplaySettings() = config.GetDisplaySettings();
        m_config.GetTimeSettings() = config.GetTimeSettings();
        m_config.GetAmplitudeSettings() = config.GetAmplitudeSettings();
        m_config.GetUISettings() = config.GetUISettings();
        m_config.GetPerformanceSettings() = config.GetPerformanceSettings();
        m_config.SetColumns(config.GetColumns());
        
        // Clear cache
        if (m_cache)
        {
            m_cache->Clear();
        }
        
        RefreshDisplay();
    }
    catch (const std::exception& e)
    {
        HandleError(CString(e.what()));
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
        HandleError(CString(e.what()));
    }
}

// Public interface methods
void DataListCtrl_Optimized::init_columns(std::vector<int>* width_columns)
{
    try
    {
        if (!width_columns)
        {
            HandleError(_T("Invalid width columns parameter"));
            return;
        }
        
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
    }
    catch (const std::exception& e)
    {
        HandleError(CString(e.what()));
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
        auto& displayConfig = m_config.GetDisplayConfig();
        displayConfig.SetDataTransform(transform);
        
        if (m_cache)
        {
            m_cache->Clear();
        }
        
        RefreshDisplay();
    }
    catch (const std::exception& e)
    {
        HandleError(CString(e.what()));
    }
}

void DataListCtrl_Optimized::set_spike_plot_mode(int mode)
{
    try
    {
        auto& displayConfig = m_config.GetDisplayConfig();
        displayConfig.SetSpikePlotMode(mode);
        
        if (m_cache)
        {
            m_cache->Clear();
        }
        
        RefreshDisplay();
    }
    catch (const std::exception& e)
    {
        HandleError(CString(e.what()));
    }
}

void DataListCtrl_Optimized::set_selected_class(int classIndex)
{
    try
    {
        auto& displayConfig = m_config.GetDisplayConfig();
        displayConfig.SetSelectedClass(classIndex);
        
        if (m_cache)
        {
            m_cache->Clear();
        }
        
        RefreshDisplay();
    }
    catch (const std::exception& e)
    {
        HandleError(CString(e.what()));
    }
}

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
            HandleError(_T("Failed to create image list"));
            return;
        }
        
        SetImageList(m_imageList.get(), LVSIL_SMALL);
        
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
        HandleError(CString(e.what()));
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
        HandleError(CString(e.what()));
    }
}

void DataListCtrl_Optimized::SetParentWindow(CWnd* parent)
{
    m_parentWindow = parent;
}

// Event handlers
void DataListCtrl_Optimized::OnDestroy()
{
    // Clean up resources
    if (m_infos)
    {
        delete m_infos;
        m_infos = nullptr;
    }
    
    CListCtrl::OnDestroy();
}

void DataListCtrl_Optimized::OnSize(UINT nType, int cx, int cy)
{
    try
    {
        CListCtrl::OnSize(nType, cx, cy);
        
        // Resize columns to fit new size
        if (cx > 0 && cy > 0)
        {
            fit_columns_to_size(cx);
        }
    }
    catch (const std::exception& e)
    {
        HandleError(CString(e.what()));
    }
}

void DataListCtrl_Optimized::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    try
    {
        CListCtrl::OnVScroll(nSBCode, nPos, pScrollBar);
        
        // Update display for new visible items
        UpdateDisplayInfo(nullptr);
    }
    catch (const std::exception& e)
    {
        HandleError(CString(e.what()));
    }
}

void DataListCtrl_Optimized::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    try
    {
        CListCtrl::OnHScroll(nSBCode, nPos, pScrollBar);
        
        // Update display for new visible items
        UpdateDisplayInfo(nullptr);
    }
    catch (const std::exception& e)
    {
        HandleError(CString(e.what()));
    }
}

void DataListCtrl_Optimized::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    try
    {
        CListCtrl::OnKeyUp(nChar, nRepCnt, nFlags);
        
        // Handle keyboard navigation
        if (nChar == VK_UP || nChar == VK_DOWN || nChar == VK_PRIOR || nChar == VK_NEXT)
        {
            UpdateDisplayInfo(nullptr);
        }
    }
    catch (const std::exception& e)
    {
        HandleError(CString(e.what()));
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
        HandleError(CString(e.what()));
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
        HandleError(CString(e.what()));
    }
}

void DataListCtrl_Optimized::OnGetDisplayInfo(NMHDR* pNMHDR, LRESULT* pResult)
{
    try
    {
        LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
        
        if (pDispInfo->item.mask & LVIF_TEXT)
        {
            HandleDisplayInfoRequest(pDispInfo);
        }
        
        *pResult = 0;
    }
    catch (const std::exception& e)
    {
        HandleError(CString(e.what()));
        *pResult = 1;
    }
}

// Private methods
void DataListCtrl_Optimized::CreateEmptyBitmap()
{
    try
    {
        if (m_emptyBitmap)
        {
            m_emptyBitmap.reset();
        }
        
        m_emptyBitmap = std::make_unique<CBitmap>();
        m_emptyBitmap->CreateBitmap(16, 16, 1, 1, nullptr);
    }
    catch (const std::exception& e)
    {
        HandleError(CString(e.what()));
    }
}

void DataListCtrl_Optimized::SetupVirtualListControl()
{
    try
    {
        DWORD currentStyle = GetStyle();
        TRACE(_T("DataListCtrl_Optimized::SetupVirtualListControl - Current style: 0x%08X\n"), currentStyle);
        
        if (!(currentStyle & LVS_OWNERDATA))
        {
            ModifyStyle(0, LVS_OWNERDATA);
            TRACE(_T("DataListCtrl_Optimized::SetupVirtualListControl - LVS_OWNERDATA style added\n"));
        }
        else
        {
            TRACE(_T("DataListCtrl_Optimized::SetupVirtualListControl - LVS_OWNERDATA style already present\n"));
        }
    }
    catch (const std::exception& e)
    {
        HandleError(CString(e.what()));
    }
}

void DataListCtrl_Optimized::ProcessRowUpdate(int index)
{
    try
    {
        if (!IsValidIndex(index))
        {
            return;
        }
        
        // Update cache
        UpdateCache(index, 0); // displayMode = 0 for now
        
        // Redraw the item
        if (GetSafeHwnd())
        {
            RedrawItems(index, index);
        }
    }
    catch (const std::exception& e)
    {
        HandleError(CString(e.what()));
    }
}

void DataListCtrl_Optimized::UpdateDisplayInfo(LV_DISPINFO* pDispInfo)
{
    try
    {
        if (!m_initialized || !GetSafeHwnd())
        {
            return;
        }
        
        // Get current display mode for cache optimization
        int currentDisplayMode = m_config.GetDisplayConfig().GetDisplayMode();
        
        // Get visible range with some buffer for smooth scrolling
        int firstVisible = GetTopIndex();
        int lastVisible = firstVisible + GetCountPerPage();
        int itemCount = GetItemCount();
        
        // Add buffer for smoother scrolling (pre-cache adjacent items)
        int bufferSize = 5;
        int startIndex = max(0, firstVisible - bufferSize);
        int endIndex = min(itemCount - 1, lastVisible + bufferSize);
        
        // Update cache for visible items plus buffer
        for (int i = startIndex; i <= endIndex; ++i)
        {
            // Only update if not already cached with current display mode
            auto cachedRow = m_cache ? m_cache->GetCachedRow(i) : nullptr;
            if (!cachedRow)
            {
                UpdateCache(i, currentDisplayMode);
            }
        }
        
        // Redraw only the actually visible items for better performance
        RedrawItems(firstVisible, lastVisible);
    }
    catch (const std::exception& e)
    {
        HandleError(CString(e.what()));
    }
}

void DataListCtrl_Optimized::HandleDisplayInfoRequest(LV_DISPINFO* pDispInfo)
{
    try
    {
        int index = pDispInfo->item.iItem;
        
        if (!IsValidIndex(index))
        {
            return;
        }
        
        // Get current display mode from configuration
        int currentDisplayMode = m_config.GetDisplayConfig().GetDisplayMode();
        
        // Check cache first - use display mode for cache key
        auto cachedRow = m_cache ? m_cache->GetCachedRow(index) : nullptr;
        if (cachedRow)
        {
            HandleTextDisplay(pDispInfo, cachedRow);
            HandleImageDisplay(pDispInfo, index, currentDisplayMode);
            return;
        }
        
        // Load from database if not cached
        if (m_parentWindow)
        {
            const auto pdb_doc = static_cast<ViewdbWave_Optimized*>(m_parentWindow)->GetDocument();
            if (pdb_doc)
            {
                auto newRow = std::make_unique<DataListCtrl_Row_Optimized>();
                newRow->SetIndex(index);
                
                if (LoadRowDataFromDatabase(pdb_doc, index, *newRow))
                {
                    // Cache the row with current display mode
                    if (m_cache)
                    {
                        m_cache->SetCachedRow(index, newRow.get(), currentDisplayMode);
                    }
                    
                    HandleTextDisplay(pDispInfo, newRow.get());
                    HandleImageDisplay(pDispInfo, index, currentDisplayMode);
                }
            }
        }
    }
    catch (const std::exception& e)
    {
        HandleError(CString(e.what()));
    }
}

void DataListCtrl_Optimized::HandleTextDisplay(LV_DISPINFO* pDispInfo, DataListCtrl_Row_Optimized* row)
{
    if (!(pDispInfo->item.mask & LVIF_TEXT) || !row)
    {
        return;
    }
    
    CString cs;
    bool flag = TRUE;
    
    switch (pDispInfo->item.iSubItem)
    {
    case DLC_COLUMN_CURVE: 
        flag = FALSE; // Curve column shows image, not text
        break;
    case DLC_COLUMN_INDEX: 
        cs.Format(_T("%i"), row->GetIndex());
        break;
    case DLC_COLUMN_INSECT: 
        cs.Format(_T("%i"), row->GetInsectId());
        break;
    case DLC_COLUMN_SENSI: 
        cs = row->GetSensillumName();
        break;
    case DLC_COLUMN_STIM1: 
        cs = row->GetStimulus1();
        break;
    case DLC_COLUMN_CONC1: 
        cs = row->GetConcentration1();
        break;
    case DLC_COLUMN_STIM2: 
        cs = row->GetStimulus2();
        break;
    case DLC_COLUMN_CONC2: 
        cs = row->GetConcentration2();
        break;
    case DLC_COLUMN_NBSPK: 
        cs = row->GetNSpikes();
        break;
    case DLC_COLUMN_FLAG: 
        cs = row->GetFlag();
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

void DataListCtrl_Optimized::HandleImageDisplay(LV_DISPINFO* pDispInfo, int index, int displayMode)
{
    if (!(pDispInfo->item.mask & LVIF_IMAGE) || pDispInfo->item.iSubItem != DLC_COLUMN_CURVE)
    {
        return;
    }
    
    // Set image based on display mode
    switch (displayMode)
    {
    case 0: // DISPLAY_MODE_EMPTY - grey rectangle
        pDispInfo->item.iImage = 0; // Use empty bitmap
        break;
    case 1: // DISPLAY_MODE_DATA - show data curve
        pDispInfo->item.iImage = index + 1; // Offset to avoid 0
        break;
    case 2: // DISPLAY_MODE_SPIKES - show spikes
        pDispInfo->item.iImage = index + 1000; // Different offset for spikes
        break;
    default:
        pDispInfo->item.iImage = 0; // Default to empty
        break;
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
        HandleError(CString(e.what()));
    }
}

void DataListCtrl_Optimized::UpdateCache(int index, int displayMode)
{
    try
    {
        if (!m_cache || !m_cachingEnabled)
        {
            return;
        }
        
        // Check if already cached with the same display mode
        auto existingCachedRow = m_cache->GetCachedRow(index);
        if (existingCachedRow)
        {
            return; // Already cached, no need to update
        }
        
        // Load data from database if not in memory
        if (m_parentWindow)
        {
            const auto pdb_doc = static_cast<ViewdbWave_Optimized*>(m_parentWindow)->GetDocument();
            if (pdb_doc)
            {
                auto newRow = std::make_unique<DataListCtrl_Row_Optimized>();
                newRow->SetIndex(index);
                
                if (LoadRowDataFromDatabase(pdb_doc, index, *newRow))
                {
                    // Cache the row with current display mode
                    m_cache->SetCachedRow(index, newRow.get(), displayMode);
                    TRACE(_T("DataListCtrl_Optimized::UpdateCache - Cached row %d with display mode %d\n"), index, displayMode);
                }
            }
        }
    }
    catch (const std::exception& e)
    {
        HandleError(CString(e.what()));
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
        HandleError(CString(e.what()));
    }
}

void DataListCtrl_Optimized::SaveColumnWidths()
{
    try
    {
        if (!GetSafeHwnd())
        {
            return;
        }
        
        auto& columns = m_config.GetColumns();
        for (size_t i = 0; i < columns.size(); ++i)
        {
            int width = GetColumnWidth(static_cast<int>(i));
            columns[i].width = width;
        }
    }
    catch (const std::exception& e)
    {
        HandleError(CString(e.what()));
    }
}

void DataListCtrl_Optimized::LoadColumnWidths()
{
    try
    {
        if (!GetSafeHwnd())
        {
            return;
        }
        
        auto& columns = m_config.GetColumns();
        for (size_t i = 0; i < columns.size(); ++i)
        {
            int width = columns[i].width;
            if (width > 0)
            {
                SetColumnWidth(static_cast<int>(i), width);
            }
        }
    }
    catch (const std::exception& e)
    {
        HandleError(CString(e.what()));
    }
}

void DataListCtrl_Optimized::ApplyColumnConfiguration()
{
    try
    {
        if (!GetSafeHwnd())
        {
            return;
        }
        
        auto& columns = m_config.GetColumns();
        for (size_t i = 0; i < columns.size(); ++i)
        {
            const auto& column = columns[i];
            SetColumnWidth(static_cast<int>(i), column.width);
        }
    }
    catch (const std::exception& e)
    {
        HandleError(CString(e.what()));
    }
}

bool DataListCtrl_Optimized::LoadRowDataFromDatabase(CdbWaveDoc* pdb_doc, int index, DataListCtrl_Row_Optimized& row)
{
    try
    {
        if (!pdb_doc)
        {
            return false;
        }
        
        // Set the current record position in the database
        if (!pdb_doc->db_set_current_record_position(index))
        {
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
        
        return true;
    }
    catch (const std::exception& e)
    {
        HandleError(CString(e.what()));
        return false;
    }
}

void DataListCtrl_Optimized::HandleError(const CString& message)
{
    TRACE(_T("DataListCtrl_Optimized::HandleError - %s\n"), message);
    // Simple error handling - just log the error
}

void DataListCtrl_Optimized::LogError(const CString& message) const
{
    TRACE(_T("DataListCtrl_Optimized Error: %s\n"), message);
}

void DataListCtrl_Optimized::SetupDefaultConfiguration()
{
    try
    {
        // Set default display mode to "no display" (grey rectangle) - mode 0
        auto& displayConfig = m_config.GetDisplayConfig();
        displayConfig.SetDisplayMode(0); // DISPLAY_MODE_EMPTY
        
        // Set default time range
        auto& timeConfig = m_config.GetTimeSettings();
        timeConfig.SetTimeSpan(0.0f, 100.0f);
        
        // Set default amplitude span
        auto& amplitudeConfig = m_config.GetAmplitudeSettings();
        amplitudeConfig.SetMvSpan(1.0f);
        
        // Set default UI settings
        auto& uiConfig = m_config.GetUISettings();
        uiConfig.SetDisplayFileName(false); // Don't display file names by default
        
        TRACE(_T("DataListCtrl_Optimized::SetupDefaultConfiguration - Default display mode set to: 0 (no display)\n"));
    }
    catch (const std::exception& e)
    {
        HandleError(CString(e.what()));
    }
}

void DataListCtrl_Optimized::InitializeColumns()
{
    try
    {
        if (!GetSafeHwnd())
        {
            return;
        }
        
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
    try
    {
        InitializeColumns();
        LoadColumnWidths();
    }
    catch (const std::exception& e)
    {
        HandleError(CString(e.what()));
    }
}

void DataListCtrl_Optimized::SetupImageList()
{
    try
    {
        if (!GetSafeHwnd())
        {
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

void DataListCtrl_Optimized::InitializeImageList()
{
    SetupImageList();
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

// Message map
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
