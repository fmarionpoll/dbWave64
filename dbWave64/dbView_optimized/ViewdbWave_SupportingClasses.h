#pragma once

#include <memory>
#include <vector>
#include <string>
#include <afxwin.h>
// Removed: mutex, functional, future, atomic, chrono, map - no longer needed for simplified configuration


// Forward declarations
class CdbWaveDoc;
class CdbWaveApp;
class DataListCtrl_Optimized;
class data_list_ctrl_configuration;
class CdbTableMain;
class CSpikeDoc;
class CTabCtrl;
class CEdit;
class CButton;

// Simplified constants - keeping only essential control IDs
namespace ViewdbWaveConstants
{
    // Essential control IDs (using VW_ prefix to avoid macro conflicts)
    constexpr int VW_IDC_TIMEFIRST = IDC_TIMEFIRST;
    constexpr int VW_IDC_TIMELAST = IDC_TIMELAST;
    constexpr int VW_IDC_AMPLITUDESPAN = IDC_AMPLITUDESPAN;
    constexpr int VW_IDC_SPIKECLASS = IDC_SPIKECLASS;
    constexpr int VW_IDC_DISPLAYDATA = IDC_DISPLAYDATA;
    constexpr int VW_IDC_DISPLAY_SPIKES = IDC_DISPLAY_SPIKES;
    constexpr int VW_IDC_DISPLAY_NOTHING = IDC_DISPLAY_NOTHING;
    constexpr int VW_IDC_CHECKFILENAME = IDC_CHECKFILENAME;
    constexpr int VW_IDC_FILTERCHECK = IDC_FILTERCHECK;
    constexpr int VW_IDC_RADIOALLCLASSES = IDC_RADIOALLCLASSES;
    constexpr int VW_IDC_RADIOONECLASS = IDC_RADIOONECLASS;
    constexpr int VW_IDC_TAB1 = IDC_TAB1;
    constexpr int VW_IDC_LISTCTRL = IDC_LISTCTRL;
    
    // Essential command IDs for message map
    constexpr int ID_VIEW_REFRESH = 32771;  // Custom refresh command
    constexpr int ID_VIEW_AUTO_REFRESH = 32772;  // Custom auto-refresh command
    
    // Removed: Performance settings, UI settings - not needed for core functionality
}

// Simplified error handling - using standard exceptions instead of complex custom exceptions
// Removed complex state management - not needed for single-user database access

// Simplified supporting classes - removed complex state management, performance monitoring, and async operations
// These were over-engineered for single-user database access and made the code harder to debug

// Simplified configuration manager
class ViewdbWaveConfiguration
{
public:
    ViewdbWaveConfiguration();
    ~ViewdbWaveConfiguration() = default;
    
    // Simple load/save operations with basic error handling
    void LoadFromRegistry(const CString& section);
    void SaveToRegistry(const CString& section) const;
    void LoadFromIniFile(const CString& filename, const CString& section);
    void SaveToIniFile(const CString& filename, const CString& section) const;
    
    // Getters and setters for configuration values
    double GetTimeFirst() const { return m_timeFirst; }
    void SetTimeFirst(double value) { m_timeFirst = value; }
    
    double GetTimeLast() const { return m_timeLast; }
    void SetTimeLast(double value) { m_timeLast = value; }
    
    double GetAmplitudeSpan() const { return m_amplitudeSpan; }
    void SetAmplitudeSpan(double value) { m_amplitudeSpan = value; }
    
    bool IsDisplayFileNameEnabled() const { return m_displayFileName; }
    void SetDisplayFileName(bool enabled) { m_displayFileName = enabled; }
    
    bool IsFilterEnabled() const { return m_filterEnabled; }
    void SetFilterEnabled(bool enabled) { m_filterEnabled = enabled; }
    
    // Display mode methods
    int GetDisplayMode() const { return m_displayMode; }
    void SetDisplayMode(int mode) { m_displayMode = mode; }
    
    bool GetFilterData() const { return m_filterEnabled; }
    bool GetDisplayAllClasses() const { return m_displayAllClasses; }
    void SetDisplayAllClasses(bool enabled) { m_displayAllClasses = enabled; }
    
private:
    double m_timeFirst;
    double m_timeLast;
    double m_amplitudeSpan;
    bool m_displayFileName;
    bool m_filterEnabled;
    int m_displayMode;
    bool m_displayAllClasses;
    // Removed: std::mutex m_configMutex - no longer needed for single-user access
    // Removed: bool m_threadSafe - no longer needed
};
