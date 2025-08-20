# Row Selection and Display Fixes

## Issues Addressed

### Issue 1: No Visual Selection Effect
**Problem**: The selected row was not visually highlighted with a grey background.

**Root Cause**: The virtual list control selection state logic was flawed. It was using `GetNextItem(-1, flag)` to determine the current selection, but this doesn't work properly in virtual mode.

**Solution Applied**:
1. **Added selection tracking member variable**: Added `m_current_selection_` to `DataListCtrl_Optimized` to track the currently selected item.
2. **Updated constructor**: Initialized `m_current_selection_` to -1 in the constructor.
3. **Fixed `set_current_selection` method**: Now stores the selection in `m_current_selection_` and properly redraws items.
4. **Fixed `handle_display_info_request`**: Now uses the stored `m_current_selection_` value to set the selection state instead of calling `GetNextItem()`.

**Key Changes**:
```cpp
// Added to DataListCtrl_Optimized.h
int m_current_selection_; // Track the currently selected item

// Updated constructor
DataListCtrl_Optimized::DataListCtrl_Optimized()
    : m_infos_(nullptr)
    , m_initialized_(false)
    , m_caching_enabled_(true)
    , m_parent_window_(nullptr)
    , m_current_selection_(-1)  // Initialize selection tracking
{
    // ...
}

// Updated set_current_selection method
void DataListCtrl_Optimized::set_current_selection(int record_position)
{
    // Store the current selection
    const int old_selection = m_current_selection_;
    m_current_selection_ = record_position;
    
    // Redraw items to show selection change
    if (old_selection >= 0)
        RedrawItems(old_selection, old_selection);
    if (record_position >= 0)
        RedrawItems(record_position, record_position);
}

// Updated handle_display_info_request
void DataListCtrl_Optimized::handle_display_info_request(LV_DISPINFO* pDispInfo)
{
    // Check if this item is currently selected using our stored selection
    if (index == m_current_selection_)
    {
        state |= LVIS_SELECTED | LVIS_FOCUSED;
        stateMask |= LVIS_SELECTED | LVIS_FOCUSED;
    }
}
```

### Issue 2: Display Column Not Showing Data/Spikes
**Problem**: The display column (curve column) was showing white instead of colored rectangles or data/spikes.

**Root Cause**: The image list was created but not populated with actual images. The `handle_image_display` method was setting image indices that didn't exist.

**Solution Applied**:
1. **Added colored rectangle creation**: Created a `create_colored_rectangles()` method that generates three colored rectangles for different display modes.
2. **Updated image list setup**: Modified `setup_image_list()` to create space for 3 images and call `create_colored_rectangles()`.
3. **Simplified image display logic**: Updated `handle_image_display()` to use fixed image indices (0=grey, 1=blue, 2=red) instead of dynamic indices.

**Key Changes**:
```cpp
// Added create_colored_rectangles method
void DataListCtrl_Optimized::create_colored_rectangles()
{
    // Create grey rectangle (empty display)
    // Create blue rectangle (data display)  
    // Create red rectangle (spikes display)
    // Add all to image list
}

// Updated setup_image_list
void DataListCtrl_Optimized::setup_image_list()
{
    m_image_list_->Create(width, height, ILC_COLOR24 | ILC_MASK, 0, 3); // Space for 3 images
    create_colored_rectangles(); // Populate with colored rectangles
}

// Updated handle_image_display
void DataListCtrl_Optimized::handle_image_display(LV_DISPINFO* pDispInfo, int index, int displayMode)
{
    switch (displayMode)
    {
    case 0: pDispInfo->item.iImage = 0; break; // Grey (empty)
    case 1: pDispInfo->item.iImage = 1; break; // Blue (data)
    case 2: pDispInfo->item.iImage = 2; break; // Red (spikes)
    }
}
```

### Issue 3: Column Widths Not Saved/Restored
**Problem**: Column widths were not persisted when changed or when the view was closed and reopened.

**Root Cause**: The `save_column_widths()` and `load_column_widths()` methods existed but were not being called at the appropriate times.

**Solution Applied**:
1. **Added column width saving on destroy**: Updated `ViewdbWave_Optimized::OnDestroy()` to call `save_column_widths()` before destroying the view.
2. **Added column width loading on initialization**: Updated `initialize_data_list_control()` to call `load_column_widths()` after the DataListCtrl is initialized.
3. **Added automatic saving on column changes**: Updated `resize_signal_column()` and `fit_columns_to_size()` to call `save_column_widths()` when column widths change.

**Key Changes**:
```cpp
// Updated OnDestroy method
void ViewdbWave_Optimized::OnDestroy()
{
    // Save column widths before destroying
    if (m_pDataListCtrl)
    {
        m_pDataListCtrl->save_column_widths();
    }
    
    // Save configuration
    save_configuration();
    
    ViewDbTable::OnDestroy();
}

// Updated initialize_data_list_control
void ViewdbWave_Optimized::initialize_data_list_control()
{
    m_pDataListCtrl->initialize(*m_pConfiguration);
    
    // Load saved column widths after initialization
    m_pDataListCtrl->load_column_widths();
}

// Updated resize_signal_column
void DataListCtrl_Optimized::resize_signal_column(int n_pixels)
{
    // ... existing code ...
    
    // Save column widths when they change
    save_column_widths();
    
    refresh_display();
}

// Updated fit_columns_to_size
void DataListCtrl_Optimized::fit_columns_to_size(int n_pixels)
{
    // ... existing code ...
    
    // Save column widths when they change
    save_column_widths();
}
```

## Technical Details

### Files Modified:
- **`DataListCtrl_Optimized.h`**: Added `m_current_selection_` member variable and `create_colored_rectangles()` method declaration
- **`DataListCtrl_Optimized.cpp`**: 
  - Updated constructor to initialize selection tracking
  - Fixed selection state handling in `handle_display_info_request`
  - Added colored rectangle creation for display column
  - Added automatic column width saving
- **`ViewdbWave_Optimized.cpp`**: 
  - Added column width saving in `OnDestroy`
  - Added column width loading in `initialize_data_list_control`

### Key Features:
1. **Visual Selection Highlighting**: Selected rows now show with grey background highlighting
2. **Display Column Visualization**: Display column now shows colored rectangles (grey=empty, blue=data, red=spikes)
3. **Column Width Persistence**: Column widths are automatically saved when changed and restored when the view is reopened
4. **Database-Driven Selection**: Selection properly reflects the database's current record position
5. **Click-to-Select Functionality**: Users can click on rows to select them

## Verification

The fixes ensure that:
1. **Selection is visually apparent**: The currently selected record is highlighted with a grey background
2. **Display column shows content**: The curve column displays colored rectangles based on the display mode
3. **Column widths persist**: Column widths are saved when changed and restored when the view is reopened
4. **Selection synchronization**: The list control selection stays synchronized with the database's current record position
5. **User interaction works**: Clicking on rows properly updates the selection and database position

## Future Enhancements

For a complete implementation, the display column could be enhanced to show:
- **Actual data curves**: Instead of colored rectangles, display actual waveform data
- **Spike markers**: Show actual spike timing and classification
- **Dynamic images**: Generate images on-demand based on the actual data content
- **Caching**: Cache generated images for better performance

## Related Files

- `DataListCtrl_Optimized.h/cpp` - Core selection and display functionality
- `ViewdbWave_Optimized.h/cpp` - View management and column width persistence
- `ROW_SELECTION_HIGHLIGHTING.md` - Previous documentation of selection highlighting fixes
