# Registry Path Configuration for dbWave64

## Overview

The registry path configuration has been updated to store dbWave64 data under the `FMP/dbWave64` structure, matching the installation directory structure in Program Files.

## Current Configuration

### Registry Path Structure

**Before**: `HKEY_CURRENT_USER\Software\dbWave64\`
**After**: `HKEY_CURRENT_USER\Software\FMP\dbWave64\`

### Complete Registry Path Example

```
HKEY_CURRENT_USER\
└── Software\
    └── FMP\
        └── dbWave64\
            └── DataListCtrl_Optimized\
                ├── ImageWidth
                ├── ImageHeight
                ├── DisplayMode
                ├── TimeFirst
                ├── TimeLast
                ├── MvSpan
                ├── ColumnCount
                ├── Column0_Width
                ├── Column0_Header
                ├── Column1_Width
                ├── Column1_Header
                └── ... (additional columns)
```

## Configuration Options

### 1. **FMP/dbWave64 Structure (Current - Recommended)**
```cpp
constexpr const TCHAR* REGISTRY_BASE_PATH = _T("Software\\FMP\\dbWave64\\");
```
**Registry Path**: `HKEY_CURRENT_USER\Software\FMP\dbWave64\`

**Benefits**:
- Matches installation directory structure (`Program Files\FMP\dbWave64\`)
- Clear separation from other applications
- Organized under FMP company namespace
- Easy to identify and manage

### 2. **Original Structure**
```cpp
constexpr const TCHAR* REGISTRY_BASE_PATH = _T("Software\\dbWave64\\");
```
**Registry Path**: `HKEY_CURRENT_USER\Software\dbWave64\`

**Use Case**: If you want to maintain the original registry structure

### 3. **Custom Company Structure**
```cpp
constexpr const TCHAR* REGISTRY_BASE_PATH = _T("Software\\YourCompany\\dbWave64\\");
```
**Registry Path**: `HKEY_CURRENT_USER\Software\YourCompany\dbWave64\`

**Use Case**: If you want to use your company name in the registry path

## How to Change the Registry Path

### Step 1: Edit the Configuration Header

Open `DataListCtrl_Configuration.h` and locate the registry configuration section:

```cpp
// Registry configuration
// Change this to modify where registry data is stored
// Options:
// - _T("Software\\FMP\\dbWave64\\")  // FMP/dbWave64 structure (recommended)
// - _T("Software\\dbWave64\\")       // Original structure
// - _T("Software\\YourCompany\\dbWave64\\")  // Custom company structure
constexpr const TCHAR* REGISTRY_BASE_PATH = _T("Software\\FMP\\dbWave64\\");
```

### Step 2: Choose Your Path

Change the `REGISTRY_BASE_PATH` constant to your desired path:

```cpp
// For FMP/dbWave64 structure (recommended)
constexpr const TCHAR* REGISTRY_BASE_PATH = _T("Software\\FMP\\dbWave64\\");

// For original structure
constexpr const TCHAR* REGISTRY_BASE_PATH = _T("Software\\dbWave64\\");

// For custom company structure
constexpr const TCHAR* REGISTRY_BASE_PATH = _T("Software\\YourCompany\\dbWave64\\");
```

### Step 3: Recompile

After changing the path, recompile the application. The new registry path will be used for all future saves and loads.

## Migration Considerations

### Existing Data Migration

If you have existing registry data under the old path, you may want to migrate it:

1. **Automatic Migration**: The application will automatically use the new path for new data
2. **Manual Migration**: You can manually copy registry keys from the old path to the new path
3. **Clean Start**: Simply delete the old registry keys and start fresh

### Registry Cleanup

To clean up old registry data:

1. Open Registry Editor (`regedit`)
2. Navigate to `HKEY_CURRENT_USER\Software\dbWave64\`
3. Delete the entire `dbWave64` key (if it exists)
4. The application will now use the new FMP/dbWave64 structure

## Registry Structure Details

### Data Stored in Registry

The following data is automatically saved to and loaded from the registry:

1. **Display Settings**:
   - Image width and height
   - Display mode
   - Data transform settings
   - Spike plot mode
   - Selected class

2. **Time Settings**:
   - Time first/last values
   - Time span enabled flag

3. **Amplitude Settings**:
   - MV span value
   - MV span enabled flag

4. **UI Settings**:
   - Display file name flag

5. **Performance Settings**:
   - Caching enabled
   - Lazy loading enabled
   - Async processing enabled
   - Max cache size
   - Batch size

6. **Column Configuration**:
   - Column count
   - Individual column widths
   - Column headers
   - Column visibility
   - Column format

### Registry Key Naming Convention

```
HKEY_CURRENT_USER\Software\FMP\dbWave64\{Section}\{Key}
```

Where:
- `{Section}` is the configuration section (e.g., "DataListCtrl_Optimized")
- `{Key}` is the specific setting name (e.g., "ImageWidth", "ColumnCount")

## Benefits of FMP/dbWave64 Structure

1. **Organization**: Clear separation from other applications
2. **Installation Alignment**: Matches the Program Files installation structure
3. **Company Branding**: Reflects the FMP company namespace
4. **Easy Management**: Simple to locate and manage registry entries
5. **Future Expansion**: Room for additional FMP applications under the same namespace

## Troubleshooting

### Registry Access Issues

If you encounter registry access issues:

1. **Permissions**: Ensure the application has write permissions to `HKEY_CURRENT_USER\Software\`
2. **Antivirus**: Some antivirus software may block registry access
3. **UAC**: User Account Control may require elevated permissions

### Data Not Persisting

If settings are not being saved:

1. **Check Registry Path**: Verify the `REGISTRY_BASE_PATH` is correctly set
2. **Registry Permissions**: Ensure write access to the registry
3. **Application Logs**: Check for error messages in the application logs

## Example Registry Export

Here's an example of what the registry structure looks like:

```reg
Windows Registry Editor Version 5.00

[HKEY_CURRENT_USER\Software\FMP\dbWave64\DataListCtrl_Optimized]
"ImageWidth"="400"
"ImageHeight"="50"
"DisplayMode"="1"
"TimeFirst"="0.000000"
"TimeLast"="0.000000"
"SetTimeSpan"="0"
"MvSpan"="0.000000"
"SetMvSpan"="0"
"DisplayFileName"="0"
"CachingEnabled"="1"
"LazyLoadingEnabled"="1"
"AsyncProcessingEnabled"="1"
"MaxCacheSize"="100"
"BatchSize"="10"
"ColumnCount"="4"
"Column0_Width"="50"
"Column0_Header"="Index"
"Column0_Index"="0"
"Column0_Visible"="1"
"Column0_Format"="0"
"Column1_Width"="100"
"Column1_Header"="Data"
"Column1_Index"="1"
"Column1_Visible"="1"
"Column1_Format"="0"
```

## Summary

The registry path configuration has been successfully updated to use the `FMP/dbWave64` structure, providing better organization and alignment with the installation directory structure. The configuration is easily customizable through the `REGISTRY_BASE_PATH` constant in the header file.
