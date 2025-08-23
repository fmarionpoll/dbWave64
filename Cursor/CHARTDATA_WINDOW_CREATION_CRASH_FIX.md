# ChartData Window Creation Crash Fix

## Problem Description

After implementing the `CGraphImageList` class, the application crashed during ChartData window creation with the following error:

```
Debug Assertion Failed!
Program: C:\WINDOWS\SYSTEM32\mfc140ud.dll
File: D:\a\_work\1\s\src\vctools\VC7Libs\Ship\ATLMFC\Src\MFC\wincore.cpp
Line: 776
```

The TRACE output showed:
- ChartData allocation successful: `pTempChart = 0C0800D8`
- Crash occurred during `pTempChart->Create()` call

## Root Cause

The crash was caused by attempting to create a child window (`ChartData`) without a proper parent window. MFC requires child windows to have a valid parent window, and passing `nullptr` as the parent was causing the assertion failure in the MFC window creation code.

## Solution Implemented

### 1. Removed Manual Window Class Registration
Removed the manual window class registration code that was attempting to register "ChartWnd" and "ChartSpikeBar" classes. MFC automatically handles window class registration for classes that inherit from `CWnd`.

### 2. Added Temporary Parent Window
Created a temporary parent window using a static control before creating the ChartData/ChartSpikeBar windows:

```cpp
// Create a temporary parent window for the ChartData
CWnd tempParent;
if (!tempParent.CreateEx(0, _T("STATIC"), _T("TEMP_PARENT"), 
                        WS_POPUP | WS_VISIBLE,
                        CRect(0, 0, 1, 1), nullptr, 0))
{
    TRACE(_T("CGraphImageList::RenderDataPlot - Failed to create temporary parent window\n"));
    delete pTempChart;
    return;
}

// Create the ChartData window as a child of the temporary parent
if (!pTempChart->Create(_T("TEMP_DATAWND"), WS_CHILD, 
                       CRect(0, 0, infos.image_width, infos.image_height), 
                       &tempParent, 0))
{
    TRACE(_T("CGraphImageList::RenderDataPlot - ChartData window creation failed\n"));
    tempParent.DestroyWindow();
    delete pTempChart;
    return;
}
```

### 3. Proper Cleanup
Added proper cleanup of the temporary parent window:

```cpp
// Clean up
pDataDoc->acq_close_file();
pTempChart->DestroyWindow();
tempParent.DestroyWindow();  // Clean up temporary parent
delete pTempChart;
```

## Key Changes

### Files Modified: `dbwave64/dbWave64/dbView/CGraphImageList.cpp`

1. **RenderDataPlot method**:
   - Removed manual window class registration
   - Added temporary parent window creation
   - Updated window creation to use temporary parent
   - Added temporary parent cleanup

2. **RenderSpikePlot method**:
   - Applied same changes as RenderDataPlot
   - Removed manual window class registration
   - Added temporary parent window creation
   - Updated window creation to use temporary parent
   - Added temporary parent cleanup

## Technical Details

### Why This Fix Works

1. **MFC Window Hierarchy**: MFC requires child windows to have a valid parent window. The temporary parent provides this requirement.

2. **Static Control as Parent**: Using a static control as the temporary parent is lightweight and doesn't interfere with the rendering process.

3. **Proper Cleanup**: Both the chart window and temporary parent are properly destroyed to prevent memory leaks.

4. **MFC Automatic Registration**: MFC automatically handles window class registration for `CWnd`-derived classes, so manual registration was unnecessary and potentially problematic.

### Window Creation Process

1. **Allocate ChartData/ChartSpikeBar object**
2. **Create temporary parent window** (static control)
3. **Create chart window as child** of temporary parent
4. **Configure chart settings** (data, display parameters)
5. **Render to provided DC**
6. **Clean up**: Destroy chart window, destroy temporary parent, delete chart object

## Testing Results

After implementing this fix:
- ✅ ChartData allocation succeeds
- ✅ Window creation succeeds without crashes
- ✅ Rendering to DC works properly
- ✅ Memory cleanup is handled correctly
- ✅ No MFC assertion failures

## Benefits

1. **Stability**: Eliminates MFC assertion failures during window creation
2. **Compatibility**: Works with MFC's automatic window class registration
3. **Memory Safety**: Proper cleanup prevents memory leaks
4. **Simplicity**: Removes complex manual window class registration code

## Conclusion

The crash was resolved by providing a proper parent window for the ChartData/ChartSpikeBar child windows. This approach is more compatible with MFC's window management system and eliminates the assertion failures that were occurring during window creation.
