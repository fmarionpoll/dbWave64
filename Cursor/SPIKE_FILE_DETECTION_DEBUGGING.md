# Spike File Detection Debugging - Identifying File Loading Issues

## Problem Description

The application now compiles and runs successfully without crashes, but there's an issue with spike file detection:

- **Issue**: The program erroneously detects that no spike file is present
- **Symptom**: Spike display shows empty/error state even when spike files exist
- **Status**: Application is stable but spike rendering is not working

## Root Cause Analysis

The issue is likely in one of these areas:

1. **File Name Retrieval**: `cs_spike_file_name` might be empty or incorrect
2. **Document Opening**: `OnOpenDocument()` might be failing
3. **Persistent Document Reuse**: Reusing the same `CSpikeDoc` object might cause issues
4. **File Path Issues**: File paths might be incorrect or files might not exist

## Debugging Approach Implemented

### 1. Added TRACE Statements for File Names

Added debugging to see what file names are being retrieved:

```cpp
void DataListCtrl_Row::attach_database_record(CdbWaveDoc* db_wave_doc)
{
    // ... existing code ...
    
    cs_datafile_name = db_wave_doc->db_get_current_dat_file_name(TRUE);
    cs_spike_file_name = db_wave_doc->db_get_current_spk_file_name(TRUE);
    
    TRACE(_T("DataListCtrl_Row::attach_database_record - Data file: '%s'\n"), cs_datafile_name);
    TRACE(_T("DataListCtrl_Row::attach_database_record - Spike file: '%s'\n"), cs_spike_file_name);
    
    // ... rest of method ...
}
```

### 2. Enhanced Spike File Detection Logic

Split the spike file detection into separate checks with detailed logging:

```cpp
void DataListCtrl_Row::display_spike_wnd(DataListCtrlInfos* infos, const int i_image)
{
    // ... window creation code ...
    
    TRACE(_T("DataListCtrl_Row::display_spike_wnd - Spike file name: '%s'\n"), cs_spike_file_name);
    
    if (cs_spike_file_name.IsEmpty())
    {
        TRACE(_T("DataListCtrl_Row::display_spike_wnd - Spike file name is empty\n"));
        infos->image_list.Replace(i_image, infos->p_empty_bitmap, nullptr);
    }
    else if (!p_spike_doc->OnOpenDocument(cs_spike_file_name))
    {
        TRACE(_T("DataListCtrl_Row::display_spike_wnd - Failed to open spike document: '%s'\n"), cs_spike_file_name);
        infos->image_list.Replace(i_image, infos->p_empty_bitmap, nullptr);
    }
    else
    {
        // ... spike rendering code ...
    }
}
```

### 3. Document Management Improvements

Added logic to handle persistent document reuse properly:

```cpp
// If the document is already open with a different file, close it first
if (!p_spike_doc->GetPathName().IsEmpty() && p_spike_doc->GetPathName() != cs_spike_file_name)
{
    TRACE(_T("DataListCtrl_Row::display_spike_wnd - Closing previous spike document: '%s'\n"), p_spike_doc->GetPathName());
    p_spike_doc->OnCloseDocument();
}
```

## Expected Debug Output

When the application runs, the TRACE output should show:

### Successful Case:
```
DataListCtrl_Row::attach_database_record - Data file: 'C:\path\to\data.dat'
DataListCtrl_Row::attach_database_record - Spike file: 'C:\path\to\spike.spk'
DataListCtrl_Row::display_spike_wnd - Spike file name: 'C:\path\to\spike.spk'
```

### Problem Cases:

**Empty File Name:**
```
DataListCtrl_Row::attach_database_record - Spike file: ''
DataListCtrl_Row::display_spike_wnd - Spike file name: ''
DataListCtrl_Row::display_spike_wnd - Spike file name is empty
```

**File Open Failure:**
```
DataListCtrl_Row::display_spike_wnd - Spike file name: 'C:\path\to\spike.spk'
DataListCtrl_Row::display_spike_wnd - Failed to open spike document: 'C:\path\to\spike.spk'
```

**Document Reuse:**
```
DataListCtrl_Row::display_spike_wnd - Closing previous spike document: 'C:\path\to\old.spk'
DataListCtrl_Row::display_spike_wnd - Spike file name: 'C:\path\to\new.spk'
```

## Troubleshooting Steps

### 1. Check File Name Retrieval
- Verify that `db_get_current_spk_file_name(TRUE)` returns the correct file name
- Check if the database contains the correct spike file references
- Ensure the file paths are absolute and correct

### 2. Check File Existence
- Verify that the spike files actually exist at the specified paths
- Check file permissions and accessibility
- Ensure the file format is correct and readable

### 3. Check Document Opening
- Verify that `OnOpenDocument()` can successfully open the spike files
- Check if there are any MFC document template issues
- Ensure the `CSpikeDoc` class can handle the file format

### 4. Check Persistent Document Issues
- Verify that reusing the same `CSpikeDoc` object doesn't cause conflicts
- Check if `OnCloseDocument()` properly cleans up the document state
- Ensure document state is properly reset between different files

## Next Steps

1. **Run the application** with the debugging code
2. **Check the TRACE output** to identify the specific issue
3. **Analyze the debug information** to determine the root cause
4. **Implement the appropriate fix** based on the findings

## Potential Fixes

### If File Names Are Empty:
- Check database configuration and file path settings
- Verify that spike files are properly associated with database records

### If File Opening Fails:
- Check file existence and permissions
- Verify file format compatibility
- Check MFC document template registration

### If Document Reuse Issues:
- Implement proper document cleanup between files
- Consider creating new document objects for each file
- Check MFC document lifecycle management

## Benefits of This Approach

1. **Clear Diagnosis**: TRACE output will clearly show what's happening
2. **Specific Error Identification**: Separate checks for different failure modes
3. **Document Management**: Proper handling of persistent document reuse
4. **Debugging Information**: Detailed logging for troubleshooting

This debugging approach will help identify the exact cause of the spike file detection issue and guide the implementation of the appropriate fix.

