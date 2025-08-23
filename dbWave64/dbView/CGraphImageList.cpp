#include "StdAfx.h"
#include "CGraphImageList.h"
#include "ColorNames.h"
#include <../ChartData.h>
#include <../ChartSpikeBar.h>
#include <../AcqDataDoc.h>
#include <../SpikeDoc.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CBitmap* CGraphImageList::GenerateDataImage(int width, int height, 
                                           const CString& dataFileName, 
                                           const DataListCtrlInfos& infos)
{
    // Create bitmap and memory DC
    CBitmap* pBitmap = new CBitmap;
    CDC memDC;
    CDC* pScreenDC = CDC::FromHandle(::GetDC(nullptr));
    
    VERIFY(memDC.CreateCompatibleDC(pScreenDC));
    VERIFY(pBitmap->CreateBitmap(width, height, 
                                pScreenDC->GetDeviceCaps(PLANES),
                                pScreenDC->GetDeviceCaps(BITSPIXEL), nullptr));
    
    memDC.SelectObject(pBitmap);
    memDC.SetMapMode(pScreenDC->GetMapMode());
    
    // Render the data image
    RenderDataToDC(&memDC, dataFileName, infos);
    
    ::ReleaseDC(nullptr, pScreenDC->GetSafeHdc());
    return pBitmap;
}

CBitmap* CGraphImageList::GenerateSpikeImage(int width, int height,
                                            const CString& spikeFileName,
                                            const DataListCtrlInfos& infos)
{
    // Create bitmap and memory DC
    CBitmap* pBitmap = new CBitmap;
    CDC memDC;
    CDC* pScreenDC = CDC::FromHandle(::GetDC(nullptr));
    
    VERIFY(memDC.CreateCompatibleDC(pScreenDC));
    VERIFY(pBitmap->CreateBitmap(width, height, 
                                pScreenDC->GetDeviceCaps(PLANES),
                                pScreenDC->GetDeviceCaps(BITSPIXEL), nullptr));
    
    memDC.SelectObject(pBitmap);
    memDC.SetMapMode(pScreenDC->GetMapMode());
    
    // Render the spike image
    RenderSpikeToDC(&memDC, spikeFileName, infos);
    
    ::ReleaseDC(nullptr, pScreenDC->GetSafeHdc());
    return pBitmap;
}

CBitmap* CGraphImageList::GenerateEmptyImage(int width, int height)
{
    // Create bitmap and memory DC
    CBitmap* pBitmap = new CBitmap;
    CDC memDC;
    CDC* pScreenDC = CDC::FromHandle(::GetDC(nullptr));
    
    VERIFY(memDC.CreateCompatibleDC(pScreenDC));
    VERIFY(pBitmap->CreateBitmap(width, height, 
                                pScreenDC->GetDeviceCaps(PLANES),
                                pScreenDC->GetDeviceCaps(BITSPIXEL), nullptr));
    
    memDC.SelectObject(pBitmap);
    memDC.SetMapMode(pScreenDC->GetMapMode());
    
    // Render the empty image
    RenderEmptyToDC(&memDC, width, height);
    
    ::ReleaseDC(nullptr, pScreenDC->GetSafeHdc());
    return pBitmap;
}

void CGraphImageList::RenderDataToDC(CDC* pDC, const CString& dataFileName, 
                                    const DataListCtrlInfos& infos)
{
    if (dataFileName.IsEmpty())
    {
        // Render "no data" message
        pDC->SetBkColor(col_silver);
        pDC->SetTextColor(col_black);
        pDC->FillSolidRect(0, 0, infos.image_width, infos.image_height, col_silver);
        
        CString message = _T("No data file");
        CRect rect(0, 0, infos.image_width, infos.image_height);
        pDC->DrawText(message, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        return;
    }
    
    // Create document object dynamically (like original code)
    AcqDataDoc* pDataDoc = new AcqDataDoc;
    if (!pDataDoc)
        return;
    
    if (LoadDataDocument(dataFileName, pDataDoc))
    {
        RenderDataPlot(pDC, pDataDoc, infos);
    }
    else
    {
        // Render error message
        pDC->SetBkColor(col_silver);
        pDC->SetTextColor(col_red);
        pDC->FillSolidRect(0, 0, infos.image_width, infos.image_height, col_silver);
        
        CString message = _T("Data file error");
        CRect rect(0, 0, infos.image_width, infos.image_height);
        pDC->DrawText(message, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }
    
    // Clean up
    delete pDataDoc;
}

void CGraphImageList::RenderSpikeToDC(CDC* pDC, const CString& spikeFileName,
                                     const DataListCtrlInfos& infos)
{
    if (spikeFileName.IsEmpty())
    {
        // Render "no spike data" message
        pDC->SetBkColor(col_silver);
        pDC->SetTextColor(col_black);
        pDC->FillSolidRect(0, 0, infos.image_width, infos.image_height, col_silver);
        
        CString message = _T("No spike file");
        CRect rect(0, 0, infos.image_width, infos.image_height);
        pDC->DrawText(message, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        return;
    }
    
    // Create document object dynamically (like original code)
    CSpikeDoc* pSpikeDoc = new CSpikeDoc;
    if (!pSpikeDoc)
        return;
    
    if (LoadSpikeDocument(spikeFileName, pSpikeDoc))
    {
        RenderSpikePlot(pDC, pSpikeDoc, infos);
    }
    else
    {
        // Render error message
        pDC->SetBkColor(col_silver);
        pDC->SetTextColor(col_red);
        pDC->FillSolidRect(0, 0, infos.image_width, infos.image_height, col_silver);
        
        CString message = _T("Spike file error");
        CRect rect(0, 0, infos.image_width, infos.image_height);
        pDC->DrawText(message, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }
    
    // Clean up
    delete pSpikeDoc;
}

void CGraphImageList::RenderEmptyToDC(CDC* pDC, int width, int height)
{
    // Render grey rectangle (same as original empty bitmap)
    pDC->SetBkColor(col_silver);
    pDC->FillSolidRect(0, 0, width, height, col_silver);
    
    // Draw border
    CPen pen;
    pen.CreatePen(PS_SOLID, 1, col_black);
    CPen* pOldPen = pDC->SelectObject(&pen);
    
    CRect rect(1, 0, width, height);
    pDC->Rectangle(&rect);
    
    pDC->SelectObject(pOldPen);
}

bool CGraphImageList::LoadDataDocument(const CString& dataFileName, AcqDataDoc* pDataDoc)
{
    if (!pDataDoc || dataFileName.IsEmpty())
        return false;
    
    // Pass the CString directly (like original code)
    return pDataDoc->open_document(dataFileName);
}

void CGraphImageList::RenderDataPlot(CDC* pDC, AcqDataDoc* pDataDoc, 
                                    const DataListCtrlInfos& infos)
{
    if (!pDataDoc || !pDC)
        return;
    
    // Create a temporary ChartData window object (like original code)
    ChartData* pTempChart = new ChartData;
    if (!pTempChart)
        return;
    
    // Create the window (required for proper MFC functionality)
    if (!pTempChart->Create(_T("TEMP_DATAWND"), WS_CHILD, 
                           CRect(0, 0, infos.image_width, infos.image_height), 
                           nullptr, 0))
    {
        delete pTempChart;
        return;
    }
    
    pTempChart->set_b_use_dib(FALSE);
    
    // Set up the chart with data (like original code)
    pDataDoc->read_data_infos();
    pTempChart->attach_data_file(pDataDoc);
    pTempChart->load_all_channels(infos.data_transform);
    pTempChart->load_data_within_window(infos.b_set_time_span, infos.t_first, infos.t_last);
    pTempChart->adjust_gain(infos.b_set_mv_span, infos.mv_span);
    
    // Render to the provided DC
    pTempChart->plot_data_to_dc(pDC);
    
    // Clean up
    pDataDoc->acq_close_file();
    pTempChart->DestroyWindow();
    delete pTempChart;
}

bool CGraphImageList::LoadSpikeDocument(const CString& spikeFileName, CSpikeDoc* pSpikeDoc)
{
    if (!pSpikeDoc || spikeFileName.IsEmpty())
        return false;
    
    // Pass the CString directly (like original code)
    return pSpikeDoc->OnOpenDocument(spikeFileName);
}

void CGraphImageList::RenderSpikePlot(CDC* pDC, CSpikeDoc* pSpikeDoc,
                                     const DataListCtrlInfos& infos)
{
    if (!pSpikeDoc || !pDC)
        return;
    
    // Create a temporary ChartSpikeBar window object (like original code)
    ChartSpikeBar* pTempSpike = new ChartSpikeBar;
    if (!pTempSpike)
        return;
    
    // Create the window (required for proper MFC functionality)
    if (!pTempSpike->Create(_T("TEMP_SPKWND"), WS_CHILD, 
                           CRect(0, 0, infos.image_width, infos.image_height), 
                           nullptr, 0))
    {
        delete pTempSpike;
        return;
    }
    
    pTempSpike->set_b_use_dib(FALSE);
    pTempSpike->set_display_all_files(false);
    
    // Set up the spike chart (like original code)
    pTempSpike->set_spike_doc(pSpikeDoc);
    pTempSpike->set_plot_mode(infos.spike_plot_mode, infos.selected_class);
    
    // Set time intervals if needed
    if (infos.b_set_time_span)
    {
        long l_first = static_cast<long>(infos.t_first * pSpikeDoc->get_acq_rate());
        long l_last = static_cast<long>(infos.t_last * pSpikeDoc->get_acq_rate());
        pTempSpike->set_time_intervals(l_first, l_last);
    }
    
    // Render to the provided DC
    pTempSpike->plot_data_to_dc(pDC);
    
    // Clean up
    pTempSpike->DestroyWindow();
    delete pTempSpike;
}
