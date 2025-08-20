# DDX_Text Fix Guide for ViewdbWave_Optimized

## Problem
The original DataExchange method had compilation errors with "no viable function" when using:
```cpp
DDX_Text(pDX, ViewdbWaveConstants::VW_IDC_TIMEFIRST, *m_pTimeFirstEdit);
DDX_Text(pDX, ViewdbWaveConstants::VW_IDC_TIMELAST, *m_pTimeLastEdit);
DDX_Text(pDX, ViewdbWaveConstants::VW_IDC_AMPLITUDESPAN, *m_pAmplitudeSpanEdit);
DDX_Text(pDX, ViewdbWaveConstants::VW_IDC_SPIKECLASS, *m_pAmplitudeSpanEdit);
```

## Root Cause
1. **Wrong DDX Function**: `DDX_Text` is used for binding control values to variables (like `CString`, `int`, `double`), not for associating custom control objects.
2. **Custom CEditCtrl Objects**: The member variables are `std::unique_ptr<CEditCtrl>`, which are custom control objects, not simple data types.
3. **Duplicate Variable**: The last line incorrectly used `*m_pAmplitudeSpanEdit` instead of `*m_pSpikeClassEdit`.

## Solution
Use `DDX_Control` instead of `DDX_Text` for custom control objects:

```cpp
void ViewdbWave_Optimized::DoDataExchange(CDataExchange* pDX)
{
    ViewDbTable::DoDataExchange(pDX);
    
    // Associate the DataListCtrl_Optimized with the dialog control
    DDX_Control(pDX, ViewdbWaveConstants::VW_IDC_LISTCTRL, *m_pDataListCtrl);
    
    // Use DDX_Control for custom CEditCtrl objects instead of DDX_Text
    DDX_Control(pDX, ViewdbWaveConstants::VW_IDC_TIMEFIRST, *m_pTimeFirstEdit);
    DDX_Control(pDX, ViewdbWaveConstants::VW_IDC_TIMELAST, *m_pTimeLastEdit);
    DDX_Control(pDX, ViewdbWaveConstants::VW_IDC_AMPLITUDESPAN, *m_pAmplitudeSpanEdit);
    DDX_Control(pDX, ViewdbWaveConstants::VW_IDC_SPIKECLASS, *m_pSpikeClassEdit);
    DDX_Control(pDX, ViewdbWaveConstants::VW_IDC_TAB1, spk_list_tab_ctrl);
}
```

## DDX Function Guidelines

### DDX_Control
- **Purpose**: Associates dialog control IDs with custom control objects
- **Use for**: `CEditCtrl`, `CListCtrl`, `CTabCtrl`, custom control classes
- **Syntax**: `DDX_Control(pDX, control_id, control_object)`

### DDX_Text
- **Purpose**: Binds control values to simple data types
- **Use for**: `CString`, `int`, `double`, `float`, `long`
- **Syntax**: `DDX_Text(pDX, control_id, variable)`

### DDX_Check
- **Purpose**: Binds checkbox state to boolean variables
- **Use for**: `BOOL`, `bool`
- **Syntax**: `DDX_Check(pDX, control_id, variable)`

### DDX_Radio
- **Purpose**: Binds radio button group to integer variable
- **Use for**: `int` (represents selected radio button index)
- **Syntax**: `DDX_Radio(pDX, control_id, variable)`

## Best Practices

1. **Use DDX_Control for Custom Controls**: Always use `DDX_Control` when working with custom control classes derived from MFC controls.

2. **Use DDX_Text for Data Binding**: Use `DDX_Text` only when you want to bind control values to simple data types.

3. **Initialize Controls First**: Make sure custom controls are properly initialized before calling `DoDataExchange`.

4. **Check Control Existence**: Validate that control pointers are not null before using them.

5. **Consistent Naming**: Use consistent naming conventions for control member variables.

## Example: Mixed Usage
```cpp
void MyDialog::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    
    // Custom control objects
    DDX_Control(pDX, IDC_CUSTOM_EDIT, m_pCustomEdit);
    DDX_Control(pDX, IDC_CUSTOM_LIST, m_pCustomList);
    
    // Simple data binding
    DDX_Text(pDX, IDC_EDIT_VALUE, m_strValue);
    DDX_Text(pDX, IDC_EDIT_NUMBER, m_nNumber);
    DDX_Check(pDX, IDC_CHECK_ENABLED, m_bEnabled);
}
```

## Files Modified
- `dbwave64/dbWave64/dbView_optimized/ViewdbWave_Optimized.cpp` - Fixed DataExchange method
