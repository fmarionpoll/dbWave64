#pragma once

#include "DataListCtrl_Infos.h"
#include <functional>

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
    static CBitmap* GenerateDataImage(int width, int height, CString& dataFileName, const DataListCtrlInfos& infos);
    static CBitmap* GenerateSpikeImage(int width, int height, CString& spikeFileName, const DataListCtrlInfos& infos);
    static CBitmap* GenerateEmptyImage(int width, int height);
    
    // Build persistent empty bitmap (for backward compatibility)
    static CBitmap* BuildEmptyBitmap(int width, int height, CDC* pDC = nullptr);

private:
    // Common bitmap creation and setup methods
    static CBitmap* CreateBitmap(int width, int height, CDC* pScreenDC);
    static void SetupMemoryDC(CDC& memDC, CBitmap* pBitmap, CDC* pScreenDC);
    static CBitmap* GenerateImageWithRenderer(int width, int height, void (*renderFunction)(CDC*));
    
    // Wrapper functions for rendering (to avoid lambda functions)
    static void RenderDataWrapper(CDC* pDC);
    static void RenderSpikeWrapper(CDC* pDC);
    static void RenderEmptyWrapper(CDC* pDC);
    
    // Static data for wrapper functions
    static CString* s_pDataFileName;
    static CString* s_pSpikeFileName;
    static const DataListCtrlInfos* s_pInfos;
    static int s_width;
    static int s_height;
    
    // Character set isolated rendering methods
    static void render_empty_to_dc(CDC* pDC, int width, int height);
    
    // Helper methods for data rendering
    static void render_data_to_dc(CDC* pDC, CString& dataFileName, const DataListCtrlInfos& infos);
    static bool load_data_document(CString& dataFileName, AcqDataDoc* pDataDoc);
    static void render_data_plot(CDC* pDC, AcqDataDoc* pDataDoc, const DataListCtrlInfos& infos);
    
    // Helper methods for spike rendering
    static void render_spike_to_dc(CDC* pDC, CString& spikeFileName, const DataListCtrlInfos& infos);
    static bool load_spike_document(CString& spikeFileName, CSpikeDoc* pSpikeDoc);
    static void render_spike_plot(CDC* pDC, CSpikeDoc* pSpikeDoc,
                               const DataListCtrlInfos& infos);
    static void render_error_message(CDC* pDC, const DataListCtrlInfos& infos, const CString& message);
};


