# Solution 3: Custom Image List with Character Set Isolation

## Problem Analysis

The current approach has fundamental issues:

1. **CImageList Character Set Dependency**: The `CImageList::Create()` method is character set dependent
2. **ChartData/ChartSpikeBar Rendering**: These classes have internal character set dependencies
3. **Complex Interaction**: The combination of image list + custom windows + character set creates a fragile system

## Root Cause

The issue is that we're trying to force character set immunity at the wrong level. Instead of trying to make the existing complex system work, we should create a **character set isolated image generation system**.

## Proposed Solution: CGraphImageList

### Concept
Create a custom `CGraphImageList` class that:
1. **Isolates character set dependencies** in image generation
2. **Provides a clean interface** for generating chart images
3. **Eliminates the need for temporary ChartData/ChartSpikeBar windows**

### Architecture

```
DataListCtrl
├── CGraphImageList (new)
│   ├── GenerateDataImage() - Character set isolated
│   ├── GenerateSpikeImage() - Character set isolated
│   └── GenerateEmptyImage() - Character set isolated
└── Standard CImageList (for storage only)
```

### Implementation Plan

#### 1. Create CGraphImageList Class
```cpp
class CGraphImageList
{
public:
    static CBitmap* GenerateDataImage(int width, int height, 
                                    const CString& dataFileName, 
                                    const DataListCtrlInfos& infos);
    
    static CBitmap* GenerateSpikeImage(int width, int height,
                                     const CString& spikeFileName,
                                     const DataListCtrlInfos& infos);
    
    static CBitmap* GenerateEmptyImage(int width, int height);

private:
    // Character set isolated rendering methods
    static void RenderDataToDC(CDC* pDC, const CString& dataFileName, 
                              const DataListCtrlInfos& infos);
    static void RenderSpikeToDC(CDC* pDC, const CString& spikeFileName,
                               const DataListCtrlInfos& infos);
};
```

#### 2. Character Set Isolation Strategy
- **No temporary windows**: Generate images directly to memory DC
- **Explicit character set context**: Use `_T()` macros consistently
- **Isolated rendering**: Separate rendering logic from window management

#### 3. Integration with DataListCtrl
```cpp
// In DataListCtrl_Row::set_display_parameters()
switch (infos->display_mode)
{
case 1: // Data
    {
        CBitmap* pBitmap = CGraphImageList::GenerateDataImage(
            infos->image_width, infos->image_height,
            cs_datafile_name, *infos);
        infos->image_list.Replace(i_image, pBitmap, nullptr);
        delete pBitmap;
    }
    break;
case 2: // Spike
    {
        CBitmap* pBitmap = CGraphImageList::GenerateSpikeImage(
            infos->image_width, infos->image_height,
            cs_spike_file_name, *infos);
        infos->image_list.Replace(i_image, pBitmap, nullptr);
        delete pBitmap;
    }
    break;
default: // Empty
    {
        CBitmap* pBitmap = CGraphImageList::GenerateEmptyImage(
            infos->image_width, infos->image_height);
        infos->image_list.Replace(i_image, pBitmap, nullptr);
        delete pBitmap;
    }
    break;
}
```

## Benefits

1. **True Character Set Immunity**: No character set dependencies in image generation
2. **Simplified Architecture**: Eliminates complex window creation/destruction
3. **Better Performance**: No temporary windows, direct rendering
4. **Maintainable**: Clear separation of concerns
5. **Linux Migration Ready**: Can easily replace Windows-specific rendering

## Alternative: Complete Rewrite

If the above approach is still complex, consider a **complete rewrite** using:

### Option A: Custom Drawing
```cpp
class DataListCtrl : public CListCtrl
{
protected:
    virtual void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
    // Custom drawing for the data column
};
```

### Option B: Owner-Drawn List Control
```cpp
class DataListCtrl : public CListCtrl
{
protected:
    virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
    // Complete control over item rendering
};
```

### Option C: Virtual List Control
```cpp
class DataListCtrl : public CListCtrl
{
protected:
    virtual void OnGetDispInfo(NMHDR* pNMHDR, LRESULT* pResult);
    // Virtual list with custom image generation
};
```

## Recommendation

**Start with Solution 3 (CGraphImageList)** because:
1. It addresses the root cause (character set isolation)
2. It's less invasive than a complete rewrite
3. It provides a clear path forward
4. It maintains compatibility with existing code

If Solution 3 doesn't work, then consider the complete rewrite options.
