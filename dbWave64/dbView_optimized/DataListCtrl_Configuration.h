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
namespace DataListCtrl_ConfigConstants
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
    
    // Registry configuration - Now managed by centralized RegistryManager
// The base path is configured in RegistryManager::GetInstance().SetBasePath()
// Default: "Software\\FMP\\dbWave64\\"
}

// Exception handling
enum class Configuration_Error
{
    SUCCESS,
    INVALID_PARAMETER,
    OUT_OF_RANGE,
    INVALID_MODE,
    RESOURCE_FAILED
};

class Configuration_Exception : public std::exception
{
public:
    explicit Configuration_Exception(Configuration_Error error, const CString& message = _T(""));
    Configuration_Error GetError() const { return m_error; }
    CString GetMessage() const { return m_message; }
    const char* what() const noexcept override;
    
private:
    Configuration_Error m_error;
    CString m_message;
};

// Display settings
class display_settings
{
public:
    display_settings();
    
    // Getters
    int get_image_width() const { return m_image_width_; }
    int get_image_height() const { return m_image_height_; }
    int get_data_transform() const { return m_data_transform_; }
    int get_display_mode() const { return m_display_mode_; }
    int get_spike_plot_mode() const { return m_spike_plot_mode_; }
    int get_selected_class() const { return m_selected_class_; }
    
    // Setters with validation
    void set_image_width(int width);
    void set_image_height(int height);
    void set_data_transform(int transform);
    void set_display_mode(int mode);
    void set_spike_plot_mode(int mode);
    void set_selected_class(int class_index);
    
    // Validation
    static bool is_valid_image_width(int width);
    bool is_valid_image_height(int height) const;
    bool is_valid_display_mode(int mode) const;
    bool is_valid_spike_plot_mode(int mode) const;
    bool is_valid_selected_class(int class_index) const;
    
    // Reset to defaults
    void reset_to_defaults();
    
private:
    int m_image_width_;
    int m_image_height_;
    int m_data_transform_;
    int m_display_mode_;
    int m_spike_plot_mode_;
    int m_selected_class_;
};

// Time settings
class time_settings
{
public:
    time_settings();
    
    // Getters
    float get_time_first() const { return m_time_first_; }
    float get_time_last() const { return m_time_last_; }
    bool is_time_span_set() const { return m_set_time_span_; }
    
    // Setters with validation
    void set_time_first(float time);
    void set_time_last(float time);
    void set_time_span(float first, float last);
    void set_time_span_enabled(const bool enabled) { m_set_time_span_ = enabled; }
    
    // Validation
    bool is_valid_time_value(float time) const;
    bool is_valid_time_span(float first, float last) const;
    
    // Reset to defaults
    void reset_to_defaults();
    
private:
    float m_time_first_;
    float m_time_last_;
    bool m_set_time_span_;
};

// Amplitude settings
class amplitude_settings
{
public:
    amplitude_settings();
    
    // Getters
    float get_mv_span() const { return m_mv_span_; }
    bool is_mv_span_set() const { return m_set_mv_span_; }
    
    // Setters with validation
    void set_mv_span(float span);
    void set_mv_span_enabled(bool enabled) { m_set_mv_span_ = enabled; }
    
    // Validation
    bool is_valid_mv_span(float span) const;
    
    // Reset to defaults
    void reset_to_defaults();
    
private:
    float m_mv_span_;
    bool m_set_mv_span_;
};

// UI settings
class ui_settings
{
public:
    ui_settings();
    
    // Getters
    bool is_display_file_name() const { return m_display_file_name_; }
    CWnd* get_parent() const { return m_parent_; }
    CImageList* get_image_list() const { return m_image_list_; }
    CBitmap* get_empty_bitmap() const { return m_empty_bitmap_; }
    
    // Setters
    void set_display_file_name(bool display) { m_display_file_name_ = display; }
    void set_parent(CWnd* parent) { m_parent_ = parent; }
    void set_image_list(CImageList* imageList) { m_image_list_ = imageList; }
    void set_empty_bitmap(CBitmap* bitmap) { m_empty_bitmap_ = bitmap; }
    
    // Reset to defaults
    void reset_to_defaults();
    
private:
    bool m_display_file_name_;
    CWnd* m_parent_;
    CImageList* m_image_list_;
    CBitmap* m_empty_bitmap_;
};

// Performance settings
class performance_settings
{
public:
    performance_settings();
    
    // Getters
    bool is_caching_enabled() const { return m_caching_enabled_; }
    bool is_lazy_loading_enabled() const { return m_lazy_loading_enabled_; }
    bool is_async_processing_enabled() const { return m_async_processing_enabled_; }
    size_t get_max_cache_size() const { return m_max_cache_size_; }
    int get_batch_size() const { return m_batch_size_; }
    
    // Setters
    void set_caching_enabled(const bool enabled) { m_caching_enabled_ = enabled; }
    void set_lazy_loading_enabled(const bool enabled) { m_lazy_loading_enabled_ = enabled; }
    void set_async_processing_enabled(const bool enabled) { m_async_processing_enabled_ = enabled; }
    void set_max_cache_size(const size_t size) { m_max_cache_size_ = size; }
    void set_batch_size(const int size) { m_batch_size_ = size; }
    
    // Reset to defaults
    void reset_to_defaults();
    
private:
    bool m_caching_enabled_;
    bool m_lazy_loading_enabled_;
    bool m_async_processing_enabled_;
    size_t m_max_cache_size_;
    int m_batch_size_;
};

// Main configuration class
class data_list_ctrl_configuration
{
public:
    data_list_ctrl_configuration();
    ~data_list_ctrl_configuration();
    
    // Prevent copying
    data_list_ctrl_configuration(const data_list_ctrl_configuration&) = delete;
    data_list_ctrl_configuration& operator=(const data_list_ctrl_configuration&) = delete;
    
    // Allow moving
    data_list_ctrl_configuration(data_list_ctrl_configuration&& other) noexcept;
    data_list_ctrl_configuration& operator=(data_list_ctrl_configuration&& other) noexcept;
    
    // Access to settings
    display_settings& get_display_settings() { return m_display_settings_; }
    const display_settings& get_display_settings() const { return m_display_settings_; }
    
    time_settings& get_time_settings() { return m_time_settings_; }
    const time_settings& get_time_settings() const { return m_time_settings_; }
    
    amplitude_settings& get_amplitude_settings() { return m_amplitude_settings_; }
    const amplitude_settings& get_amplitude_settings() const { return m_amplitude_settings_; }
    
    ui_settings& get_ui_settings() { return m_ui_settings_; }
    const ui_settings& get_ui_settings() const { return m_ui_settings_; }
    
    performance_settings& get_performance_settings() { return m_performance_settings_; }
    const performance_settings& get_performance_settings() const { return m_performance_settings_; }
    
    // Configuration management
    void load_from_registry(const CString& section);
    void save_to_registry(const CString& section);
    void load_from_file(const CString& filename);
    void save_to_file(const CString& filename);
    
    // Validation
    bool validate_configuration() const;
    CString get_validation_errors() const;
    
    // Reset to defaults
    void reset_to_defaults();
    
    // Change notification
    using ChangeCallback = std::function<void(const CString& setting, const CString& value)>;
    void register_change_callback(ChangeCallback callback);
    void unregister_change_callback();
    
    // Column configuration structure
    struct column_config
    {
        int width;
        CString header;
        int index;
        bool visible;
        int format;
        
        column_config() : width(100), index(0), visible(true), format(LVCFMT_LEFT) {}
        column_config(int w, const CString& h, const int i) : width(w), header(h), index(i), visible(true), format(LVCFMT_LEFT) {}
        column_config(int w, const CString& h, const int i, const bool v, const int f) : width(w), header(h), index(i), visible(v), format(f) {}
    };
    
    // Column management
    void set_columns(const std::vector<column_config>& columns);
    std::vector<column_config> get_columns() const;
    void add_column(const column_config& column);
    void remove_column(int index);
    void clear_columns();
    
    // Configuration access methods
    const display_settings& get_display_config() const { return m_display_settings_; }
    const performance_settings& get_performance_config() const { return m_performance_settings_; }
    display_settings& get_display_config() { return m_display_settings_; }
    performance_settings& get_performance_config() { return m_performance_settings_; }
    
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
    display_settings m_display_settings_;
    time_settings m_time_settings_;
    amplitude_settings m_amplitude_settings_;
    ui_settings m_ui_settings_;
    performance_settings m_performance_settings_;
    
    // Column configuration
    std::vector<column_config> m_columns_;
    
    // Change notification
    ChangeCallback m_change_callback_;
    
    // Helper methods
    void NotifyChange(const CString& setting, const CString& value);
    void ValidateAllSettings() const;
    
    // Registry helpers - Now using centralized RegistryManager
    static void write_registry_value(const CString& section, const CString& key, const CString& value);
    static CString read_registry_value(const CString& section, const CString& key, const CString& default_value);
    
    // File helpers
    void write_ini_value(const CString& filename, const CString& section, const CString& key, const CString& value);
    CString read_ini_value(const CString& filename, const CString& section, const CString& key, const CString& default_value);
    
    // Column width persistence helpers
    void LoadColumnWidthsFromRegistry(const CString& section);
    void SaveColumnWidthsToRegistry(const CString& section);
    void LoadColumnWidthsFromFile(const CString& filename);
    void SaveColumnWidthsToFile(const CString& filename);
    void LoadDefaultColumnConfiguration();
};

// Global configuration manager
class Configuration_Manager
{
public:
    static Configuration_Manager& GetInstance();
    
    // Configuration management
    void RegisterConfiguration(const CString& name, std::shared_ptr<data_list_ctrl_configuration> config);
    std::shared_ptr<data_list_ctrl_configuration> GetConfiguration(const CString& name);
    void UnregisterConfiguration(const CString& name);
    
    // Global settings
    void SetGlobalCachingEnabled(bool enabled);
    bool IsGlobalCachingEnabled() const { return m_global_caching_enabled_; }
    
    void SetGlobalPerformanceMonitoring(bool enabled);
    bool IsGlobalPerformanceMonitoringEnabled() const { return m_global_performance_monitoring_; }
    
    // Reset all configurations
    void ResetAllConfigurations();
    
private:
    Configuration_Manager() = default;
    ~Configuration_Manager() = default;
    
    // Prevent copying
    Configuration_Manager(const Configuration_Manager&) = delete;
    Configuration_Manager& operator=(const Configuration_Manager&) = delete;
    
    std::map<CString, std::shared_ptr<data_list_ctrl_configuration>> m_configurations_;
    bool m_global_caching_enabled_ = true;
    bool m_global_performance_monitoring_ = true;
};
