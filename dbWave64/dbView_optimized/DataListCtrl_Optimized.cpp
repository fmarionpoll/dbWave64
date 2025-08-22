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
    : m_infos_(nullptr)
    , m_initialized_(false)
    , m_caching_enabled_(true)
    , m_parent_window_(nullptr)
    , m_current_selection_(-1)
{
    // Initialize with default configuration
    m_config_ = data_list_ctrl_configuration();
}

DataListCtrl_Optimized::~DataListCtrl_Optimized()
{
    // Clean up resources
    if (m_infos_)
    {
        delete m_infos_;
        m_infos_ = nullptr;
    }
}

void DataListCtrl_Optimized::initialize(const data_list_ctrl_configuration& config)
{
    try
    {
        // Copy configuration settings - avoid assignment operator
        m_config_.get_display_settings() = config.get_display_settings();
        m_config_.get_time_settings() = config.get_time_settings();
        m_config_.get_amplitude_settings() = config.get_amplitude_settings();
        m_config_.get_ui_settings() = config.get_ui_settings();
        m_config_.get_performance_settings() = config.get_performance_settings();
        m_config_.set_columns(config.get_columns());

        setup_default_configuration();
        setup_virtual_list_control();
        setup_style();
        initialize_columns();
        setup_image_list();
        create_empty_bitmap();
        
        // Initialize cache
        m_cache_ = std::make_unique<data_list_ctrl_cache>();
        
        // Initialize m_infos
        m_infos_ = new data_list_ctrl_infos();
        m_infos_->image_width = g_column_width[DLC_COLUMN_CURVE];
        m_infos_->image_height = m_config_.get_display_config().get_image_height();
        
        m_initialized_ = true;
    }
    catch (const std::exception& e)
    {
        handle_error(CString(e.what()));
    }
}

void DataListCtrl_Optimized::set_row_count(int count)
{
    try
    {
        clear_rows();
        m_rows_.resize(count);

        if (GetSafeHwnd())
        {
            SetItemCount(count);
            
        	// Ensure image list is properly set after the control is fully initialized
	        if (m_image_list_ && m_image_list_->GetImageCount() > 0)
	        {
	            SetImageList(m_image_list_.get(), LVSIL_SMALL);
	            Invalidate();
	            UpdateWindow();
	            RedrawItems(0, count - 1);
	        }
        }
    }
    catch (const std::exception& e)
    {
        handle_error(CString(e.what()));
    }
}

void DataListCtrl_Optimized::clear_rows()
{
    try
    {
        m_rows_.clear();
        
        // Check if the control has been created before calling DeleteAllItems
        if (GetSafeHwnd())
        {
            DeleteAllItems();
        }
        
        if (m_cache_)
        {
            m_cache_->clear();
        }
    }
    catch (const std::exception& e)
    {
        handle_error(CString(e.what()));
    }
}

void DataListCtrl_Optimized::add_row(std::unique_ptr<DataListCtrl_Row_Optimized> row)
{
    try
    {
        int index = static_cast<int>(m_rows_.size());
        m_rows_.push_back(std::move(row));
        
        // Add item to list control
        if (GetSafeHwnd())
        {
            InsertItem(index, _T(""));
        }
    }
    catch (const std::exception& e)
    {
        handle_error(CString(e.what()));
    }
}

void DataListCtrl_Optimized::remove_row(int index)
{
    try
    {
        if (!is_valid_index(index))
        {
            handle_error(_T("Invalid row index: ") + CString(std::to_string(index).c_str()));
            return;
        }
        
        // Remove from vector
        if (index < static_cast<int>(m_rows_.size()))
        {
            m_rows_.erase(m_rows_.begin() + index);
        }
        
        // Remove from list control
        if (GetSafeHwnd())
        {
            DeleteItem(index);
        }
        
        // Invalidate cache
        if (m_cache_)
        {
            m_cache_->invalidate_cache(index);
        }
    }
    catch (const std::exception& e)
    {
        handle_error(CString(e.what()));
    }
}

void DataListCtrl_Optimized::update_row(int index)
{
    try
    {
        if (!is_valid_index(index))
        {
            handle_error(_T("Invalid row index: ") + CString(std::to_string(index).c_str()));
            return;
        }
        
        process_row_update(index);
    }
    catch (const std::exception& e)
    {
        handle_error(CString(e.what()));
    }
}

void DataListCtrl_Optimized::set_display_mode(int mode)
{
    try
    {
        if (!is_valid_display_mode(mode))
        {
            handle_error(_T("Invalid display mode: ") + CString(std::to_string(mode).c_str()));
            return;
        }
        
        // Get current display mode
        int currentMode = m_config_.get_display_config().get_display_mode();
        
        // Only update if mode actually changed
        if (currentMode != mode)
        {
            // Update configuration
            auto& displayConfig = m_config_.get_display_config();
            displayConfig.set_display_mode(mode);
            
            // Clear cache when display mode changes (different images needed)
            if (m_cache_)
            {
                m_cache_->clear();
            }
            
            // Refresh display to show new mode
            refresh_display();
        }
    }
    catch (const std::exception& e)
    {
        handle_error(CString(e.what()));
    }
}

int DataListCtrl_Optimized::get_display_mode() 
{
    try
    {
        return m_config_.get_display_config().get_display_mode();
    }
    catch (const std::exception& e)
    {
		handle_error( CString(e.what()));
        return 0;
    }
}

void DataListCtrl_Optimized::set_time_span(float first, float last)
{
    try
    {
        if (first >= last)
        {
            handle_error(_T("Invalid time span: first must be less than last"));
            return;
        }
        
        auto& timeConfig = m_config_.get_time_settings();
        timeConfig.set_time_span(first, last);
        
        // Clear cache
        if (m_cache_)
        {
            m_cache_->clear();
        }
        
        refresh_display();
    }
    catch (const std::exception& e)
    {
        handle_error(CString(e.what()));
    }
}

void DataListCtrl_Optimized::set_amplitude_span(float span)
{
    try
    {
        if (span <= 0.0f)
        {
            handle_error(_T("Invalid amplitude span: must be positive"));
            return;
        }
        
        auto& amplitudeConfig = m_config_.get_amplitude_settings();
        amplitudeConfig.set_mv_span(span);
        
        // Clear cache
        if (m_cache_)
        {
            m_cache_->clear();
        }
        
        refresh_display();
    }
    catch (const std::exception& e)
    {
        handle_error(CString(e.what()));
    }
}

void DataListCtrl_Optimized::set_display_file_name(bool display)
{
    try
    {
        auto& uiConfig = m_config_.get_ui_settings();
        uiConfig.set_display_file_name(display);
        
        refresh_display();
    }
    catch (const std::exception& e)
    {
        handle_error(CString(e.what()));
    }
}

void DataListCtrl_Optimized::refresh_display()
{
    try
    {
        if (!GetSafeHwnd())
        {
            return;
        }
        
        // Get current display mode for logging
        int currentMode = m_config_.get_display_config().get_display_mode();
        
        // Ensure image list is set
        if (m_image_list_ && m_image_list_->GetImageCount() > 0)
        {
            SetImageList(m_image_list_.get(), LVSIL_SMALL);
        }
        
        // Force a complete redraw of all items
        const int itemCount = GetItemCount();
        if (itemCount > 0)
        {
            RedrawItems(0, itemCount - 1);
        }
        Invalidate();
        UpdateWindow();
    }
    catch (const std::exception& e)
    {
        handle_error(CString(e.what()));
    }
}

void DataListCtrl_Optimized::set_configuration(const data_list_ctrl_configuration& config)
{
    try
    {
        // Copy configuration settings - avoid assignment operator
        m_config_.get_display_settings() = config.get_display_settings();
        m_config_.get_time_settings() = config.get_time_settings();
        m_config_.get_amplitude_settings() = config.get_amplitude_settings();
        m_config_.get_ui_settings() = config.get_ui_settings();
        m_config_.get_performance_settings() = config.get_performance_settings();
        m_config_.set_columns(config.get_columns());
        
        // Clear cache
        if (m_cache_)
        {
            m_cache_->clear();
        }
        
        refresh_display();
    }
    catch (const std::exception& e)
    {
        handle_error(CString(e.what()));
    }
}

void DataListCtrl_Optimized::clear_cache()
{
    try
    {
        if (m_cache_)
        {
            m_cache_->clear();
        }
    }
    catch (const std::exception& e)
    {
        handle_error(CString(e.what()));
    }
}

// Public interface methods
void DataListCtrl_Optimized::init_columns(std::vector<int>* width_columns)
{
    try
    {
        if (!width_columns)
        {
            handle_error(_T("Invalid width columns parameter"));
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
        handle_error(CString(e.what()));
    }
}

void DataListCtrl_Optimized::set_time_intervals(float first, float last)
{
    set_time_span(first, last);
}

void DataListCtrl_Optimized::set_data_transform(int transform)
{
    try
    {
        auto& displayConfig = m_config_.get_display_config();
        displayConfig.set_data_transform(transform);
        
        if (m_cache_)
        {
            m_cache_->clear();
        }
        
        refresh_display();
    }
    catch (const std::exception& e)
    {
        handle_error(CString(e.what()));
    }
}

void DataListCtrl_Optimized::set_spike_plot_mode(int mode)
{
    try
    {
        auto& displayConfig = m_config_.get_display_config();
        displayConfig.set_spike_plot_mode(mode);
        
        if (m_cache_)
        {
            m_cache_->clear();
        }
        
        refresh_display();
    }
    catch (const std::exception& e)
    {
        handle_error(CString(e.what()));
    }
}

void DataListCtrl_Optimized::set_selected_class(int classIndex)
{
    try
    {
        auto& displayConfig = m_config_.get_display_config();
        displayConfig.set_selected_class(classIndex);
        
        if (m_cache_)
        {
            m_cache_->clear();
        }
        
        refresh_display();
    }
    catch (const std::exception& e)
    {
        handle_error(CString(e.what()));
    }
}

void DataListCtrl_Optimized::resize_signal_column(int n_pixels)
{
    try
    {
        if (!m_initialized_ || !m_infos_ || !GetSafeHwnd())
            return;
            
        // Update the signal column width in the global array
        g_column_width[DLC_COLUMN_CURVE] = n_pixels;
        
        // Update the image list with new width
        if (m_image_list_)
        {
            m_image_list_->DeleteImageList();
        }
        
        m_infos_->image_width = n_pixels;
        m_infos_->image_height = m_config_.get_display_config().get_image_height();
        
        m_image_list_ = std::make_unique<CImageList>();
        if (!m_image_list_->Create(m_infos_->image_width, m_infos_->image_height, ILC_COLOR4, 10, 10))
        {
            handle_error(_T("Failed to create image list"));
            return;
        }
        
        SetImageList(m_image_list_.get(), LVSIL_SMALL);
        
        // Update the configuration
        auto columns = m_config_.get_columns();
        if (columns.size() > DLC_COLUMN_CURVE)
        {
            columns[DLC_COLUMN_CURVE].width = n_pixels;
            m_config_.set_columns(columns);
        }
        
        // Save column widths when they change
        save_column_widths();
        
        // Refresh the display
        refresh_display();
    }
    catch (const std::exception& e)
    {
        handle_error(CString(e.what()));
    }
}

void DataListCtrl_Optimized::fit_columns_to_size(int n_pixels)
{
    try
    {
        if (!m_initialized_)
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
            
            // Save column widths when they change
            save_column_widths();
        }
    }
    catch (const std::exception& e)
    {
        handle_error(CString(e.what()));
    }
}

void DataListCtrl_Optimized::set_parent_window(CWnd* parent)
{
    m_parent_window_ = parent;
}

// Event handlers
void DataListCtrl_Optimized::OnDestroy()
{
    // Clean up resources
    if (m_infos_)
    {
        delete m_infos_;
        m_infos_ = nullptr;
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
        handle_error(CString(e.what()));
    }
}

void DataListCtrl_Optimized::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    try
    {
        CListCtrl::OnVScroll(nSBCode, nPos, pScrollBar);
        
        // Update display for new visible items
        update_display_info(nullptr);
    }
    catch (const std::exception& e)
    {
        handle_error(CString(e.what()));
    }
}

void DataListCtrl_Optimized::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    try
    {
        CListCtrl::OnHScroll(nSBCode, nPos, pScrollBar);
        
        // Update display for new visible items
        update_display_info(nullptr);
    }
    catch (const std::exception& e)
    {
        handle_error(CString(e.what()));
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
            update_display_info(nullptr);
        }
    }
    catch (const std::exception& e)
    {
        handle_error(CString(e.what()));
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
        handle_error(CString(e.what()));
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
        handle_error(CString(e.what()));
    }
}

void DataListCtrl_Optimized::OnLButtonDown(UINT nFlags, CPoint point)
{
    try
    {
        // Call the base class first
        CListCtrl::OnLButtonDown(nFlags, point);
        
        // Get the item under the cursor
        int itemIndex = HitTest(point);
        if (itemIndex >= 0)
        {
            // Update our selection tracking
            set_current_selection(itemIndex);
            
            // Notify the parent window about the selection change
            if (m_parent_window_)
            {
                const auto pdb_doc = static_cast<ViewdbWave_Optimized*>(m_parent_window_)->GetDocument();
                if (pdb_doc)
                {
                    if (pdb_doc->db_set_current_record_position(itemIndex))
                    {
                        pdb_doc->UpdateAllViews(nullptr, HINT_DOC_MOVE_RECORD);

                    }
                }
            }
        }
    }
    catch (const std::exception& e)
    {
        handle_error(CString(e.what()));
    }
}

void DataListCtrl_Optimized::center_item_in_viewport(int itemIndex)
{
    try
    {
        if (!GetSafeHwnd() || itemIndex < 0 || itemIndex >= GetItemCount())
        {
            return;
        }

        // First ensure the item is visible
        EnsureVisible(itemIndex, FALSE);
        
        // Get the current visible range
        int topIndex = GetTopIndex();
        int visibleCount = GetCountPerPage();
        int bottomIndex = topIndex + visibleCount - 1;

        // Always center the item, even if it's already visible
        // Calculate the target top index to center the item
        int targetTopIndex = itemIndex - (visibleCount / 2);
        if (targetTopIndex < 0) 
            targetTopIndex = 0;
        
        // Only scroll if the target position is different from current position
        if (targetTopIndex != topIndex)
        {
            // Use the standard MFC approach: ensure the target item is visible
            // This will automatically scroll the list to show the item
            EnsureVisible(targetTopIndex, FALSE);
            UpdateWindow();
        }
    }
    catch (const std::exception& e)
    {
        handle_error(CString(e.what()));
    }
}

void DataListCtrl_Optimized::OnColumnClick(NMHDR* pNMHDR, LRESULT* pResult)
{
    try
    {
        // Handle column click for sorting (if needed in the future)
        // For now, just log the event
        NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
        *pResult = 0;
    }
    catch (const std::exception& e)
    {
        handle_error(CString(e.what()));
        *pResult = 1;
    }
}

void DataListCtrl_Optimized::OnGetDisplayInfo(NMHDR* pNMHDR, LRESULT* pResult)
{
    try
    {
        LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
        if (pDispInfo->item.mask & LVIF_TEXT)
        {
            // Request state and image information as well
            pDispInfo->item.mask |= LVIF_STATE | LVIF_IMAGE;
        }
        
        // Always handle the display info request to ensure selection state is properly set
        handle_display_info_request(pDispInfo);
        *pResult = 0;
    }
    catch (const std::exception& e)
    {
        handle_error(CString(e.what()));
        *pResult = 1;
    }
}

// Private methods
void DataListCtrl_Optimized::create_empty_bitmap()
{
    try
    {
        if (m_empty_bitmap_)
        {
            m_empty_bitmap_.reset();
        }
        
        m_empty_bitmap_ = std::make_unique<CBitmap>();
        m_empty_bitmap_->CreateBitmap(16, 16, 1, 1, nullptr);
    }
    catch (const std::exception& e)
    {
        handle_error(CString(e.what()));
    }
}

void DataListCtrl_Optimized::create_colored_rectangles()
{
    try
    {
        if (!m_image_list_)
        {
            return;
        }
        
        // Use consistent dimensions - get width from column width or use default
        int width = g_column_width[DLC_COLUMN_CURVE];
        if (width <= 0)
        {
            width = 300; // Default width for curve column
        }
        const int height = 16;
        
        TRACE(_T("DataListCtrl_Optimized::create_colored_rectangles - Creating rectangles with size %dx%d\n"), width, height);
        
        // Create grey rectangle (empty display)
        {
            std::unique_ptr<CBitmap> greyBitmap = std::make_unique<CBitmap>();
            if (!greyBitmap->CreateCompatibleBitmap(GetDC(), width, height))
            {
                return;
            }
            
            CDC memDC;
            memDC.CreateCompatibleDC(GetDC());
            CBitmap* pOldBitmap = memDC.SelectObject(greyBitmap.get());
            
            // Fill with grey color
            CBrush greyBrush(RGB(128, 128, 128));
            CRect rect(0, 0, width, height);
            memDC.FillRect(&rect, &greyBrush);
            
            memDC.SelectObject(pOldBitmap);
            int greyIndex = m_image_list_->Add(greyBitmap.get(), RGB(128, 128, 128));
            TRACE(_T("DataListCtrl_Optimized::create_colored_rectangles - Added grey rectangle at index %d\n"), greyIndex);
            
            // Verify the image was added successfully
            if (greyIndex < 0)
            {
                TRACE(_T("DataListCtrl_Optimized::create_colored_rectangles - Failed to add grey bitmap to image list\n"));
                return;
            }
        }
        
        // Create blue rectangle (data display)
        {
            std::unique_ptr<CBitmap> blueBitmap = std::make_unique<CBitmap>();
            if (!blueBitmap->CreateCompatibleBitmap(GetDC(), width, height))
            {
                TRACE(_T("DataListCtrl_Optimized::create_colored_rectangles - Failed to create blue bitmap\n"));
                return;
            }
            
            CDC memDC;
            memDC.CreateCompatibleDC(GetDC());
            CBitmap* pOldBitmap = memDC.SelectObject(blueBitmap.get());
            
            // Fill with blue color
            CBrush blueBrush(RGB(0, 0, 255));
            CRect rect(0, 0, width, height);
            memDC.FillRect(&rect, &blueBrush);
            
            memDC.SelectObject(pOldBitmap);
            int blueIndex = m_image_list_->Add(blueBitmap.get(), RGB(0, 0, 255));
            TRACE(_T("DataListCtrl_Optimized::create_colored_rectangles - Added blue rectangle at index %d\n"), blueIndex);
            
            // Verify the image was added successfully
            if (blueIndex < 0)
            {
                TRACE(_T("DataListCtrl_Optimized::create_colored_rectangles - Failed to add blue bitmap to image list\n"));
                return;
            }
        }
        
        // Create red rectangle (spikes display)
        {
            std::unique_ptr<CBitmap> redBitmap = std::make_unique<CBitmap>();
            if (!redBitmap->CreateCompatibleBitmap(GetDC(), width, height))
            {
                TRACE(_T("DataListCtrl_Optimized::create_colored_rectangles - Failed to create red bitmap\n"));
                return;
            }
            
            CDC memDC;
            memDC.CreateCompatibleDC(GetDC());
            CBitmap* pOldBitmap = memDC.SelectObject(redBitmap.get());
            
            // Fill with red color
            CBrush redBrush(RGB(255, 0, 0));
            CRect rect(0, 0, width, height);
            memDC.FillRect(&rect, &redBrush);
            
            memDC.SelectObject(pOldBitmap);
            int redIndex = m_image_list_->Add(redBitmap.get(), RGB(255, 0, 0));
            TRACE(_T("DataListCtrl_Optimized::create_colored_rectangles - Added red rectangle at index %d\n"), redIndex);
            
            // Verify the image was added successfully
            if (redIndex < 0)
            {
                TRACE(_T("DataListCtrl_Optimized::create_colored_rectangles - Failed to add red bitmap to image list\n"));
                return;
            }
        }
        
        TRACE(_T("DataListCtrl_Optimized::create_colored_rectangles - Created 3 colored rectangles successfully\n"));
    }
    catch (const std::exception& e)
    {
        handle_error(CString(e.what()));
    }
}

void DataListCtrl_Optimized::setup_virtual_list_control()
{
    try
    {
        DWORD currentStyle = GetStyle();
        
        if (!(currentStyle & LVS_OWNERDATA))
        {
            ModifyStyle(0, LVS_OWNERDATA);
        }
    }
    catch (const std::exception& e)
    {
        handle_error(CString(e.what()));
    }
}

void DataListCtrl_Optimized::setup_style()
{
    try
    {
        DWORD currentStyle = GetStyle();
        if (!(currentStyle & LVS_OWNERDATA))
        {
            ModifyStyle(0, LVS_OWNERDATA);
        }
        DWORD extendedStyle = GetExtendedStyle()
            | LVS_EX_FULLROWSELECT | LVS_EX_ONECLICKACTIVATE | LVS_EX_SUBITEMIMAGES;
        //| LVS_EX_GRIDLINES // when this style is set, data vertical bar is suppressed
        SetExtendedStyle(extendedStyle);
    }
    catch (const std::exception& e)
    {
        handle_error(CString(e.what()));
    }
}

void DataListCtrl_Optimized::process_row_update(int index)
{
    try
    {
        if (!is_valid_index(index))
        {
            return;
        }

        update_cache(index, 0); // displayMode = 0 for now
        if (GetSafeHwnd())
        {
            RedrawItems(index, index);
        }
    }
    catch (const std::exception& e)
    {
        handle_error(CString(e.what()));
    }
}

void DataListCtrl_Optimized::update_display_info(LV_DISPINFO* pDispInfo)
{
    try
    {
        if (!m_initialized_ || !GetSafeHwnd())
        {
            return;
        }
        
        // Get current display mode for cache optimization
        int currentDisplayMode = m_config_.get_display_config().get_display_mode();
        
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
            auto cachedRow = m_cache_ ? m_cache_->get_cached_row(i) : nullptr;
            if (!cachedRow)
            {
                update_cache(i, currentDisplayMode);
            }
        }
        
        // Redraw only the actually visible items for better performance
        RedrawItems(firstVisible, lastVisible);
    }
    catch (const std::exception& e)
    {
        handle_error(CString(e.what()));
    }
}

void DataListCtrl_Optimized::handle_display_info_request(LV_DISPINFO* pDispInfo)
{
    try
    {
        int index = pDispInfo->item.iItem;
        
        if (!is_valid_index(index))
        {
            return;
        }
        
        // Handle selection state for virtual list control
        if (pDispInfo->item.mask & LVIF_STATE)
        {
            // Get current selection state
            DWORD state = 0;
            DWORD stateMask = 0;
            
            // Check if this item is currently selected using our stored selection
            if (index == m_current_selection_)
            {
                state |= LVIS_SELECTED | LVIS_FOCUSED;
                stateMask |= LVIS_SELECTED | LVIS_FOCUSED;
            }
            
            pDispInfo->item.state = state;
            pDispInfo->item.stateMask = stateMask;
        }
        
        // Get current display mode from configuration
        int currentDisplayMode = m_config_.get_display_config().get_display_mode();
        
        // Check cache first - use display mode for cache key
        auto cachedRow = m_cache_ ? m_cache_->get_cached_row(index) : nullptr;
        if (cachedRow)
        {
            handle_text_display(pDispInfo, cachedRow);
            handle_image_display(pDispInfo, index, currentDisplayMode);
            return;
        }
        
        // Load from database if not cached
        if (m_parent_window_)
        {
            const auto pdb_doc = static_cast<ViewdbWave_Optimized*>(m_parent_window_)->GetDocument();
            if (pdb_doc)
            {
                auto newRow = std::make_unique<DataListCtrl_Row_Optimized>();
                newRow->set_index(index);
                
                if (load_row_data_from_database(pdb_doc, index, *newRow))
                {
                    // Cache the row with current display mode
                    if (m_cache_)
                    {
                        m_cache_->set_cached_row(index, newRow.get(), currentDisplayMode);
                    }
                    
                    handle_text_display(pDispInfo, newRow.get());
                    handle_image_display(pDispInfo, index, currentDisplayMode);
                }
            }
        }
    }
    catch (const std::exception& e)
    {
        handle_error(CString(e.what()));
    }
}

void DataListCtrl_Optimized::handle_text_display(LV_DISPINFO* pDispInfo, DataListCtrl_Row_Optimized* row)
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
        cs.Format(_T("%i"), row->get_index());
        break;
    case DLC_COLUMN_INSECT: 
        cs.Format(_T("%i"), row->get_insect_id());
        break;
    case DLC_COLUMN_SENSI: 
        cs = row->get_sensillum_name();
        break;
    case DLC_COLUMN_STIM1: 
        cs = row->get_stimulus1();
        break;
    case DLC_COLUMN_CONC1: 
        cs = row->get_concentration1();
        break;
    case DLC_COLUMN_STIM2: 
        cs = row->get_stimulus2();
        break;
    case DLC_COLUMN_CONC2: 
        cs = row->get_concentration2();
        break;
    case DLC_COLUMN_NBSPK: 
        cs = row->get_n_spikes();
        break;
    case DLC_COLUMN_FLAG: 
        cs = row->get_flag();
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

void DataListCtrl_Optimized::handle_image_display(LV_DISPINFO* pDispInfo, int index, int displayMode)
{
    if (!(pDispInfo->item.mask & LVIF_IMAGE) || pDispInfo->item.iSubItem != DLC_COLUMN_CURVE)
    {
        return;
    }

    // For now, create a simple colored rectangle based on display mode
    // In a full implementation, this would create actual data/spike images
    switch (displayMode)
    {
    case 0: // DISPLAY_MODE_EMPTY - grey rectangle
        pDispInfo->item.iImage = 0; // Use empty bitmap (grey)
        TRACE(_T("DataListCtrl_Optimized::handle_image_display - Setting grey image (index 0)\n"));
        break;
    case 1: // DISPLAY_MODE_DATA - show data curve (blue rectangle for now)
        pDispInfo->item.iImage = 1; // Blue rectangle
        TRACE(_T("DataListCtrl_Optimized::handle_image_display - Setting blue image (index 1)\n"));
        break;
    case 2: // DISPLAY_MODE_SPIKES - show spikes (red rectangle for now)
        pDispInfo->item.iImage = 2; // Red rectangle
        TRACE(_T("DataListCtrl_Optimized::handle_image_display - Setting red image (index 2)\n"));
        break;
    default:
        pDispInfo->item.iImage = 0; // Default to empty
        TRACE(_T("DataListCtrl_Optimized::handle_image_display - Setting default grey image (index 0)\n"));
        break;
    }
}

void DataListCtrl_Optimized::process_display_mode(int rowIndex, int displayMode)
{
    try
    {
        ensure_row_exists(rowIndex);
        
        if (m_rows_[rowIndex])
        {
            // Process the display mode for the row
            // This would typically involve updating the row's display mode
        }
    }
    catch (const std::exception& e)
    {
        handle_error(CString(e.what()));
    }
}

void DataListCtrl_Optimized::update_cache(int index, int displayMode)
{
    try
    {
        if (!m_cache_ || !m_caching_enabled_)
        {
            return;
        }
        
        // Check if already cached with the same display mode
        auto existingCachedRow = m_cache_->get_cached_row(index);
        if (existingCachedRow)
        {
            return; // Already cached, no need to update
        }
        
        // Load data from database if not in memory
        if (m_parent_window_)
        {
            const auto pdb_doc = static_cast<ViewdbWave_Optimized*>(m_parent_window_)->GetDocument();
            if (pdb_doc)
            {
                auto newRow = std::make_unique<DataListCtrl_Row_Optimized>();
                newRow->set_index(index);
                
                if (load_row_data_from_database(pdb_doc, index, *newRow))
                {
                    // Cache the row with current display mode
                    m_cache_->set_cached_row(index, newRow.get(), displayMode);
                }
            }
        }
    }
    catch (const std::exception& e)
    {
        handle_error(CString(e.what()));
    }
}

void DataListCtrl_Optimized::invalidate_cache_for_row(int index)
{
    try
    {
        if (m_cache_)
        {
            m_cache_->invalidate_cache(index);
        }
    }
    catch (const std::exception& e)
    {
        handle_error(CString(e.what()));
    }
}

void DataListCtrl_Optimized::save_column_widths()
{
    try
    {
        if (!GetSafeHwnd())
        {
            return;
        }
        
        // Save current column widths to global array
        for (int i = 0; i < DLC_N_COLUMNS; ++i)
        {
            int width = GetColumnWidth(i);
            if (width > 0)
            {
                g_column_width[i] = width;
            }
        }
        
        // Save to registry for persistence across application restarts
        CWinApp* pApp = AfxGetApp();
        if (pApp)
        {
            bool savedToRegistry = false;
            for (int i = 0; i < DLC_N_COLUMNS; ++i)
            {
                CString keyName;
                keyName.Format(_T("ColumnWidth_%d"), i);
                int width = g_column_width[i];
                if (width > 0)
                {
                    pApp->WriteProfileInt(_T("DataListCtrl_Optimized"), keyName, width);
                    savedToRegistry = true;
                }
            }
            
            if (!savedToRegistry)
            {
                TRACE(_T("DataListCtrl_Optimized::save_column_widths - No valid column widths to save\n"));
            }
        }
        else
        {
            TRACE(_T("DataListCtrl_Optimized::save_column_widths - Failed to get application instance\n"));
        }
    }
    catch (const std::exception& e)
    {
        handle_error(CString(e.what()));
    }
}

void DataListCtrl_Optimized::load_column_widths()
{
    try
    {
        if (!GetSafeHwnd())
        {
            return;
        }
        
        // First try to load from registry
        CWinApp* pApp = AfxGetApp();
        bool loadedFromRegistry = false;
        
        if (pApp)
        {
            for (int i = 0; i < DLC_N_COLUMNS; ++i)
            {
                CString keyName;
                keyName.Format(_T("ColumnWidth_%d"), i);
                int width = pApp->GetProfileInt(_T("DataListCtrl_Optimized"), keyName, -1);
                if (width > 0)
                {
                    g_column_width[i] = width;
                    loadedFromRegistry = true;
                }
            }
        }
        
        // Apply column widths to the control
        for (int i = 0; i < DLC_N_COLUMNS; ++i)
        {
            const int width = g_column_width[i];
            if (width > 0)
            {
                SetColumnWidth(static_cast<int>(i), width);
            }
            else
            {
                // Set default widths for columns that don't have saved values
                int defaultWidth = 100; // Default for most columns
                if (i == DLC_COLUMN_CURVE)
                {
                    defaultWidth = 300; // Default width for curve column
                }
                else if (i == DLC_COLUMN_INDEX)
                {
                    defaultWidth = 80; // Default width for index column
                }
                
                SetColumnWidth(static_cast<int>(i), defaultWidth);
                g_column_width[i] = defaultWidth;
            }
        }
        
        loadedFromRegistry;
    }
    catch (const std::exception& e)
    {
        handle_error(CString(e.what()));
    }
}

void DataListCtrl_Optimized::apply_column_configuration()
{
    try
    {
        if (!GetSafeHwnd())
        {
            return;
        }
        
        auto& columns = m_config_.get_columns();
        for (size_t i = 0; i < columns.size(); ++i)
        {
            const auto& column = columns[i];
            SetColumnWidth(static_cast<int>(i), column.width);
        }
    }
    catch (const std::exception& e)
    {
        handle_error(CString(e.what()));
    }
}

bool DataListCtrl_Optimized::load_row_data_from_database(CdbWaveDoc* pdb_doc, int index, DataListCtrl_Row_Optimized& row)
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
        row.set_data_file_name(pdb_doc->db_get_current_dat_file_name(TRUE));
        row.set_spike_file_name(pdb_doc->db_get_current_spk_file_name(TRUE));
        
        // Get database table
        const auto database = pdb_doc->db_table;
        if (!database)
            return false;
        
        // Load record data using the same approach as the original DataListCtrl_Row
        DB_ITEMDESC desc;
        
        // Set the index
        row.set_index(index);
        
        // Get record ID
        database->get_record_item_value(CH_ID, &desc);
        row.set_record_id(desc.l_val);
        
        // Get insect ID
        database->get_record_item_value(CH_IDINSECT, &desc);
        row.set_insect_id(desc.l_val);
        
        // Get stimulus 1
        database->get_record_item_value(CH_STIM1_KEY, &desc);
        row.set_stimulus1(desc.cs_val);
        
        // Get concentration 1
        database->get_record_item_value(CH_CONC1_KEY, &desc);
        row.set_concentration1(desc.cs_val);
        
        // Get stimulus 2
        database->get_record_item_value(CH_STIM2_KEY, &desc);
        row.set_stimulus2(desc.cs_val);
        
        // Get concentration 2
        database->get_record_item_value(CH_CONC2_KEY, &desc);
        row.set_concentration2(desc.cs_val);
        
        // Get sensillum name
        database->get_record_item_value(CH_SENSILLUM_KEY, &desc);
        row.set_sensillum_name(desc.cs_val);
        
        // Get flag
        database->get_record_item_value(CH_FLAG, &desc);
        CString flagStr;
        flagStr.Format(_T("%i"), desc.l_val);
        row.set_flag(flagStr);
        
        // Get number of spikes
        if (row.get_spike_file_name().IsEmpty())
        {
            row.set_n_spikes(_T(""));
        }
        else
        {
            database->get_record_item_value(CH_NSPIKES, &desc);
            const int n_spikes = desc.l_val;
            database->get_record_item_value(CH_NSPIKECLASSES, &desc);
            CString spikesStr;
            spikesStr.Format(_T("%i (%i classes)"), n_spikes, desc.l_val);
            row.set_n_spikes(spikesStr);
        }
        
        return true;
    }
    catch (const std::exception& e)
    {
        handle_error(CString(e.what()));
        return false;
    }
}

void DataListCtrl_Optimized::handle_error(const CString& message)
{
    TRACE(_T("DataListCtrl_Optimized::HandleError - %s\n"), message);
    // Simple error handling - just log the error
}

void DataListCtrl_Optimized::set_current_selection(int record_position)
{
    try
    {
        if (!GetSafeHwnd())
        {
            return;
        }
        
        // Validate the record position
        if (record_position < 0 || record_position >= GetItemCount())
        {
            return;
        }
        
        // Store the current selection
        int oldSelection = m_current_selection_;
        m_current_selection_ = record_position;
        
        // Clear previous selection
        if (oldSelection >= 0 && oldSelection < GetItemCount())
        {
            SetItemState(oldSelection, 0, LVIS_SELECTED | LVIS_FOCUSED);
            RedrawItems(oldSelection, oldSelection);
        }
        
        // Set new selection
        SetItemState(record_position, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
        
        // Ensure the selected item is visible and centered
        EnsureVisible(record_position, FALSE);
        
        // Force a redraw of the affected items
        RedrawItems(record_position, record_position);
        if (oldSelection >= 0 && oldSelection < GetItemCount() && oldSelection != record_position)
        {
            RedrawItems(oldSelection, oldSelection);
        }
        
        // Update the window to show the changes
        UpdateWindow();
    }
    catch (const std::exception& e)
    {
        handle_error(CString(e.what()));
    }
}

void DataListCtrl_Optimized::setup_default_configuration()
{
    try
    {
        // Set default display mode to "no display" (grey rectangle) - mode 0
        auto& displayConfig = m_config_.get_display_config();
        displayConfig.set_display_mode(0); // DISPLAY_MODE_EMPTY
        
        // Set default time range
        auto& timeConfig = m_config_.get_time_settings();
        timeConfig.set_time_span(0.0f, 100.0f);
        
        // Set default amplitude span
        auto& amplitudeConfig = m_config_.get_amplitude_settings();
        amplitudeConfig.set_mv_span(1.0f);
        
        // Set default UI settings
        auto& uiConfig = m_config_.get_ui_settings();
        uiConfig.set_display_file_name(false); // Don't display file names by default
    }
    catch (const std::exception& e)
    {
        handle_error(CString(e.what()));
    }
}

void DataListCtrl_Optimized::initialize_columns()
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
        if (m_infos_)
            m_infos_->image_width = g_column_width[DLC_COLUMN_CURVE];
            
        // Initialize the configuration with the correct column information
        std::vector<data_list_ctrl_configuration::column_config> columns;
        for (int i = 0; i < DLC_N_COLUMNS; i++)
        {
            data_list_ctrl_configuration::column_config col;
            col.width = g_column_width[i];
            col.header = g_column_headers_[i];
            col.format = g_column_format_[i];
            col.index = i;
            col.visible = true;
            columns.push_back(col);
        }
        m_config_.set_columns(columns);
    }
    catch (const std::exception& e)
    {
        handle_error(CString(e.what()));
    }
}

void DataListCtrl_Optimized::setup_columns()
{
    try
    {
        initialize_columns();
        load_column_widths();
    }
    catch (const std::exception& e)
    {
        handle_error(CString(e.what()));
    }
}

void DataListCtrl_Optimized::setup_image_list()
{
    try
    {
        if (!GetSafeHwnd())
        {
            return;
        }
        
        // Use fixed dimensions for the image list to ensure consistency
        // Use a reasonable default width if column width is not set
        int width = g_column_width[DLC_COLUMN_CURVE];
        if (width <= 0)
        {
            width = 300; // Default width for curve column
        }
        const int height = 16; // Fixed height for consistency
        
        TRACE(_T("DataListCtrl_Optimized::setup_image_list - Creating image list with size %dx%d\n"), width, height);
        
        m_image_list_ = std::make_unique<CImageList>();
        if (!m_image_list_->Create(width, height, ILC_COLOR24 | ILC_MASK, 0, 3))
        {
            TRACE(_T("DataListCtrl_Optimized::setup_image_list - Failed to create image list\n"));
            handle_error(_T("Failed to create image list"));
            return;
        }
        
        TRACE(_T("DataListCtrl_Optimized::setup_image_list - Image list created successfully\n"));
        
        // Create colored rectangles for different display modes
        create_colored_rectangles();
        
        // Always try to set the image list - don't check for failure here
        SetImageList(m_image_list_.get(), LVSIL_SMALL);
        TRACE(_T("DataListCtrl_Optimized::setup_image_list - Image list set\n"));
        
        // Force a refresh to ensure the image list is properly applied
        if (GetSafeHwnd())
        {
            Invalidate();
            UpdateWindow();
        }
    }
    catch (const std::exception& e)
    {
        handle_error(CString(e.what()));
    }
}

void DataListCtrl_Optimized::initialize_image_list()
{
    setup_image_list();
}

void DataListCtrl_Optimized::ensure_row_exists(int index)
{
    if (index >= static_cast<int>(m_rows_.size()))
    {
        m_rows_.resize(index + 1);
    }
    
    if (!m_rows_[index])
    {
        m_rows_[index] = std::make_unique<DataListCtrl_Row_Optimized>(index);
    }
}

bool DataListCtrl_Optimized::is_valid_index(int index) const
{
    // For virtual list control, check against the total item count, not the rows vector size
    if (!GetSafeHwnd())
        return false;
    
    const int itemCount = GetItemCount();
    return index >= 0 && index < itemCount;
}

bool DataListCtrl_Optimized::is_valid_display_mode(int mode) const
{
    return mode >= 0 && mode <= 2; // Assuming 0-2 are valid display modes
}

void DataListCtrl_Optimized::validate_configuration() const
{
    if (!m_config_.validate_configuration())
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
    ON_WM_LBUTTONDOWN()
    ON_NOTIFY_REFLECT(LVN_GETDISPINFO, OnGetDisplayInfo)
END_MESSAGE_MAP()
