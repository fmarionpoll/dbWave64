#pragma once

// MFC includes
#include <afx.h>

// Standard library includes
#include <vector>
#include <functional>

// Forward declarations
class CWinApp;

// Registry error handling
enum class RegistryError
{
    SUCCESS,
    INVALID_PARAMETER,
    ACCESS_DENIED,
    KEY_NOT_FOUND,
    VALUE_NOT_FOUND,
    WRITE_FAILED,
    READ_FAILED,
    DELETE_FAILED,
    CREATE_FAILED,
    UNKNOWN_ERROR
};

class RegistryException : public std::exception
{
public:
    explicit RegistryException(RegistryError error, const CString& message = _T(""));
    RegistryError GetError() const { return m_error; }
    CString GetMessage() const { return m_message; }
    const char* what() const noexcept override;
    
private:
    RegistryError m_error;
    CString m_message;
};

// Registry value types
enum class RegistryValueType
{
    STRING,
    DWORD,
    BINARY,
    MULTI_STRING,
    EXPAND_STRING
};

// Registry value structure
struct RegistryValue
{
    CString name;
    CString stringValue;
    DWORD dwordValue;
    std::vector<BYTE> binaryValue;
    std::vector<CString> multiStringValue;
    RegistryValueType type;
    bool expandString;
    
    RegistryValue() : dwordValue(0), type(RegistryValueType::STRING), expandString(false) {}
    RegistryValue(const CString& name, const CString& value) 
        : name(name), stringValue(value), dwordValue(0), type(RegistryValueType::STRING), expandString(false) {}
    RegistryValue(const CString& name, DWORD value) 
        : name(name), dwordValue(value), type(RegistryValueType::DWORD), expandString(false) {}
};

// Main registry manager class
class RegistryManager
{
public:
    // Singleton pattern
    static RegistryManager& GetInstance();
    
    // Prevent copying
    RegistryManager(const RegistryManager&) = delete;
    RegistryManager& operator=(const RegistryManager&) = delete;
    
    // Configuration
    void SetBasePath(const CString& basePath);
    CString GetBasePath() const { return m_basePath; }
    
    // Registry operations - Direct API
    bool CreateKey(const CString& section, const CString& key);
    bool DeleteKey(const CString& section, const CString& key);
    bool KeyExists(const CString& section, const CString& key) const;
    
    // Value operations - Direct API
    bool WriteStringValue(const CString& section, const CString& key, const CString& value);
    bool WriteDwordValue(const CString& section, const CString& key, DWORD value);
    bool WriteBinaryValue(const CString& section, const CString& key, const std::vector<BYTE>& data);
    bool WriteMultiStringValue(const CString& section, const CString& key, const std::vector<CString>& values);
    
    CString ReadStringValue(const CString& section, const CString& key, const CString& defaultValue = _T("")) const;
    DWORD ReadDwordValue(const CString& section, const CString& key, DWORD defaultValue = 0) const;
    std::vector<BYTE> ReadBinaryValue(const CString& section, const CString& key) const;
    std::vector<CString> ReadMultiStringValue(const CString& section, const CString& key) const;
    
    bool DeleteValue(const CString& section, const CString& key);
    bool ValueExists(const CString& section, const CString& key) const;
    
    // Enumeration
    std::vector<CString> EnumKeys(const CString& section) const;
    std::vector<RegistryValue> EnumValues(const CString& section) const;
    
    // MFC Profile compatibility
    CString GetProfileString(const CString& section, const CString& key, const CString& defaultValue = _T("")) const;
    int GetProfileInt(const CString& section, const CString& key, int defaultValue = 0) const;
    bool WriteProfileString(const CString& section, const CString& key, const CString& value);
    bool WriteProfileInt(const CString& section, const CString& key, int value);
    
    // Private Profile compatibility
    static CString GetPrivateProfileString(const CString& filename, const CString& section, const CString& key, const CString& defaultValue = _T(""));
    static int GetPrivateProfileInt(const CString& filename, const CString& section, const CString& key, int defaultValue = 0);
    static bool WritePrivateProfileString(const CString& filename, const CString& section, const CString& key, const CString& value);
    bool WritePrivateProfileInt(const CString& filename, const CString& section, const CString& key, int value);
    
    // Section operations
    bool GetPrivateProfileSection(const CString& filename, const CString& section, std::vector<CString>& lines) const;
    bool WritePrivateProfileSection(const CString& filename, const CString& section, const std::vector<CString>& lines);
    
    // Migration utilities
    bool MigrateFromOldPath(const CString& oldBasePath, const CString& newBasePath);
    bool BackupSection(const CString& section, const CString& backupPath);
    bool RestoreSection(const CString& section, const CString& backupPath);
    
    // Utility methods
    CString GetFullKeyPath(const CString& section, const CString& key) const;
    RegistryError GetLastError() const { return m_lastError; }
    CString GetLastErrorMessage() const { return m_lastErrorMessage; }
    
    // Change notification
    using ChangeCallback = std::function<void(const CString& section, const CString& key, const CString& value)>;
    void RegisterChangeCallback(ChangeCallback callback);
    void UnregisterChangeCallback();
    
private:
    RegistryManager();
    ~RegistryManager() = default;
    
    // Helper methods
    static HKEY GetRootKey() { return HKEY_CURRENT_USER; }
    CString BuildFullPath(const CString& section, const CString& key) const;
    void SetLastError(RegistryError error, const CString& message = _T(""));
    bool EnsureKeyExists(const CString& section);
    
    // Member variables
    CString m_basePath;
    RegistryError m_lastError;
    CString m_lastErrorMessage;
    ChangeCallback m_changeCallback;
    
    // Constants
    static constexpr const TCHAR* DEFAULT_BASE_PATH = _T("Software\\FMP\\dbWave64\\");
    static constexpr const TCHAR* LEGACY_BASE_PATH = _T("Software\\dbWave64\\");
};

// Convenience macros for backward compatibility
#define REGISTRY_MANAGER RegistryManager::GetInstance()

// Legacy compatibility functions (for existing code)
inline CString ReadRegistryValue(const CString& section, const CString& key, const CString& defaultValue = _T(""))
{
    return RegistryManager::GetInstance().ReadStringValue(section, key, defaultValue);
}

inline bool WriteRegistryValue(const CString& section, const CString& key, const CString& value)
{
    return RegistryManager::GetInstance().WriteStringValue(section, key, value);
}

inline int ReadRegistryInt(const CString& section, const CString& key, int defaultValue = 0)
{
    return static_cast<int>(RegistryManager::GetInstance().ReadDwordValue(section, key, static_cast<DWORD>(defaultValue)));
}

inline bool WriteRegistryInt(const CString& section, const CString& key, int value)
{
    return RegistryManager::GetInstance().WriteDwordValue(section, key, static_cast<DWORD>(value));
}
