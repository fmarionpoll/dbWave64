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
    static CBitmap* GenerateDataImage(int width, int height, CString& dataFileName, const DataListCtrlInfos& infos);
    static CBitmap* GenerateSpikeImage(int width, int height, CString& spikeFileName, const DataListCtrlInfos& infos);
    static CBitmap* GenerateEmptyImage(int width, int height);
    
    // Build persistent empty bitmap (for backward compatibility)
    static CBitmap* BuildEmptyBitmap(int width, int height, CDC* pDC = nullptr);

private:
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


