#pragma once

#include "DataListCtrl_Infos.h"

// Forward declarations
class CDC;
class CBitmap;
class AcqDataDoc;
class CSpikeDoc;

// Character set isolated image generation for DataListCtrl
class CGraphImageList
{
public:
    // Generate images for different display modes
    static CBitmap* GenerateDataImage(int width, int height, 
                                    CString& dataFileName, 
                                    const DataListCtrlInfos& infos);
    
    static CBitmap* GenerateSpikeImage(int width, int height,
                                     CString& spikeFileName,
                                     const DataListCtrlInfos& infos);
    
    static CBitmap* GenerateEmptyImage(int width, int height);

private:
    // Character set isolated rendering methods
    static void RenderDataToDC(CDC* pDC, CString& dataFileName, 
                              const DataListCtrlInfos& infos);
    static void RenderSpikeToDC(CDC* pDC, CString& spikeFileName,
                               const DataListCtrlInfos& infos);
    static void RenderEmptyToDC(CDC* pDC, int width, int height);
    
    // Helper methods for data rendering
    static bool LoadDataDocument(CString& dataFileName, AcqDataDoc* pDataDoc);
    static void RenderDataPlot(CDC* pDC, AcqDataDoc* pDataDoc, 
                              const DataListCtrlInfos& infos);
    
    // Helper methods for spike rendering
    static bool LoadSpikeDocument(CString& spikeFileName, CSpikeDoc* pSpikeDoc);
    static void RenderSpikePlot(CDC* pDC, CSpikeDoc* pSpikeDoc,
                               const DataListCtrlInfos& infos);
};

