# Compilation Fixes for Persistent Document Objects Approach

## Problem Description

After implementing the persistent document objects approach, compilation errors occurred due to incorrect method names:

```
error C2039: 'acq_open_file': is not a member of 'AcqDataDoc'
error C2039: 'set_data_doc': is not a member of 'ChartData'
```

## Root Cause

The implementation used incorrect method names that don't exist in the actual class definitions:

1. **AcqDataDoc**: Used `acq_open_file()` instead of `open_document()`
2. **ChartData**: Used `set_data_doc()` instead of `attach_data_file()`

## Solution Implemented

### 1. Corrected AcqDataDoc Method Call

**Before:**
```cpp
if (cs_datafile_name.IsEmpty() || !p_data_doc->acq_open_file(cs_datafile_name))
```

**After:**
```cpp
if (cs_datafile_name.IsEmpty() || !p_data_doc->open_document(cs_datafile_name))
```

### 2. Corrected ChartData Method Call

**Before:**
```cpp
p_chart_data_wnd->set_data_doc(p_data_doc);
plot_data(infos, i_image);
```

**After:**
```cpp
p_data_doc->read_data_infos();
p_chart_data_wnd->attach_data_file(p_data_doc);
p_chart_data_wnd->load_all_channels(infos->data_transform);
p_chart_data_wnd->load_data_within_window(infos->b_set_time_span, infos->t_first, infos->t_last);
p_chart_data_wnd->adjust_gain(infos->b_set_mv_span, infos->mv_span);

p_data_doc->acq_close_file();
plot_data(infos, i_image);
```

## Complete Fixed Implementation

### display_data_wnd Method

```cpp
void DataListCtrl_Row::display_data_wnd(DataListCtrlInfos* infos, const int i_image)
{
    // Create data window and data document if necessary
    if (p_chart_data_wnd == nullptr)
    {
        p_chart_data_wnd = new ChartData;
        ASSERT(p_chart_data_wnd != NULL);
        
        // Create a temporary parent window for the ChartData
        CWnd tempParent;
        if (!tempParent.CreateEx(0, _T("STATIC"), _T("TEMP_PARENT"),
                                WS_POPUP | WS_VISIBLE,
                                CRect(0, 0, 1, 1), nullptr, 0))
        {
            TRACE(_T("DataListCtrl_Row::display_data_wnd - Failed to create temporary parent window\n"));
            delete p_chart_data_wnd;
            p_chart_data_wnd = nullptr;
            return;
        }
        
        if (!p_chart_data_wnd->Create(_T("TEMP_DATAWND"), WS_CHILD,
                                    CRect(0, 0, infos->image_width, infos->image_height),
                                    &tempParent, 0))
        {
            TRACE(_T("DataListCtrl_Row::display_data_wnd - ChartData window creation failed\n"));
            tempParent.DestroyWindow();
            delete p_chart_data_wnd;
            p_chart_data_wnd = nullptr;
            return;
        }
        
        tempParent.DestroyWindow();
    }

    // Create data document if necessary
    if (p_data_doc == nullptr)
    {
        p_data_doc = new AcqDataDoc;
        ASSERT(p_data_doc != NULL);
    }

    if (cs_datafile_name.IsEmpty() || !p_data_doc->open_document(cs_datafile_name))
    {
        infos->image_list.Replace(i_image, infos->p_empty_bitmap, nullptr);
    }
    else
    {
        p_data_doc->read_data_infos();
        p_chart_data_wnd->attach_data_file(p_data_doc);
        p_chart_data_wnd->load_all_channels(infos->data_transform);
        p_chart_data_wnd->load_data_within_window(infos->b_set_time_span, infos->t_first, infos->t_last);
        p_chart_data_wnd->adjust_gain(infos->b_set_mv_span, infos->mv_span);
        
        p_data_doc->acq_close_file();
        plot_data(infos, i_image);
    }
}
```

## Key Changes Made

### 1. Method Name Corrections
- `acq_open_file()` → `open_document()`
- `set_data_doc()` → `attach_data_file()`

### 2. Additional Required Calls
- `p_data_doc->read_data_infos()` - Read file information
- `p_chart_data_wnd->load_all_channels()` - Load channel data
- `p_chart_data_wnd->load_data_within_window()` - Load data within time window
- `p_chart_data_wnd->adjust_gain()` - Adjust gain settings
- `p_data_doc->acq_close_file()` - Close file after processing

### 3. Proper Data Flow
1. Open document with `open_document()`
2. Read data information with `read_data_infos()`
3. Attach data file to chart with `attach_data_file()`
4. Load and configure data with various methods
5. Close file with `acq_close_file()`
6. Plot the data

## Verification

The corrected implementation now:
- ✅ **Compiles successfully** with correct method names
- ✅ **Follows the original dbWave2 pattern** for data handling
- ✅ **Maintains persistent document objects** for efficiency
- ✅ **Properly manages file resources** with open/close operations
- ✅ **Handles all required data configuration** steps

## Benefits

1. **Correct API Usage**: Uses the actual methods available in the classes
2. **Complete Data Processing**: Includes all necessary steps for data loading and configuration
3. **Resource Management**: Properly opens and closes files
4. **Consistent with Original**: Follows the same pattern as the working dbWave2 implementation

## Conclusion

The compilation errors were resolved by using the correct method names and implementing the complete data processing workflow. The persistent document objects approach now compiles successfully and should provide the expected performance and stability benefits.
