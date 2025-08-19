#pragma once

#include <memory>
#include <mutex>
#include <vector>
#include <functional>
#include <future>
#include <atomic>
#include <chrono>
#include <map>
#include <string>
#include <afxwin.h>
#include <afxext.h>
#include <afxcmn.h>

// Forward declarations
class CListCtrl;
class CImageList;
class CBitmap;
class CDC;
class CBrush;
class CPen;
class DataListCtrl_Row_Optimized;

// Constants namespace
namespace DataListCtrlConstants
{
    // Default values - using definitions from DataListCtrl_Configuration.h
    constexpr int DEFAULT_DISPLAY_MODE = 0;
    constexpr float DEFAULT_TIME_FIRST = 0.0f;
    constexpr float DEFAULT_TIME_LAST = 100.0f;
    constexpr float DEFAULT_MV_SPAN = 1.0f;
    
    // Performance settings
    constexpr size_t DEFAULT_CACHE_SIZE = 100;
    constexpr size_t DEFAULT_BATCH_SIZE = 10;
    constexpr std::chrono::milliseconds UI_UPDATE_THROTTLE{16}; // ~60 FPS
    
    // Column IDs
    constexpr int COLUMN_INDEX = 0;
    constexpr int COLUMN_TIME = 1;
    constexpr int COLUMN_AMPLITUDE = 2;
    constexpr int COLUMN_FILENAME = 3;
    constexpr int COLUMN_STATUS = 4;
    
    // Image list indices
    constexpr int IMAGE_INDEX_DEFAULT = 0;
    constexpr int IMAGE_INDEX_SELECTED = 1;
    constexpr int IMAGE_INDEX_ERROR = 2;
}

// Exception handling
enum class DataListCtrlError
{
    SUCCESS,
    INVALID_PARAMETER,
    INVALID_INDEX,
    MEMORY_ALLOCATION_FAILED,
    GDI_RESOURCE_FAILED,
    UI_UPDATE_FAILED,
    CACHE_OPERATION_FAILED,
    INITIALIZATION_FAILED,
    CONFIGURATION_ERROR,
    ROW_OPERATION_FAILED,
    DISPLAY_MODE_ERROR,
    ASYNC_OPERATION_FAILED,
    SERIALIZATION_FAILED,
    WINDOW_CREATION_FAILED,
    FILE_OPEN_FAILED,
    DATABASE_ACCESS_FAILED
};

class DataListCtrlException : public std::exception
{
public:
    explicit DataListCtrlException(DataListCtrlError error, const CString& message = _T(""));
    DataListCtrlError GetError() const { return m_error; }
    CString GetMessage() const { return m_message; }
    const char* what() const noexcept override;
    
private:
    DataListCtrlError m_error;
    CString m_message;
};

// Row data structure
struct DataListCtrlRow
{
    int index;
    double time;
    double amplitude;
    CString filename;
    CString status;
    bool selected;
    bool visible;
    int displayMode;
    std::chrono::steady_clock::time_point timestamp;
    bool isValid;
    
    DataListCtrlRow()
        : index(0)
        , time(0.0)
        , amplitude(0.0)
        , selected(false)
        , visible(true)
        , displayMode(0)
        , timestamp(std::chrono::steady_clock::now())
        , isValid(true)
    {
    }
    
    DataListCtrlRow(int idx, double t, double amp, const CString& file, const CString& stat)
        : index(idx)
        , time(t)
        , amplitude(amp)
        , filename(file)
        , status(stat)
        , selected(false)
        , visible(true)
        , displayMode(0)
        , timestamp(std::chrono::steady_clock::now())
        , isValid(true)
    {
    }
};

// Cache for row data
class DataListCtrlCache
{
public:
    explicit DataListCtrlCache(size_t maxSize = DataListCtrlConstants::DEFAULT_CACHE_SIZE);
    ~DataListCtrlCache() = default;
    
    void AddRow(int index, const DataListCtrlRow& row);
    bool GetRow(int index, DataListCtrlRow& row) const;
    void RemoveRow(int index);
    void InvalidateCache(int index);  // Alias for RemoveRow for compatibility
    void SetCachedRow(int index, DataListCtrl_Row_Optimized* row, int displayMode);
    void Clear();
    size_t GetSize() const;
    size_t GetMaxSize() const { return m_maxSize; }
    void SetMaxSize(size_t maxSize);
    
    // Bitmap caching methods (for DataListCtrl_Row_Optimized)
    CBitmap* GetCachedBitmap(int index, int displayMode);
    void SetCachedBitmap(int index, std::unique_ptr<CBitmap> bitmap, int displayMode);
    void ClearExpiredCache();
    void ClearAll();
    void EvictOldestIfNeeded();
    
    // Performance tracking
    size_t GetHitCount() const { return m_hitCount; }
    size_t GetMissCount() const { return m_missCount; }
    double GetHitRatio() const;
    void ResetStats();
    
private:
    // Row data cache
    std::map<int, DataListCtrlRow> m_cache;
    
    // Bitmap cache (for DataListCtrl_Row_Optimized)
    struct CachedBitmap
    {
        std::unique_ptr<CBitmap> bitmap;
        std::chrono::steady_clock::time_point timestamp;
        bool isValid;
        int displayMode;
        
        // Default constructor required by std::map
        CachedBitmap()
            : bitmap(nullptr)
            , timestamp(std::chrono::steady_clock::now())
            , isValid(false)
            , displayMode(0)
        {
        }
        
        CachedBitmap(std::unique_ptr<CBitmap> bmp, int mode)
            : bitmap(std::move(bmp))
            , timestamp(std::chrono::steady_clock::now())
            , isValid(true)
            , displayMode(mode)
        {
        }
    };
    std::map<int, CachedBitmap> m_bitmapCache;
    
    size_t m_maxSize;
    mutable std::mutex m_cacheMutex;
    
    // Performance statistics
    mutable std::atomic<size_t> m_hitCount;
    mutable std::atomic<size_t> m_missCount;
};

// GDI Resource Manager for RAII
class GdiResourceManager
{
public:
    GdiResourceManager();
    ~GdiResourceManager();
    
    // DC management
    CDC* CreateCompatibleDC(CDC* pDC);
    void DeleteDC(CDC* pDC);
    
    // Bitmap management
    CBitmap* CreateBitmap(int width, int height, int planes, int bitsPerPixel, const void* data);
    void DeleteBitmap(CBitmap* pBitmap);
    
    // Brush management
    CBrush* CreateSolidBrush(COLORREF color);
    void DeleteBrush(CBrush* pBrush);
    
    // Pen management
    CPen* CreatePen(int style, int width, COLORREF color);
    void DeletePen(CPen* pPen);
    
    // Image list management
    CImageList* CreateImageList(int width, int height, UINT flags, int initial, int grow);
    void DeleteImageList(CImageList* pImageList);
    
private:
    std::vector<std::unique_ptr<CDC>> m_dcList;
    std::vector<std::unique_ptr<CBitmap>> m_bitmapList;
    std::vector<std::unique_ptr<CBrush>> m_brushList;
    std::vector<std::unique_ptr<CPen>> m_penList;
    std::vector<std::unique_ptr<CImageList>> m_imageListList;
    mutable std::mutex m_resourceMutex;
};

// Performance monitor for DataListCtrl
class DataListCtrlPerformanceMonitor
{
public:
    DataListCtrlPerformanceMonitor();
    ~DataListCtrlPerformanceMonitor() = default;
    
    void StartOperation(const CString& operationName);
    void EndOperation(const CString& operationName);
    void SetEnabled(bool enabled) { m_enabled = enabled; }
    bool IsEnabled() const { return m_enabled; }
    
    // Performance metrics
    struct Metrics
    {
        std::chrono::microseconds lastUIUpdateTime{0};
        std::chrono::microseconds averageUIUpdateTime{0};
        size_t totalUIUpdates = 0;
        size_t cacheHits = 0;
        size_t cacheMisses = 0;
        size_t rowOperations = 0;
        size_t displayModeChanges = 0;
        
        void Reset();
    };
    
    Metrics GetMetrics() const;
    CString GetPerformanceReport() const;
    void Reset();
    
private:
    bool m_enabled;
    std::chrono::steady_clock::time_point m_startTime;
    std::map<CString, std::chrono::steady_clock::time_point> m_operationStartTimes;
    std::map<CString, size_t> m_operationCounts;
    std::map<CString, std::chrono::microseconds> m_operationDurations;
    mutable std::mutex m_metricsMutex;
};

// Configuration validator
class DataListCtrlConfigurationValidator
{
public:
    static bool ValidateImageDimensions(int width, int height);
    static bool ValidateTimeRange(double first, double last);
    static bool ValidateAmplitudeSpan(double span);
    static bool ValidateDisplayMode(int mode);
    static bool ValidateCacheSize(size_t size);
    static bool ValidateBatchSize(size_t size);
    
    static CString GetValidationErrorMessage(DataListCtrlError error);
};

// Row operation manager
class DataListCtrlRowManager
{
public:
    DataListCtrlRowManager();
    ~DataListCtrlRowManager() = default;
    
    // Row operations
    void AddRow(const DataListCtrlRow& row);
    void RemoveRow(int index);
    void UpdateRow(int index, const DataListCtrlRow& row);
    bool GetRow(int index, DataListCtrlRow& row) const;
    void ClearRows();
    
    // Batch operations
    void AddRows(const std::vector<DataListCtrlRow>& rows);
    void RemoveRows(const std::vector<int>& indices);
    void UpdateRows(const std::map<int, DataListCtrlRow>& rowUpdates);
    
    // Query operations
    size_t GetRowCount() const;
    std::vector<DataListCtrlRow> GetVisibleRows() const;
    std::vector<DataListCtrlRow> GetSelectedRows() const;
    std::vector<int> GetRowIndices() const;
    
    // Filtering and sorting
    void SetFilter(std::function<bool(const DataListCtrlRow&)> filter);
    void ClearFilter();
    void SortRows(std::function<bool(const DataListCtrlRow&, const DataListCtrlRow&)> comparator);
    
private:
    std::vector<DataListCtrlRow> m_rows;
    std::function<bool(const DataListCtrlRow&)> m_filter;
    mutable std::mutex m_rowsMutex;
};

// Display mode manager
class DataListCtrlDisplayManager
{
public:
    DataListCtrlDisplayManager();
    ~DataListCtrlDisplayManager() = default;
    
    // Display mode management
    void SetDisplayMode(int mode);
    int GetDisplayMode() const { return m_displayMode; }
    bool IsValidDisplayMode(int mode) const;
    
    // Image list management
    void CreateImageList(int width, int height);
    void DestroyImageList();
    CImageList* GetImageList() const { return m_pImageList.get(); }
    
    // Drawing operations
    void DrawRow(CDC* pDC, const DataListCtrlRow& row, const CRect& rect);
    void DrawBackground(CDC* pDC, const CRect& rect);
    void DrawSelection(CDC* pDC, const CRect& rect);
    
    // Color management
    void SetBackgroundColor(COLORREF color);
    void SetTextColor(COLORREF color);
    void SetSelectionColor(COLORREF color);
    COLORREF GetBackgroundColor() const { return m_backgroundColor; }
    COLORREF GetTextColor() const { return m_textColor; }
    COLORREF GetSelectionColor() const { return m_selectionColor; }
    
private:
    int m_displayMode;
    std::unique_ptr<CImageList> m_pImageList;
    COLORREF m_backgroundColor;
    COLORREF m_textColor;
    COLORREF m_selectionColor;
    mutable std::mutex m_displayMutex;
};
