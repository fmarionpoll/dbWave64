# Persistent Document Objects Approach - Efficient and Stable Rendering

## Problem Description

The previous approach of creating new `CSpikeDoc` and `AcqDataDoc` objects for each rendering call was inefficient and problematic:

1. **Performance Issues**: Creating and destroying document objects for each display update cycle
2. **MFC Document Management Conflicts**: Frequent creation/destruction of `CDocument`-derived objects
3. **Memory Fragmentation**: Repeated allocation/deallocation cycles
4. **Staggering Update Cycles**: The display update message cycle is very frequent, making the overhead significant

## Root Cause Analysis

The fundamental issue was that we were using a **temporary object approach** where:
- New `CSpikeDoc` and `AcqDataDoc` objects were created for each rendering call
- Objects were destroyed immediately after rendering
- This happened on every display update cycle
- MFC's document management system was constantly conflicting with manual object lifecycle

## Solution Implemented: Persistent Document Objects

We implemented a **persistent document objects approach** that:

1. **Associates document objects with each row** as member variables
2. **Creates objects once** and reuses them throughout the row's lifetime
3. **Eliminates frequent allocation/deallocation** cycles
4. **Avoids MFC document management conflicts** by maintaining stable object references

### Implementation Details

#### 1. Persistent Member Variables

The `DataListCtrl_Row` class now maintains persistent document objects:

```cpp
class DataListCtrl_Row : public CObject
{
    // ... other members ...
    
    // Persistent document objects
    AcqDataDoc* p_data_doc {nullptr};
    CSpikeDoc* p_spike_doc {nullptr};
    
    // Persistent chart windows
    ChartData* p_chart_data_wnd {nullptr};
    ChartSpikeBar* p_chart_spike_wnd {nullptr};
    
    // ... methods ...
};
```

#### 2. Lazy Initialization

Document objects are created only when needed and reused:

```cpp
void DataListCtrl_Row::display_data_wnd(DataListCtrlInfos* infos, const int i_image)
{
    // Create data window and data document if necessary
    if (p_chart_data_wnd == nullptr)
    {
        p_chart_data_wnd = new ChartData;
        // ... window creation code ...
    }

    // Create data document if necessary
    if (p_data_doc == nullptr)
    {
        p_data_doc = new AcqDataDoc;
    }
    
    // Use existing objects for rendering
    if (cs_datafile_name.IsEmpty() || !p_data_doc->acq_open_file(cs_datafile_name))
    {
        // Handle error case
    }
    else
    {
        p_chart_data_wnd->set_data_doc(p_data_doc);
        plot_data(infos, i_image);
    }
}
```

#### 3. Proper Cleanup

Objects are properly cleaned up in the destructor:

```cpp
DataListCtrl_Row::~DataListCtrl_Row()
{
    delete p_chart_data_wnd;
    delete p_chart_spike_wnd;
    
    SAFE_DELETE(p_data_doc)
    SAFE_DELETE(p_spike_doc)
    
    // ... other cleanup ...
}
```

## Key Benefits

### 1. Performance Improvements
- **Eliminated Allocation Overhead**: No more repeated `new`/`delete` cycles
- **Reduced Memory Fragmentation**: Stable object references
- **Faster Rendering**: Reuse of existing objects
- **Better Cache Performance**: Objects remain in memory

### 2. Stability Enhancements
- **No More MFC Conflicts**: Stable document object references
- **Eliminated Crashes**: No more "destroying an unsaved document" warnings
- **Consistent Behavior**: Predictable object lifecycle
- **Robust Error Handling**: Better error recovery

### 3. Memory Efficiency
- **Reduced Memory Churn**: Objects persist throughout row lifetime
- **Lower Peak Memory Usage**: No temporary object creation spikes
- **Better Resource Management**: Controlled object lifecycle
- **Predictable Memory Patterns**: Consistent memory usage

### 4. Code Quality
- **Simplified Logic**: Clear separation of concerns
- **Better Maintainability**: Easier to debug and modify
- **Consistent Patterns**: Follows established MFC patterns
- **Reduced Complexity**: Eliminated complex exception handling

## Technical Implementation

### Object Lifecycle Management

1. **Creation**: Objects created on first use (lazy initialization)
2. **Reuse**: Same objects used for all subsequent rendering calls
3. **Cleanup**: Objects destroyed when row is destroyed
4. **Error Recovery**: Graceful handling of file loading failures

### Window Management

1. **Temporary Parent Windows**: Used only for initial window creation
2. **Persistent Chart Windows**: Created once and reused
3. **Proper Cleanup**: Windows destroyed with row objects
4. **Error Handling**: Graceful failure if window creation fails

### File Handling

1. **File Loading**: Files loaded into persistent document objects
2. **Error Recovery**: Fallback to empty bitmap on file errors
3. **State Management**: Document state maintained between renders
4. **Resource Management**: Proper file handle cleanup

## Comparison with Previous Approaches

### Previous Approach (Temporary Objects)
- ❌ Created new objects for each render
- ❌ Frequent allocation/deallocation
- ❌ MFC document management conflicts
- ❌ Performance overhead
- ❌ Memory fragmentation
- ❌ Crashes and instability

### New Approach (Persistent Objects)
- ✅ Objects created once per row
- ✅ Reused throughout row lifetime
- ✅ Stable MFC document references
- ✅ Optimal performance
- ✅ Efficient memory usage
- ✅ Stable and reliable

## Testing Results

After implementing this approach:
- ✅ **No More Crashes**: Application remains stable during frequent updates
- ✅ **Improved Performance**: Faster rendering with less overhead
- ✅ **Consistent Behavior**: Predictable and reliable operation
- ✅ **Better Resource Usage**: Efficient memory and object management
- ✅ **Eliminated MFC Conflicts**: No more document management issues

## Future Considerations

### 1. Optimization Opportunities
- **Object Pooling**: Consider pooling for very large datasets
- **Lazy Loading**: Load file data only when needed
- **Caching**: Cache rendered bitmaps for repeated displays
- **Background Processing**: Handle file loading in background threads

### 2. Enhanced Features
- **Dynamic Updates**: Handle file changes during runtime
- **Memory Monitoring**: Track object memory usage
- **Performance Metrics**: Monitor rendering performance
- **Error Reporting**: Enhanced error reporting and recovery

### 3. Scalability
- **Large Datasets**: Handle very large numbers of rows efficiently
- **Memory Management**: Optimize for memory-constrained environments
- **Threading**: Consider multi-threaded rendering for large datasets
- **Virtualization**: Implement virtual scrolling for large lists

## Conclusion

The persistent document objects approach successfully resolves both the performance and stability issues by:

1. **Eliminating the overhead** of repeated object creation/destruction
2. **Providing stable object references** that work well with MFC
3. **Improving memory efficiency** through object reuse
4. **Enhancing application stability** by avoiding MFC conflicts

This approach is more efficient, stable, and maintainable than the previous temporary object approach. It follows established MFC patterns and provides a solid foundation for future enhancements.
