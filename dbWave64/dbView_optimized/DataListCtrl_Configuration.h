#pragma once

// MFC includes (must come first)
#include <afx.h>
#include <afxwin.h>

// Standard library includes
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <map>

// Forward declarations
class CImageList;
class CBitmap;
class CWnd;

// Constants namespace
namespace DataListCtrlConfigConstants
{
    // Display modes
    constexpr int DISPLAY_MODE_DATA = 1;
    constexpr int DISPLAY_MODE_SPIKE = 2;
    constexpr int DISPLAY_MODE_EMPTY = 0;
    
    // Plot modes
    constexpr int PLOT_BLACK = 0;
    constexpr int PLOT_COLOR = 1;
    constexpr int PLOT_GREY = 2;
    
    // Default values
    constexpr int DEFAULT_IMAGE_WIDTH = 400;
    constexpr int DEFAULT_IMAGE_HEIGHT = 50;
    constexpr int DEFAULT_DATA_TRANSFORM = 0;
    constexpr int DEFAULT_DISPLAY_MODE = 1;
    constexpr int DEFAULT_SPIKE_PLOT_MODE = PLOT_BLACK;
    constexpr int DEFAULT_SELECTED_CLASS = 0;
    constexpr float DEFAULT_TIME_FIRST = 0.0f;
    constexpr float DEFAULT_TIME_LAST = 0.0f;
    constexpr float DEFAULT_MV_SPAN = 0.0f;
    
    // Validation ranges
    constexpr int MIN_IMAGE_WIDTH = 50;
    constexpr int MAX_IMAGE_WIDTH = 2000;
    constexpr int MIN_IMAGE_HEIGHT = 20;
    constexpr int MAX_IMAGE_HEIGHT = 500;
    constexpr float MIN_TIME_VALUE = 0.0f;
    constexpr float MAX_TIME_VALUE = 1000000.0f;
    constexpr float MIN_MV_SPAN = 0.0f;
    constexpr float MAX_MV_SPAN = 1000000.0f;
}

// Exception handling
enum class ConfigurationError
{
    SUCCESS,
    INVALID_PARAMETER,
    OUT_OF_RANGE,
    INVALID_MODE,
    RESOURCE_FAILED
};

class ConfigurationException : public std::exception
{
public:
    explicit ConfigurationException(ConfigurationError error, const CString& message = _T(""));
    ConfigurationError GetError() const { return m_error; }
    CString GetMessage() const { return m_message; }
    const char* what() const noexcept override;
    
private:
    ConfigurationError m_error;
    CString m_message;
};

// Display settings
class DisplaySettings
{
public:
    DisplaySettings();
    
    // Getters
    int GetImageWidth() const { return m_imageWidth; }
    int GetImageHeight() const { return m_imageHeight; }
    int GetDataTransform() const { return m_dataTransform; }
    int GetDisplayMode() const { return m_displayMode; }
    int GetSpikePlotMode() const { return m_spikePlotMode; }
    int GetSelectedClass() const { return m_selectedClass; }
    
    // Setters with validation
    void SetImageWidth(int width);
    void SetImageHeight(int height);
    void SetDataTransform(int transform);
    void SetDisplayMode(int mode);
    void SetSpikePlotMode(int mode);
    void SetSelectedClass(int classIndex);
    
    // Validation
    bool IsValidImageWidth(int width) const;
    bool IsValidImageHeight(int height) const;
    bool IsValidDisplayMode(int mode) const;
    bool IsValidSpikePlotMode(int mode) const;
    bool IsValidSelectedClass(int classIndex) const;
    
    // Reset to defaults
    void ResetToDefaults();
    
private:
    int m_imageWidth;
    int m_imageHeight;
    int m_dataTransform;
    int m_displayMode;
    int m_spikePlotMode;
    int m_selectedClass;
};

// Time settings
class TimeSettings
{
public:
    TimeSettings();
    
    // Getters
    float GetTimeFirst() const { return m_timeFirst; }
    float GetTimeLast() const { return m_timeLast; }
    bool IsTimeSpanSet() const { return m_setTimeSpan; }
    
    // Setters with validation
    void SetTimeFirst(float time);
    void SetTimeLast(float time);
    void SetTimeSpan(float first, float last);
    void SetTimeSpanEnabled(bool enabled) { m_setTimeSpan = enabled; }
    
    // Validation
    bool IsValidTimeValue(float time) const;
    bool IsValidTimeSpan(float first, float last) const;
    
    // Reset to defaults
    void ResetToDefaults();
    
private:
    float m_timeFirst;
    float m_timeLast;
    bool m_setTimeSpan;
};

// Amplitude settings
class AmplitudeSettings
{
public:
    AmplitudeSettings();
    
    // Getters
    float GetMvSpan() const { return m_mvSpan; }
    bool IsMvSpanSet() const { return m_setMvSpan; }
    
    // Setters with validation
    void SetMvSpan(float span);
    void SetMvSpanEnabled(bool enabled) { m_setMvSpan = enabled; }
    
    // Validation
    bool IsValidMvSpan(float span) const;
    
    // Reset to defaults
    void ResetToDefaults();
    
private:
    float m_mvSpan;
    bool m_setMvSpan;
};

// UI settings
class UISettings
{
public:
    UISettings();
    
    // Getters
    bool IsDisplayFileName() const { return m_displayFileName; }
    CWnd* GetParent() const { return m_parent; }
    CImageList* GetImageList() const { return m_imageList; }
    CBitmap* GetEmptyBitmap() const { return m_emptyBitmap; }
    
    // Setters
    void SetDisplayFileName(bool display) { m_displayFileName = display; }
    void SetParent(CWnd* parent) { m_parent = parent; }
    void SetImageList(CImageList* imageList) { m_imageList = imageList; }
    void SetEmptyBitmap(CBitmap* bitmap) { m_emptyBitmap = bitmap; }
    
    // Reset to defaults
    void ResetToDefaults();
    
private:
    bool m_displayFileName;
    CWnd* m_parent;
    CImageList* m_imageList;
    CBitmap* m_emptyBitmap;
};

// Performance settings
class PerformanceSettings
{
public:
    PerformanceSettings();
    
    // Getters
    bool IsCachingEnabled() const { return m_cachingEnabled; }
    bool IsLazyLoadingEnabled() const { return m_lazyLoadingEnabled; }
    bool IsAsyncProcessingEnabled() const { return m_asyncProcessingEnabled; }
    size_t GetMaxCacheSize() const { return m_maxCacheSize; }
    int GetBatchSize() const { return m_batchSize; }
    
    // Setters
    void SetCachingEnabled(bool enabled) { m_cachingEnabled = enabled; }
    void SetLazyLoadingEnabled(bool enabled) { m_lazyLoadingEnabled = enabled; }
    void SetAsyncProcessingEnabled(bool enabled) { m_asyncProcessingEnabled = enabled; }
    void SetMaxCacheSize(size_t size) { m_maxCacheSize = size; }
    void SetBatchSize(int size) { m_batchSize = size; }
    
    // Reset to defaults
    void ResetToDefaults();
    
private:
    bool m_cachingEnabled;
    bool m_lazyLoadingEnabled;
    bool m_asyncProcessingEnabled;
    size_t m_maxCacheSize;
    int m_batchSize;
};

// Main configuration class
class DataListCtrlConfiguration
{
public:
    DataListCtrlConfiguration();
    ~DataListCtrlConfiguration();
    
    // Prevent copying
    DataListCtrlConfiguration(const DataListCtrlConfiguration&) = delete;
    DataListCtrlConfiguration& operator=(const DataListCtrlConfiguration&) = delete;
    
    // Allow moving
    DataListCtrlConfiguration(DataListCtrlConfiguration&& other) noexcept;
    DataListCtrlConfiguration& operator=(DataListCtrlConfiguration&& other) noexcept;
    
    // Access to settings
    DisplaySettings& GetDisplaySettings() { return m_displaySettings; }
    const DisplaySettings& GetDisplaySettings() const { return m_displaySettings; }
    
    TimeSettings& GetTimeSettings() { return m_timeSettings; }
    const TimeSettings& GetTimeSettings() const { return m_timeSettings; }
    
    AmplitudeSettings& GetAmplitudeSettings() { return m_amplitudeSettings; }
    const AmplitudeSettings& GetAmplitudeSettings() const { return m_amplitudeSettings; }
    
    UISettings& GetUISettings() { return m_uiSettings; }
    const UISettings& GetUISettings() const { return m_uiSettings; }
    
    PerformanceSettings& GetPerformanceSettings() { return m_performanceSettings; }
    const PerformanceSettings& GetPerformanceSettings() const { return m_performanceSettings; }
    
    // Configuration management
    void LoadFromRegistry(const CString& section);
    void SaveToRegistry(const CString& section);
    void LoadFromFile(const CString& filename);
    void SaveToFile(const CString& filename);
    
    // Validation
    bool ValidateConfiguration() const;
    CString GetValidationErrors() const;
    
    // Reset to defaults
    void ResetToDefaults();
    
    // Change notification
    using ChangeCallback = std::function<void(const CString& setting, const CString& value)>;
    void RegisterChangeCallback(ChangeCallback callback);
    void UnregisterChangeCallback();
    
    // Column configuration structure
    struct ColumnConfig
    {
        int width;
        CString header;
        int index;
        bool visible;
        int format;
        
        ColumnConfig() : width(100), index(0), visible(true), format(LVCFMT_LEFT) {}
        ColumnConfig(int w, const CString& h, int i) : width(w), header(h), index(i), visible(true), format(LVCFMT_LEFT) {}
        ColumnConfig(int w, const CString& h, int i, bool v, int f) : width(w), header(h), index(i), visible(v), format(f) {}
    };
    
    // Column management
    void SetColumns(const std::vector<ColumnConfig>& columns);
    std::vector<ColumnConfig> GetColumns() const;
    void AddColumn(const ColumnConfig& column);
    void RemoveColumn(int index);
    void ClearColumns();
    
    // Configuration access methods
    const DisplaySettings& GetDisplayConfig() const { return m_displaySettings; }
    const PerformanceSettings& GetPerformanceConfig() const { return m_performanceSettings; }
    DisplaySettings& GetDisplayConfig() { return m_displaySettings; }
    PerformanceSettings& GetPerformanceConfig() { return m_performanceSettings; }
    
    // Compatibility with old DataListCtrlInfos structure
    struct LegacyInfo
    {
        CWnd* parent;
        CImageList* p_image_list;  // Changed to pointer to avoid copying CImageList
        CBitmap* p_empty_bitmap;
        int image_width;
        int image_height;
        int data_transform;
        int display_mode;
        int spike_plot_mode;
        int selected_class;
        float t_first;
        float t_last;
        float mv_span;
        bool b_set_time_span;
        bool b_set_mv_span;
        bool b_display_file_name;
    };
    
    void LoadFromLegacyInfo(const LegacyInfo& info);
    LegacyInfo ToLegacyInfo() const;
    
    // Utility methods
    CString ToString() const;
    void FromString(const CString& configString);
    
private:
    // Settings objects
    DisplaySettings m_displaySettings;
    TimeSettings m_timeSettings;
    AmplitudeSettings m_amplitudeSettings;
    UISettings m_uiSettings;
    PerformanceSettings m_performanceSettings;
    
    // Column configuration
    std::vector<ColumnConfig> m_columns;
    
    // Change notification
    ChangeCallback m_changeCallback;
    
    // Helper methods
    void NotifyChange(const CString& setting, const CString& value);
    void ValidateAllSettings() const;
    
    // Registry helpers
    void WriteRegistryValue(const CString& section, const CString& key, const CString& value);
    CString ReadRegistryValue(const CString& section, const CString& key, const CString& defaultValue);
    
    // File helpers
    void WriteIniValue(const CString& filename, const CString& section, const CString& key, const CString& value);
    CString ReadIniValue(const CString& filename, const CString& section, const CString& key, const CString& defaultValue);
};

// Global configuration manager
class ConfigurationManager
{
public:
    static ConfigurationManager& GetInstance();
    
    // Configuration management
    void RegisterConfiguration(const CString& name, std::shared_ptr<DataListCtrlConfiguration> config);
    std::shared_ptr<DataListCtrlConfiguration> GetConfiguration(const CString& name);
    void UnregisterConfiguration(const CString& name);
    
    // Global settings
    void SetGlobalCachingEnabled(bool enabled);
    bool IsGlobalCachingEnabled() const { return m_globalCachingEnabled; }
    
    void SetGlobalPerformanceMonitoring(bool enabled);
    bool IsGlobalPerformanceMonitoringEnabled() const { return m_globalPerformanceMonitoring; }
    
    // Reset all configurations
    void ResetAllConfigurations();
    
private:
    ConfigurationManager() = default;
    ~ConfigurationManager() = default;
    
    // Prevent copying
    ConfigurationManager(const ConfigurationManager&) = delete;
    ConfigurationManager& operator=(const ConfigurationManager&) = delete;
    
    std::map<CString, std::shared_ptr<DataListCtrlConfiguration>> m_configurations;
    bool m_globalCachingEnabled = true;
    bool m_globalPerformanceMonitoring = true;
};
