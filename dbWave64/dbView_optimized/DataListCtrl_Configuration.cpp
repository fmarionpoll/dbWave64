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
ConfigurationException::ConfigurationException(ConfigurationError error, const CString& message)
    : m_error(error), m_message(message)
{
}

const char* ConfigurationException::what() const noexcept
{
    static CStringA errorMessage;
    errorMessage = m_message;
    return errorMessage.GetString();
}

// DisplaySettings implementation
DisplaySettings::DisplaySettings()
    : m_imageWidth(DataListCtrlConfigConstants::DEFAULT_IMAGE_WIDTH)
    , m_imageHeight(DataListCtrlConfigConstants::DEFAULT_IMAGE_HEIGHT)
    , m_dataTransform(DataListCtrlConfigConstants::DEFAULT_DATA_TRANSFORM)
    , m_displayMode(DataListCtrlConfigConstants::DEFAULT_DISPLAY_MODE)
    , m_spikePlotMode(DataListCtrlConfigConstants::DEFAULT_SPIKE_PLOT_MODE)
    , m_selectedClass(DataListCtrlConfigConstants::DEFAULT_SELECTED_CLASS)
{
}

void DisplaySettings::SetImageWidth(int width)
{
    if (!IsValidImageWidth(width))
    {
        throw ConfigurationException(ConfigurationError::OUT_OF_RANGE, 
            _T("Invalid image width: ") + CString(std::to_string(width).c_str()));
    }
    m_imageWidth = width;
}

void DisplaySettings::SetImageHeight(int height)
{
    if (!IsValidImageHeight(height))
    {
        throw ConfigurationException(ConfigurationError::OUT_OF_RANGE, 
            _T("Invalid image height: ") + CString(std::to_string(height).c_str()));
    }
    m_imageHeight = height;
}

void DisplaySettings::SetDataTransform(int transform)
{
    m_dataTransform = transform;
}

void DisplaySettings::SetDisplayMode(int mode)
{
    if (!IsValidDisplayMode(mode))
    {
        throw ConfigurationException(ConfigurationError::INVALID_MODE, 
            _T("Invalid display mode: ") + CString(std::to_string(mode).c_str()));
    }
    m_displayMode = mode;
}

void DisplaySettings::SetSpikePlotMode(int mode)
{
    if (!IsValidSpikePlotMode(mode))
    {
        throw ConfigurationException(ConfigurationError::INVALID_MODE, 
            _T("Invalid spike plot mode: ") + CString(std::to_string(mode).c_str()));
    }
    m_spikePlotMode = mode;
}

void DisplaySettings::SetSelectedClass(int classIndex)
{
    if (!IsValidSelectedClass(classIndex))
    {
        throw ConfigurationException(ConfigurationError::OUT_OF_RANGE, 
            _T("Invalid selected class: ") + CString(std::to_string(classIndex).c_str()));
    }
    m_selectedClass = classIndex;
}

bool DisplaySettings::IsValidImageWidth(int width) const
{
    return width >= DataListCtrlConfigConstants::MIN_IMAGE_WIDTH && 
           width <= DataListCtrlConfigConstants::MAX_IMAGE_WIDTH;
}

bool DisplaySettings::IsValidImageHeight(int height) const
{
    return height >= DataListCtrlConfigConstants::MIN_IMAGE_HEIGHT && 
           height <= DataListCtrlConfigConstants::MAX_IMAGE_HEIGHT;
}

bool DisplaySettings::IsValidDisplayMode(int mode) const
{
    return mode >= DataListCtrlConfigConstants::DISPLAY_MODE_EMPTY && 
           mode <= DataListCtrlConfigConstants::DISPLAY_MODE_SPIKE;
}

bool DisplaySettings::IsValidSpikePlotMode(int mode) const
{
    return mode >= DataListCtrlConfigConstants::PLOT_BLACK && 
           mode <= DataListCtrlConfigConstants::PLOT_GREY;
}

bool DisplaySettings::IsValidSelectedClass(int classIndex) const
{
    return classIndex >= 0;
}

void DisplaySettings::ResetToDefaults()
{
    m_imageWidth = DataListCtrlConfigConstants::DEFAULT_IMAGE_WIDTH;
    m_imageHeight = DataListCtrlConfigConstants::DEFAULT_IMAGE_HEIGHT;
    m_dataTransform = DataListCtrlConfigConstants::DEFAULT_DATA_TRANSFORM;
    m_displayMode = DataListCtrlConfigConstants::DEFAULT_DISPLAY_MODE;
    m_spikePlotMode = DataListCtrlConfigConstants::DEFAULT_SPIKE_PLOT_MODE;
    m_selectedClass = DataListCtrlConfigConstants::DEFAULT_SELECTED_CLASS;
}

// TimeSettings implementation
TimeSettings::TimeSettings()
    : m_timeFirst(DataListCtrlConfigConstants::DEFAULT_TIME_FIRST)
    , m_timeLast(DataListCtrlConfigConstants::DEFAULT_TIME_LAST)
    , m_setTimeSpan(false)
{
}

void TimeSettings::SetTimeFirst(float time)
{
    if (!IsValidTimeValue(time))
    {
        throw ConfigurationException(ConfigurationError::OUT_OF_RANGE, 
            _T("Invalid time first value: ") + CString(std::to_string(time).c_str()));
    }
    m_timeFirst = time;
}

void TimeSettings::SetTimeLast(float time)
{
    if (!IsValidTimeValue(time))
    {
        throw ConfigurationException(ConfigurationError::OUT_OF_RANGE, 
            _T("Invalid time last value: ") + CString(std::to_string(time).c_str()));
    }
    m_timeLast = time;
}

void TimeSettings::SetTimeSpan(float first, float last)
{
    if (!IsValidTimeSpan(first, last))
    {
        throw ConfigurationException(ConfigurationError::OUT_OF_RANGE, 
            _T("Invalid time span: ") + CString(std::to_string(first).c_str()) + 
            _T(" to ") + CString(std::to_string(last).c_str()));
    }
    m_timeFirst = first;
    m_timeLast = last;
    m_setTimeSpan = true;
}

bool TimeSettings::IsValidTimeValue(float time) const
{
    return time >= DataListCtrlConfigConstants::MIN_TIME_VALUE && 
           time <= DataListCtrlConfigConstants::MAX_TIME_VALUE;
}

bool TimeSettings::IsValidTimeSpan(float first, float last) const
{
    return IsValidTimeValue(first) && IsValidTimeValue(last) && first <= last;
}

void TimeSettings::ResetToDefaults()
{
    m_timeFirst = DataListCtrlConfigConstants::DEFAULT_TIME_FIRST;
    m_timeLast = DataListCtrlConfigConstants::DEFAULT_TIME_LAST;
    m_setTimeSpan = false;
}

// AmplitudeSettings implementation
AmplitudeSettings::AmplitudeSettings()
    : m_mvSpan(DataListCtrlConfigConstants::DEFAULT_MV_SPAN)
    , m_setMvSpan(false)
{
}

void AmplitudeSettings::SetMvSpan(float span)
{
    if (!IsValidMvSpan(span))
    {
        throw ConfigurationException(ConfigurationError::OUT_OF_RANGE, 
            _T("Invalid MV span: ") + CString(std::to_string(span).c_str()));
    }
    m_mvSpan = span;
}

bool AmplitudeSettings::IsValidMvSpan(float span) const
{
    return span >= DataListCtrlConfigConstants::MIN_MV_SPAN && 
           span <= DataListCtrlConfigConstants::MAX_MV_SPAN;
}

void AmplitudeSettings::ResetToDefaults()
{
    m_mvSpan = DataListCtrlConfigConstants::DEFAULT_MV_SPAN;
    m_setMvSpan = false;
}

// UISettings implementation
UISettings::UISettings()
    : m_displayFileName(false)
    , m_parent(nullptr)
    , m_imageList(nullptr)
    , m_emptyBitmap(nullptr)
{
}

void UISettings::ResetToDefaults()
{
    m_displayFileName = false;
    m_parent = nullptr;
    m_imageList = nullptr;
    m_emptyBitmap = nullptr;
}

// PerformanceSettings implementation
PerformanceSettings::PerformanceSettings()
    : m_cachingEnabled(true)
    , m_lazyLoadingEnabled(true)
    , m_asyncProcessingEnabled(true)
    , m_maxCacheSize(100)
    , m_batchSize(10)
{
}

void PerformanceSettings::ResetToDefaults()
{
    m_cachingEnabled = true;
    m_lazyLoadingEnabled = true;
    m_asyncProcessingEnabled = true;
    m_maxCacheSize = 100;
    m_batchSize = 10;
}

// DataListCtrlConfiguration implementation
DataListCtrlConfiguration::DataListCtrlConfiguration()
{
}

DataListCtrlConfiguration::~DataListCtrlConfiguration()
{
}

DataListCtrlConfiguration::DataListCtrlConfiguration(DataListCtrlConfiguration&& other) noexcept
    : m_displaySettings(std::move(other.m_displaySettings))
    , m_timeSettings(std::move(other.m_timeSettings))
    , m_amplitudeSettings(std::move(other.m_amplitudeSettings))
    , m_uiSettings(std::move(other.m_uiSettings))
    , m_performanceSettings(std::move(other.m_performanceSettings))
    , m_changeCallback(std::move(other.m_changeCallback))
{
}

DataListCtrlConfiguration& DataListCtrlConfiguration::operator=(DataListCtrlConfiguration&& other) noexcept
{
    if (this != &other)
    {
        m_displaySettings = std::move(other.m_displaySettings);
        m_timeSettings = std::move(other.m_timeSettings);
        m_amplitudeSettings = std::move(other.m_amplitudeSettings);
        m_uiSettings = std::move(other.m_uiSettings);
        m_performanceSettings = std::move(other.m_performanceSettings);
        m_changeCallback = std::move(other.m_changeCallback);
    }
    return *this;
}

void DataListCtrlConfiguration::LoadFromRegistry(const CString& section)
{
    try
    {
        // Load display settings
        m_displaySettings.SetImageWidth(_ttoi(read_registry_value(section, _T("ImageWidth"),
            CString(std::to_string(DataListCtrlConfigConstants::DEFAULT_IMAGE_WIDTH).c_str()))));
        m_displaySettings.SetImageHeight(_ttoi(read_registry_value(section, _T("ImageHeight"),
            CString(std::to_string(DataListCtrlConfigConstants::DEFAULT_IMAGE_HEIGHT).c_str()))));
        m_displaySettings.SetDisplayMode(_ttoi(read_registry_value(section, _T("DisplayMode"),
            CString(std::to_string(DataListCtrlConfigConstants::DEFAULT_DISPLAY_MODE).c_str()))));

        // Load time settings
        CString default_value = float_to_string(DataListCtrlConfigConstants::DEFAULT_TIME_FIRST);
		CString strValue = read_registry_value(section, _T("TimeFirst"), default_value);
        float value = static_cast<float>(_ttof(strValue));
        m_timeSettings.SetTimeFirst(value);
        default_value = float_to_string(DataListCtrlConfigConstants::DEFAULT_TIME_LAST);
        value = static_cast<float>(_ttof(read_registry_value(section, _T("TimeLast"), default_value)));
        m_timeSettings.SetTimeLast(value);
        m_timeSettings.SetTimeSpanEnabled(_ttoi(read_registry_value(section, _T("SetTimeSpan"), _T("0"))) != 0);

        // Load amplitude settings
        default_value = float_to_string(DataListCtrlConfigConstants::DEFAULT_MV_SPAN);
        value = static_cast<float>(_ttof(read_registry_value(section, _T("MvSpan"), default_value)));
        m_amplitudeSettings.SetMvSpan(value);

        m_amplitudeSettings.SetMvSpanEnabled(_ttoi(read_registry_value(section, _T("SetMvSpan"), _T("0"))) != 0);
        
        // Load UI settings
        m_uiSettings.SetDisplayFileName(_ttoi(read_registry_value(section, _T("DisplayFileName"), _T("0"))) != 0);
        
        // Load performance settings
        m_performanceSettings.SetCachingEnabled(_ttoi(read_registry_value(section, _T("CachingEnabled"), _T("1"))) != 0);
        m_performanceSettings.SetLazyLoadingEnabled(_ttoi(read_registry_value(section, _T("LazyLoadingEnabled"), _T("1"))) != 0);
        m_performanceSettings.SetAsyncProcessingEnabled(_ttoi(read_registry_value(section, _T("AsyncProcessingEnabled"), _T("1"))) != 0);
        m_performanceSettings.SetMaxCacheSize(_ttoi(read_registry_value(section, _T("MaxCacheSize"), _T("100"))));
        m_performanceSettings.SetBatchSize(_ttoi(read_registry_value(section, _T("BatchSize"), _T("10"))));
        
        // Load column widths
        LoadColumnWidthsFromRegistry(section);
    }
    catch (const std::exception& e)
    {
        throw ConfigurationException(ConfigurationError::RESOURCE_FAILED, 
            _T("Failed to load configuration from registry: ") + CString(e.what()));
    }
}

void DataListCtrlConfiguration::SaveToRegistry(const CString& section)
{
    try
    {
        // Save display settings
        write_registry_value(section, _T("ImageWidth"), CString(std::to_string(m_displaySettings.GetImageWidth()).c_str()));
        write_registry_value(section, _T("ImageHeight"), CString(std::to_string(m_displaySettings.GetImageHeight()).c_str()));
        write_registry_value(section, _T("DisplayMode"), CString(std::to_string(m_displaySettings.GetDisplayMode()).c_str()));
        
        // Save time settings
        write_registry_value(section, _T("TimeFirst"), float_to_string(m_timeSettings.GetTimeFirst()));
        write_registry_value(section, _T("TimeLast"), float_to_string(m_timeSettings.GetTimeLast()));
        write_registry_value(section, _T("SetTimeSpan"), m_timeSettings.IsTimeSpanSet() ? _T("1") : _T("0"));
        
        // Save amplitude settings
        write_registry_value(section, _T("MvSpan"), float_to_string(m_amplitudeSettings.GetMvSpan()));
        write_registry_value(section, _T("SetMvSpan"), m_amplitudeSettings.IsMvSpanSet() ? _T("1") : _T("0"));
        
        // Save UI settings
        write_registry_value(section, _T("DisplayFileName"), m_uiSettings.IsDisplayFileName() ? _T("1") : _T("0"));
        
        // Save performance settings
        write_registry_value(section, _T("CachingEnabled"), m_performanceSettings.IsCachingEnabled() ? _T("1") : _T("0"));
        write_registry_value(section, _T("LazyLoadingEnabled"), m_performanceSettings.IsLazyLoadingEnabled() ? _T("1") : _T("0"));
        write_registry_value(section, _T("AsyncProcessingEnabled"), m_performanceSettings.IsAsyncProcessingEnabled() ? _T("1") : _T("0"));
        write_registry_value(section, _T("MaxCacheSize"), CString(std::to_string(m_performanceSettings.GetMaxCacheSize()).c_str()));
        write_registry_value(section, _T("BatchSize"), CString(std::to_string(m_performanceSettings.GetBatchSize()).c_str()));
        
        // Save column widths
        SaveColumnWidthsToRegistry(section);
    }
    catch (const std::exception& e)
    {
        throw ConfigurationException(ConfigurationError::RESOURCE_FAILED, 
            _T("Failed to save configuration to registry: ") + CString(e.what()));
    }
}

void DataListCtrlConfiguration::LoadFromFile(const CString& filename)
{
    try
    {
        // Load display settings
        m_displaySettings.SetImageWidth(_ttoi(read_ini_value(filename, _T("Display"), _T("ImageWidth"), 
            CString(std::to_string(DataListCtrlConfigConstants::DEFAULT_IMAGE_WIDTH).c_str()))));
        m_displaySettings.SetImageHeight(_ttoi(read_ini_value(filename, _T("Display"), _T("ImageHeight"), 
            CString(std::to_string(DataListCtrlConfigConstants::DEFAULT_IMAGE_HEIGHT).c_str()))));
        m_displaySettings.SetDisplayMode(_ttoi(read_ini_value(filename, _T("Display"), _T("DisplayMode"), 
            CString(std::to_string(DataListCtrlConfigConstants::DEFAULT_DISPLAY_MODE).c_str()))));
        
        // Load time settings
        CString default_value = float_to_string(DataListCtrlConfigConstants::DEFAULT_TIME_FIRST);
        float value = static_cast<float>(_ttof(read_ini_value(filename, _T("Time"), _T("TimeFirst"), default_value)));
        m_timeSettings.SetTimeFirst(value);
		default_value = float_to_string(DataListCtrlConfigConstants::DEFAULT_TIME_LAST);
		value = static_cast<float>(_ttof(read_ini_value(filename, _T("Time"), _T("TimeLast"), default_value)));
        m_timeSettings.SetTimeLast(value);
        m_timeSettings.SetTimeSpanEnabled(_ttoi(read_ini_value(filename, _T("Time"), _T("SetTimeSpan"), _T("0"))) != 0);
        
        // Load amplitude settings
		default_value = float_to_string(DataListCtrlConfigConstants::DEFAULT_MV_SPAN);
		value = static_cast<float>(_ttof(read_ini_value(filename, _T("Amplitude"), _T("MvSpan"), default_value)));
        m_amplitudeSettings.SetMvSpan(value);
        m_amplitudeSettings.SetMvSpanEnabled(_ttoi(read_ini_value(filename, _T("Amplitude"), _T("SetMvSpan"), _T("0"))) != 0);
        
        // Load UI settings
        m_uiSettings.SetDisplayFileName(_ttoi(read_ini_value(filename, _T("UI"), _T("DisplayFileName"), _T("0"))) != 0);
        
        // Load performance settings
        m_performanceSettings.SetCachingEnabled(_ttoi(read_ini_value(filename, _T("Performance"), _T("CachingEnabled"), _T("1"))) != 0);
        m_performanceSettings.SetLazyLoadingEnabled(_ttoi(read_ini_value(filename, _T("Performance"), _T("LazyLoadingEnabled"), _T("1"))) != 0);
        m_performanceSettings.SetAsyncProcessingEnabled(_ttoi(read_ini_value(filename, _T("Performance"), _T("AsyncProcessingEnabled"), _T("1"))) != 0);
        m_performanceSettings.SetMaxCacheSize(_ttoi(read_ini_value(filename, _T("Performance"), _T("MaxCacheSize"), _T("100"))));
        m_performanceSettings.SetBatchSize(_ttoi(read_ini_value(filename, _T("Performance"), _T("BatchSize"), _T("10"))));
        
        // Load column widths
        LoadColumnWidthsFromFile(filename);
    }
    catch (const std::exception& e)
    {
        throw ConfigurationException(ConfigurationError::RESOURCE_FAILED, 
            _T("Failed to load configuration from file: ") + CString(e.what()));
    }
}

void DataListCtrlConfiguration::SaveToFile(const CString& filename)
{
    try
    {
        // Save display settings
        write_ini_value(filename, _T("Display"), _T("ImageWidth"), CString(std::to_string(m_displaySettings.GetImageWidth()).c_str()));
        write_ini_value(filename, _T("Display"), _T("ImageHeight"), CString(std::to_string(m_displaySettings.GetImageHeight()).c_str()));
        write_ini_value(filename, _T("Display"), _T("DisplayMode"), CString(std::to_string(m_displaySettings.GetDisplayMode()).c_str()));
        
        // Save time settings
        write_ini_value(filename, _T("Time"), _T("TimeFirst"), float_to_string(m_timeSettings.GetTimeFirst()));
        write_ini_value(filename, _T("Time"), _T("TimeLast"), float_to_string(m_timeSettings.GetTimeLast()));
        write_ini_value(filename, _T("Time"), _T("SetTimeSpan"), m_timeSettings.IsTimeSpanSet() ? _T("1") : _T("0"));
        
        // Save amplitude settings
        write_ini_value(filename, _T("Amplitude"), _T("MvSpan"), float_to_string(m_amplitudeSettings.GetMvSpan()));
        write_ini_value(filename, _T("Amplitude"), _T("SetMvSpan"), m_amplitudeSettings.IsMvSpanSet() ? _T("1") : _T("0"));
        
        // Save UI settings
        write_ini_value(filename, _T("UI"), _T("DisplayFileName"), m_uiSettings.IsDisplayFileName() ? _T("1") : _T("0"));
        
        // Save performance settings
        write_ini_value(filename, _T("Performance"), _T("CachingEnabled"), m_performanceSettings.IsCachingEnabled() ? _T("1") : _T("0"));
        write_ini_value(filename, _T("Performance"), _T("LazyLoadingEnabled"), m_performanceSettings.IsLazyLoadingEnabled() ? _T("1") : _T("0"));
        write_ini_value(filename, _T("Performance"), _T("AsyncProcessingEnabled"), m_performanceSettings.IsAsyncProcessingEnabled() ? _T("1") : _T("0"));
        write_ini_value(filename, _T("Performance"), _T("MaxCacheSize"), CString(std::to_string(m_performanceSettings.GetMaxCacheSize()).c_str()));
        write_ini_value(filename, _T("Performance"), _T("BatchSize"), CString(std::to_string(m_performanceSettings.GetBatchSize()).c_str()));
        
        // Save column widths
        SaveColumnWidthsToFile(filename);
    }
    catch (const std::exception& e)
    {
        throw ConfigurationException(ConfigurationError::RESOURCE_FAILED, 
            _T("Failed to save configuration to file: ") + CString(e.what()));
    }
}

bool DataListCtrlConfiguration::ValidateConfiguration() const
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

CString DataListCtrlConfiguration::GetValidationErrors() const
{
    CString errors;
    
    try
    {
        ValidateAllSettings();
    }
    catch (const ConfigurationException& e)
    {
        errors = e.GetMessage();
    }
    catch (const std::exception& e)
    {
        errors = CString(e.what());
    }
    
    return errors;
}

void DataListCtrlConfiguration::ResetToDefaults()
{
    m_displaySettings.ResetToDefaults();
    m_timeSettings.ResetToDefaults();
    m_amplitudeSettings.ResetToDefaults();
    m_uiSettings.ResetToDefaults();
    m_performanceSettings.ResetToDefaults();
}

void DataListCtrlConfiguration::RegisterChangeCallback(ChangeCallback callback)
{
    m_changeCallback = callback;
}

void DataListCtrlConfiguration::UnregisterChangeCallback()
{
    m_changeCallback = nullptr;
}

void DataListCtrlConfiguration::LoadFromLegacyInfo(const LegacyInfo& info)
{
    m_uiSettings.SetParent(info.parent);
    m_uiSettings.SetImageList(info.p_image_list);  // Pass pointer directly
    m_uiSettings.SetEmptyBitmap(info.p_empty_bitmap);
    m_displaySettings.SetImageWidth(info.image_width);
    m_displaySettings.SetImageHeight(info.image_height);
    m_displaySettings.SetDataTransform(info.data_transform);
    m_displaySettings.SetDisplayMode(info.display_mode);
    m_displaySettings.SetSpikePlotMode(info.spike_plot_mode);
    m_displaySettings.SetSelectedClass(info.selected_class);
    m_timeSettings.SetTimeFirst(info.t_first);
    m_timeSettings.SetTimeLast(info.t_last);
    m_amplitudeSettings.SetMvSpan(info.mv_span);
    m_timeSettings.SetTimeSpanEnabled(info.b_set_time_span);
    m_amplitudeSettings.SetMvSpanEnabled(info.b_set_mv_span);
    m_uiSettings.SetDisplayFileName(info.b_display_file_name);
}

DataListCtrlConfiguration::LegacyInfo DataListCtrlConfiguration::ToLegacyInfo() const
{
    LegacyInfo info;
    info.parent = m_uiSettings.GetParent();
    info.p_image_list = m_uiSettings.GetImageList();  // Assign pointer directly
    info.p_empty_bitmap = m_uiSettings.GetEmptyBitmap();
    info.image_width = m_displaySettings.GetImageWidth();
    info.image_height = m_displaySettings.GetImageHeight();
    info.data_transform = m_displaySettings.GetDataTransform();
    info.display_mode = m_displaySettings.GetDisplayMode();
    info.spike_plot_mode = m_displaySettings.GetSpikePlotMode();
    info.selected_class = m_displaySettings.GetSelectedClass();
    info.t_first = m_timeSettings.GetTimeFirst();
    info.t_last = m_timeSettings.GetTimeLast();
    info.mv_span = m_amplitudeSettings.GetMvSpan();
    info.b_set_time_span = m_timeSettings.IsTimeSpanSet();
    info.b_set_mv_span = m_amplitudeSettings.IsMvSpanSet();
    info.b_display_file_name = m_uiSettings.IsDisplayFileName();
    return info;
}

CString DataListCtrlConfiguration::ToString() const
{
    CString result;
    result.Format(_T("Display: %dx%d, Mode: %d, Transform: %d\n")
                  _T("Time: %.2f-%.2f, Set: %s\n")
                  _T("Amplitude: %.2f, Set: %s\n")
                  _T("UI: DisplayFileName: %s\n")
                  _T("Performance: Cache: %s, Lazy: %s, Async: %s"),
                  m_displaySettings.GetImageWidth(), m_displaySettings.GetImageHeight(),
                  m_displaySettings.GetDisplayMode(), m_displaySettings.GetDataTransform(),
                  m_timeSettings.GetTimeFirst(), m_timeSettings.GetTimeLast(),
                  m_timeSettings.IsTimeSpanSet() ? _T("Yes") : _T("No"),
                  m_amplitudeSettings.GetMvSpan(),
                  m_amplitudeSettings.IsMvSpanSet() ? _T("Yes") : _T("No"),
                  m_uiSettings.IsDisplayFileName() ? _T("Yes") : _T("No"),
                  m_performanceSettings.IsCachingEnabled() ? _T("Yes") : _T("No"),
                  m_performanceSettings.IsLazyLoadingEnabled() ? _T("Yes") : _T("No"),
                  m_performanceSettings.IsAsyncProcessingEnabled() ? _T("Yes") : _T("No"));
    return result;
}

void DataListCtrlConfiguration::FromString(const CString& configString)
{
    // Parse configuration string and set values
    // This is a simplified implementation - in practice, you'd want more robust parsing
    ResetToDefaults();
    
    // Example parsing (simplified)
    if (configString.Find(_T("Display:")) >= 0)
    {
        // Parse display settings
        // Implementation would parse the string and set values
    }
}

void DataListCtrlConfiguration::NotifyChange(const CString& setting, const CString& value)
{
    if (m_changeCallback)
    {
        m_changeCallback(setting, value);
    }
}

void DataListCtrlConfiguration::ValidateAllSettings() const
{
    // Validate display settings
    if (!m_displaySettings.IsValidImageWidth(m_displaySettings.GetImageWidth()))
    {
        throw ConfigurationException(ConfigurationError::OUT_OF_RANGE, _T("Invalid image width"));
    }
    if (!m_displaySettings.IsValidImageHeight(m_displaySettings.GetImageHeight()))
    {
        throw ConfigurationException(ConfigurationError::OUT_OF_RANGE, _T("Invalid image height"));
    }
    if (!m_displaySettings.IsValidDisplayMode(m_displaySettings.GetDisplayMode()))
    {
        throw ConfigurationException(ConfigurationError::INVALID_MODE, _T("Invalid display mode"));
    }
    
    // Validate time settings
    if (!m_timeSettings.IsValidTimeValue(m_timeSettings.GetTimeFirst()))
    {
        throw ConfigurationException(ConfigurationError::OUT_OF_RANGE, _T("Invalid time first value"));
    }
    if (!m_timeSettings.IsValidTimeValue(m_timeSettings.GetTimeLast()))
    {
        throw ConfigurationException(ConfigurationError::OUT_OF_RANGE, _T("Invalid time last value"));
    }
    if (m_timeSettings.IsTimeSpanSet() && 
        !m_timeSettings.IsValidTimeSpan(m_timeSettings.GetTimeFirst(), m_timeSettings.GetTimeLast()))
    {
        throw ConfigurationException(ConfigurationError::OUT_OF_RANGE, _T("Invalid time span"));
    }
    
    // Validate amplitude settings
    if (!m_amplitudeSettings.IsValidMvSpan(m_amplitudeSettings.GetMvSpan()))
    {
        throw ConfigurationException(ConfigurationError::OUT_OF_RANGE, _T("Invalid MV span"));
    }
}

void DataListCtrlConfiguration::write_registry_value(const CString& section, const CString& key, const CString& value)
{
    // Use centralized registry manager
    RegistryManager::GetInstance().WriteStringValue(section, key, value);
}

CString DataListCtrlConfiguration::read_registry_value(const CString& section, const CString& key, const CString& default_value)
{
    // Use centralized registry manager
    return RegistryManager::GetInstance().ReadStringValue(section, key, default_value);
}

void DataListCtrlConfiguration::write_ini_value(const CString& filename, const CString& section, const CString& key, const CString& value)
{
    WritePrivateProfileString(section, key, value, filename);
}

CString DataListCtrlConfiguration::read_ini_value(const CString& filename, const CString& section, const CString& key, const CString& default_value)
{
    TCHAR buffer[256];
    GetPrivateProfileString(section, key, default_value, buffer, sizeof(buffer), filename);
    return CString(buffer);
}

// Column management implementation
void DataListCtrlConfiguration::SetColumns(const std::vector<ColumnConfig>& columns)
{
    m_columns = columns;
    NotifyChange(_T("Columns"), _T("Updated"));
}

std::vector<DataListCtrlConfiguration::ColumnConfig> DataListCtrlConfiguration::GetColumns() const
{
    return m_columns;
}

void DataListCtrlConfiguration::AddColumn(const ColumnConfig& column)
{
    m_columns.push_back(column);
    NotifyChange(_T("Columns"), _T("Added"));
}

void DataListCtrlConfiguration::RemoveColumn(int index)
{
    if (index >= 0 && index < static_cast<int>(m_columns.size()))
    {
        m_columns.erase(m_columns.begin() + index);
        NotifyChange(_T("Columns"), _T("Removed"));
    }
}

void DataListCtrlConfiguration::ClearColumns()
{
    m_columns.clear();
    NotifyChange(_T("Columns"), _T("Cleared"));
}

// Column width persistence helper methods
void DataListCtrlConfiguration::LoadColumnWidthsFromRegistry(const CString& section)
{
    try
    {
        // Load column count first
        int columnCount = _ttoi(read_registry_value(section, _T("ColumnCount"), _T("0")));
        
        if (columnCount > 0)
        {
            m_columns.clear();
            m_columns.reserve(columnCount);
            
            for (int i = 0; i < columnCount; ++i)
            {
                CString columnKey;
                columnKey.Format(_T("Column%d"), i);
                
                ColumnConfig column;
                column.width = _ttoi(read_registry_value(section, columnKey + _T("_Width"), _T("100")));
                column.header = read_registry_value(section, columnKey + _T("_Header"), _T(""));
                column.index = _ttoi(read_registry_value(section, columnKey + _T("_Index"), _T("0")));
                column.visible = _ttoi(read_registry_value(section, columnKey + _T("_Visible"), _T("1"))) != 0;
                column.format = _ttoi(read_registry_value(section, columnKey + _T("_Format"), _T("0")));
                
                m_columns.push_back(column);
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

void DataListCtrlConfiguration::SaveColumnWidthsToRegistry(const CString& section)
{
    try
    {
        // Save column count
        write_registry_value(section, _T("ColumnCount"), CString(std::to_string(m_columns.size()).c_str()));
        
        // Save each column configuration
        for (size_t i = 0; i < m_columns.size(); ++i)
        {
            CString columnKey;
            columnKey.Format(_T("Column%d"), static_cast<int>(i));
            
            const auto& column = m_columns[i];
            write_registry_value(section, columnKey + _T("_Width"), CString(std::to_string(column.width).c_str()));
            write_registry_value(section, columnKey + _T("_Header"), column.header);
            write_registry_value(section, columnKey + _T("_Index"), CString(std::to_string(column.index).c_str()));
            write_registry_value(section, columnKey + _T("_Visible"), column.visible ? _T("1") : _T("0"));
            write_registry_value(section, columnKey + _T("_Format"), CString(std::to_string(column.format).c_str()));
        }
    }
    catch (const std::exception& e)
    {
        throw ConfigurationException(ConfigurationError::RESOURCE_FAILED, 
            _T("Failed to save column widths to registry: ") + CString(e.what()));
    }
}

void DataListCtrlConfiguration::LoadColumnWidthsFromFile(const CString& filename)
{
    try
    {
        // Load column count first
        int columnCount = _ttoi(read_ini_value(filename, _T("Columns"), _T("ColumnCount"), _T("0")));
        
        if (columnCount > 0)
        {
            m_columns.clear();
            m_columns.reserve(columnCount);
            
            for (int i = 0; i < columnCount; ++i)
            {
                CString columnKey;
                columnKey.Format(_T("Column%d"), i);
                
                ColumnConfig column;
                column.width = _ttoi(read_ini_value(filename, _T("Columns"), columnKey + _T("_Width"), _T("100")));
                column.header = read_ini_value(filename, _T("Columns"), columnKey + _T("_Header"), _T(""));
                column.index = _ttoi(read_ini_value(filename, _T("Columns"), columnKey + _T("_Index"), _T("0")));
                column.visible = _ttoi(read_ini_value(filename, _T("Columns"), columnKey + _T("_Visible"), _T("1"))) != 0;
                column.format = _ttoi(read_ini_value(filename, _T("Columns"), columnKey + _T("_Format"), _T("0")));
                
                m_columns.push_back(column);
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

void DataListCtrlConfiguration::SaveColumnWidthsToFile(const CString& filename)
{
    try
    {
        // Save column count
        write_ini_value(filename, _T("Columns"), _T("ColumnCount"), CString(std::to_string(m_columns.size()).c_str()));
        
        // Save each column configuration
        for (size_t i = 0; i < m_columns.size(); ++i)
        {
            CString columnKey;
            columnKey.Format(_T("Column%d"), static_cast<int>(i));
            
            const auto& column = m_columns[i];
            write_ini_value(filename, _T("Columns"), columnKey + _T("_Width"), CString(std::to_string(column.width).c_str()));
            write_ini_value(filename, _T("Columns"), columnKey + _T("_Header"), column.header);
            write_ini_value(filename, _T("Columns"), columnKey + _T("_Index"), CString(std::to_string(column.index).c_str()));
            write_ini_value(filename, _T("Columns"), columnKey + _T("_Visible"), column.visible ? _T("1") : _T("0"));
            write_ini_value(filename, _T("Columns"), columnKey + _T("_Format"), CString(std::to_string(column.format).c_str()));
        }
    }
    catch (const std::exception& e)
    {
        throw ConfigurationException(ConfigurationError::RESOURCE_FAILED, 
            _T("Failed to save column widths to file: ") + CString(e.what()));
    }
}

void DataListCtrlConfiguration::LoadDefaultColumnConfiguration()
{
    // Create default column configuration matching the original DataListCtrl
    m_columns.clear();
    m_columns = {
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
ConfigurationManager& ConfigurationManager::GetInstance()
{
    static ConfigurationManager instance;
    return instance;
}

void ConfigurationManager::RegisterConfiguration(const CString& name, std::shared_ptr<DataListCtrlConfiguration> config)
{
    m_configurations[name] = config;
}

std::shared_ptr<DataListCtrlConfiguration> ConfigurationManager::GetConfiguration(const CString& name)
{
    auto it = m_configurations.find(name);
    if (it != m_configurations.end())
    {
        return it->second;
    }
    return nullptr;
}

void ConfigurationManager::UnregisterConfiguration(const CString& name)
{
    m_configurations.erase(name);
}

void ConfigurationManager::SetGlobalCachingEnabled(bool enabled)
{
    m_globalCachingEnabled = enabled;
}

void ConfigurationManager::SetGlobalPerformanceMonitoring(bool enabled)
{
    m_globalPerformanceMonitoring = enabled;
}

void ConfigurationManager::ResetAllConfigurations()
{
    for (auto& pair : m_configurations)
    {
        if (pair.second)
        {
            pair.second->ResetToDefaults();
        }
    }
}
