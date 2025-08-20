# Crash Fix: DDX_Control with Uninitialized Button Pointers

## Problem
The application crashed immediately when selecting the `ViewdbWave_Optimized` view with the following error:
```
Exception thrown at 0x5694C992 (mfc140d.dll) in dbWave64.exe: 0xC0000005: Access violation reading location 0x00000020.
```

The crash occurred at line 86 in `DoDataExchange` during `OnInitialUpdate` at line 435.

## Root Cause
The crash was caused by trying to dereference uninitialized button pointers in `DoDataExchange`:

```cpp
// These lines were causing the crash:
DDX_Control(pDX, ViewdbWaveConstants::VW_IDC_DISPLAYDATA, *m_pDisplayDataButton);
DDX_Control(pDX, ViewdbWaveConstants::VW_IDC_DISPLAY_SPIKES, *m_pDisplaySpikesButton);
DDX_Control(pDX, ViewdbWaveConstants::VW_IDC_DISPLAY_NOTHING, *m_pDisplayNothingButton);
// ... and other button controls
```

**The Problem:**
- Button pointers (`m_pDisplayDataButton`, etc.) were declared as raw pointers (`CButton*`)
- In the constructor, they were initialized to `nullptr`
- `DoDataExchange` was called before the pointers were properly initialized
- Dereferencing `nullptr` with `*m_pDisplayDataButton` caused the access violation

## Solution
1. **Removed DDX_Control calls for button controls** from `DoDataExchange`
2. **Added manual initialization** of button pointers in `initialize_controls()` using `GetDlgItem`

### Code Changes

**Before (in DoDataExchange):**
```cpp
void ViewdbWave_Optimized::DoDataExchange(CDataExchange* pDX)
{
    ViewDbTable::DoDataExchange(pDX);
    
    // ... other controls ...
    
    // CRASH: These lines dereferenced nullptr pointers
    DDX_Control(pDX, ViewdbWaveConstants::VW_IDC_DISPLAYDATA, *m_pDisplayDataButton);
    DDX_Control(pDX, ViewdbWaveConstants::VW_IDC_DISPLAY_SPIKES, *m_pDisplaySpikesButton);
    DDX_Control(pDX, ViewdbWaveConstants::VW_IDC_DISPLAY_NOTHING, *m_pDisplayNothingButton);
    // ... more button controls ...
}
```

**After (in DoDataExchange):**
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
    
    // Note: Button controls will be initialized manually in initialize_controls()
    // to avoid nullptr dereference issues during DoDataExchange
}
```

**Added to initialize_controls():**
```cpp
void ViewdbWave_Optimized::initialize_controls()
{
    try
    {
        // Get tab control
        m_pTabCtrl = static_cast<CTabCtrl*>(GetDlgItem(ViewdbWaveConstants::VW_IDC_TAB1));
        
        // Initialize button controls manually
        m_pDisplayDataButton = static_cast<CButton*>(GetDlgItem(ViewdbWaveConstants::VW_IDC_DISPLAYDATA));
        m_pDisplaySpikesButton = static_cast<CButton*>(GetDlgItem(ViewdbWaveConstants::VW_IDC_DISPLAY_SPIKES));
        m_pDisplayNothingButton = static_cast<CButton*>(GetDlgItem(ViewdbWaveConstants::VW_IDC_DISPLAY_NOTHING));
        m_pCheckFileNameButton = static_cast<CButton*>(GetDlgItem(ViewdbWaveConstants::VW_IDC_CHECKFILENAME));
        m_pFilterCheckButton = static_cast<CButton*>(GetDlgItem(ViewdbWaveConstants::VW_IDC_FILTERCHECK));
        m_pRadioAllClassesButton = static_cast<CButton*>(GetDlgItem(ViewdbWaveConstants::VW_IDC_RADIOALLCLASSES));
        m_pRadioOneClassButton = static_cast<CButton*>(GetDlgItem(ViewdbWaveConstants::VW_IDC_RADIOONECLASS));
        
        // Validate controls
        if (!m_pTimeFirstEdit || !m_pTimeLastEdit || !m_pAmplitudeSpanEdit || !m_pSpikeClassEdit)
        {
            throw std::runtime_error("Required edit controls not found");
        }
        
        // Validate button controls
        if (!m_pDisplayDataButton || !m_pDisplaySpikesButton || !m_pDisplayNothingButton ||
            !m_pCheckFileNameButton || !m_pFilterCheckButton || !m_pRadioAllClassesButton || !m_pRadioOneClassButton)
        {
            throw std::runtime_error("Required button controls not found");
        }
        
        // ... rest of initialization ...
    }
    catch (const std::exception& e)
    {
        handle_error(CString(e.what()));
    }
}
```

## Why This Fix Works
1. **Proper Initialization Order**: Button pointers are now initialized after the dialog is created and before they're used
2. **No Nullptr Dereference**: `GetDlgItem` returns valid pointers to existing controls
3. **Validation**: Added checks to ensure all required controls are found
4. **Error Handling**: Proper exception handling if controls are missing

## Testing
- The application should no longer crash when selecting the `ViewdbWave_Optimized` view
- All button controls should be properly accessible
- Button functionality should work as expected

## Files Modified
- `dbwave64/dbWave64/dbView_optimized/ViewdbWave_Optimized.cpp`
  - Modified `DoDataExchange()` to remove button DDX_Control calls
  - Enhanced `initialize_controls()` to manually initialize button pointers
