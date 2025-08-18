# Filename Display on Red Rectangles - Implementation

## Overview
Enhanced the red rectangle display functionality to show the filename of each record on top of the red rectangle, making it easier to identify which record is being displayed.

## Changes Made

### 1. Enhanced `display_empty_wnd()` Function (DataListCtrl_Row.cpp)

**Key Improvements:**
- **Custom Bitmap Creation**: Instead of using a generic empty bitmap, now creates a custom bitmap for each row
- **Red Rectangle with Text**: Draws a red rectangle with white text showing the filename
- **Filename Processing**: 
  - Extracts just the filename (without path) from the full file path
  - Shows "No file" if no filename is available
  - Truncates long filenames to 20 characters with "..." suffix
- **Text Centering**: Centers the filename text both horizontally and vertically on the rectangle
- **Font Management**: Uses Arial font, size 10, for clear readability

**Technical Details:**
- Creates a compatible DC and bitmap for each row
- Uses RGB(255, 0, 0) for pure red background
- Uses RGB(255, 255, 255) for white text
- Sets transparent background mode for text
- Properly manages font resources
- **Fixed VS2022 Compilation Issue**: Changed from static to instance method to access member variables

### 2. Updated `force_red_rectangle_display()` Function (DataListCtrl.cpp)

**Changes:**
- Removed dependency on generic `build_empty_bitmap()` function
- Updated debug messages to reflect filename display functionality
- Simplified the function since each row now creates its own custom bitmap

### 3. Fixed Compilation Issue (DataListCtrl_Row.h)

**Problem:** VS2022 complained about accessing non-static member `cs_datafile_name` in a static function
**Solution:** Changed `display_empty_wnd()` from static to instance method
**Impact:** Function can now properly access instance member variables like `cs_datafile_name`

## Features

### Filename Display Logic
1. **Full Path Handling**: Extracts filename from full path using `ReverseFind('\\')`
2. **Empty File Handling**: Shows "No file" when no filename is available
3. **Long Filename Truncation**: Limits display to 20 characters with "..." suffix
4. **Text Centering**: Automatically centers text on the rectangle

### Visual Design
- **Red Background**: Pure red (RGB(255, 0, 0)) for clear visibility
- **White Text**: High contrast white text for readability
- **Black Border**: Thin black border around the rectangle
- **Arial Font**: Clean, readable font at size 10

## Example Output

The red rectangles will now display:
- `datafile001.dat` (if filename is short)
- `very_long_filename_that_gets_truncated...` (if filename is long)
- `No file` (if no filename is available)

## Debug Output

The enhanced debug tracing will show:
```
DEBUG: display_empty_wnd() - Successfully replaced image 0 with red rectangle and filename: datafile001.dat
DEBUG: force_red_rectangle_display() - Forcing red rectangle display with filenames
```

## Benefits

1. **Easy Identification**: Users can immediately see which record each red rectangle represents
2. **Better UX**: No need to hover or check other columns to identify records
3. **Consistent Display**: Each record shows its specific filename
4. **Robust Handling**: Gracefully handles missing files and long filenames
5. **Compilation Fixed**: Resolves VS2022 static member access error

## Testing

To test the filename display:
1. Load a database with records
2. Click "Display Nothing" button
3. Verify that red rectangles appear with filenames displayed in white text
4. Check that long filenames are properly truncated
5. Verify that records without files show "No file"

## Future Enhancements

Potential improvements could include:
- Configurable font size and type
- Different colors for different file types
- Tooltip showing full filename on hover
- Configurable text positioning (top, center, bottom)
