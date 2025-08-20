# Debugging Approach for Row Selection and Display Fixes

## Issues to Debug

Since the previous fixes didn't resolve the issues, we need to systematically debug each problem:

### Issue 1: No Visual Selection Effect
**Debugging Steps:**
1. **Check if `set_current_selection` is being called**: Look for TRACE messages in the debug output
2. **Check if `handle_display_info_request` is setting state**: Look for TRACE messages about selection state
3. **Check if `OnGetDisplayInfo` is requesting state**: Verify that `LVIF_STATE` is being added to the mask
4. **Check if virtual list control is properly configured**: Verify `LVS_OWNERDATA` style is set

**Expected TRACE Output:**
```
DataListCtrl_Optimized::set_current_selection - Called with position X
DataListCtrl_Optimized::set_current_selection - Setting selection from Y to X
DataListCtrl_Optimized::handle_display_info_request - Setting selection state for item X
```

### Issue 2: Display Column Not Showing Data/Spikes
**Debugging Steps:**
1. **Check if image list is being created**: Look for TRACE messages about image list creation
2. **Check if colored rectangles are being added**: Look for TRACE messages about adding rectangles
3. **Check if `handle_image_display` is being called**: Look for TRACE messages about image display
4. **Check if display mode is correct**: Verify the current display mode in configuration

**Expected TRACE Output:**
```
DataListCtrl_Optimized::setup_image_list - Creating image list with size WxH
DataListCtrl_Optimized::create_colored_rectangles - Added grey rectangle at index 0
DataListCtrl_Optimized::create_colored_rectangles - Added blue rectangle at index 1
DataListCtrl_Optimized::create_colored_rectangles - Added red rectangle at index 2
DataListCtrl_Optimized::handle_image_display - Item X, subitem 2, displayMode Y
```

### Issue 3: Column Widths Not Saved/Restored
**Debugging Steps:**
1. **Check if `save_column_widths` is being called**: Look for TRACE messages when view is destroyed
2. **Check if `load_column_widths` is being called**: Look for TRACE messages during initialization
3. **Check if column widths are being saved**: Look for TRACE messages about column widths
4. **Check if configuration is being persisted**: Verify that the configuration object is being saved

**Expected TRACE Output:**
```
DataListCtrl_Optimized::save_column_widths - Called
DataListCtrl_Optimized::save_column_widths - Saving X columns
DataListCtrl_Optimized::save_column_widths - Column Y width: Z
DataListCtrl_Optimized::load_column_widths - Called
DataListCtrl_Optimized::load_column_widths - Loading X columns
DataListCtrl_Optimized::load_column_widths - Column Y width: Z
```

## Debugging Commands

### To Enable Debug Output:
1. **Build in Debug mode**: Ensure the project is built in Debug configuration
2. **Enable TRACE output**: Make sure TRACE macros are enabled
3. **Check Output Window**: Look for TRACE messages in Visual Studio's Output window

### To Test Each Issue:

#### Test Selection Highlighting:
1. **Click on a row**: Should see TRACE messages about `set_current_selection`
2. **Check if row highlights**: The selected row should have a grey background
3. **Check debug output**: Look for selection state messages

#### Test Display Column:
1. **Change display mode**: Use the display mode buttons (if available)
2. **Check curve column**: Should see colored rectangles instead of white
3. **Check debug output**: Look for image display messages

#### Test Column Width Persistence:
1. **Resize a column**: Drag column header to resize
2. **Close and reopen view**: Should see save/load messages
3. **Check if widths persist**: Column widths should be restored

## Common Issues and Solutions

### If No TRACE Messages Appear:
- **Check build configuration**: Ensure Debug build
- **Check TRACE macro**: Verify TRACE is defined
- **Check output window**: Make sure Output window is visible

### If Selection Still Not Working:
- **Check virtual list control**: Verify `LVS_OWNERDATA` style
- **Check message map**: Ensure `ON_NOTIFY_REFLECT(LVN_GETDISPINFO, OnGetDisplayInfo)` is present
- **Check state mask**: Verify `LVIF_STATE` is being set

### If Display Column Still White:
- **Check image list creation**: Look for image list creation messages
- **Check bitmap creation**: Verify bitmaps are being created successfully
- **Check image indices**: Ensure correct image indices are being set

### If Column Widths Not Persisting:
- **Check configuration object**: Verify configuration is being saved
- **Check save/load timing**: Ensure methods are called at right time
- **Check column count**: Verify correct number of columns

## Next Steps

1. **Run the application in Debug mode**
2. **Check the Output window for TRACE messages**
3. **Identify which specific issue is failing**
4. **Focus debugging on the failing component**
5. **Apply targeted fixes based on debug output**

## Files to Monitor

- **DataListCtrl_Optimized.cpp**: Main debugging output
- **ViewdbWave_Optimized.cpp**: View lifecycle debugging
- **Output window**: TRACE message output
- **Debugger**: Breakpoints for detailed inspection
