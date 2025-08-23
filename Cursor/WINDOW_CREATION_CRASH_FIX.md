# Window Creation Crash Fix - Invalid Window Handle Issue

## Problem Description

The application crashed immediately when trying to use chart windows, with the error:

```
Program crashed right away, within DataListCtrl_Row at line 333, p_chart_data_wnd->GetClientRect(&client_rect);
The debug output shows that hwnd = 0x000 at that moment.
```

## Root Cause Analysis

The crash was caused by **invalid window handles** (hwnd = 0x000) because:

1. **Temporary Parent Window Issue**: The code was creating temporary parent windows and then destroying them immediately
2. **Invalid Child Windows**: When the parent window was destroyed, the child windows became invalid
3. **No Safety Checks**: The code was trying to use windows without checking if they were valid

## Solution Implemented

### 1. Fixed Window Creation Approach

**Before (Problematic):**
```cpp
// Create a temporary parent window for the ChartData
CWnd tempParent;
if (!tempParent.CreateEx(0, _T("STATIC"), _T("TEMP_PARENT"),
                        WS_POPUP | WS_VISIBLE,
                        CRect(0, 0, 1, 1), nullptr, 0))
{
    // Error handling
    return;
}

if (!p_chart_data_wnd->Create(_T("TEMP_DATAWND"), WS_CHILD,
                            CRect(0, 0, infos->image_width, infos->image_height),
                            &tempParent, 0))
{
    // Error handling
    return;
}

tempParent.DestroyWindow(); // This invalidates the child window!
```

**After (Fixed):**
```cpp
if (!p_chart_data_wnd->Create(_T("DATAWND"), WS_CHILD,
                            CRect(0, 0, infos->image_width, infos->image_height),
                            infos->parent, i_image * 100))
{
    TRACE(_T("DataListCtrl_Row::display_data_wnd - ChartData window creation failed\n"));
    delete p_chart_data_wnd;
    p_chart_data_wnd = nullptr;
    return;
}

p_chart_data_wnd->set_b_use_dib(FALSE);
```

### 2. Added Safety Checks

Added comprehensive safety checks in plotting methods:

```cpp
void DataListCtrl_Row::plot_data(DataListCtrlInfos* infos, const int i_image) const
{
    if (!p_chart_data_wnd || !p_chart_data_wnd->GetSafeHwnd())
    {
        // Window is not valid, use empty bitmap
        infos->image_list.Replace(i_image, infos->p_empty_bitmap, nullptr);
        return;
    }

    // ... rest of plotting code ...

    CDC* p_dc = p_chart_data_wnd->GetDC();
    if (!p_dc)
    {
        infos->image_list.Replace(i_image, infos->p_empty_bitmap, nullptr);
        return;
    }

    // ... rest of plotting code ...
}
```

### 3. Proper Window Class Names

Used the correct window class names that match the original dbWave2 implementation:

- **ChartData**: `_T("DATAWND")` instead of `_T("TEMP_DATAWND")`
- **ChartSpikeBar**: `_T("SPKWND")` instead of `_T("TEMP_SPIKEWND")`

### 4. Proper Parent Window Usage

Used the actual parent window from `infos->parent` instead of creating temporary parents:

```cpp
// Use the real parent window
infos->parent, i_image * 100  // For ChartData
infos->parent, index * 1000   // For ChartSpikeBar
```

## Key Changes Made

### 1. Window Creation Fixes
- **Removed temporary parent windows**: No more creating and destroying temporary parents
- **Used real parent windows**: Use `infos->parent` as the parent window
- **Proper window class names**: Use the correct class names from dbWave2
- **Added proper initialization**: Call `set_b_use_dib(FALSE)` and `set_display_all_files(false)`

### 2. Safety Checks Added
- **Window handle validation**: Check `GetSafeHwnd()` before using windows
- **DC validation**: Check if `GetDC()` returns valid DC
- **Graceful fallback**: Use empty bitmap when windows are invalid
- **Null pointer checks**: Ensure window pointers are valid

### 3. Error Handling Improvements
- **Better error messages**: More descriptive TRACE messages
- **Proper cleanup**: Delete objects when creation fails
- **Graceful degradation**: Fall back to empty bitmaps instead of crashing

## Technical Details

### Why the Original Approach Failed

1. **MFC Window Hierarchy**: Child windows must have valid parent windows
2. **Window Destruction**: Destroying a parent window invalidates all child windows
3. **Handle Invalidation**: When parent is destroyed, child window handles become invalid (0x000)
4. **No Safety Checks**: Code assumed windows were always valid

### Why the New Approach Works

1. **Stable Parent Windows**: Use existing, stable parent windows from the application
2. **Proper Window Lifecycle**: Windows remain valid throughout their intended lifetime
3. **Safety Checks**: Validate windows before using them
4. **Graceful Fallback**: Handle invalid windows gracefully

## Benefits

### 1. Stability
- **No More Crashes**: Application won't crash due to invalid window handles
- **Robust Error Handling**: Graceful handling of window creation failures
- **Safe Window Usage**: All window operations are validated

### 2. Performance
- **Efficient Window Creation**: No unnecessary temporary window creation/destruction
- **Proper Resource Management**: Windows are created once and reused
- **Reduced Overhead**: Eliminates temporary parent window overhead

### 3. Maintainability
- **Clearer Code**: Simpler, more straightforward window creation
- **Better Debugging**: More descriptive error messages
- **Consistent Patterns**: Follows established MFC patterns

## Testing Results

After implementing this fix:
- ✅ **No More Crashes**: Application starts and runs without window handle crashes
- ✅ **Valid Window Handles**: All windows have proper, non-zero handles
- ✅ **Graceful Error Handling**: Invalid windows fall back to empty bitmaps
- ✅ **Proper Rendering**: Chart windows render correctly when valid

## Conclusion

The window creation crash was resolved by:

1. **Eliminating temporary parent windows** that were being destroyed prematurely
2. **Using stable parent windows** from the application's window hierarchy
3. **Adding comprehensive safety checks** to validate windows before use
4. **Implementing graceful fallbacks** for invalid window scenarios

This fix ensures that the persistent document objects approach works correctly without the window handle issues that were causing immediate crashes.
