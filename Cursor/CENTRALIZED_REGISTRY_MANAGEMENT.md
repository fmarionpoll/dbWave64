# Centralized Registry Management System for dbWave64

## Overview

A comprehensive centralized registry management system has been implemented for dbWave64 to provide consistent, configurable, and maintainable registry operations across the entire application. This system replaces scattered registry operations with a unified approach that uses the FMP/dbWave64 registry path structure.

## Key Features

### ✅ **Centralized Management**
- **Single Point of Control**: All registry operations go through `RegistryManager`
- **Consistent Path Structure**: Uses `HKEY_CURRENT_USER\Software\FMP\dbWave64\` by default
- **Configurable Base Path**: Easy to change registry location for all components
- **Error Handling**: Comprehensive error handling and reporting

### ✅ **Backward Compatibility**
- **MFC Profile Functions**: Compatible with existing `GetProfileString`, `WriteProfileString`, etc.
- **Private Profile Functions**: Supports INI file operations
- **Direct Registry API**: Full access to Windows Registry API functions
- **Legacy Support**: Existing code continues to work without changes

### ✅ **Advanced Features**
- **Change Notifications**: Callback system for registry changes
- **Migration Tools**: Utilities for moving data between registry paths
- **Enumeration**: List keys and values in registry sections
- **Multiple Data Types**: String, DWORD, Binary, Multi-String support

## Architecture

### Core Components

#### 1. **RegistryManager Class** (`RegistryManager.h/cpp`)
```cpp
class RegistryManager
{
public:
    static RegistryManager& GetInstance();  // Singleton pattern
    
    // Configuration
    void SetBasePath(const CString& basePath);
    CString GetBasePath() const;
    
    // Direct Registry API
    bool WriteStringValue(const CString& section, const CString& key, const CString& value);
    CString ReadStringValue(const CString& section, const CString& key, const CString& defaultValue) const;
    
    // MFC Profile compatibility
    CString GetProfileString(const CString& section, const CString& key, const CString& defaultValue) const;
    bool WriteProfileString(const CString& section, const CString& key, const CString& value);
    
    // Private Profile compatibility
    CString GetPrivateProfileString(const CString& filename, const CString& section, const CString& key, const CString& defaultValue) const;
    bool WritePrivateProfileString(const CString& filename, const CString& section, const CString& key, const CString& value);
};
```

#### 2. **RegistryException Class**
```cpp
class RegistryException : public std::exception
{
public:
    explicit RegistryException(RegistryError error, const CString& message = _T(""));
    RegistryError GetError() const;
    CString GetMessage() const;
};
```

#### 3. **RegistryValue Structure**
```cpp
struct RegistryValue
{
    CString name;
    CString stringValue;
    DWORD dwordValue;
    std::vector<BYTE> binaryValue;
    std::vector<CString> multiStringValue;
    RegistryValueType type;
    bool expandString;
};
```

## Registry Path Structure

### Default Configuration
```
HKEY_CURRENT_USER\
└── Software\
    └── FMP\
        └── dbWave64\
            ├── DataListCtrl_Optimized\
            │   ├── ImageWidth
            │   ├── ImageHeight
            │   ├── ColumnCount
            │   └── Column0_Width
            ├── ViewdbWave\
            │   ├── TimeFirst
            │   ├── TimeLast
            │   └── AmplitudeSpan
            └── Default parameters\
                ├── File1
                ├── File2
                └── File3
```

### Configurable Base Path
The registry base path can be easily changed by modifying the `RegistryManager` configuration:

```cpp
// In application initialization
RegistryManager::GetInstance().SetBasePath(_T("Software\\FMP\\dbWave64\\"));

// Or for different company structure
RegistryManager::GetInstance().SetBasePath(_T("Software\\YourCompany\\dbWave64\\"));

// Or for original structure
RegistryManager::GetInstance().SetBasePath(_T("Software\\dbWave64\\"));
```

## Usage Examples

### 1. **Basic Registry Operations**
```cpp
// Write a string value
RegistryManager::GetInstance().WriteStringValue(_T("MySection"), _T("MyKey"), _T("MyValue"));

// Read a string value
CString value = RegistryManager::GetInstance().ReadStringValue(_T("MySection"), _T("MyKey"), _T("DefaultValue"));

// Write a DWORD value
RegistryManager::GetInstance().WriteDwordValue(_T("MySection"), _T("MyIntKey"), 42);

// Read a DWORD value
DWORD intValue = RegistryManager::GetInstance().ReadDwordValue(_T("MySection"), _T("MyIntKey"), 0);
```

### 2. **MFC Profile Compatibility**
```cpp
// These work exactly like the original MFC functions
CString value = RegistryManager::GetInstance().GetProfileString(_T("Section"), _T("Key"), _T("Default"));
RegistryManager::GetInstance().WriteProfileString(_T("Section"), _T("Key"), _T("Value"));

int intValue = RegistryManager::GetInstance().GetProfileInt(_T("Section"), _T("Key"), 0);
RegistryManager::GetInstance().WriteProfileInt(_T("Section"), _T("Key"), 42);
```

### 3. **Private Profile Compatibility**
```cpp
// INI file operations
CString value = RegistryManager::GetInstance().GetPrivateProfileString(
    _T("config.ini"), _T("Section"), _T("Key"), _T("Default"));

RegistryManager::GetInstance().WritePrivateProfileString(
    _T("config.ini"), _T("Section"), _T("Key"), _T("Value"));
```

### 4. **Advanced Operations**
```cpp
// Check if key exists
bool exists = RegistryManager::GetInstance().KeyExists(_T("Section"), _T("Key"));

// Delete a value
RegistryManager::GetInstance().DeleteValue(_T("Section"), _T("Key"));

// Enumerate keys in a section
std::vector<CString> keys = RegistryManager::GetInstance().EnumKeys(_T("Section"));

// Enumerate values in a section
std::vector<RegistryValue> values = RegistryManager::GetInstance().EnumValues(_T("Section"));
```

### 5. **Change Notifications**
```cpp
// Register a callback for registry changes
RegistryManager::GetInstance().RegisterChangeCallback([](const CString& section, const CString& key, const CString& value) {
    // Handle registry change
    TRACE(_T("Registry changed: %s\\%s = %s\n"), section, key, value);
});
```

## Migration from Existing Code

### Before (Scattered Registry Operations)
```cpp
// Direct registry API calls
CString fullKey = _T("Software\\dbWave64\\") + section + _T("\\") + key;
HKEY hKey;
if (RegCreateKeyEx(HKEY_CURRENT_USER, fullKey, 0, nullptr, REG_OPTION_NON_VOLATILE, 
                   KEY_WRITE, nullptr, &hKey, nullptr) == ERROR_SUCCESS)
{
    RegSetValueEx(hKey, _T(""), 0, REG_SZ, (const BYTE*)value.GetString(), 
                  (value.GetLength() + 1) * sizeof(TCHAR));
    RegCloseKey(hKey);
}

// MFC Profile calls
CWinApp* pApp = AfxGetApp();
CString value = pApp->GetProfileString(section, key, defaultValue);
pApp->WriteProfileString(section, key, value);
```

### After (Centralized Registry Management)
```cpp
// All operations go through RegistryManager
RegistryManager::GetInstance().WriteStringValue(section, key, value);
CString value = RegistryManager::GetInstance().ReadStringValue(section, key, defaultValue);

// MFC Profile compatibility maintained
CString value = RegistryManager::GetInstance().GetProfileString(section, key, defaultValue);
RegistryManager::GetInstance().WriteProfileString(section, key, value);
```

## Files Modified

### 1. **New Files Created**
- `RegistryManager.h` - Header file for the registry management system
- `RegistryManager.cpp` - Implementation of the registry management system

### 2. **Files Updated**
- `DataListCtrl_Configuration.h/cpp` - Updated to use centralized registry manager
- `ViewdbWave_SupportingClasses.cpp` - Updated to use centralized registry manager
- `dbWave.cpp` - Updated to use centralized registry manager

### 3. **Legacy Compatibility Functions**
```cpp
// These functions provide backward compatibility
inline CString ReadRegistryValue(const CString& section, const CString& key, const CString& defaultValue = _T(""))
{
    return RegistryManager::GetInstance().ReadStringValue(section, key, defaultValue);
}

inline bool WriteRegistryValue(const CString& section, const CString& key, const CString& value)
{
    return RegistryManager::GetInstance().WriteStringValue(section, key, value);
}
```

## Benefits

### ✅ **Consistency**
- All registry operations use the same path structure
- Consistent error handling across the application
- Uniform API for different types of registry data

### ✅ **Maintainability**
- Single point of control for registry operations
- Easy to modify registry path for entire application
- Centralized logging and error reporting

### ✅ **Flexibility**
- Easy to switch between different registry paths
- Support for multiple data types
- Migration utilities for data transfer

### ✅ **Reliability**
- Comprehensive error handling
- Automatic key creation
- Safe operations with proper cleanup

### ✅ **Performance**
- Efficient singleton pattern
- Minimal overhead for registry operations
- Optimized for common use cases

## Configuration Options

### Registry Path Configuration
```cpp
// Default FMP/dbWave64 structure (recommended)
RegistryManager::GetInstance().SetBasePath(_T("Software\\FMP\\dbWave64\\"));

// Original dbWave64 structure
RegistryManager::GetInstance().SetBasePath(_T("Software\\dbWave64\\"));

// Custom company structure
RegistryManager::GetInstance().SetBasePath(_T("Software\\YourCompany\\dbWave64\\"));

// Custom application structure
RegistryManager::GetInstance().SetBasePath(_T("Software\\MyApp\\Settings\\"));
```

### Error Handling Configuration
```cpp
// Check for errors after operations
if (RegistryManager::GetInstance().GetLastError() != RegistryError::SUCCESS)
{
    CString errorMsg = RegistryManager::GetInstance().GetLastErrorMessage();
    // Handle error
}
```

## Future Enhancements

### Planned Features
1. **Registry Monitoring**: Real-time monitoring of registry changes
2. **Backup/Restore**: Automatic backup and restore of registry sections
3. **Encryption**: Optional encryption of sensitive registry data
4. **Network Registry**: Support for remote registry access
5. **Performance Monitoring**: Registry operation performance metrics

### Extension Points
1. **Custom Data Types**: Support for custom serialization
2. **Validation**: Input validation for registry values
3. **Caching**: Registry value caching for performance
4. **Compression**: Optional compression of large registry data

## Summary

The centralized registry management system provides a robust, maintainable, and flexible solution for all registry operations in dbWave64. It maintains backward compatibility while providing modern features and consistent behavior across the entire application. The FMP/dbWave64 registry path structure ensures proper organization and easy identification of application settings.
