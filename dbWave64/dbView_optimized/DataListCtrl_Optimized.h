#pragma once

#include "DataListCtrl_SupportingClasses.h"
#include "DataListCtrl_Configuration.h"
#include <afxwin.h>
#include <afxcmn.h>
#include <chrono>

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

// Constants namespace for chrono types
namespace DataListCtrlConstants
{
    // Performance settings - using MAX_CACHE_SIZE from DataListCtrl_SupportingClasses.h
    extern const std::chrono::milliseconds SCROLL_THROTTLE_TIME;
}

// Column configuration arrays (matching original DataListCtrl)
extern int g_column_width[DLC_N_COLUMNS];
extern CString g_column_headers_[DLC_N_COLUMNS];
extern int g_column_format_[DLC_N_COLUMNS];
extern int g_column_index_[DLC_N_COLUMNS];

// Exception handling - using definitions from DataListCtrl_SupportingClasses.h

// Performance monitoring
struct DataListCtrlPerformanceMetrics
{
    std::chrono::microseconds lastUpdateTime;
    std::chrono::microseconds averageUpdateTime;
    size_t totalUpdates;
    size_t cacheHits;
    size_t cacheMisses;
    size_t scrollEvents;
    size_t displayInfoCalls;
    
    DataListCtrlPerformanceMetrics()
        : lastUpdateTime(0)
        , averageUpdateTime(0)
        , totalUpdates(0)
        , cacheHits(0)
        , cacheMisses(0)
        , scrollEvents(0)
        , displayInfoCalls(0)
    {
    }
    
    void Reset()
    {
        lastUpdateTime = std::chrono::microseconds(0);
        averageUpdateTime = std::chrono::microseconds(0);
        totalUpdates = 0;
        cacheHits = 0;
        cacheMisses = 0;
        scrollEvents = 0;
        displayInfoCalls = 0;
    }
};

// Configuration management - using definition from DataListCtrl_Configuration.h

// Caching system - using definition from DataListCtrl_SupportingClasses.h

// Main optimized DataListCtrl class
class DataListCtrl_Optimized : public CListCtrl
{
    // Note: DECLARE_DYNAMIC removed due to MFC compatibility issues
    // If dynamic class support is needed, use DECLARE_DYNCREATE instead

public:
    // Constructors
    DataListCtrl_Optimized();
    virtual ~DataListCtrl_Optimized();
    
    // Prevent copying
    DataListCtrl_Optimized(const DataListCtrl_Optimized&) = delete;
    DataListCtrl_Optimized& operator=(const DataListCtrl_Optimized&) = delete;
    
    // Allow moving
    DataListCtrl_Optimized(DataListCtrl_Optimized&& other) noexcept;
    DataListCtrl_Optimized& operator=(DataListCtrl_Optimized&& other) noexcept;
    
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
    
    // Performance monitoring
    DataListCtrlPerformanceMetrics GetPerformanceMetrics() const { return m_performanceMetrics; }
    void ResetPerformanceMetrics();
    void EnablePerformanceMonitoring(bool enable) { m_performanceMonitoringEnabled = enable; }
    
    // Caching
    void EnableCaching(bool enable) { m_cachingEnabled = enable; }
    bool IsCachingEnabled() const { return m_cachingEnabled; }
    void ClearCache();
    
    // Thread safety
    void SetThreadSafe(bool threadSafe) { m_threadSafe = threadSafe; }
    bool IsThreadSafe() const { return m_threadSafe; }
    
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
    // Note: OnInitialUpdate() removed - this is a CView method, not appropriate for CListCtrl
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
    
    // Performance and monitoring
    DataListCtrlPerformanceMetrics m_performanceMetrics;
    bool m_performanceMonitoringEnabled;
    std::chrono::steady_clock::time_point m_lastScrollTime;
    
    // State management
    bool m_initialized;
    bool m_cachingEnabled;
    bool m_threadSafe;
    mutable std::mutex m_mutex;
    
    // UI components
    CWnd* m_parentWindow;
    std::unique_ptr<CImageList> m_imageList;
    std::unique_ptr<CBitmap> m_emptyBitmap;
    
    // Async processing
    std::future<void> m_asyncUpdateFuture;
    std::atomic<bool> m_updateCancelled;
    
    // Core functionality methods
    void InitializeColumns();
    void InitializeImageList();
    void CreateEmptyBitmap();
    void SetupDefaultConfiguration();
    
    // Row management helpers
    void EnsureRowExists(int index);
    void ProcessRowUpdate(int index);
    void BatchProcessRows(int startIndex, int endIndex);
    
    // Display management
    void UpdateDisplayInfo(LV_DISPINFO* pDispInfo);
    void HandleDisplayInfoRequest(LV_DISPINFO* pDispInfo);
    void ProcessDisplayMode(int rowIndex, int displayMode);
    
    // Caching helpers
    void UpdateCache(int index, int displayMode);
    void InvalidateCacheForRow(int index);
    
    // Performance measurement
    template<typename Func>
    auto MeasurePerformance(Func&& func, const CString& operation = _T("")) -> decltype(func())
    {
        if (!m_performanceMonitoringEnabled)
            return std::forward<Func>(func)();
        
        auto start = std::chrono::high_resolution_clock::now();
        auto result = std::forward<Func>(func)();
        auto end = std::chrono::high_resolution_clock::now();
        
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        m_performanceMetrics.lastUpdateTime = duration;
        m_performanceMetrics.totalUpdates++;
        
        // Update average
        if (m_performanceMetrics.totalUpdates > 1)
        {
            auto total = m_performanceMetrics.averageUpdateTime * (m_performanceMetrics.totalUpdates - 1) + duration;
            m_performanceMetrics.averageUpdateTime = total / m_performanceMetrics.totalUpdates;
        }
        else
        {
            m_performanceMetrics.averageUpdateTime = duration;
        }
        
        return result;
    }
    
    // Thread safety helpers
    void LockIfThreadSafe() const
    {
        if (m_threadSafe)
            m_mutex.lock();
    }
    
    void UnlockIfThreadSafe() const
    {
        if (m_threadSafe)
            m_mutex.unlock();
    }
    
    // Error handling
    void HandleError(DataListCtrlError error, const CString& message = _T(""));
    void LogError(const CString& message) const;
    void LogPerformance(const CString& operation, std::chrono::microseconds duration) const;
    
    // Utility methods
    bool IsValidIndex(int index) const;
    bool IsValidDisplayMode(int mode) const;
    void ValidateConfiguration() const;
    
    // Async processing
    void StartAsyncUpdate();
    void CancelAsyncUpdate();
    void ProcessAsyncUpdate();
    
    // Scroll handling
    void HandleScrollEvent(UINT scrollCode, UINT position);
    bool ShouldThrottleScroll() const;
    
    // Column management
    void SaveColumnWidths();
    void LoadColumnWidths();
    void ApplyColumnConfiguration();
};
