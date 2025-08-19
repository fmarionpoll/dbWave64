#include "StdAfx.h"
#include "DataListCtrl_Configuration.h"

// Helper function to convert float to CString
CString FloatToString(float value)
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
        m_displaySettings.SetImageWidth(_ttoi(ReadRegistryValue(section, _T("ImageWidth"),
            CString(std::to_string(DataListCtrlConfigConstants::DEFAULT_IMAGE_WIDTH).c_str()))));
        m_displaySettings.SetImageHeight(_ttoi(ReadRegistryValue(section, _T("ImageHeight"),
            CString(std::to_string(DataListCtrlConfigConstants::DEFAULT_IMAGE_HEIGHT).c_str()))));
        m_displaySettings.SetDisplayMode(_ttoi(ReadRegistryValue(section, _T("DisplayMode"),
            CString(std::to_string(DataListCtrlConfigConstants::DEFAULT_DISPLAY_MODE).c_str()))));

        // Load time settings
        CString default_value = FloatToString((DataListCtrlConfigConstants::DEFAULT_TIME_FIRST));
        float value = _ttof(ReadRegistryValue(section, _T("TimeFirst"), default_value));
        m_timeSettings.SetTimeFirst(value);
        default_value = FloatToString((DataListCtrlConfigConstants::DEFAULT_TIME_LAST));
        value = _ttof(ReadRegistryValue(section, _T("TimeLast"), default_value));
        m_timeSettings.SetTimeLast(value);
        m_timeSettings.SetTimeSpanEnabled(_ttoi(ReadRegistryValue(section, _T("SetTimeSpan"), _T("0"))) != 0);

        // Load amplitude settings
        default_value = FloatToString(static_cast<float>(DataListCtrlConfigConstants::DEFAULT_MV_SPAN));
        value = _ttof(ReadRegistryValue(section, _T("MvSpan"), default_value));
        m_amplitudeSettings.SetMvSpan(value);

        m_amplitudeSettings.SetMvSpanEnabled(_ttoi(ReadRegistryValue(section, _T("SetMvSpan"), _T("0"))) != 0);
        
        // Load UI settings
        m_uiSettings.SetDisplayFileName(_ttoi(ReadRegistryValue(section, _T("DisplayFileName"), _T("0"))) != 0);
        
        // Load performance settings
        m_performanceSettings.SetCachingEnabled(_ttoi(ReadRegistryValue(section, _T("CachingEnabled"), _T("1"))) != 0);
        m_performanceSettings.SetLazyLoadingEnabled(_ttoi(ReadRegistryValue(section, _T("LazyLoadingEnabled"), _T("1"))) != 0);
        m_performanceSettings.SetAsyncProcessingEnabled(_ttoi(ReadRegistryValue(section, _T("AsyncProcessingEnabled"), _T("1"))) != 0);
        m_performanceSettings.SetMaxCacheSize(_ttoi(ReadRegistryValue(section, _T("MaxCacheSize"), _T("100"))));
        m_performanceSettings.SetBatchSize(_ttoi(ReadRegistryValue(section, _T("BatchSize"), _T("10"))));
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
        WriteRegistryValue(section, _T("ImageWidth"), CString(std::to_string(m_displaySettings.GetImageWidth()).c_str()));
        WriteRegistryValue(section, _T("ImageHeight"), CString(std::to_string(m_displaySettings.GetImageHeight()).c_str()));
        WriteRegistryValue(section, _T("DisplayMode"), CString(std::to_string(m_displaySettings.GetDisplayMode()).c_str()));
        
        // Save time settings
        WriteRegistryValue(section, _T("TimeFirst"), FloatToString(m_timeSettings.GetTimeFirst()));
        WriteRegistryValue(section, _T("TimeLast"), FloatToString(m_timeSettings.GetTimeLast()));
        WriteRegistryValue(section, _T("SetTimeSpan"), m_timeSettings.IsTimeSpanSet() ? _T("1") : _T("0"));
        
        // Save amplitude settings
        WriteRegistryValue(section, _T("MvSpan"), FloatToString(m_amplitudeSettings.GetMvSpan()));
        WriteRegistryValue(section, _T("SetMvSpan"), m_amplitudeSettings.IsMvSpanSet() ? _T("1") : _T("0"));
        
        // Save UI settings
        WriteRegistryValue(section, _T("DisplayFileName"), m_uiSettings.IsDisplayFileName() ? _T("1") : _T("0"));
        
        // Save performance settings
        WriteRegistryValue(section, _T("CachingEnabled"), m_performanceSettings.IsCachingEnabled() ? _T("1") : _T("0"));
        WriteRegistryValue(section, _T("LazyLoadingEnabled"), m_performanceSettings.IsLazyLoadingEnabled() ? _T("1") : _T("0"));
        WriteRegistryValue(section, _T("AsyncProcessingEnabled"), m_performanceSettings.IsAsyncProcessingEnabled() ? _T("1") : _T("0"));
        WriteRegistryValue(section, _T("MaxCacheSize"), CString(std::to_string(m_performanceSettings.GetMaxCacheSize()).c_str()));
        WriteRegistryValue(section, _T("BatchSize"), CString(std::to_string(m_performanceSettings.GetBatchSize()).c_str()));
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
        m_displaySettings.SetImageWidth(_ttoi(ReadIniValue(filename, _T("Display"), _T("ImageWidth"), 
            CString(std::to_string(DataListCtrlConfigConstants::DEFAULT_IMAGE_WIDTH).c_str()))));
        m_displaySettings.SetImageHeight(_ttoi(ReadIniValue(filename, _T("Display"), _T("ImageHeight"), 
            CString(std::to_string(DataListCtrlConfigConstants::DEFAULT_IMAGE_HEIGHT).c_str()))));
        m_displaySettings.SetDisplayMode(_ttoi(ReadIniValue(filename, _T("Display"), _T("DisplayMode"), 
            CString(std::to_string(DataListCtrlConfigConstants::DEFAULT_DISPLAY_MODE).c_str()))));
        
        // Load time settings
        CString default_value = FloatToString((DataListCtrlConfigConstants::DEFAULT_TIME_FIRST));
        float value = _ttof(ReadIniValue(filename, _T("Time"), _T("TimeFirst"), default_value));
        m_timeSettings.SetTimeFirst(value);
		default_value = FloatToString((DataListCtrlConfigConstants::DEFAULT_TIME_LAST));
		value = _ttof(ReadIniValue(filename, _T("Time"), _T("TimeLast"), default_value));
        m_timeSettings.SetTimeLast(value);
        m_timeSettings.SetTimeSpanEnabled(_ttoi(ReadIniValue(filename, _T("Time"), _T("SetTimeSpan"), _T("0"))) != 0);
        
        // Load amplitude settings
		default_value = FloatToString(static_cast<float>(DataListCtrlConfigConstants::DEFAULT_MV_SPAN));
		value = _ttof(ReadIniValue(filename, _T("Amplitude"), _T("MvSpan"), default_value));
        m_amplitudeSettings.SetMvSpan(value);
        m_amplitudeSettings.SetMvSpanEnabled(_ttoi(ReadIniValue(filename, _T("Amplitude"), _T("SetMvSpan"), _T("0"))) != 0);
        
        // Load UI settings
        m_uiSettings.SetDisplayFileName(_ttoi(ReadIniValue(filename, _T("UI"), _T("DisplayFileName"), _T("0"))) != 0);
        
        // Load performance settings
        m_performanceSettings.SetCachingEnabled(_ttoi(ReadIniValue(filename, _T("Performance"), _T("CachingEnabled"), _T("1"))) != 0);
        m_performanceSettings.SetLazyLoadingEnabled(_ttoi(ReadIniValue(filename, _T("Performance"), _T("LazyLoadingEnabled"), _T("1"))) != 0);
        m_performanceSettings.SetAsyncProcessingEnabled(_ttoi(ReadIniValue(filename, _T("Performance"), _T("AsyncProcessingEnabled"), _T("1"))) != 0);
        m_performanceSettings.SetMaxCacheSize(_ttoi(ReadIniValue(filename, _T("Performance"), _T("MaxCacheSize"), _T("100"))));
        m_performanceSettings.SetBatchSize(_ttoi(ReadIniValue(filename, _T("Performance"), _T("BatchSize"), _T("10"))));
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
        WriteIniValue(filename, _T("Display"), _T("ImageWidth"), CString(std::to_string(m_displaySettings.GetImageWidth()).c_str()));
        WriteIniValue(filename, _T("Display"), _T("ImageHeight"), CString(std::to_string(m_displaySettings.GetImageHeight()).c_str()));
        WriteIniValue(filename, _T("Display"), _T("DisplayMode"), CString(std::to_string(m_displaySettings.GetDisplayMode()).c_str()));
        
        // Save time settings
        WriteIniValue(filename, _T("Time"), _T("TimeFirst"), FloatToString(m_timeSettings.GetTimeFirst()));
        WriteIniValue(filename, _T("Time"), _T("TimeLast"), FloatToString(m_timeSettings.GetTimeLast()));
        WriteIniValue(filename, _T("Time"), _T("SetTimeSpan"), m_timeSettings.IsTimeSpanSet() ? _T("1") : _T("0"));
        
        // Save amplitude settings
        WriteIniValue(filename, _T("Amplitude"), _T("MvSpan"), FloatToString(m_amplitudeSettings.GetMvSpan()));
        WriteIniValue(filename, _T("Amplitude"), _T("SetMvSpan"), m_amplitudeSettings.IsMvSpanSet() ? _T("1") : _T("0"));
        
        // Save UI settings
        WriteIniValue(filename, _T("UI"), _T("DisplayFileName"), m_uiSettings.IsDisplayFileName() ? _T("1") : _T("0"));
        
        // Save performance settings
        WriteIniValue(filename, _T("Performance"), _T("CachingEnabled"), m_performanceSettings.IsCachingEnabled() ? _T("1") : _T("0"));
        WriteIniValue(filename, _T("Performance"), _T("LazyLoadingEnabled"), m_performanceSettings.IsLazyLoadingEnabled() ? _T("1") : _T("0"));
        WriteIniValue(filename, _T("Performance"), _T("AsyncProcessingEnabled"), m_performanceSettings.IsAsyncProcessingEnabled() ? _T("1") : _T("0"));
        WriteIniValue(filename, _T("Performance"), _T("MaxCacheSize"), CString(std::to_string(m_performanceSettings.GetMaxCacheSize()).c_str()));
        WriteIniValue(filename, _T("Performance"), _T("BatchSize"), CString(std::to_string(m_performanceSettings.GetBatchSize()).c_str()));
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

void DataListCtrlConfiguration::WriteRegistryValue(const CString& section, const CString& key, const CString& value)
{
    CString fullKey = _T("Software\\dbWave64\\") + section + _T("\\") + key;
    HKEY hKey;
    if (RegCreateKeyEx(HKEY_CURRENT_USER, fullKey, 0, nullptr, REG_OPTION_NON_VOLATILE, 
                       KEY_WRITE, nullptr, &hKey, nullptr) == ERROR_SUCCESS)
    {
        RegSetValueEx(hKey, _T(""), 0, REG_SZ, (const BYTE*)value.GetString(), 
                      (value.GetLength() + 1) * sizeof(TCHAR));
        RegCloseKey(hKey);
    }
}

CString DataListCtrlConfiguration::ReadRegistryValue(const CString& section, const CString& key, const CString& defaultValue)
{
    CString fullKey = _T("Software\\dbWave64\\") + section + _T("\\") + key;
    HKEY hKey;
    if (RegOpenKeyEx(HKEY_CURRENT_USER, fullKey, 0, KEY_READ, &hKey) == ERROR_SUCCESS)
    {
        TCHAR buffer[256];
        DWORD bufferSize = sizeof(buffer);
        DWORD type = REG_SZ;
        if (RegQueryValueEx(hKey, _T(""), nullptr, &type, (BYTE*)buffer, &bufferSize) == ERROR_SUCCESS)
        {
            RegCloseKey(hKey);
            return CString(buffer);
        }
        RegCloseKey(hKey);
    }
    return defaultValue;
}

void DataListCtrlConfiguration::WriteIniValue(const CString& filename, const CString& section, const CString& key, const CString& value)
{
    WritePrivateProfileString(section, key, value, filename);
}

CString DataListCtrlConfiguration::ReadIniValue(const CString& filename, const CString& section, const CString& key, const CString& defaultValue)
{
    TCHAR buffer[256];
    GetPrivateProfileString(section, key, defaultValue, buffer, sizeof(buffer), filename);
    return CString(buffer);
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
