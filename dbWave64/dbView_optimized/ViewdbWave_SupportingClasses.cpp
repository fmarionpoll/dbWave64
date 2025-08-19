#include "stdafx.h"
#include "ViewdbWave_SupportingClasses.h"
#include "RegistryManager.h"
#include "DataListCtrl_Configuration.h"
#include <sstream>

// Simplified implementation - removed complex exception handling, state management, performance monitoring

// ViewdbWaveConfiguration implementation
ViewdbWaveConfiguration::ViewdbWaveConfiguration()
    : m_timeFirst(0.0)
    , m_timeLast(100.0)
    , m_amplitudeSpan(1.0)
    , m_displayFileName(false)
    , m_filterEnabled(false)
    , m_displayMode(DataListCtrlConfigConstants::DISPLAY_MODE_EMPTY)
    , m_displayAllClasses(true)
{
}

void ViewdbWaveConfiguration::LoadFromRegistry(const CString& section)
{
    try
    {
        // Simple registry loading with basic error handling
        CString appName = AfxGetApp()->m_pszAppName;
        
        m_timeFirst = AfxGetApp()->GetProfileDouble(section, _T("TimeFirst"), 0.0);
        m_timeLast = AfxGetApp()->GetProfileDouble(section, _T("TimeLast"), 100.0);
        m_amplitudeSpan = AfxGetApp()->GetProfileDouble(section, _T("AmplitudeSpan"), 1.0);
        m_displayFileName = AfxGetApp()->GetProfileInt(section, _T("DisplayFileName"), 0) != 0;
        m_filterEnabled = AfxGetApp()->GetProfileInt(section, _T("FilterEnabled"), 0) != 0;
        m_displayMode = AfxGetApp()->GetProfileInt(section, _T("DisplayMode"), DataListCtrlConfigConstants::DISPLAY_MODE_EMPTY);
        m_displayAllClasses = AfxGetApp()->GetProfileInt(section, _T("DisplayAllClasses"), 1) != 0;
    }
    catch (const std::exception& e)
    {
        // Simple error handling - just use defaults
        TRACE(_T("ViewdbWaveConfiguration::LoadFromRegistry - Error loading from registry: %s\n"), CString(e.what()));
    }
}

void ViewdbWaveConfiguration::SaveToRegistry(const CString& section) const
{
    try
    {
        // Simple registry saving with basic error handling
        CString appName = AfxGetApp()->m_pszAppName;
        
        AfxGetApp()->WriteProfileDouble(section, _T("TimeFirst"), m_timeFirst);
        AfxGetApp()->WriteProfileDouble(section, _T("TimeLast"), m_timeLast);
        AfxGetApp()->WriteProfileDouble(section, _T("AmplitudeSpan"), m_amplitudeSpan);
        AfxGetApp()->WriteProfileInt(section, _T("DisplayFileName"), m_displayFileName ? 1 : 0);
        AfxGetApp()->WriteProfileInt(section, _T("FilterEnabled"), m_filterEnabled ? 1 : 0);
        AfxGetApp()->WriteProfileInt(section, _T("DisplayMode"), m_displayMode);
        AfxGetApp()->WriteProfileInt(section, _T("DisplayAllClasses"), m_displayAllClasses ? 1 : 0);
    }
    catch (const std::exception& e)
    {
        // Simple error handling - just log the error
        TRACE(_T("ViewdbWaveConfiguration::SaveToRegistry - Error saving to registry: %s\n"), CString(e.what()));
    }
}

void ViewdbWaveConfiguration::LoadFromIniFile(const CString& filename, const CString& section)
{
    try
    {
        // Simple INI file loading with basic error handling
        m_timeFirst = GetPrivateProfileDouble(section, _T("TimeFirst"), 0.0, filename);
        m_timeLast = GetPrivateProfileDouble(section, _T("TimeLast"), 100.0, filename);
        m_amplitudeSpan = GetPrivateProfileDouble(section, _T("AmplitudeSpan"), 1.0, filename);
        m_displayFileName = GetPrivateProfileInt(section, _T("DisplayFileName"), 0, filename) != 0;
        m_filterEnabled = GetPrivateProfileInt(section, _T("FilterEnabled"), 0, filename) != 0;
        m_displayMode = GetPrivateProfileInt(section, _T("DisplayMode"), DataListCtrlConfigConstants::DISPLAY_MODE_EMPTY, filename);
        m_displayAllClasses = GetPrivateProfileInt(section, _T("DisplayAllClasses"), 1, filename) != 0;
    }
    catch (const std::exception& e)
    {
        // Simple error handling - just use defaults
        TRACE(_T("ViewdbWaveConfiguration::LoadFromIniFile - Error loading from INI file: %s\n"), CString(e.what()));
    }
}

void ViewdbWaveConfiguration::SaveToIniFile(const CString& filename, const CString& section) const
{
    try
    {
        // Simple INI file saving with basic error handling
        CString timeFirstStr, timeLastStr, amplitudeSpanStr;
        timeFirstStr.Format(_T("%.6f"), m_timeFirst);
        timeLastStr.Format(_T("%.6f"), m_timeLast);
        amplitudeSpanStr.Format(_T("%.6f"), m_amplitudeSpan);
        
        WritePrivateProfileString(section, _T("TimeFirst"), timeFirstStr, filename);
        WritePrivateProfileString(section, _T("TimeLast"), timeLastStr, filename);
        WritePrivateProfileString(section, _T("AmplitudeSpan"), amplitudeSpanStr, filename);
        WritePrivateProfileString(section, _T("DisplayFileName"), m_displayFileName ? _T("1") : _T("0"), filename);
        WritePrivateProfileString(section, _T("FilterEnabled"), m_filterEnabled ? _T("1") : _T("0"), filename);
        
        CString displayModeStr;
        displayModeStr.Format(_T("%d"), m_displayMode);
        WritePrivateProfileString(section, _T("DisplayMode"), displayModeStr, filename);
        WritePrivateProfileString(section, _T("DisplayAllClasses"), m_displayAllClasses ? _T("1") : _T("0"), filename);
    }
    catch (const std::exception& e)
    {
        // Simple error handling - just log the error
        TRACE(_T("ViewdbWaveConfiguration::SaveToIniFile - Error saving to INI file: %s\n"), CString(e.what()));
    }
}

// Helper function for reading double values from INI files
double GetPrivateProfileDouble(LPCTSTR lpAppName, LPCTSTR lpKeyName, double nDefault, LPCTSTR lpFileName)
{
    TCHAR szBuffer[256];
    DWORD dwResult = GetPrivateProfileString(lpAppName, lpKeyName, _T(""), szBuffer, sizeof(szBuffer), lpFileName);
    
    if (dwResult > 0)
    {
        return _ttof(szBuffer);
    }
    
    return nDefault;
}
