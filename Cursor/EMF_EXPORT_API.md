# EMF Export API Documentation

## Overview

This document describes the API for Enhanced Metafile (EMF) export functionality in dbWave64. The EMF export system allows chart data to be exported as vector graphics for use in presentations, publications, and documentation.

## Architecture

```
┌─────────────────────┐
│  ViewSpikeDetect    │  ← High-level view coordinating export
│  (or other views)   │
└──────────┬──────────┘
           │ calls
           ↓
┌─────────────────────┐
│  ChartWnd           │  ← Base class with virtual export methods
│  (ChartData,        │
│   ChartSpikeBar,    │
│   ChartSpikeShape)  │
└──────────┬──────────┘
           │ delegates to
           ↓
┌─────────────────────┐
│ EmfExportHelper     │  ← Static helper class with reusable logic
└─────────────────────┘
           │ uses
           ↓
┌─────────────────────┐
│ EmfLayoutConstants  │  ← Centralized layout constants
└─────────────────────┘
```

---

## Core Classes

### 1. EmfExportHelper

**File**: `dbWave64/EmfExportHelper.h`, `dbWave64/EmfExportHelper.cpp`

**Purpose**: Centralizes EMF export rendering logic to eliminate code duplication.

**Design**: Static utility class (cannot be instantiated).

#### Key Methods:

##### DrawAxes
```cpp
static void DrawAxes(CDC* p_dc, const CRect& rc);
```
**Description**: Draws border and tick marks around a chart area in MM_TEXT mode.

**Parameters**:
- `p_dc`: Device context for drawing (must be EMF DC or compatible)
- `rc`: Rectangle defining the chart area

**Usage**:
```cpp
CRect chart_rect(0, 0, 400, 300);
EmfExportHelper::DrawAxes(pDC, chart_rect);
```

**Visual Result**:
- Rectangular border around chart area
- 4 tick marks on bottom edge
- 4 tick marks on left edge
- Gray color (RGB(100, 100, 100))
- 1 pixel line width

---

##### DrawScaleBar
```cpp
static void DrawScaleBar(CDC* p_dc, const CRect& rc, 
                         double dt_seconds, double px_per_volt, 
                         CString* out_label = nullptr);
```
**Description**: Draws an L-shaped scale bar with optional text labels.

**Parameters**:
- `p_dc`: Device context for drawing
- `rc`: Full rectangle (including space for scale bar)
- `dt_seconds`: Time span displayed (for horizontal scale). Use 0 if not applicable.
- `px_per_volt`: Pixels per volt (for vertical scale). Use 0 if not applicable.
- `out_label`: Optional output - receives formatted label string

**Usage**:
```cpp
CRect full_rect(0, 0, 400, 300);
double time_span = 1.0;  // 1 second
double px_per_v = 150.0; // 150 pixels per volt
CString label;

EmfExportHelper::DrawScaleBar(pDC, full_rect, time_span, px_per_v, &label);
// label now contains something like "vert=10 mV, horz=200 ms"
```

**Visual Result**:
- L-shaped mark at bottom-left corner
- Position: 5px from left edge, 5px from bottom edge
- Black color (RGB(0, 0, 0))
- 2 pixel line width
- Text label positioned after horizontal bar end
- Uses NiceUnit for human-readable increments

---

##### CalculateScaleBarLength
```cpp
static int CalculateScaleBarLength(double data_span, int display_size_px, 
                                   double target_ratio, double& out_span);
```
**Description**: Calculates appropriate scale bar length based on data span and display size.

**Parameters**:
- `data_span`: Total span of data (time in seconds or voltage in volts)
- `display_size_px`: Available display size in pixels
- `target_ratio`: Desired ratio (e.g., 0.2 for 20% of display)
- `out_span`: Output parameter - actual span represented by scale bar

**Returns**: Length of scale bar in pixels

**Algorithm**:
1. Calculate target span: `data_span * target_ratio`
2. Round to "nice" value using NiceUnit (e.g., 1, 2, 5, 10, 20...)
3. Ensure doesn't exceed data_span
4. Convert to pixels
5. Enforce minimum length

**Example**:
```cpp
double actual_span = 0.0;
int length = EmfExportHelper::CalculateScaleBarLength(
    1.5,    // 1.5 seconds of data
    800,    // 800 pixels wide
    0.2,    // Want ~20% (160px)
    actual_span
);
// length might be ~150px, actual_span might be 0.2 (200ms - a "nice" value)
```

---

##### FormatScaleLabel
```cpp
static CString FormatScaleLabel(double span_volts, double span_seconds);
```
**Description**: Formats scale bar label text with appropriate units.

**Parameters**:
- `span_volts`: Voltage span (0 if not applicable)
- `span_seconds`: Time span (0 if not applicable)

**Returns**: Formatted label string

**Format Examples**:
- `"vert=1 mV"` (voltage only)
- `"horz=200 ms"` (time only)
- `"vert=1 mV, horz=200 ms"` (both)
- `""` (empty if both zero)

**Unit Conversion**:
- Automatically selects appropriate units (µV, mV, V, µs, ms, s, etc.)
- Uses NiceUnit for consistent formatting
- Prefers millivolts and milliseconds when appropriate

---

##### CreateExportFont
```cpp
static CFont* CreateExportFont(CDC* p_dc, 
                               int font_size_pt = EmfLayout::TEXT_FONT_SIZE_PT);
```
**Description**: Creates a standard font for EMF export text.

**Parameters**:
- `p_dc`: Device context (for DPI calculation)
- `font_size_pt`: Font size in points (default: 8pt)

**Returns**: Pointer to CFont object (caller must delete)

**Usage**:
```cpp
CFont* font = EmfExportHelper::CreateExportFont(pDC);
CFont* oldFont = pDC->SelectObject(font);
// ... draw text ...
pDC->SelectObject(oldFont);
delete font;
```

**Font Specification**:
- Face: Arial
- Weight: FW_NORMAL (400)
- Charset: DEFAULT_CHARSET
- Height: Calculated based on DPI

---

##### DrawText
```cpp
static void DrawText(CDC* p_dc, const CString& text, int x, int y, 
                     COLORREF color = EmfLayout::COLOR_TEXT);
```
**Description**: Draws text in MM_TEXT mode with standard formatting.

**Parameters**:
- `p_dc`: Device context for drawing
- `text`: Text to draw
- `x`, `y`: Coordinates (device units)
- `color`: Text color (default: black)

**Features**:
- Handles DC save/restore automatically
- Creates and manages font
- Sets transparent background
- Handles empty strings gracefully

**Usage**:
```cpp
EmfExportHelper::DrawText(pDC, _T("Scale: 1mV"), 100, 50, RGB(0, 0, 0));
```

---

##### GetDataRectangle
```cpp
static CRect GetDataRectangle(const CRect& full_rect);
```
**Description**: Calculates the data rectangle (excluding scale bar margins) from full rectangle.

**Parameters**:
- `full_rect`: Full rectangle including scale bar space

**Returns**: Rectangle for data display (reduced by margins)

**Calculation**:
```
data_rect.left = full_rect.left + LEFT_MARGIN_FOR_SCALE (20px)
data_rect.right = full_rect.right (unchanged)
data_rect.top = full_rect.top (unchanged)
data_rect.bottom = full_rect.bottom - BOTTOM_MARGIN_FOR_TEXT (20px)
```

**Usage**:
```cpp
CRect full_rect(0, 0, 400, 300);
CRect data_rect = EmfExportHelper::GetDataRectangle(full_rect);
// data_rect = (20, 0, 400, 280)
```

---

##### IsEmfDC
```cpp
static bool IsEmfDC(CDC* p_dc);
```
**Description**: Validates that a device context is an Enhanced Metafile DC.

**Returns**: true if p_dc is an EMF DC, false otherwise

**Usage**:
```cpp
if (EmfExportHelper::IsEmfDC(pDC))
{
    // EMF-specific optimizations
}
else
{
    // Regular drawing
}
```

---

### 2. EmfLayoutConstants

**File**: `dbWave64/EmfLayoutConstants.h`

**Purpose**: Centralizes layout constants for consistent EMF export appearance.

**Namespace**: `EmfLayout`

#### Constants:

##### Margins
```cpp
constexpr int LEFT_MARGIN_FOR_SCALE = 20;       // Space for vertical scale bar
constexpr int BOTTOM_MARGIN_FOR_TEXT = 20;      // Space for text labels
constexpr int SCALE_BAR_MARGIN_LEFT = 5;        // Distance to L-mark origin
constexpr int SCALE_BAR_MARGIN_BOTTOM = 5;      // Distance to L-mark origin
constexpr int SCALE_BAR_MAX_HEIGHT_OFFSET = 25; // Max height to prevent overlap
```

##### Scale Bar Sizing
```cpp
constexpr int SCALE_BAR_MIN_LENGTH_PX = 12;     // Minimum scale bar length
constexpr double SCALE_BAR_TIME_RATIO = 0.2;    // Target 20% of width
constexpr double SCALE_BAR_VOLT_RATIO = 0.25;   // Target 25% of height
```

##### Text Formatting
```cpp
constexpr int TEXT_FONT_SIZE_PT = 8;            // 8 point font
constexpr int TEXT_OFFSET_FROM_SCALE_X = 6;     // Horizontal offset
constexpr int TEXT_OFFSET_FROM_SCALE_Y = 10;    // Vertical offset
```

##### Pen and Line Styles
```cpp
constexpr int SCALE_BAR_PEN_WIDTH = 2;          // Width of L-mark lines
constexpr int AXES_PEN_WIDTH = 1;               // Width of axis borders
constexpr int TICK_MARK_LENGTH = 6;             // Length of tick marks
constexpr int TICK_MARK_COUNT = 4;              // Number of ticks per side
```

##### Colors
```cpp
constexpr COLORREF COLOR_SCALE_BAR = RGB(0, 0, 0);      // Black
constexpr COLORREF COLOR_AXES = RGB(100, 100, 100);     // Gray
constexpr COLORREF COLOR_TEXT = RGB(0, 0, 0);           // Black
```

##### Font Specification
```cpp
constexpr WCHAR FONT_FACE[] = L"Arial";
constexpr int FONT_WEIGHT = FW_NORMAL;
constexpr BYTE FONT_CHARSET = DEFAULT_CHARSET;
```

**Usage**:
```cpp
#include "EmfLayoutConstants.h"

// Use constants instead of magic numbers
rect.left += EmfLayout::LEFT_MARGIN_FOR_SCALE;
```

---

### 3. ChartWnd Base Class Methods

**File**: `dbWave64/Chart/ChartWnd.h`, `dbWave64/Chart/ChartWnd.cpp`

#### Virtual Methods for EMF Export:

##### draw_axes_to_emf
```cpp
virtual void draw_axes_to_emf(CDC* p_dc, const CRect& rc) const;
```
**Description**: Virtual method allowing derived classes to customize axis drawing.

**Default Implementation**: Delegates to `EmfExportHelper::DrawAxes()`

**Override Example**:
```cpp
void MyChartClass::draw_axes_to_emf(CDC* p_dc, const CRect& rc) const
{
    // Custom behavior
    EmfExportHelper::DrawAxes(p_dc, rc);
    // Additional custom drawing...
}
```

##### draw_scale_bar_to_emf
```cpp
virtual void draw_scale_bar_to_emf(CDC* p_dc, const CRect& rc, 
                                   double dt_seconds, double px_per_volt,
                                   CString* out_label) const;
```
**Description**: Virtual method allowing derived classes to customize scale bar drawing.

**Default Implementation**: Delegates to `EmfExportHelper::DrawScaleBar()`

---

## Adding EMF Export to a New Chart Type

### Step-by-Step Guide:

#### 1. Inherit from ChartWnd
```cpp
class MyNewChart : public ChartWnd
{
public:
    // ...existing methods...
    
    // Add EMF export method
    void export_to_emf(CDC* p_dc, const CRect& rect) const;
    double get_pixels_per_volt(const CRect& rc) const;
};
```

#### 2. Implement export_to_emf
```cpp
void MyNewChart::export_to_emf(CDC* p_dc, const CRect& rect) const
{
    const int saved = p_dc->SaveDC();
    p_dc->SetMapMode(MM_TEXT);
    p_dc->SelectClipRgn(nullptr);
    
    // Draw your chart data here
    // Use MM_TEXT coordinates (Y increases downward)
    // Example:
    CPen pen(PS_SOLID, 2, RGB(0, 0, 0));
    const auto old_pen = p_dc->SelectObject(&pen);
    
    // ... draw chart content ...
    
    if (old_pen) p_dc->SelectObject(old_pen);
    p_dc->RestoreDC(saved);
}
```

#### 3. Implement get_pixels_per_volt
```cpp
double MyNewChart::get_pixels_per_volt(const CRect& rc) const
{
    if (!m_data_valid)
        return 0.0;
    
    // Calculate based on your data
    double volt_range = m_max_volts - m_min_volts;
    if (volt_range <= 0.0)
        return 0.0;
    
    return static_cast<double>(rc.Height()) / volt_range;
}
```

#### 4. Use in View's render_for_export
```cpp
void MyView::render_for_export(CDC* p_dc)
{
    CRect r1(0, 0, 400, 300);
    
    // Calculate data rectangle
    CRect data_r1 = EmfExportHelper::GetDataRectangle(r1);
    
    // Export chart content
    my_chart_.export_to_emf(p_dc, &data_r1);
    
    // Draw axes
    my_chart_.draw_axes_to_emf(p_dc, data_r1);
    
    // Draw scale bar
    double time_span = /* calculate */;
    double px_per_v = my_chart_.get_pixels_per_volt(data_r1);
    CString label;
    my_chart_.draw_scale_bar_to_emf(p_dc, r1, time_span, px_per_v, &label);
}
```

---

## Coordinate Systems

### MM_TEXT Mode
- **Origin**: Top-left corner
- **X-axis**: Increases right (→)
- **Y-axis**: Increases downward (↓)
- **Units**: Device pixels

**Used for**: EMF export (compatibility with Office apps)

### MM_ANISOTROPIC Mode
- **Origin**: Configurable via `SetViewportOrg`
- **X-axis**: Configurable
- **Y-axis**: Configurable (typically increases upward)
- **Units**: Logical units

**Used for**: On-screen display

### Conversion Example:
```cpp
// In MM_TEXT, to make Y increase upward:
const int y_viewport_extent = -rect.Height();  // Negative!
const int y_viewport_offset = rect.top + rect.Height() / 2;

// Transform value:
int y_device = y_viewport_offset + 
               MulDiv(y_value - y_offset, y_viewport_extent, y_extent);
```

---

## Common Pitfalls

### 1. Forgetting to Negate Y-Extent in MM_TEXT
**Problem**: Positive voltages render downward
```cpp
// WRONG:
const int y_extent = rect.Height();

// RIGHT:
const int y_extent = -rect.Height();
```

### 2. Not Saving/Restoring DC State
**Problem**: Pen/brush/font changes affect subsequent drawing
```cpp
// ALWAYS do this:
const int saved = p_dc->SaveDC();
// ... draw ...
p_dc->RestoreDC(saved);
```

### 3. Using Chart Rect Instead of Full Rect for Scale Bars
**Problem**: Scale bars don't have space to render
```cpp
// WRONG:
CRect data_rect = EmfExportHelper::GetDataRectangle(full_rect);
EmfExportHelper::DrawScaleBar(p_dc, data_rect, ...);  // No space!

// RIGHT:
CRect data_rect = EmfExportHelper::GetDataRectangle(full_rect);
chart.export_to_emf(p_dc, &data_rect);  // Draw in data rect
EmfExportHelper::DrawScaleBar(p_dc, full_rect, ...);  // Scale in full rect
```

### 4. Hardcoding Layout Constants
**Problem**: Inconsistent appearance, hard to maintain
```cpp
// WRONG:
rect.left += 20;
rect.bottom -= 20;

// RIGHT:
rect = EmfExportHelper::GetDataRectangle(rect);
```

### 5. Not Using NiceUnit for Scale Values
**Problem**: Ugly scale values like "0.1732 seconds"
```cpp
// WRONG:
label.Format(_T("%.4f s"), span);

// RIGHT:
label = EmfExportHelper::FormatScaleLabel(0.0, span);
```

---

## Testing

### Unit Tests
**File**: `tests/EmfExportHelperTests.cpp`

**Run Tests**:
```
Open Test Explorer in Visual Studio
Build solution
Run all tests in EmfExportHelperTests
```

**Coverage**:
- All public methods of EmfExportHelper
- Edge cases (zero values, null pointers, empty strings)
- Integration scenarios
- Performance benchmarks

### Manual Testing Checklist:
- [ ] Export chart to clipboard
- [ ] Paste into PowerPoint - verify appearance
- [ ] Paste into Word - verify appearance
- [ ] Import into CorelDraw - verify scale bars have joined corners
- [ ] Check font consistency (all text should be 8pt Arial)
- [ ] Verify scale bar positions (outside data area, inside full rectangle)
- [ ] Check for spike truncation (compare exported vs on-screen)
- [ ] Verify zero-volt baseline appears
- [ ] Test with different data densities (10, 100, 1000+ spikes)

---

## Performance Considerations

### Optimization Tips:

1. **Avoid Redundant Calculations**:
```cpp
// Calculate once
double px_per_v = chart.get_pixels_per_volt(rect);

// Reuse
DrawScaleBar(p_dc, r1, dt1, px_per_v, &lab1);
DrawScaleBar(p_dc, r2, dt2, px_per_v, &lab2);
```

2. **Use Paths for Complex Shapes**:
```cpp
BeginPath(hdc);
// ... many drawing operations ...
EndPath(hdc);
StrokePath(hdc);  // Renders all at once
```

3. **Limit Scale Bar Complexity**:
- Already enforced via `SCALE_BAR_MAX_HEIGHT_OFFSET`
- Prevents excessive vertical bars

4. **Profile Before Optimizing**:
- Use Performance Profiler in Visual Studio
- Most EMF export time is in data rendering, not annotations

---

## Version History

### Version 1.0 (2025-10-08)
- Initial API documentation
- EmfExportHelper class created
- EmfLayoutConstants namespace created
- Unit tests added
- ChartWnd base class methods standardized

---

## See Also

- `EMF_EXPORT_SESSION_SUMMARY_2025-10-08.md` - Implementation details
- `tests/EmfExportHelperTests.cpp` - Example usage and tests
- Microsoft Documentation: [Enhanced-Format Metafiles](https://learn.microsoft.com/en-us/windows/win32/gdi/enhanced-format-metafiles)
- NiceUnit.h - Unit formatting utilities

---

## Support

For questions or issues:
1. Check this documentation
2. Review unit tests for examples
3. Examine existing implementations (ChartData, ChartSpikeBar, ChartSpikeShape)
4. Consult session summary documents in `Cursor/` directory





