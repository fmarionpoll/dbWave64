# UNICODE Character Set Fix - DataListCtrl Display Issue

## Problem Summary
- **Issue**: DataListCtrl in dbWave64 failed to display data and spike images (only grey rectangles showed)
- **Environment**: VS2022, Win32 build, MultiByte character set
- **Working Environment**: VS2019, Win32 build, MultiByte character set

## Root Cause
**Character Set Mismatch**: VS2022 is stricter about character set consistency than VS2019, particularly in complex window hierarchies.

## Solution
**Compile with UNICODE character set** instead of MultiByte.

## Why This Affected Only DataListCtrl

### 1. **Complex Window Hierarchy**
DataListCtrl creates multiple ChartData child windows:
```cpp
p_chart_data_wnd->Create(_T("DATAWND"), WS_CHILD, 
    CRect(0, 0, infos->image_width, infos->image_height), 
    infos->parent, i_image * 100);
```

### 2. **String-Based Window Identification**
The `_T("DATAWND")` class name behaves differently:
- **UNICODE**: `L"DATAWND"` (wide string)
- **MultiByte**: `"DATAWND"` (narrow string)

### 3. **VS2022 Strictness**
VS2022 enforces stricter character set compliance in:
- Window class registration
- Device context string operations
- Message passing between windows

### 4. **Why Other Views Worked**
Other ChartData views use simpler window creation patterns that don't rely on the same string-based window identification system.

## Technical Details

### Window Creation Process
1. DataListCtrl creates ChartData windows as children
2. Each window gets a unique ID: `i_image * 100`
3. VS2022 in MultiByte mode fails to properly register these windows
4. Device context operations fail, preventing data rendering

### Character Set Impact
- **MultiByte**: Narrow strings, 8-bit characters
- **UNICODE**: Wide strings, 16-bit characters
- VS2022 requires consistency throughout the window hierarchy

## Prevention for Future

### 1. **Project Settings**
Always use consistent character sets:
```xml
<CharacterSet>Unicode</CharacterSet>
```

### 2. **Code Guidelines**
- Use `_T()` macro consistently for string literals
- Avoid mixing narrow and wide string operations
- Test complex window hierarchies in both character sets

### 3. **Migration Checklist**
When migrating from VS2019 to VS2022:
- [ ] Verify character set settings
- [ ] Test complex window hierarchies
- [ ] Check string handling in custom controls
- [ ] Validate device context operations

## Related Issues
This type of issue can occur in:
- Custom controls with complex window hierarchies
- Applications using multiple child windows
- Code that relies on string-based window identification
- MFC applications with custom window classes

## Files Modified
- `dbWave64.vcxproj` - Changed CharacterSet from MultiByte to Unicode
- No source code changes required

## Testing
- [x] DataListCtrl displays data curves correctly
- [x] DataListCtrl displays spike images correctly
- [x] Grey rectangle fallback still works
- [x] Other ChartData views continue to work
- [x] No regression in other functionality
