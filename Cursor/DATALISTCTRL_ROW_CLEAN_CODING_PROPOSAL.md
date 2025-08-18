# DataListCtrl_Row.cpp - Clean Coding Proposal

## Current Issues Analysis

### 1. **Memory Management Issues**
- **Problem**: Manual memory management with raw pointers and manual deletion
- **Risk**: Memory leaks, dangling pointers, exception safety issues
- **Current Code**:
```cpp
delete p_chart_data_wnd;
delete p_chart_spike_wnd;
SAFE_DELETE(p_data_doc)
SAFE_DELETE(p_spike_doc)
```

### 2. **Resource Management**
- **Problem**: No RAII for GDI resources (DC, Bitmap, Brush, Pen)
- **Risk**: Resource leaks, especially in error paths
- **Current Code**:
```cpp
const auto p_dc = p_chart_data_wnd->GetDC();
CDC mem_dc;
CBitmap bitmap_plot;
// Manual cleanup required
```

### 3. **Error Handling**
- **Problem**: Inconsistent error handling, some errors ignored
- **Risk**: Silent failures, undefined behavior
- **Current Code**:
```cpp
if (!create_result)
{
    delete p_chart_data_wnd;
    p_chart_data_wnd = nullptr;
    return; // Error silently ignored
}
```

### 4. **Code Duplication**
- **Problem**: Similar plotting logic repeated in `plot_data()` and `plot_spikes()`
- **Risk**: Maintenance burden, inconsistent behavior

### 5. **Magic Numbers and Constants**
- **Problem**: Hard-coded values scattered throughout
- **Risk**: Difficult to maintain, unclear intent

### 6. **Long Methods**
- **Problem**: Methods like `set_display_parameters()` are too long and complex
- **Risk**: Hard to understand, test, and maintain

## Clean Coding Proposal

### 1. **Smart Pointer Implementation**

```cpp
// Header file changes
#include <memory>
#include <vector>

class DataListCtrl_Row : public CObject
{
private:
    // Replace raw pointers with smart pointers
    std::unique_ptr<AcqDataDoc> p_data_doc;
    std::unique_ptr<ChartData> p_chart_data_wnd;
    std::unique_ptr<CSpikeDoc> p_spike_doc;
    std::unique_ptr<ChartSpikeBar> p_chart_spike_wnd;
    
    // Add RAII wrapper for GDI resources
    class GdiResourceManager;
    std::unique_ptr<GdiResourceManager> gdi_manager;
};
```

### 2. **RAII GDI Resource Management**

```cpp
class GdiResourceManager
{
public:
    class DeviceContext
    {
    public:
        DeviceContext(CWnd* pWnd);
        ~DeviceContext();
        
        CDC* GetDC() { return m_pDC.get(); }
        operator CDC*() { return m_pDC.get(); }
        
    private:
        std::unique_ptr<CDC> m_pDC;
        CWnd* m_pWnd;
    };
    
    class CompatibleDC
    {
    public:
        CompatibleDC(CDC* pDC);
        ~CompatibleDC();
        
        CDC* GetDC() { return m_pMemDC.get(); }
        operator CDC*() { return m_pMemDC.get(); }
        
    private:
        std::unique_ptr<CDC> m_pMemDC;
    };
    
    class Bitmap
    {
    public:
        Bitmap(int width, int height, CDC* pDC);
        ~Bitmap();
        
        CBitmap* GetBitmap() { return m_pBitmap.get(); }
        operator CBitmap*() { return m_pBitmap.get(); }
        
    private:
        std::unique_ptr<CBitmap> m_pBitmap;
    };
};
```

### 3. **Constants and Configuration**

```cpp
namespace DataListCtrlConstants
{
    // Display modes
    constexpr int DISPLAY_MODE_DATA = 1;
    constexpr int DISPLAY_MODE_SPIKE = 2;
    constexpr int DISPLAY_MODE_EMPTY = 0;
    
    // Image dimensions
    constexpr int DEFAULT_IMAGE_WIDTH = 200;
    constexpr int DEFAULT_IMAGE_HEIGHT = 150;
    
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
}
```

### 4. **Error Handling Strategy**

```cpp
enum class DataListCtrlError
{
    SUCCESS,
    INVALID_INDEX,
    WINDOW_CREATION_FAILED,
    FILE_OPEN_FAILED,
    MEMORY_ALLOCATION_FAILED,
    GDI_RESOURCE_FAILED
};

class DataListCtrlException : public std::exception
{
public:
    explicit DataListCtrlException(DataListCtrlError error, const CString& message = _T(""));
    DataListCtrlError GetError() const { return m_error; }
    CString GetMessage() const { return m_message; }
    
private:
    DataListCtrlError m_error;
    CString m_message;
};
```

### 5. **Refactored Method Structure**

```cpp
class DataListCtrl_Row : public CObject
{
public:
    // Constructor with proper initialization
    DataListCtrl_Row();
    explicit DataListCtrl_Row(int index);
    
    // Destructor (no manual cleanup needed with smart pointers)
    ~DataListCtrl_Row() override = default;
    
    // Copy constructor and assignment operator
    DataListCtrl_Row(const DataListCtrl_Row& other);
    DataListCtrl_Row& operator=(const DataListCtrl_Row& other);
    
    // Move constructor and assignment operator
    DataListCtrl_Row(DataListCtrl_Row&& other) noexcept;
    DataListCtrl_Row& operator=(DataListCtrl_Row&& other) noexcept;
    
private:
    // Core functionality broken into smaller methods
    void InitializeDisplayComponents(DataListCtrlInfos* infos, int imageIndex);
    void ValidateImageIndex(int imageIndex, CdbWaveDoc* pDoc);
    void ProcessDisplayMode(DataListCtrlInfos* infos, int imageIndex);
    
    // Data window management
    void CreateDataWindow(DataListCtrlInfos* infos, int imageIndex);
    void LoadDataDocument();
    void ConfigureDataWindow(DataListCtrlInfos* infos);
    
    // Spike window management
    void CreateSpikeWindow(DataListCtrlInfos* infos, int imageIndex);
    void LoadSpikeDocument();
    void ConfigureSpikeWindow(DataListCtrlInfos* infos);
    
    // Plotting functionality
    void PlotToImageList(DataListCtrlInfos* infos, int imageIndex, 
                        ChartData* pChart, const CString& filename);
    void CreatePlotBitmap(DataListCtrlInfos* infos, int imageIndex,
                         ChartData* pChart, CBitmap& bitmap);
    
    // Utility methods
    CString ExtractFilename(const CString& fullPath) const;
    void TruncateFilename(CString& filename) const;
    bool IsValidIndex(int index, int maxCount) const;
};
```

### 6. **Improved Serialization**

```cpp
void DataListCtrl_Row::Serialize(CArchive& ar)
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
        throw DataListCtrlException(DataListCtrlError::SERIALIZATION_FAILED, 
                                   _T("Serialization failed"));
    }
}

private:
    void SerializeStoring(CArchive& ar);
    void SerializeLoading(CArchive& ar);
    void SerializeStrings(CArchive& ar, bool isStoring);
    void SerializeObjects(CArchive& ar, bool isStoring);
```

### 7. **Thread Safety Considerations**

```cpp
class DataListCtrl_Row : public CObject
{
private:
    mutable std::mutex m_mutex;
    
    // Thread-safe methods
    void SetDisplayParameters(DataListCtrlInfos* infos, int imageIndex)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        // Implementation
    }
};
```

### 8. **Unit Testing Support**

```cpp
class DataListCtrl_Row : public CObject
{
public:
    // Test-friendly methods
    bool IsDisplayProcessed() const { return display_processed; }
    int GetLastDisplayMode() const { return last_display_mode; }
    
    // Dependency injection for testing
    void SetDataDocument(std::unique_ptr<AcqDataDoc> doc) { p_data_doc = std::move(doc); }
    void SetSpikeDocument(std::unique_ptr<CSpikeDoc> doc) { p_spike_doc = std::move(doc); }
};
```

## Implementation Priority

### Phase 1: Critical Safety Improvements
1. Replace raw pointers with smart pointers
2. Implement RAII for GDI resources
3. Add proper error handling

### Phase 2: Code Quality Improvements
1. Extract constants and configuration
2. Break down large methods
3. Eliminate code duplication

### Phase 3: Advanced Features
1. Add thread safety
2. Implement comprehensive error handling
3. Add unit testing support

## Benefits of Clean Implementation

1. **Memory Safety**: No memory leaks or dangling pointers
2. **Exception Safety**: Resources properly cleaned up even during exceptions
3. **Maintainability**: Smaller, focused methods easier to understand and modify
4. **Testability**: Clear separation of concerns enables unit testing
5. **Performance**: RAII eliminates manual resource management overhead
6. **Thread Safety**: Proper synchronization for multi-threaded environments

## Migration Strategy

1. **Gradual Migration**: Implement changes incrementally to minimize risk
2. **Backward Compatibility**: Maintain existing public interface during transition
3. **Testing**: Comprehensive testing at each phase
4. **Documentation**: Update documentation to reflect new patterns and practices
