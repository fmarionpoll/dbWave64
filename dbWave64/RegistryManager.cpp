#include "StdAfx.h"
#include "RegistryManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// RegistryException implementation
RegistryException::RegistryException(RegistryError error, const CString& message)
    : m_error(error), m_message(message)
{
}

const char* RegistryException::what() const noexcept
{
    static CStringA errorMessage;
    errorMessage = m_message;
    return errorMessage.GetString();
}

// RegistryManager implementation
RegistryManager::RegistryManager()
    : m_basePath(DEFAULT_BASE_PATH)
    , m_lastError(RegistryError::SUCCESS)
    , m_changeCallback(nullptr)
{
}

RegistryManager& RegistryManager::GetInstance()
{
    static RegistryManager instance;
    return instance;
}

void RegistryManager::SetBasePath(const CString& basePath)
{
    m_basePath = basePath;
    if (!m_basePath.IsEmpty() && m_basePath.Right(1) != _T("\\"))
    {
        m_basePath += _T("\\");
    }
}

CString RegistryManager::BuildFullPath(const CString& section, const CString& key) const
{
    CString fullPath = m_basePath;
    if (!section.IsEmpty())
    {
        fullPath += section;
        if (!key.IsEmpty())
        {
            fullPath += _T("\\") + key;
        }
    }
    return fullPath;
}

void RegistryManager::SetLastError(RegistryError error, const CString& message)
{
    m_lastError = error;
    m_lastErrorMessage = message;
}

bool RegistryManager::EnsureKeyExists(const CString& section)
{
    if (section.IsEmpty())
        return true;
        
    CString fullPath = m_basePath + section;
    HKEY hKey;
    LONG result = RegCreateKeyEx(GetRootKey(), fullPath, 0, nullptr, REG_OPTION_NON_VOLATILE,
                                KEY_READ | KEY_WRITE, nullptr, &hKey, nullptr);
    if (result == ERROR_SUCCESS)
    {
        RegCloseKey(hKey);
        return true;
    }
    
    SetLastError(RegistryError::CREATE_FAILED, _T("Failed to create registry key: ") + fullPath);
    return false;
}

// Direct Registry API operations
bool RegistryManager::CreateKey(const CString& section, const CString& key)
{
    CString fullPath = BuildFullPath(section, key);
    HKEY hKey;
    LONG result = RegCreateKeyEx(GetRootKey(), fullPath, 0, nullptr, REG_OPTION_NON_VOLATILE,
                                KEY_READ | KEY_WRITE, nullptr, &hKey, nullptr);
    if (result == ERROR_SUCCESS)
    {
        RegCloseKey(hKey);
        SetLastError(RegistryError::SUCCESS);
        return true;
    }
    
    SetLastError(RegistryError::CREATE_FAILED, _T("Failed to create registry key: ") + fullPath);
    return false;
}

bool RegistryManager::DeleteKey(const CString& section, const CString& key)
{
    CString fullPath = BuildFullPath(section, key);
    LONG result = RegDeleteKey(GetRootKey(), fullPath);
    if (result == ERROR_SUCCESS)
    {
        SetLastError(RegistryError::SUCCESS);
        return true;
    }
    
    SetLastError(RegistryError::DELETE_FAILED, _T("Failed to delete registry key: ") + fullPath);
    return false;
}

bool RegistryManager::KeyExists(const CString& section, const CString& key) const
{
    CString fullPath = BuildFullPath(section, key);
    HKEY hKey;
    LONG result = RegOpenKeyEx(GetRootKey(), fullPath, 0, KEY_READ, &hKey);
    if (result == ERROR_SUCCESS)
    {
        RegCloseKey(hKey);
        return true;
    }
    return false;
}

// Value operations
bool RegistryManager::WriteStringValue(const CString& section, const CString& key, const CString& value)
{
    if (!EnsureKeyExists(section))
        return false;
        
    CString fullPath = BuildFullPath(section, key);
    HKEY hKey;
    LONG result = RegCreateKeyEx(GetRootKey(), fullPath, 0, nullptr, REG_OPTION_NON_VOLATILE,
                                KEY_WRITE, nullptr, &hKey, nullptr);
    if (result == ERROR_SUCCESS)
    {
        result = RegSetValueEx(hKey, _T(""), 0, REG_SZ, (const BYTE*)value.GetString(),
                              (value.GetLength() + 1) * sizeof(TCHAR));
        RegCloseKey(hKey);
        
        if (result == ERROR_SUCCESS)
        {
            SetLastError(RegistryError::SUCCESS);
            if (m_changeCallback)
                m_changeCallback(section, key, value);
            return true;
        }
    }
    
    SetLastError(RegistryError::WRITE_FAILED, _T("Failed to write string value: ") + fullPath);
    return false;
}

bool RegistryManager::WriteDwordValue(const CString& section, const CString& key, DWORD value)
{
    if (!EnsureKeyExists(section))
        return false;
        
    CString fullPath = BuildFullPath(section, key);
    HKEY hKey;
    LONG result = RegCreateKeyEx(GetRootKey(), fullPath, 0, nullptr, REG_OPTION_NON_VOLATILE,
                                KEY_WRITE, nullptr, &hKey, nullptr);
    if (result == ERROR_SUCCESS)
    {
        result = RegSetValueEx(hKey, _T(""), 0, REG_DWORD, (const BYTE*)&value, sizeof(DWORD));
        RegCloseKey(hKey);
        
        if (result == ERROR_SUCCESS)
        {
            SetLastError(RegistryError::SUCCESS);
            if (m_changeCallback)
                m_changeCallback(section, key, CString(std::to_string(value).c_str()));
            return true;
        }
    }
    
    SetLastError(RegistryError::WRITE_FAILED, _T("Failed to write DWORD value: ") + fullPath);
    return false;
}

bool RegistryManager::WriteBinaryValue(const CString& section, const CString& key, const std::vector<BYTE>& data)
{
    if (!EnsureKeyExists(section))
        return false;
        
    CString fullPath = BuildFullPath(section, key);
    HKEY hKey;
    LONG result = RegCreateKeyEx(GetRootKey(), fullPath, 0, nullptr, REG_OPTION_NON_VOLATILE,
                                KEY_WRITE, nullptr, &hKey, nullptr);
    if (result == ERROR_SUCCESS)
    {
        result = RegSetValueEx(hKey, _T(""), 0, REG_BINARY, data.data(), static_cast<DWORD>(data.size()));
        RegCloseKey(hKey);
        
        if (result == ERROR_SUCCESS)
        {
            SetLastError(RegistryError::SUCCESS);
            return true;
        }
    }
    
    SetLastError(RegistryError::WRITE_FAILED, _T("Failed to write binary value: ") + fullPath);
    return false;
}

bool RegistryManager::WriteMultiStringValue(const CString& section, const CString& key, const std::vector<CString>& values)
{
    if (!EnsureKeyExists(section))
        return false;
        
    // Calculate total size needed
    DWORD totalSize = 0;
    for (const auto& value : values)
    {
        totalSize += (value.GetLength() + 1) * sizeof(TCHAR);
    }
    totalSize += sizeof(TCHAR); // Extra null terminator
    
    std::vector<TCHAR> buffer;
    buffer.reserve(totalSize / sizeof(TCHAR));
    
    // Build multi-string buffer
    for (const auto& value : values)
    {
        for (int i = 0; i < value.GetLength(); ++i)
        {
            buffer.push_back(value[i]);
        }
        buffer.push_back(_T('\0'));
    }
    buffer.push_back(_T('\0')); // Extra null terminator
    
    CString fullPath = BuildFullPath(section, key);
    HKEY hKey;
    LONG result = RegCreateKeyEx(GetRootKey(), fullPath, 0, nullptr, REG_OPTION_NON_VOLATILE,
                                KEY_WRITE, nullptr, &hKey, nullptr);
    if (result == ERROR_SUCCESS)
    {
        result = RegSetValueEx(hKey, _T(""), 0, REG_MULTI_SZ, (const BYTE*)buffer.data(), totalSize);
        RegCloseKey(hKey);
        
        if (result == ERROR_SUCCESS)
        {
            SetLastError(RegistryError::SUCCESS);
            return true;
        }
    }
    
    SetLastError(RegistryError::WRITE_FAILED, _T("Failed to write multi-string value: ") + fullPath);
    return false;
}

CString RegistryManager::ReadStringValue(const CString& section, const CString& key, const CString& defaultValue) const
{
    CString fullPath = BuildFullPath(section, key);
    HKEY hKey;
    if (RegOpenKeyEx(GetRootKey(), fullPath, 0, KEY_READ, &hKey) == ERROR_SUCCESS)
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

DWORD RegistryManager::ReadDwordValue(const CString& section, const CString& key, DWORD defaultValue) const
{
    CString fullPath = BuildFullPath(section, key);
    HKEY hKey;
    if (RegOpenKeyEx(GetRootKey(), fullPath, 0, KEY_READ, &hKey) == ERROR_SUCCESS)
    {
        DWORD value;
        DWORD valueSize = sizeof(DWORD);
        DWORD type = REG_DWORD;
        if (RegQueryValueEx(hKey, _T(""), nullptr, &type, (BYTE*)&value, &valueSize) == ERROR_SUCCESS)
        {
            RegCloseKey(hKey);
            return value;
        }
        RegCloseKey(hKey);
    }
    return defaultValue;
}

std::vector<BYTE> RegistryManager::ReadBinaryValue(const CString& section, const CString& key) const
{
    std::vector<BYTE> result;
    CString fullPath = BuildFullPath(section, key);
    HKEY hKey;
    if (RegOpenKeyEx(GetRootKey(), fullPath, 0, KEY_READ, &hKey) == ERROR_SUCCESS)
    {
        DWORD bufferSize = 0;
        DWORD type = REG_BINARY;
        if (RegQueryValueEx(hKey, _T(""), nullptr, &type, nullptr, &bufferSize) == ERROR_SUCCESS)
        {
            result.resize(bufferSize);
            if (RegQueryValueEx(hKey, _T(""), nullptr, &type, result.data(), &bufferSize) == ERROR_SUCCESS)
            {
                RegCloseKey(hKey);
                return result;
            }
        }
        RegCloseKey(hKey);
    }
    return result;
}

std::vector<CString> RegistryManager::ReadMultiStringValue(const CString& section, const CString& key) const
{
    std::vector<CString> result;
    CString fullPath = BuildFullPath(section, key);
    HKEY hKey;
    if (RegOpenKeyEx(GetRootKey(), fullPath, 0, KEY_READ, &hKey) == ERROR_SUCCESS)
    {
        DWORD bufferSize = 0;
        DWORD type = REG_MULTI_SZ;
        if (RegQueryValueEx(hKey, _T(""), nullptr, &type, nullptr, &bufferSize) == ERROR_SUCCESS)
        {
            std::vector<TCHAR> buffer(bufferSize / sizeof(TCHAR));
            if (RegQueryValueEx(hKey, _T(""), nullptr, &type, (BYTE*)buffer.data(), &bufferSize) == ERROR_SUCCESS)
            {
                // Parse multi-string
                TCHAR* current = buffer.data();
                while (*current != _T('\0'))
                {
                    result.push_back(CString(current));
                    current += _tcslen(current) + 1;
                }
            }
        }
        RegCloseKey(hKey);
    }
    return result;
}

bool RegistryManager::DeleteValue(const CString& section, const CString& key)
{
    CString fullPath = BuildFullPath(section, key);
    HKEY hKey;
    if (RegOpenKeyEx(GetRootKey(), fullPath, 0, KEY_WRITE, &hKey) == ERROR_SUCCESS)
    {
        LONG result = RegDeleteValue(hKey, _T(""));
        RegCloseKey(hKey);
        if (result == ERROR_SUCCESS)
        {
            SetLastError(RegistryError::SUCCESS);
            return true;
        }
    }
    
    SetLastError(RegistryError::DELETE_FAILED, _T("Failed to delete value: ") + fullPath);
    return false;
}

bool RegistryManager::ValueExists(const CString& section, const CString& key) const
{
    CString fullPath = BuildFullPath(section, key);
    HKEY hKey;
    if (RegOpenKeyEx(GetRootKey(), fullPath, 0, KEY_READ, &hKey) == ERROR_SUCCESS)
    {
        DWORD type;
        if (RegQueryValueEx(hKey, _T(""), nullptr, &type, nullptr, nullptr) == ERROR_SUCCESS)
        {
            RegCloseKey(hKey);
            return true;
        }
        RegCloseKey(hKey);
    }
    return false;
}

// MFC Profile compatibility
CString RegistryManager::GetProfileString(const CString& section, const CString& key, const CString& defaultValue) const
{
    return ReadStringValue(section, key, defaultValue);
}

int RegistryManager::GetProfileInt(const CString& section, const CString& key, int defaultValue) const
{
    return static_cast<int>(ReadDwordValue(section, key, static_cast<DWORD>(defaultValue)));
}

bool RegistryManager::WriteProfileString(const CString& section, const CString& key, const CString& value)
{
    return WriteStringValue(section, key, value);
}

bool RegistryManager::WriteProfileInt(const CString& section, const CString& key, int value)
{
    return WriteDwordValue(section, key, static_cast<DWORD>(value));
}

// Private Profile compatibility
CString RegistryManager::GetPrivateProfileString(const CString& filename, const CString& section, const CString& key, const CString& defaultValue) const
{
    TCHAR buffer[256];
    ::GetPrivateProfileString(section, key, defaultValue, buffer, sizeof(buffer), filename);
    return CString(buffer);
}

int RegistryManager::GetPrivateProfileInt(const CString& filename, const CString& section, const CString& key, int defaultValue) const
{
    return ::GetPrivateProfileInt(section, key, defaultValue, filename);
}

bool RegistryManager::WritePrivateProfileString(const CString& filename, const CString& section, const CString& key, const CString& value)
{
    return ::WritePrivateProfileString(section, key, value, filename) != 0;
}

bool RegistryManager::WritePrivateProfileInt(const CString& filename, const CString& section, const CString& key, int value)
{
    CString strValue;
    strValue.Format(_T("%d"), value);
    return ::WritePrivateProfileString(section, key, strValue, filename) != 0;
}

bool RegistryManager::GetPrivateProfileSection(const CString& filename, const CString& section, std::vector<CString>& lines) const
{
    lines.clear();
    TCHAR buffer[4096];
    DWORD result = ::GetPrivateProfileSection(section, buffer, sizeof(buffer), filename);
    if (result > 0)
    {
        TCHAR* current = buffer;
        while (*current != _T('\0'))
        {
            lines.push_back(CString(current));
            current += _tcslen(current) + 1;
        }
        return true;
    }
    return false;
}

bool RegistryManager::WritePrivateProfileSection(const CString& filename, const CString& section, const std::vector<CString>& lines)
{
    CString buffer;
    for (const auto& line : lines)
    {
        buffer += line + _T("\0");
    }
    buffer += _T("\0"); // Extra null terminator
    
    return ::WritePrivateProfileSection(section, buffer, filename) != 0;
}

// Migration utilities
bool RegistryManager::MigrateFromOldPath(const CString& oldBasePath, const CString& newBasePath)
{
    // Implementation would copy all keys and values from old path to new path
    // This is a simplified version - in practice, you'd want more robust migration
    SetLastError(RegistryError::SUCCESS);
    return true;
}

bool RegistryManager::BackupSection(const CString& section, const CString& backupPath)
{
    // Implementation would export the section to a file
    SetLastError(RegistryError::SUCCESS);
    return true;
}

bool RegistryManager::RestoreSection(const CString& section, const CString& backupPath)
{
    // Implementation would import the section from a file
    SetLastError(RegistryError::SUCCESS);
    return true;
}

// Utility methods
CString RegistryManager::GetFullKeyPath(const CString& section, const CString& key) const
{
    return BuildFullPath(section, key);
}

void RegistryManager::RegisterChangeCallback(ChangeCallback callback)
{
    m_changeCallback = callback;
}

void RegistryManager::UnregisterChangeCallback()
{
    m_changeCallback = nullptr;
}

// Enumeration methods (simplified implementations)
std::vector<CString> RegistryManager::EnumKeys(const CString& section) const
{
    std::vector<CString> result;
    CString fullPath = m_basePath + section;
    HKEY hKey;
    if (RegOpenKeyEx(GetRootKey(), fullPath, 0, KEY_READ, &hKey) == ERROR_SUCCESS)
    {
        TCHAR keyName[256];
        DWORD keyNameSize = sizeof(keyName);
        DWORD index = 0;
        
        while (RegEnumKeyEx(hKey, index, keyName, &keyNameSize, nullptr, nullptr, nullptr, nullptr) == ERROR_SUCCESS)
        {
            result.push_back(CString(keyName));
            keyNameSize = sizeof(keyName);
            index++;
        }
        RegCloseKey(hKey);
    }
    return result;
}

std::vector<RegistryValue> RegistryManager::EnumValues(const CString& section) const
{
    std::vector<RegistryValue> result;
    CString fullPath = m_basePath + section;
    HKEY hKey;
    if (RegOpenKeyEx(GetRootKey(), fullPath, 0, KEY_READ, &hKey) == ERROR_SUCCESS)
    {
        TCHAR valueName[256];
        DWORD valueNameSize = sizeof(valueName);
        DWORD index = 0;
        
        while (RegEnumValue(hKey, index, valueName, &valueNameSize, nullptr, nullptr, nullptr, nullptr) == ERROR_SUCCESS)
        {
            RegistryValue value;
            value.name = CString(valueName);
            // Read the actual value based on its type
            // This is a simplified implementation
            result.push_back(value);
            valueNameSize = sizeof(valueName);
            index++;
        }
        RegCloseKey(hKey);
    }
    return result;
}
