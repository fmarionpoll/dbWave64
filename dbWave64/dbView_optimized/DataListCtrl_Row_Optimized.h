#pragma once

// MFC includes (must come first)
#include <afx.h>
#include <afxwin.h>

// Standard library includes
#include <memory>
#include <mutex>
#include <string>

#include <chrono>

#include <functional>

// Supporting classes
#include "DataListCtrl_Configuration.h"

// Forward declarations for supporting classes
class data_list_ctrl_cache;
enum class data_list_ctrl_error;

// Forward declarations
class AcqDataDoc;
class ChartData;
class CSpikeDoc;
class ChartSpikeBar;
class CdbWaveDoc;
struct data_list_ctrl_infos;
class CDC;
class CBitmap;
class CWnd;

// Constants namespace
namespace data_list_ctrl_constants
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
    bool load_database_record(CdbWaveDoc* db_wave_doc, int index);
    void set_display_parameters(data_list_ctrl_infos* infos, int imageIndex);
    void reset_display_processed();
    
    // Serialization
    void Serialize(CArchive& ar) override;
    
    // Test-friendly methods
    bool is_display_processed() const { return m_display_processed_; }
    int get_last_display_mode() const { return m_last_display_mode_; }
    
    // Public getter methods for database fields
    int get_index() const { return m_index_; }
    long get_insect_id() const { return m_insect_id_; }
    int get_record_id() const { return m_record_id_; }
    CString get_comment() const { return m_comment_; }
    CString get_data_file_name() const { return m_data_file_name_; }
    CString get_spike_file_name() const { return m_spike_file_name_; }
    CString get_sensillum_name() const { return m_sensillum_name_; }
    CString get_stimulus1() const { return m_stimulus1_; }
    CString get_concentration1() const { return m_concentration1_; }
    CString get_stimulus2() const { return m_stimulus2_; }
    CString get_concentration2() const { return m_concentration2_; }
    CString get_n_spikes() const { return m_n_spikes_; }
    CString get_flag() const { return m_flag_; }
    CString get_date() const { return m_date_; }
    ChartData* get_data_window() const { return m_p_chart_data_wnd_.get(); }
    ChartSpikeBar* get_spike_window() const { return m_p_chart_spike_wnd_.get(); }
    AcqDataDoc* get_data_document() const { return m_p_data_doc_.get(); }
    CSpikeDoc* get_spike_document() const { return m_p_spike_doc_.get(); }
    
    // Public setter methods for database fields
    void set_index(const int index) { m_index_ = index; }
    void set_insect_id(const long insect_id) { m_insect_id_ = insect_id; }
    void set_record_id(const int record_id) { m_record_id_ = record_id; }
    void set_comment(const CString& comment) { m_comment_ = comment; }
    void set_data_file_name(const CString& file_name) { m_data_file_name_ = file_name; }
    void set_spike_file_name(const CString& file_name) { m_spike_file_name_ = file_name; }
    void set_sensillum_name(const CString& name) { m_sensillum_name_ = name; }
    void set_stimulus1(const CString& stimulus) { m_stimulus1_ = stimulus; }
    void set_concentration1(const CString& concentration) { m_concentration1_ = concentration; }
    void set_stimulus2(const CString& stimulus) { m_stimulus2_ = stimulus; }
    void set_concentration2(const CString& concentration) { m_concentration2_ = concentration; }
    void set_n_spikes(const CString& spikes) { m_n_spikes_ = spikes; }
    void set_flag(const CString& flag) { m_flag_ = flag; }
    void set_date(const CString& date) { m_date_ = date; }
    
    // Dependency injection for testing
    void set_data_document(std::unique_ptr<AcqDataDoc> doc);
    void set_spike_document(std::unique_ptr<CSpikeDoc> doc);
    void set_chart_data_window(std::unique_ptr<ChartData> chart);
    void set_chart_spike_window(std::unique_ptr<ChartSpikeBar> chart);
    
    // Configuration
    void set_cache_enabled(const bool enabled) { m_cache_enabled_ = enabled; }
    bool is_cache_enabled() const { return m_cache_enabled_; }
    
    // Performance monitoring
    struct PerformanceMetrics
    {
        std::chrono::microseconds last_render_time{0};
        size_t cacheHits = 0;
        size_t cacheMisses = 0;
        size_t total_renders = 0;
    };
    
    PerformanceMetrics get_performance_metrics() const { return m_performance_metrics_; }
    void reset_performance_metrics();

private:
    // Core data members
    int m_index_{0};
    long m_insect_id_{0};
    int m_record_id_{0};
    
    // String data
    CString m_comment_;
    CString m_data_file_name_;
    CString m_spike_file_name_;
    CString m_sensillum_name_;
    CString m_stimulus1_;
    CString m_concentration1_;
    CString m_stimulus2_;
    CString m_concentration2_;
    CString m_n_spikes_;
    CString m_flag_;
    CString m_date_;
    
    // Smart pointer managed resources
    std::unique_ptr<AcqDataDoc> m_p_data_doc_;
    std::unique_ptr<ChartData> m_p_chart_data_wnd_;
    std::unique_ptr<CSpikeDoc> m_p_spike_doc_;
    std::unique_ptr<ChartSpikeBar> m_p_chart_spike_wnd_;
    
    // Display state
    bool m_display_processed_{false};
    int m_last_display_mode_{-1};
    bool m_init_{false};
    bool m_changed_{false};
    WORD m_version_{0};
    
    // Thread safety
    mutable std::mutex m_mutex_;
    
    // Caching
    std::unique_ptr<data_list_ctrl_cache> m_cache_;
    bool m_cache_enabled_{true};
    
    // Performance monitoring
    PerformanceMetrics m_performance_metrics_;
    
    // Core functionality methods
    void process_display_mode(data_list_ctrl_infos* infos, int image_index);
    
    // Data window management
    void create_data_window(data_list_ctrl_infos* infos, int image_index);
    void load_data_document();
    void configure_data_window(data_list_ctrl_infos* infos);
    void display_data_window(data_list_ctrl_infos* infos, int image_index);
    void plot_to_image_list(data_list_ctrl_infos* infos, int image_index, ChartData* pChart, const CString& filename);
    void create_plot_bitmap(data_list_ctrl_infos* infos, int image_index, ChartData* pChart, CBitmap& bitmap);

    
    // Spike window management
    void create_spike_window(data_list_ctrl_infos* infos, int image_index);
    void load_spike_document();
    void configure_spike_window(data_list_ctrl_infos* infos);
    void display_spike_window(data_list_ctrl_infos* infos, int image_index);
    void plot_to_image_list(data_list_ctrl_infos* infos, int image_index, ChartSpikeBar* pChart, const CString& filename);
    void create_plot_bitmap(data_list_ctrl_infos* infos, int image_index, ChartSpikeBar* pChart, CBitmap& bitmap);

    
    // Empty window management
    void display_empty_window(data_list_ctrl_infos* infos, int image_index);

    // Utility methods
    CString extract_filename(const CString& full_path) const;
    void truncate_filename(CString& filename) const;
    bool is_valid_index(int index, int max_count) const;
    bool is_valid_display_mode(int mode) const;
    
    // Error handling
    void handle_error(data_list_ctrl_error error, const CString& message = _T(""));
    void log_error(const CString& message) const;
    
    // Serialization helpers
    void serialize_storing(CArchive& ar);
    void serialize_loading(CArchive& ar);
    void serialize_strings(CArchive& ar, bool isStoring);
    void serialize_objects(CArchive& ar, bool isStoring);
    
    // Performance measurement - simplified for void functions only
    template<typename Func>
    void MeasurePerformance(Func&& func)
    {
        auto start = std::chrono::high_resolution_clock::now();
        std::forward<Func>(func)();
        auto end = std::chrono::high_resolution_clock::now();
        
        m_performance_metrics_.last_render_time = 
            std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        m_performance_metrics_.total_renders++;
    }
};
