# DBWAVE64 RUNTIME DLL FIXES

## Problem Summary
The dbwave64 project compiles successfully but crashes at runtime with DLL loading errors:
- `oldaapi32.dll` not found
- `OLMEM32.dll` not found  
- `ceds64int.dll` not found

**Root Cause**: The required DLLs are located in `include\toAdd` directory, but the executable doesn't know where to find them at runtime.

## Solution Options

### Solution 1: Copy DLLs to Output Directory (Recommended)

#### Step 1: Add Post-Build Event
Add a post-build event to automatically copy DLLs to the output directory.

**File**: `dbWave64/dbWave64.vcxproj`

Add this to each build configuration:

```xml
<PostBuildEvent>
  <Command>copy "$(ProjectDir)include\toAdd\*.dll" "$(OutDir)"</Command>
  <Message>Copying runtime DLLs to output directory</Message>
</PostBuildEvent>
```

#### Step 2: Alternative - Copy Specific DLLs
If you prefer to copy only specific DLLs:

```xml
<PostBuildEvent>
  <Command>
    copy "$(ProjectDir)include\toAdd\oldaapi32.dll" "$(OutDir)"
    copy "$(ProjectDir)include\toAdd\OLMEM32.dll" "$(OutDir)"
    copy "$(ProjectDir)include\toAdd\ceds64int.dll" "$(OutDir)"
  </Command>
  <Message>Copying required runtime DLLs</Message>
</PostBuildEvent>
```

### Solution 2: Set DLL Directory in Code

#### Step 1: Modify Application Startup
Add DLL directory setting in the main application file.

**File**: `dbWave64/dbWave64/dbWave.cpp`

Add this near the beginning of the application:

```cpp
// Add at the top of the file after includes
#include <windows.h>

// Add in the application initialization
BOOL CdbWaveApp::InitInstance()
{
    // Set DLL directory to include\toAdd
    SetDllDirectory(_T("include\\toAdd"));
    
    // ... rest of existing initialization code
}
```

### Solution 3: Use Windows DLL Search Path

#### Option A: Add to PATH Environment Variable
Add the DLL directory to the system PATH (not recommended for development).

#### Option B: Use LoadLibrary with Full Path
Load DLLs explicitly with full paths (more complex but gives full control).

### Solution 4: Deploy with Application (Production)

#### Create Deployment Package
For production deployment, create a package that includes:
- Executable
- All required DLLs
- Configuration files

## Implementation Details

### Required DLLs
Based on the project dependencies:

1. **oldaapi32.dll** - Data Translation Data Acquisition API
2. **OLMEM32.dll** - Data Translation Memory Management
3. **ceds64int.dll** - CED Spike2 Data File Access

### DLL Locations
- **Source**: `include\toAdd\`
- **Target**: `Debug\` or `Release\` (output directory)

### Verification Steps

#### Step 1: Check DLL Existence
```powershell
Test-Path "include\toAdd\oldaapi32.dll"
Test-Path "include\toAdd\OLMEM32.dll"
Test-Path "include\toAdd\ceds64int.dll"
```

#### Step 2: Check Output Directory
After building, verify DLLs are copied:
```powershell
Test-Path "Debug\oldaapi32.dll"
Test-Path "Debug\OLMEM32.dll"
Test-Path "Debug\ceds64int.dll"
```

#### Step 3: Test Application
Run the debug executable and verify it starts without DLL errors.

## Recommended Implementation

### Step 1: Add Post-Build Event
Modify `dbWave64.vcxproj` to add post-build events for all configurations.

### Step 2: Test Build Process
Build the project and verify DLLs are copied to output directory.

### Step 3: Test Runtime
Run the debug executable and confirm it starts successfully.

## Alternative: Development Environment Setup

### Option 1: Visual Studio Debug Settings
Set the working directory in Visual Studio debug settings to include the DLL directory.

### Option 2: Batch File Launcher
Create a batch file that sets PATH and launches the application:

```batch
@echo off
set PATH=%PATH%;%~dp0include\toAdd
start dbWave64.exe
```

## Troubleshooting

### Common Issues
1. **DLL not found**: Verify DLL exists in source directory
2. **Wrong architecture**: Ensure 32-bit/64-bit DLL compatibility
3. **Permission denied**: Run as administrator if needed
4. **Path issues**: Use absolute paths or relative paths correctly

### Debug Steps
1. Use Dependency Walker to check DLL dependencies
2. Check Windows Event Viewer for detailed error messages
3. Use Process Monitor to track DLL loading attempts
4. Verify DLL architecture matches executable

## Conclusion

The recommended solution is to add a post-build event that copies the required DLLs from `include\toAdd` to the output directory. This ensures the application can find all required dependencies at runtime.

## Files to Modify
- `dbWave64/dbWave64.vcxproj` - Add post-build events
- `dbWave64/dbWave64/dbWave.cpp` - Optional: Add SetDllDirectory call

## Verification
After implementing the fix:
1. Build the project
2. Verify DLLs are in output directory
3. Run the executable successfully
