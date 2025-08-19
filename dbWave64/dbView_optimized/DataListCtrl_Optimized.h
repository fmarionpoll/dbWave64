#pragma once

#include "DataListCtrl_SupportingClasses.h"
#include "DataListCtrl_Configuration.h"
#include <afxwin.h>
#include <afxcmn.h>

// Forward declarations
class DataListCtrl_Row_Optimized;
struct DataListCtrlInfos;
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
    virtual ~DataListCtrl_Optimized();
    
    // Prevent copying
    DataListCtrl_Optimized(const DataListCtrl_Optimized&) = delete;
    DataListCtrl_Optimized& operator=(const DataListCtrl_Optimized&) = delete;
    
    // Initialization and setup
    void Initialize(const DataListCtrlConfiguration& config);
    bool IsInitialized() const { return m_initialized; }
    void SetupColumns();
    void SetupImageList();
    void SetParentWindow(CWnd* parent);
    
    // Row management
    void SetRowCount(int count);
    void ClearRows();
    void AddRow(std::unique_ptr<DataListCtrl_Row_Optimized> row);
    void RemoveRow(int index);
    void UpdateRow(int index);
    
    // Display management
    void SetDisplayMode(int mode);
    void SetTimeSpan(float first, float last);
    void SetAmplitudeSpan(float span);
    void SetDisplayFileName(bool display);
    void RefreshDisplay();
    
    // Configuration
    void SetConfiguration(const DataListCtrlConfiguration& config);
    const DataListCtrlConfiguration& GetConfiguration() const { return m_config; }
    
    // Caching
    void EnableCaching(bool enable) { m_cachingEnabled = enable; }
    bool IsCachingEnabled() const { return m_cachingEnabled; }
    void ClearCache();
    
    // Public interface methods (maintaining compatibility)
    void init_columns(std::vector<int>* width_columns);
    void set_amplitude_span(float span);
    void set_display_file_name(bool display);
    void set_time_intervals(float first, float last);
    void set_data_transform(int transform);
    void set_spike_plot_mode(int mode);
    void set_selected_class(int classIndex);
    
    // Signal column adjustment methods (from original DataListCtrl)
    void resize_signal_column(int n_pixels);
    void fit_columns_to_size(int n_pixels);
    
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
    
    DECLARE_MESSAGE_MAP()

private:
    // Core data members
    std::vector<std::unique_ptr<DataListCtrl_Row_Optimized>> m_rows;
    std::unique_ptr<DataListCtrlCache> m_cache;
    DataListCtrlConfiguration m_config;
    DataListCtrlInfos* m_infos;
    
    // State management
    bool m_initialized;
    bool m_cachingEnabled;
    
    // UI components
    CWnd* m_parentWindow;
    std::unique_ptr<CImageList> m_imageList;
    std::unique_ptr<CBitmap> m_emptyBitmap;
    
    // Core functionality methods
    void InitializeColumns();
    void InitializeImageList();
    void CreateEmptyBitmap();
    void SetupDefaultConfiguration();
    void SetupVirtualListControl();
    
    // Row management helpers
    void EnsureRowExists(int index);
    void ProcessRowUpdate(int index);
    
    // Display management
    void UpdateDisplayInfo(LV_DISPINFO* pDispInfo);
    void HandleDisplayInfoRequest(LV_DISPINFO* pDispInfo);
    void HandleTextDisplay(LV_DISPINFO* pDispInfo, DataListCtrl_Row_Optimized* row);
    void HandleImageDisplay(LV_DISPINFO* pDispInfo, int index, int displayMode);
    void ProcessDisplayMode(int rowIndex, int displayMode);
    
    // Caching helpers
    void UpdateCache(int index, int displayMode);
    void InvalidateCacheForRow(int index);
    
    // Error handling
    void HandleError(const CString& message = _T(""));
    void LogError(const CString& message) const;
    
    // Utility methods
    bool IsValidIndex(int index) const;
    bool IsValidDisplayMode(int mode) const;
    void ValidateConfiguration() const;
    
    // Column management
    void SaveColumnWidths();
    void LoadColumnWidths();
    void ApplyColumnConfiguration();
    
    // Database access
    bool LoadRowDataFromDatabase(CdbWaveDoc* pdb_doc, int index, DataListCtrl_Row_Optimized& row);
};
