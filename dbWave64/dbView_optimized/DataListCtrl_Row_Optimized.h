#pragma once

// MFC includes (must come first)
#include <afx.h>
#include <afxwin.h>

// Standard library includes
#include <memory>
#include <mutex>
#include <string>
#include <vector>
#include <chrono>
#include <unordered_map>
#include <functional>

// Supporting classes
#include "DataListCtrl_SupportingClasses.h"
#include "DataListCtrl_Configuration.h"

// Forward declarations
class AcqDataDoc;
class ChartData;
class CSpikeDoc;
class ChartSpikeBar;
class CdbWaveDoc;
struct DataListCtrlInfos;
class CDC;
class CBitmap;
class CWnd;

// Constants namespace
namespace DataListCtrlConstants
{
    // Display modes - using definitions from DataListCtrl_Configuration.h
    
    // Image dimensions - using definitions from DataListCtrl_Configuration.h
    
    // Colors
    constexpr COLORREF COLOR_LIGHT_GREY = RGB(192, 192, 192);
    constexpr COLORREF COLOR_WHITE = RGB(255, 255, 255);
    constexpr COLORREF COLOR_BLACK = RGB(0, 0, 0);
    
    // Font settings
    constexpr int FONT_SIZE = 10;
    constexpr int MAX_FILENAME_LENGTH = 20;
    constexpr int FILENAME_TRUNCATE_LENGTH = 17;
    
    // Window IDs
    constexpr UINT DATA_WINDOW_ID_BASE = 100;
    constexpr UINT SPIKE_WINDOW_ID_BASE = 1000;
    
    // Cache settings - using definitions from DataListCtrl_SupportingClasses.h
    constexpr std::chrono::minutes CACHE_EXPIRY_TIME{5};
}

// Exception handling - using definitions from DataListCtrl_SupportingClasses.h

// RAII GDI Resource Management - using definition from DataListCtrl_SupportingClasses.h

// Caching system - using definition from DataListCtrl_SupportingClasses.h

// Main optimized DataListCtrl_Row class
class DataListCtrl_Row_Optimized : public CObject
{
    DECLARE_SERIAL(DataListCtrl_Row_Optimized)

public:
    // Constructors
    DataListCtrl_Row_Optimized();
    explicit DataListCtrl_Row_Optimized(int index);
    
    // Destructor (no manual cleanup needed with smart pointers)
    ~DataListCtrl_Row_Optimized() override = default;
    
    // Copy constructor and assignment operator
    DataListCtrl_Row_Optimized(const DataListCtrl_Row_Optimized& other);
    DataListCtrl_Row_Optimized& operator=(const DataListCtrl_Row_Optimized& other);
    
    // Move constructor and assignment operator
    DataListCtrl_Row_Optimized(DataListCtrl_Row_Optimized&& other) noexcept;
    DataListCtrl_Row_Optimized& operator=(DataListCtrl_Row_Optimized&& other) noexcept;
    
    // Public interface
    void AttachDatabaseRecord(CdbWaveDoc* db_wave_doc);
    void SetDisplayParameters(DataListCtrlInfos* infos, int imageIndex);
    void ResetDisplayProcessed();
    
    // Serialization
    void Serialize(CArchive& ar) override;
    
    // Test-friendly methods
    bool IsDisplayProcessed() const { return m_displayProcessed; }
    int GetLastDisplayMode() const { return m_lastDisplayMode; }
    int GetIndex() const { return m_index; }
    
    // Dependency injection for testing
    void SetDataDocument(std::unique_ptr<AcqDataDoc> doc);
    void SetSpikeDocument(std::unique_ptr<CSpikeDoc> doc);
    void SetChartDataWindow(std::unique_ptr<ChartData> chart);
    void SetChartSpikeWindow(std::unique_ptr<ChartSpikeBar> chart);
    
    // Configuration
    void SetCacheEnabled(bool enabled) { m_cacheEnabled = enabled; }
    bool IsCacheEnabled() const { return m_cacheEnabled; }
    
    // Performance monitoring
    struct PerformanceMetrics
    {
        std::chrono::microseconds lastRenderTime{0};
        size_t cacheHits = 0;
        size_t cacheMisses = 0;
        size_t totalRenders = 0;
    };
    
    PerformanceMetrics GetPerformanceMetrics() const { return m_performanceMetrics; }
    void ResetPerformanceMetrics();

private:
    // Core data members
    int m_index{0};
    long m_insectId{0};
    int m_recordId{0};
    
    // String data
    CString m_comment;
    CString m_dataFileName;
    CString m_spikeFileName;
    CString m_sensillumName;
    CString m_stimulus1;
    CString m_concentration1;
    CString m_stimulus2;
    CString m_concentration2;
    CString m_nSpikes;
    CString m_flag;
    CString m_date;
    
    // Smart pointer managed resources
    std::unique_ptr<AcqDataDoc> m_pDataDoc;
    std::unique_ptr<ChartData> m_pChartDataWnd;
    std::unique_ptr<CSpikeDoc> m_pSpikeDoc;
    std::unique_ptr<ChartSpikeBar> m_pChartSpikeWnd;
    
    // Display state
    bool m_displayProcessed{false};
    int m_lastDisplayMode{-1};
    bool m_init{false};
    bool m_changed{false};
    WORD m_version{0};
    
    // Thread safety
    mutable std::mutex m_mutex;
    
    // Caching
    std::unique_ptr<DataListCtrlCache> m_cache;
    bool m_cacheEnabled{true};
    
    // Performance monitoring
    PerformanceMetrics m_performanceMetrics;
    
    // Core functionality methods
    void InitializeDisplayComponents(DataListCtrlInfos* infos, int imageIndex);
    void ValidateImageIndex(int imageIndex, CdbWaveDoc* pDoc);
    void ProcessDisplayMode(DataListCtrlInfos* infos, int imageIndex);
    
    // Data window management
    void CreateDataWindow(DataListCtrlInfos* infos, int imageIndex);
    void LoadDataDocument();
    void ConfigureDataWindow(DataListCtrlInfos* infos);
    void DisplayDataWindow(DataListCtrlInfos* infos, int imageIndex);
    
    // Spike window management
    void CreateSpikeWindow(DataListCtrlInfos* infos, int imageIndex);
    void LoadSpikeDocument();
    void ConfigureSpikeWindow(DataListCtrlInfos* infos);
    void DisplaySpikeWindow(DataListCtrlInfos* infos, int imageIndex);
    
    // Empty window management
    void DisplayEmptyWindow(DataListCtrlInfos* infos, int imageIndex);
    
    // Plotting functionality
    void PlotToImageList(DataListCtrlInfos* infos, int imageIndex, 
                        ChartData* pChart, const CString& filename);
    void PlotToImageList(DataListCtrlInfos* infos, int imageIndex, 
                        ChartSpikeBar* pChart, const CString& filename);
    void CreatePlotBitmap(DataListCtrlInfos* infos, int imageIndex,
                         ChartData* pChart, CBitmap& bitmap);
    void CreatePlotBitmap(DataListCtrlInfos* infos, int imageIndex,
                         ChartSpikeBar* pChart, CBitmap& bitmap);
    
    // Utility methods
    CString ExtractFilename(const CString& fullPath) const;
    void TruncateFilename(CString& filename) const;
    bool IsValidIndex(int index, int maxCount) const;
    bool IsValidDisplayMode(int mode) const;
    
    // Error handling
    void HandleError(DataListCtrlError error, const CString& message = _T(""));
    void LogError(const CString& message) const;
    
    // Serialization helpers
    void SerializeStoring(CArchive& ar);
    void SerializeLoading(CArchive& ar);
    void SerializeStrings(CArchive& ar, bool isStoring);
    void SerializeObjects(CArchive& ar, bool isStoring);
    
    // Performance measurement - simplified for void functions only
    template<typename Func>
    void MeasurePerformance(Func&& func)
    {
        auto start = std::chrono::high_resolution_clock::now();
        std::forward<Func>(func)();
        auto end = std::chrono::high_resolution_clock::now();
        
        m_performanceMetrics.lastRenderTime = 
            std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        m_performanceMetrics.totalRenders++;
    }
};
