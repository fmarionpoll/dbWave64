#include "StdAfx.h"
#include "DataListCtrl_Configuration.h"
#include "RegistryManager.h"

// Helper function to convert float to CString
static CString float_to_string(const float value)
{
    CString result;
    result.Format(_T("%.6f"), value);
    return result;
}

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// Exception implementation
Configuration_Exception::Configuration_Exception(Configuration_Error error, const CString& message)
    : m_error(error), m_message(message)
{
}

const char* Configuration_Exception::what() const noexcept
{
    static CStringA errorMessage;
    errorMessage = m_message;
    return errorMessage.GetString();
}

// DisplaySettings implementation
display_settings::display_settings()
    : m_image_width_(DataListCtrl_ConfigConstants::DEFAULT_IMAGE_WIDTH)
    , m_image_height_(DataListCtrl_ConfigConstants::DEFAULT_IMAGE_HEIGHT)
    , m_data_transform_(DataListCtrl_ConfigConstants::DEFAULT_DATA_TRANSFORM)
    , m_display_mode_(DataListCtrl_ConfigConstants::DEFAULT_DISPLAY_MODE)
    , m_spike_plot_mode_(DataListCtrl_ConfigConstants::DEFAULT_SPIKE_PLOT_MODE)
    , m_selected_class_(DataListCtrl_ConfigConstants::DEFAULT_SELECTED_CLASS)
{
}

void display_settings::set_image_width(int width)
{
    if (!is_valid_image_width(width))
    {
        throw Configuration_Exception(Configuration_Error::OUT_OF_RANGE, 
            _T("Invalid image width: ") + CString(std::to_string(width).c_str()));
    }
    m_image_width_ = width;
}

void display_settings::set_image_height(int height)
{
    if (!is_valid_image_height(height))
    {
        throw Configuration_Exception(Configuration_Error::OUT_OF_RANGE, 
            _T("Invalid image height: ") + CString(std::to_string(height).c_str()));
    }
    m_image_height_ = height;
}

void display_settings::set_data_transform(int transform)
{
    m_data_transform_ = transform;
}

void display_settings::set_display_mode(int mode)
{
    if (!is_valid_display_mode(mode))
    {
        throw Configuration_Exception(Configuration_Error::INVALID_MODE, 
            _T("Invalid display mode: ") + CString(std::to_string(mode).c_str()));
    }
    m_display_mode_ = mode;
}

void display_settings::set_spike_plot_mode(int mode)
{
    if (!is_valid_spike_plot_mode(mode))
    {
        throw Configuration_Exception(Configuration_Error::INVALID_MODE, 
            _T("Invalid spike plot mode: ") + CString(std::to_string(mode).c_str()));
    }
    m_spike_plot_mode_ = mode;
}

void display_settings::set_selected_class(const int class_index)
{
    if (!is_valid_selected_class(class_index))
    {
        throw Configuration_Exception(Configuration_Error::OUT_OF_RANGE, 
            _T("Invalid selected class: ") + CString(std::to_string(class_index).c_str()));
    }
    m_selected_class_ = class_index;
}

bool display_settings::is_valid_image_width(const int width)
{
    return width >= DataListCtrl_ConfigConstants::MIN_IMAGE_WIDTH && 
           width <= DataListCtrl_ConfigConstants::MAX_IMAGE_WIDTH;
}

bool display_settings::is_valid_image_height(int height) const
{
    return height >= DataListCtrl_ConfigConstants::MIN_IMAGE_HEIGHT && 
           height <= DataListCtrl_ConfigConstants::MAX_IMAGE_HEIGHT;
}

bool display_settings::is_valid_display_mode(int mode) const
{
    return mode >= DataListCtrl_ConfigConstants::DISPLAY_MODE_EMPTY && 
           mode <= DataListCtrl_ConfigConstants::DISPLAY_MODE_SPIKE;
}

bool display_settings::is_valid_spike_plot_mode(int mode) const
{
    return mode >= DataListCtrl_ConfigConstants::PLOT_BLACK && 
           mode <= DataListCtrl_ConfigConstants::PLOT_GREY;
}

bool display_settings::is_valid_selected_class(int class_index) const
{
    return class_index >= 0;
}

void display_settings::reset_to_defaults()
{
    m_image_width_ = DataListCtrl_ConfigConstants::DEFAULT_IMAGE_WIDTH;
    m_image_height_ = DataListCtrl_ConfigConstants::DEFAULT_IMAGE_HEIGHT;
    m_data_transform_ = DataListCtrl_ConfigConstants::DEFAULT_DATA_TRANSFORM;
    m_display_mode_ = DataListCtrl_ConfigConstants::DEFAULT_DISPLAY_MODE;
    m_spike_plot_mode_ = DataListCtrl_ConfigConstants::DEFAULT_SPIKE_PLOT_MODE;
    m_selected_class_ = DataListCtrl_ConfigConstants::DEFAULT_SELECTED_CLASS;
}

// TimeSettings implementation
time_settings::time_settings()
    : m_time_first_(DataListCtrl_ConfigConstants::DEFAULT_TIME_FIRST)
    , m_time_last_(DataListCtrl_ConfigConstants::DEFAULT_TIME_LAST)
    , m_set_time_span_(false)
{
}

void time_settings::set_time_first(float time)
{
    if (!is_valid_time_value(time))
    {
        throw Configuration_Exception(Configuration_Error::OUT_OF_RANGE, 
            _T("Invalid time first value: ") + CString(std::to_string(time).c_str()));
    }
    m_time_first_ = time;
}

void time_settings::set_time_last(float time)
{
    if (!is_valid_time_value(time))
    {
        throw Configuration_Exception(Configuration_Error::OUT_OF_RANGE, 
            _T("Invalid time last value: ") + CString(std::to_string(time).c_str()));
    }
    m_time_last_ = time;
}

void time_settings::set_time_span(float first, float last)
{
    if (!is_valid_time_span(first, last))
    {
        throw Configuration_Exception(Configuration_Error::OUT_OF_RANGE, 
            _T("Invalid time span: ") + CString(std::to_string(first).c_str()) + 
            _T(" to ") + CString(std::to_string(last).c_str()));
    }
    m_time_first_ = first;
    m_time_last_ = last;
    m_set_time_span_ = true;
}

bool time_settings::is_valid_time_value(float time) const
{
    return time >= DataListCtrl_ConfigConstants::MIN_TIME_VALUE && 
           time <= DataListCtrl_ConfigConstants::MAX_TIME_VALUE;
}

bool time_settings::is_valid_time_span(float first, float last) const
{
    return is_valid_time_value(first) && is_valid_time_value(last) && first <= last;
}

void time_settings::reset_to_defaults()
{
    m_time_first_ = DataListCtrl_ConfigConstants::DEFAULT_TIME_FIRST;
    m_time_last_ = DataListCtrl_ConfigConstants::DEFAULT_TIME_LAST;
    m_set_time_span_ = false;
}

// AmplitudeSettings implementation
amplitude_settings::amplitude_settings()
    : m_mv_span_(DataListCtrl_ConfigConstants::DEFAULT_MV_SPAN)
    , m_set_mv_span_(false)
{
}

void amplitude_settings::set_mv_span(float span)
{
    if (!is_valid_mv_span(span))
    {
        throw Configuration_Exception(Configuration_Error::OUT_OF_RANGE, 
            _T("Invalid MV span: ") + CString(std::to_string(span).c_str()));
    }
    m_mv_span_ = span;
}

bool amplitude_settings::is_valid_mv_span(float span) const
{
    return span >= DataListCtrl_ConfigConstants::MIN_MV_SPAN && 
           span <= DataListCtrl_ConfigConstants::MAX_MV_SPAN;
}

void amplitude_settings::reset_to_defaults()
{
    m_mv_span_ = DataListCtrl_ConfigConstants::DEFAULT_MV_SPAN;
    m_set_mv_span_ = false;
}

// UISettings implementation
ui_settings::ui_settings()
    : m_display_file_name_(false)
    , m_parent_(nullptr)
    , m_image_list_(nullptr)
    , m_empty_bitmap_(nullptr)
{
}

void ui_settings::reset_to_defaults()
{
    m_display_file_name_ = false;
    m_parent_ = nullptr;
    m_image_list_ = nullptr;
    m_empty_bitmap_ = nullptr;
}

// PerformanceSettings implementation
performance_settings::performance_settings()
    : m_caching_enabled_(true)
    , m_lazy_loading_enabled_(true)
    , m_async_processing_enabled_(true)
    , m_max_cache_size_(100)
    , m_batch_size_(10)
{
}

void performance_settings::reset_to_defaults()
{
    m_caching_enabled_ = true;
    m_lazy_loading_enabled_ = true;
    m_async_processing_enabled_ = true;
    m_max_cache_size_ = 100;
    m_batch_size_ = 10;
}

// DataListCtrlConfiguration implementation
data_list_ctrl_configuration::data_list_ctrl_configuration()
{
}

data_list_ctrl_configuration::~data_list_ctrl_configuration()
{
}

data_list_ctrl_configuration::data_list_ctrl_configuration(data_list_ctrl_configuration&& other) noexcept
    : m_display_settings_(std::move(other.m_display_settings_))
    , m_time_settings_(std::move(other.m_time_settings_))
    , m_amplitude_settings_(std::move(other.m_amplitude_settings_))
    , m_ui_settings_(std::move(other.m_ui_settings_))
    , m_performance_settings_(std::move(other.m_performance_settings_))
    , m_change_callback_(std::move(other.m_change_callback_))
{
}

data_list_ctrl_configuration& data_list_ctrl_configuration::operator=(data_list_ctrl_configuration&& other) noexcept
{
    if (this != &other)
    {
        m_display_settings_ = std::move(other.m_display_settings_);
        m_time_settings_ = std::move(other.m_time_settings_);
        m_amplitude_settings_ = std::move(other.m_amplitude_settings_);
        m_ui_settings_ = std::move(other.m_ui_settings_);
        m_performance_settings_ = std::move(other.m_performance_settings_);
        m_change_callback_ = std::move(other.m_change_callback_);
    }
    return *this;
}

void data_list_ctrl_configuration::load_from_registry(const CString& section)
{
    try
    {
        // Load display settings
        m_display_settings_.set_image_width(_ttoi(read_registry_value(section, _T("ImageWidth"),
            CString(std::to_string(DataListCtrl_ConfigConstants::DEFAULT_IMAGE_WIDTH).c_str()))));
        m_display_settings_.set_image_height(_ttoi(read_registry_value(section, _T("ImageHeight"),
            CString(std::to_string(DataListCtrl_ConfigConstants::DEFAULT_IMAGE_HEIGHT).c_str()))));
        m_display_settings_.set_display_mode(_ttoi(read_registry_value(section, _T("DisplayMode"),
            CString(std::to_string(DataListCtrl_ConfigConstants::DEFAULT_DISPLAY_MODE).c_str()))));

        // Load time settings
        CString default_value = float_to_string(DataListCtrl_ConfigConstants::DEFAULT_TIME_FIRST);
		CString strValue = read_registry_value(section, _T("TimeFirst"), default_value);
        float value = static_cast<float>(_ttof(strValue));
        m_time_settings_.set_time_first(value);
        default_value = float_to_string(DataListCtrl_ConfigConstants::DEFAULT_TIME_LAST);
        value = static_cast<float>(_ttof(read_registry_value(section, _T("TimeLast"), default_value)));
        m_time_settings_.set_time_last(value);
        m_time_settings_.set_time_span_enabled(_ttoi(read_registry_value(section, _T("SetTimeSpan"), _T("0"))) != 0);

        // Load amplitude settings
        default_value = float_to_string(DataListCtrl_ConfigConstants::DEFAULT_MV_SPAN);
        value = static_cast<float>(_ttof(read_registry_value(section, _T("MvSpan"), default_value)));
        m_amplitude_settings_.set_mv_span(value);

        m_amplitude_settings_.set_mv_span_enabled(_ttoi(read_registry_value(section, _T("SetMvSpan"), _T("0"))) != 0);
        
        // Load UI settings
        m_ui_settings_.set_display_file_name(_ttoi(read_registry_value(section, _T("DisplayFileName"), _T("0"))) != 0);
        
        // Load performance settings
        m_performance_settings_.set_caching_enabled(_ttoi(read_registry_value(section, _T("CachingEnabled"), _T("1"))) != 0);
        m_performance_settings_.set_lazy_loading_enabled(_ttoi(read_registry_value(section, _T("LazyLoadingEnabled"), _T("1"))) != 0);
        m_performance_settings_.set_async_processing_enabled(_ttoi(read_registry_value(section, _T("AsyncProcessingEnabled"), _T("1"))) != 0);
        m_performance_settings_.set_max_cache_size(_ttoi(read_registry_value(section, _T("MaxCacheSize"), _T("100"))));
        m_performance_settings_.set_batch_size(_ttoi(read_registry_value(section, _T("BatchSize"), _T("10"))));
        
        // Load column widths
        LoadColumnWidthsFromRegistry(section);
    }
    catch (const std::exception& e)
    {
        throw Configuration_Exception(Configuration_Error::RESOURCE_FAILED, 
            _T("Failed to load configuration from registry: ") + CString(e.what()));
    }
}

void data_list_ctrl_configuration::save_to_registry(const CString& section)
{
    try
    {
        // Save display settings
        write_registry_value(section, _T("ImageWidth"), CString(std::to_string(m_display_settings_.get_image_width()).c_str()));
        write_registry_value(section, _T("ImageHeight"), CString(std::to_string(m_display_settings_.get_image_height()).c_str()));
        write_registry_value(section, _T("DisplayMode"), CString(std::to_string(m_display_settings_.get_display_mode()).c_str()));
        
        // Save time settings
        write_registry_value(section, _T("TimeFirst"), float_to_string(m_time_settings_.get_time_first()));
        write_registry_value(section, _T("TimeLast"), float_to_string(m_time_settings_.get_time_last()));
        write_registry_value(section, _T("SetTimeSpan"), m_time_settings_.is_time_span_set() ? _T("1") : _T("0"));
        
        // Save amplitude settings
        write_registry_value(section, _T("MvSpan"), float_to_string(m_amplitude_settings_.get_mv_span()));
        write_registry_value(section, _T("SetMvSpan"), m_amplitude_settings_.is_mv_span_set() ? _T("1") : _T("0"));
        
        // Save UI settings
        write_registry_value(section, _T("DisplayFileName"), m_ui_settings_.is_display_file_name() ? _T("1") : _T("0"));
        
        // Save performance settings
        write_registry_value(section, _T("CachingEnabled"), m_performance_settings_.is_caching_enabled() ? _T("1") : _T("0"));
        write_registry_value(section, _T("LazyLoadingEnabled"), m_performance_settings_.is_lazy_loading_enabled() ? _T("1") : _T("0"));
        write_registry_value(section, _T("AsyncProcessingEnabled"), m_performance_settings_.is_async_processing_enabled() ? _T("1") : _T("0"));
        write_registry_value(section, _T("MaxCacheSize"), CString(std::to_string(m_performance_settings_.get_max_cache_size()).c_str()));
        write_registry_value(section, _T("BatchSize"), CString(std::to_string(m_performance_settings_.get_batch_size()).c_str()));
        
        // Save column widths
        SaveColumnWidthsToRegistry(section);
    }
    catch (const std::exception& e)
    {
        throw Configuration_Exception(Configuration_Error::RESOURCE_FAILED, 
            _T("Failed to save configuration to registry: ") + CString(e.what()));
    }
}

void data_list_ctrl_configuration::load_from_file(const CString& filename)
{
    try
    {
        // Load display settings
        m_display_settings_.set_image_width(_ttoi(read_ini_value(filename, _T("Display"), _T("ImageWidth"), 
            CString(std::to_string(DataListCtrl_ConfigConstants::DEFAULT_IMAGE_WIDTH).c_str()))));
        m_display_settings_.set_image_height(_ttoi(read_ini_value(filename, _T("Display"), _T("ImageHeight"), 
            CString(std::to_string(DataListCtrl_ConfigConstants::DEFAULT_IMAGE_HEIGHT).c_str()))));
        m_display_settings_.set_display_mode(_ttoi(read_ini_value(filename, _T("Display"), _T("DisplayMode"), 
            CString(std::to_string(DataListCtrl_ConfigConstants::DEFAULT_DISPLAY_MODE).c_str()))));
        
        // Load time settings
        CString default_value = float_to_string(DataListCtrl_ConfigConstants::DEFAULT_TIME_FIRST);
        float value = static_cast<float>(_ttof(read_ini_value(filename, _T("Time"), _T("TimeFirst"), default_value)));
        m_time_settings_.set_time_first(value);
		default_value = float_to_string(DataListCtrl_ConfigConstants::DEFAULT_TIME_LAST);
		value = static_cast<float>(_ttof(read_ini_value(filename, _T("Time"), _T("TimeLast"), default_value)));
        m_time_settings_.set_time_last(value);
        m_time_settings_.set_time_span_enabled(_ttoi(read_ini_value(filename, _T("Time"), _T("SetTimeSpan"), _T("0"))) != 0);
        
        // Load amplitude settings
		default_value = float_to_string(DataListCtrl_ConfigConstants::DEFAULT_MV_SPAN);
		value = static_cast<float>(_ttof(read_ini_value(filename, _T("Amplitude"), _T("MvSpan"), default_value)));
        m_amplitude_settings_.set_mv_span(value);
        m_amplitude_settings_.set_mv_span_enabled(_ttoi(read_ini_value(filename, _T("Amplitude"), _T("SetMvSpan"), _T("0"))) != 0);
        
        // Load UI settings
        m_ui_settings_.set_display_file_name(_ttoi(read_ini_value(filename, _T("UI"), _T("DisplayFileName"), _T("0"))) != 0);
        
        // Load performance settings
        m_performance_settings_.set_caching_enabled(_ttoi(read_ini_value(filename, _T("Performance"), _T("CachingEnabled"), _T("1"))) != 0);
        m_performance_settings_.set_lazy_loading_enabled(_ttoi(read_ini_value(filename, _T("Performance"), _T("LazyLoadingEnabled"), _T("1"))) != 0);
        m_performance_settings_.set_async_processing_enabled(_ttoi(read_ini_value(filename, _T("Performance"), _T("AsyncProcessingEnabled"), _T("1"))) != 0);
        m_performance_settings_.set_max_cache_size(_ttoi(read_ini_value(filename, _T("Performance"), _T("MaxCacheSize"), _T("100"))));
        m_performance_settings_.set_batch_size(_ttoi(read_ini_value(filename, _T("Performance"), _T("BatchSize"), _T("10"))));
        
        // Load column widths
        LoadColumnWidthsFromFile(filename);
    }
    catch (const std::exception& e)
    {
        throw Configuration_Exception(Configuration_Error::RESOURCE_FAILED, 
            _T("Failed to load configuration from file: ") + CString(e.what()));
    }
}

void data_list_ctrl_configuration::save_to_file(const CString& filename)
{
    try
    {
        // Save display settings
        write_ini_value(filename, _T("Display"), _T("ImageWidth"), CString(std::to_string(m_display_settings_.get_image_width()).c_str()));
        write_ini_value(filename, _T("Display"), _T("ImageHeight"), CString(std::to_string(m_display_settings_.get_image_height()).c_str()));
        write_ini_value(filename, _T("Display"), _T("DisplayMode"), CString(std::to_string(m_display_settings_.get_display_mode()).c_str()));
        
        // Save time settings
        write_ini_value(filename, _T("Time"), _T("TimeFirst"), float_to_string(m_time_settings_.get_time_first()));
        write_ini_value(filename, _T("Time"), _T("TimeLast"), float_to_string(m_time_settings_.get_time_last()));
        write_ini_value(filename, _T("Time"), _T("SetTimeSpan"), m_time_settings_.is_time_span_set() ? _T("1") : _T("0"));
        
        // Save amplitude settings
        write_ini_value(filename, _T("Amplitude"), _T("MvSpan"), float_to_string(m_amplitude_settings_.get_mv_span()));
        write_ini_value(filename, _T("Amplitude"), _T("SetMvSpan"), m_amplitude_settings_.is_mv_span_set() ? _T("1") : _T("0"));
        
        // Save UI settings
        write_ini_value(filename, _T("UI"), _T("DisplayFileName"), m_ui_settings_.is_display_file_name() ? _T("1") : _T("0"));
        
        // Save performance settings
        write_ini_value(filename, _T("Performance"), _T("CachingEnabled"), m_performance_settings_.is_caching_enabled() ? _T("1") : _T("0"));
        write_ini_value(filename, _T("Performance"), _T("LazyLoadingEnabled"), m_performance_settings_.is_lazy_loading_enabled() ? _T("1") : _T("0"));
        write_ini_value(filename, _T("Performance"), _T("AsyncProcessingEnabled"), m_performance_settings_.is_async_processing_enabled() ? _T("1") : _T("0"));
        write_ini_value(filename, _T("Performance"), _T("MaxCacheSize"), CString(std::to_string(m_performance_settings_.get_max_cache_size()).c_str()));
        write_ini_value(filename, _T("Performance"), _T("BatchSize"), CString(std::to_string(m_performance_settings_.get_batch_size()).c_str()));
        
        // Save column widths
        SaveColumnWidthsToFile(filename);
    }
    catch (const std::exception& e)
    {
        throw Configuration_Exception(Configuration_Error::RESOURCE_FAILED, 
            _T("Failed to save configuration to file: ") + CString(e.what()));
    }
}

bool data_list_ctrl_configuration::validate_configuration() const
{
    try
    {
        ValidateAllSettings();
        return true;
    }
    catch (...)
    {
        return false;
    }
}

CString data_list_ctrl_configuration::get_validation_errors() const
{
    CString errors;
    
    try
    {
        ValidateAllSettings();
    }
    catch (const Configuration_Exception& e)
    {
        errors = e.GetMessage();
    }
    catch (const std::exception& e)
    {
        errors = CString(e.what());
    }
    
    return errors;
}

void data_list_ctrl_configuration::reset_to_defaults()
{
    m_display_settings_.reset_to_defaults();
    m_time_settings_.reset_to_defaults();
    m_amplitude_settings_.reset_to_defaults();
    m_ui_settings_.reset_to_defaults();
    m_performance_settings_.reset_to_defaults();
}

void data_list_ctrl_configuration::register_change_callback(ChangeCallback callback)
{
    m_change_callback_ = callback;
}

void data_list_ctrl_configuration::unregister_change_callback()
{
    m_change_callback_ = nullptr;
}

void data_list_ctrl_configuration::LoadFromLegacyInfo(const LegacyInfo& info)
{
    m_ui_settings_.set_parent(info.parent);
    m_ui_settings_.set_image_list(info.p_image_list);  // Pass pointer directly
    m_ui_settings_.set_empty_bitmap(info.p_empty_bitmap);
    m_display_settings_.set_image_width(info.image_width);
    m_display_settings_.set_image_height(info.image_height);
    m_display_settings_.set_data_transform(info.data_transform);
    m_display_settings_.set_display_mode(info.display_mode);
    m_display_settings_.set_spike_plot_mode(info.spike_plot_mode);
    m_display_settings_.set_selected_class(info.selected_class);
    m_time_settings_.set_time_first(info.t_first);
    m_time_settings_.set_time_last(info.t_last);
    m_amplitude_settings_.set_mv_span(info.mv_span);
    m_time_settings_.set_time_span_enabled(info.b_set_time_span);
    m_amplitude_settings_.set_mv_span_enabled(info.b_set_mv_span);
    m_ui_settings_.set_display_file_name(info.b_display_file_name);
}

data_list_ctrl_configuration::LegacyInfo data_list_ctrl_configuration::ToLegacyInfo() const
{
    LegacyInfo info;
    info.parent = m_ui_settings_.get_parent();
    info.p_image_list = m_ui_settings_.get_image_list();  // Assign pointer directly
    info.p_empty_bitmap = m_ui_settings_.get_empty_bitmap();
    info.image_width = m_display_settings_.get_image_width();
    info.image_height = m_display_settings_.get_image_height();
    info.data_transform = m_display_settings_.get_data_transform();
    info.display_mode = m_display_settings_.get_display_mode();
    info.spike_plot_mode = m_display_settings_.get_spike_plot_mode();
    info.selected_class = m_display_settings_.get_selected_class();
    info.t_first = m_time_settings_.get_time_first();
    info.t_last = m_time_settings_.get_time_last();
    info.mv_span = m_amplitude_settings_.get_mv_span();
    info.b_set_time_span = m_time_settings_.is_time_span_set();
    info.b_set_mv_span = m_amplitude_settings_.is_mv_span_set();
    info.b_display_file_name = m_ui_settings_.is_display_file_name();
    return info;
}

CString data_list_ctrl_configuration::ToString() const
{
    CString result;
    result.Format(_T("Display: %dx%d, Mode: %d, Transform: %d\n")
                  _T("Time: %.2f-%.2f, Set: %s\n")
                  _T("Amplitude: %.2f, Set: %s\n")
                  _T("UI: DisplayFileName: %s\n")
                  _T("Performance: Cache: %s, Lazy: %s, Async: %s"),
                  m_display_settings_.get_image_width(), m_display_settings_.get_image_height(),
                  m_display_settings_.get_display_mode(), m_display_settings_.get_data_transform(),
                  m_time_settings_.get_time_first(), m_time_settings_.get_time_last(),
                  m_time_settings_.is_time_span_set() ? _T("Yes") : _T("No"),
                  m_amplitude_settings_.get_mv_span(),
                  m_amplitude_settings_.is_mv_span_set() ? _T("Yes") : _T("No"),
                  m_ui_settings_.is_display_file_name() ? _T("Yes") : _T("No"),
                  m_performance_settings_.is_caching_enabled() ? _T("Yes") : _T("No"),
                  m_performance_settings_.is_lazy_loading_enabled() ? _T("Yes") : _T("No"),
                  m_performance_settings_.is_async_processing_enabled() ? _T("Yes") : _T("No"));
    return result;
}

void data_list_ctrl_configuration::FromString(const CString& configString)
{
    // Parse configuration string and set values
    // This is a simplified implementation - in practice, you'd want more robust parsing
    reset_to_defaults();
    
    // Example parsing (simplified)
    if (configString.Find(_T("Display:")) >= 0)
    {
        // Parse display settings
        // Implementation would parse the string and set values
    }
}

void data_list_ctrl_configuration::NotifyChange(const CString& setting, const CString& value)
{
    if (m_change_callback_)
    {
        m_change_callback_(setting, value);
    }
}

void data_list_ctrl_configuration::ValidateAllSettings() const
{
    // Validate display settings
    if (!m_display_settings_.is_valid_image_width(m_display_settings_.get_image_width()))
    {
        throw Configuration_Exception(Configuration_Error::OUT_OF_RANGE, _T("Invalid image width"));
    }
    if (!m_display_settings_.is_valid_image_height(m_display_settings_.get_image_height()))
    {
        throw Configuration_Exception(Configuration_Error::OUT_OF_RANGE, _T("Invalid image height"));
    }
    if (!m_display_settings_.is_valid_display_mode(m_display_settings_.get_display_mode()))
    {
        throw Configuration_Exception(Configuration_Error::INVALID_MODE, _T("Invalid display mode"));
    }
    
    // Validate time settings
    if (!m_time_settings_.is_valid_time_value(m_time_settings_.get_time_first()))
    {
        throw Configuration_Exception(Configuration_Error::OUT_OF_RANGE, _T("Invalid time first value"));
    }
    if (!m_time_settings_.is_valid_time_value(m_time_settings_.get_time_last()))
    {
        throw Configuration_Exception(Configuration_Error::OUT_OF_RANGE, _T("Invalid time last value"));
    }
    if (m_time_settings_.is_time_span_set() && 
        !m_time_settings_.is_valid_time_span(m_time_settings_.get_time_first(), m_time_settings_.get_time_last()))
    {
        throw Configuration_Exception(Configuration_Error::OUT_OF_RANGE, _T("Invalid time span"));
    }
    
    // Validate amplitude settings
    if (!m_amplitude_settings_.is_valid_mv_span(m_amplitude_settings_.get_mv_span()))
    {
        throw Configuration_Exception(Configuration_Error::OUT_OF_RANGE, _T("Invalid MV span"));
    }
}

void data_list_ctrl_configuration::write_registry_value(const CString& section, const CString& key, const CString& value)
{
    // Use centralized registry manager
    RegistryManager::GetInstance().WriteStringValue(section, key, value);
}

CString data_list_ctrl_configuration::read_registry_value(const CString& section, const CString& key, const CString& default_value)
{
    // Use centralized registry manager
    return RegistryManager::GetInstance().ReadStringValue(section, key, default_value);
}

void data_list_ctrl_configuration::write_ini_value(const CString& filename, const CString& section, const CString& key, const CString& value)
{
    WritePrivateProfileString(section, key, value, filename);
}

CString data_list_ctrl_configuration::read_ini_value(const CString& filename, const CString& section, const CString& key, const CString& default_value)
{
    TCHAR buffer[256];
    GetPrivateProfileString(section, key, default_value, buffer, sizeof(buffer), filename);
    return CString(buffer);
}

// Column management implementation
void data_list_ctrl_configuration::set_columns(const std::vector<column_config>& columns)
{
    m_columns_ = columns;
    NotifyChange(_T("Columns"), _T("Updated"));
}

std::vector<data_list_ctrl_configuration::column_config> data_list_ctrl_configuration::get_columns() const
{
    return m_columns_;
}

void data_list_ctrl_configuration::add_column(const column_config& column)
{
    m_columns_.push_back(column);
    NotifyChange(_T("Columns"), _T("Added"));
}

void data_list_ctrl_configuration::remove_column(int index)
{
    if (index >= 0 && index < static_cast<int>(m_columns_.size()))
    {
        m_columns_.erase(m_columns_.begin() + index);
        NotifyChange(_T("Columns"), _T("Removed"));
    }
}

void data_list_ctrl_configuration::clear_columns()
{
    m_columns_.clear();
    NotifyChange(_T("Columns"), _T("Cleared"));
}

// Column width persistence helper methods
void data_list_ctrl_configuration::LoadColumnWidthsFromRegistry(const CString& section)
{
    try
    {
        // Load column count first
        int columnCount = _ttoi(read_registry_value(section, _T("ColumnCount"), _T("0")));
        
        if (columnCount > 0)
        {
            m_columns_.clear();
            m_columns_.reserve(columnCount);
            
            for (int i = 0; i < columnCount; ++i)
            {
                CString columnKey;
                columnKey.Format(_T("Column%d"), i);
                
                column_config column;
                column.width = _ttoi(read_registry_value(section, columnKey + _T("_Width"), _T("100")));
                column.header = read_registry_value(section, columnKey + _T("_Header"), _T(""));
                column.index = _ttoi(read_registry_value(section, columnKey + _T("_Index"), _T("0")));
                column.visible = _ttoi(read_registry_value(section, columnKey + _T("_Visible"), _T("1"))) != 0;
                column.format = _ttoi(read_registry_value(section, columnKey + _T("_Format"), _T("0")));
                
                m_columns_.push_back(column);
            }
        }
        else
        {
            // Load default column configuration if no saved columns
            LoadDefaultColumnConfiguration();
        }
    }
    catch (const std::exception&)
    {
        // If loading fails, use default configuration
        LoadDefaultColumnConfiguration();
    }
}

void data_list_ctrl_configuration::SaveColumnWidthsToRegistry(const CString& section)
{
    try
    {
        // Save column count
        write_registry_value(section, _T("ColumnCount"), CString(std::to_string(m_columns_.size()).c_str()));
        
        // Save each column configuration
        for (size_t i = 0; i < m_columns_.size(); ++i)
        {
            CString columnKey;
            columnKey.Format(_T("Column%d"), static_cast<int>(i));
            
            const auto& column = m_columns_[i];
            write_registry_value(section, columnKey + _T("_Width"), CString(std::to_string(column.width).c_str()));
            write_registry_value(section, columnKey + _T("_Header"), column.header);
            write_registry_value(section, columnKey + _T("_Index"), CString(std::to_string(column.index).c_str()));
            write_registry_value(section, columnKey + _T("_Visible"), column.visible ? _T("1") : _T("0"));
            write_registry_value(section, columnKey + _T("_Format"), CString(std::to_string(column.format).c_str()));
        }
    }
    catch (const std::exception& e)
    {
        throw Configuration_Exception(Configuration_Error::RESOURCE_FAILED, 
            _T("Failed to save column widths to registry: ") + CString(e.what()));
    }
}

void data_list_ctrl_configuration::LoadColumnWidthsFromFile(const CString& filename)
{
    try
    {
        // Load column count first
        int columnCount = _ttoi(read_ini_value(filename, _T("Columns"), _T("ColumnCount"), _T("0")));
        
        if (columnCount > 0)
        {
            m_columns_.clear();
            m_columns_.reserve(columnCount);
            
            for (int i = 0; i < columnCount; ++i)
            {
                CString columnKey;
                columnKey.Format(_T("Column%d"), i);
                
                column_config column;
                column.width = _ttoi(read_ini_value(filename, _T("Columns"), columnKey + _T("_Width"), _T("100")));
                column.header = read_ini_value(filename, _T("Columns"), columnKey + _T("_Header"), _T(""));
                column.index = _ttoi(read_ini_value(filename, _T("Columns"), columnKey + _T("_Index"), _T("0")));
                column.visible = _ttoi(read_ini_value(filename, _T("Columns"), columnKey + _T("_Visible"), _T("1"))) != 0;
                column.format = _ttoi(read_ini_value(filename, _T("Columns"), columnKey + _T("_Format"), _T("0")));
                
                m_columns_.push_back(column);
            }
        }
        else
        {
            // Load default column configuration if no saved columns
            LoadDefaultColumnConfiguration();
        }
    }
    catch (const std::exception&)
    {
        // If loading fails, use default configuration
        LoadDefaultColumnConfiguration();
    }
}

void data_list_ctrl_configuration::SaveColumnWidthsToFile(const CString& filename)
{
    try
    {
        // Save column count
        write_ini_value(filename, _T("Columns"), _T("ColumnCount"), CString(std::to_string(m_columns_.size()).c_str()));
        
        // Save each column configuration
        for (size_t i = 0; i < m_columns_.size(); ++i)
        {
            CString columnKey;
            columnKey.Format(_T("Column%d"), static_cast<int>(i));
            
            const auto& column = m_columns_[i];
            write_ini_value(filename, _T("Columns"), columnKey + _T("_Width"), CString(std::to_string(column.width).c_str()));
            write_ini_value(filename, _T("Columns"), columnKey + _T("_Header"), column.header);
            write_ini_value(filename, _T("Columns"), columnKey + _T("_Index"), CString(std::to_string(column.index).c_str()));
            write_ini_value(filename, _T("Columns"), columnKey + _T("_Visible"), column.visible ? _T("1") : _T("0"));
            write_ini_value(filename, _T("Columns"), columnKey + _T("_Format"), CString(std::to_string(column.format).c_str()));
        }
    }
    catch (const std::exception& e)
    {
        throw Configuration_Exception(Configuration_Error::RESOURCE_FAILED, 
            _T("Failed to save column widths to file: ") + CString(e.what()));
    }
}

void data_list_ctrl_configuration::LoadDefaultColumnConfiguration()
{
    // Create default column configuration matching the original DataListCtrl
    m_columns_.clear();
    m_columns_ = {
        {1, _T(""), 0, true, LVCFMT_LEFT},
        {10, _T("#"), 1, true, LVCFMT_CENTER},
        {300, _T("data"), 2, true, LVCFMT_CENTER},
        {15, _T("insect ID"), 3, true, LVCFMT_CENTER},
        {30, _T("sensillum"), 4, true, LVCFMT_CENTER},
        {30, _T("stim1"), 5, true, LVCFMT_CENTER},
        {50, _T("conc1"), 6, true, LVCFMT_CENTER},
        {40, _T("stim2"), 7, true, LVCFMT_CENTER},
        {40, _T("conc2"), 8, true, LVCFMT_CENTER},
        {40, _T("spikes"), 9, true, LVCFMT_CENTER},
        {40, _T("flag"), 10, true, LVCFMT_CENTER}
    };
}

// ConfigurationManager implementation
Configuration_Manager& Configuration_Manager::GetInstance()
{
    static Configuration_Manager instance;
    return instance;
}

void Configuration_Manager::RegisterConfiguration(const CString& name, std::shared_ptr<data_list_ctrl_configuration> config)
{
    m_configurations_[name] = config;
}

std::shared_ptr<data_list_ctrl_configuration> Configuration_Manager::GetConfiguration(const CString& name)
{
    auto it = m_configurations_.find(name);
    if (it != m_configurations_.end())
    {
        return it->second;
    }
    return nullptr;
}

void Configuration_Manager::UnregisterConfiguration(const CString& name)
{
    m_configurations_.erase(name);
}

void Configuration_Manager::SetGlobalCachingEnabled(bool enabled)
{
    m_global_caching_enabled_ = enabled;
}

void Configuration_Manager::SetGlobalPerformanceMonitoring(bool enabled)
{
    m_global_performance_monitoring_ = enabled;
}

void Configuration_Manager::ResetAllConfigurations()
{
    for (auto& pair : m_configurations_)
    {
        if (pair.second)
        {
            pair.second->reset_to_defaults();
        }
    }
}
