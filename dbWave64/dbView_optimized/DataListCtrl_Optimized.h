#pragma once

#include "DataListCtrl_SupportingClasses.h"
#include "DataListCtrl_Configuration.h"
#include <afxwin.h>
#include <afxcmn.h>

// Forward declarations
class DataListCtrl_Row_Optimized;
struct data_list_ctrl_infos;
class CdbWaveDoc;
class CWnd;

// Constants - using defines for maximum compatibility
#define DLC_N_COLUMNS 11
#define DLC_COLUMN_INDEX 1
#define DLC_COLUMN_CURVE 2
#define DLC_COLUMN_INSECT 3
#define DLC_COLUMN_SENSI 4
#define DLC_COLUMN_STIM1 5
#define DLC_COLUMN_CONC1 6
#define DLC_COLUMN_STIM2 7
#define DLC_COLUMN_CONC2 8
#define DLC_COLUMN_NBSPK 9
#define DLC_COLUMN_FLAG 10
#define DLC_BATCH_SIZE 10
#define DLC_MIN_COLUMN_WIDTH 10
#define DLC_MAX_COLUMN_WIDTH 500

// Column configuration arrays (matching original DataListCtrl)
extern int g_column_width[DLC_N_COLUMNS];
extern CString g_column_headers_[DLC_N_COLUMNS];
extern int g_column_format_[DLC_N_COLUMNS];
extern int g_column_index_[DLC_N_COLUMNS];

// Main optimized DataListCtrl class - Simplified
class DataListCtrl_Optimized : public CListCtrl
{
public:
    // Constructors
    DataListCtrl_Optimized();
    ~DataListCtrl_Optimized() override;
    
    // Prevent copying
    DataListCtrl_Optimized(const DataListCtrl_Optimized&) = delete;
    DataListCtrl_Optimized& operator=(const DataListCtrl_Optimized&) = delete;
    
    // Initialization and setup
    void initialize(const data_list_ctrl_configuration& config);
    bool is_initialized() const { return m_initialized_; }
    void setup_columns();
    void setup_image_list();
    void set_parent_window(CWnd* parent);
    
    // Row management
    void set_row_count(int count);
	int get_row_count() const { return static_cast<int>(m_rows_.size()); }
    void clear_rows();
    void add_row(std::unique_ptr<DataListCtrl_Row_Optimized> row);
    void remove_row(int index);
    void update_row(int index);
    
    // Display management
    void set_time_span(float first, float last);
    void set_amplitude_span(float span);
    void set_display_file_name(bool display);
    void refresh_display();
    
    // Selection management
    void set_current_selection(int record_position);
    
    // Configuration
    void set_configuration(const data_list_ctrl_configuration& config);
    const data_list_ctrl_configuration& get_configuration() const { return m_config_; }
    
    // Caching
    void enable_caching(const bool enable) { m_caching_enabled_ = enable; }
    bool is_caching_enabled() const { return m_caching_enabled_; }
    void clear_cache();
    
    // Public interface methods (maintaining compatibility)
    void init_columns(std::vector<int>* width_columns);
    void set_time_intervals(float first, float last);
    void set_data_transform(int transform);
    void set_spike_plot_mode(int mode);
    void set_selected_class(int classIndex);
    
    // Signal column adjustment methods (from original DataListCtrl)
    void resize_signal_column(int n_pixels);
    void fit_columns_to_size(int n_pixels);

    // Column management
    void save_column_widths();
    void load_column_widths();
    
    // Display mode management
    void set_display_mode(int mode);
    int get_display_mode();
    
    // Selection management
    int get_current_selection() const { return m_current_selection_; }
    
    // Viewport management
    void center_item_in_viewport(int itemIndex);
    
    // Override methods
    virtual void OnDestroy();
    virtual void OnSize(UINT nType, int cx, int cy);
    
protected:
    // Message map handlers
    afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnGetDisplayInfo(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnColumnClick(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    
    DECLARE_MESSAGE_MAP()

private:
    // Core data members
    std::vector<std::unique_ptr<DataListCtrl_Row_Optimized>> m_rows_;
    std::unique_ptr<DataListCtrlCache> m_cache_;
    data_list_ctrl_configuration m_config_;
    data_list_ctrl_infos* m_infos_;
    
    // State management
    bool m_initialized_;
    bool m_caching_enabled_;
    int m_current_selection_; // Track the currently selected item
    
    // UI components
    CWnd* m_parent_window_;
    std::unique_ptr<CImageList> m_image_list_;
    std::unique_ptr<CBitmap> m_empty_bitmap_;
    
    // Core functionality methods
    void initialize_columns();
    void initialize_image_list();
    void create_empty_bitmap();
    void create_colored_rectangles();
    void setup_default_configuration();
    void setup_virtual_list_control();
    
    // Row management helpers
    void ensure_row_exists(int index);
    void process_row_update(int index);
    
    // Display management
    void update_display_info(LV_DISPINFO* pDispInfo);
    void handle_display_info_request(LV_DISPINFO* pDispInfo);
    void handle_text_display(LV_DISPINFO* pDispInfo, DataListCtrl_Row_Optimized* row);
    void handle_image_display(LV_DISPINFO* pDispInfo, int index, int displayMode);
    void process_display_mode(int rowIndex, int displayMode);
    
    // Caching helpers
    void update_cache(int index, int displayMode);
    void invalidate_cache_for_row(int index);
    
    // Error handling
    void handle_error(const CString& message = _T(""));
    
    // Utility methods
    bool is_valid_index(int index) const;
    bool is_valid_display_mode(int mode) const;
    void validate_configuration() const;
    
    // Column management
    void apply_column_configuration();
    
    // Database access
    bool load_row_data_from_database(CdbWaveDoc* pdb_doc, int index, DataListCtrl_Row_Optimized& row);
};
