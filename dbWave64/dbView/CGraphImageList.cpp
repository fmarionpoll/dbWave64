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

CBitmap* CGraphImageList::GenerateDataImage(int width, int height, CString& dataFileName, const DataListCtrlInfos& infos)
{
    TRACE(_T("CGraphImageList::GenerateDataImage\n"));
    // Set up static data for wrapper function
    s_pDataFileName = &dataFileName;
    s_pInfos = &infos;
    
    return GenerateImageWithRenderer(width, height, RenderDataWrapper);
}

// Common bitmap creation and setup methods
CBitmap* CGraphImageList::CreateBitmap(int width, int height, CDC* pScreenDC)
{
    CBitmap* pBitmap = new CBitmap;
    VERIFY(pBitmap->CreateBitmap(width, height, 
                                pScreenDC->GetDeviceCaps(PLANES),
                                pScreenDC->GetDeviceCaps(BITSPIXEL), nullptr));
    return pBitmap;
}

void CGraphImageList::SetupMemoryDC(CDC& memDC, CBitmap* pBitmap, CDC* pScreenDC)
{
    VERIFY(memDC.CreateCompatibleDC(pScreenDC));
    memDC.SelectObject(pBitmap);
    memDC.SetMapMode(pScreenDC->GetMapMode());
}

CBitmap* CGraphImageList::GenerateImageWithRenderer(int width, int height, void (*renderFunction)(CDC*))
{
    // Create bitmap and memory DC
    CDC* pScreenDC = CDC::FromHandle(::GetDC(nullptr));
    CBitmap* pBitmap = CreateBitmap(width, height, pScreenDC);
    
    CDC memDC;
    SetupMemoryDC(memDC, pBitmap, pScreenDC);
    
    // Render using the provided function
    renderFunction(&memDC);
    
    // Cleanup
    ::ReleaseDC(nullptr, pScreenDC->GetSafeHdc());
    return pBitmap;
}

// Static data for wrapper functions
CString* CGraphImageList::s_pDataFileName = nullptr;
CString* CGraphImageList::s_pSpikeFileName = nullptr;
const DataListCtrlInfos* CGraphImageList::s_pInfos = nullptr;
int CGraphImageList::s_width = 0;
int CGraphImageList::s_height = 0;

// Wrapper functions for rendering
void CGraphImageList::RenderDataWrapper(CDC* pDC)
{
    if (s_pDataFileName && s_pInfos)
    {
        render_data_to_dc(pDC, *s_pDataFileName, *s_pInfos);
    }
}

void CGraphImageList::RenderSpikeWrapper(CDC* pDC)
{
    if (s_pSpikeFileName && s_pInfos)
    {
        render_spike_to_dc(pDC, *s_pSpikeFileName, *s_pInfos);
    }
}

void CGraphImageList::RenderEmptyWrapper(CDC* pDC)
{
    render_empty_to_dc(pDC, s_width, s_height);
}

CBitmap* CGraphImageList::GenerateSpikeImage(int width, int height, CString& spikeFileName, const DataListCtrlInfos& infos)
{
    TRACE(_T("CGraphImageList::GenerateSpikeImage\n"));
    // Set up static data for wrapper function
    s_pSpikeFileName = &spikeFileName;
    s_pInfos = &infos;
    
    return GenerateImageWithRenderer(width, height, RenderSpikeWrapper);
}

CBitmap* CGraphImageList::GenerateEmptyImage(int width, int height)
{
    // Set up static data for wrapper function
    s_width = width;
    s_height = height;
    
    return GenerateImageWithRenderer(width, height, RenderEmptyWrapper);
}

void CGraphImageList::render_error_message(CDC* pDC, const DataListCtrlInfos& infos, const CString& message)
{
	constexpr COLORREF background = col_gray;
    pDC->FillSolidRect(0, 0, infos.image_width, infos.image_height, background);
    CRect rect(0, 0, infos.image_width, infos.image_height);
    pDC->SetTextColor(col_white);
    pDC->SetBkColor(background);
    pDC->DrawText(message, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    return;
}

void CGraphImageList::render_data_to_dc(CDC* pDC, CString& dataFileName, 
                                    const DataListCtrlInfos& infos)
{
    TRACE(_T("CGraphImageList::render_data_to_dc\n"));
    if (dataFileName.IsEmpty())
    {
        CString message = _T("No data file");
        render_error_message(pDC, infos, message);
        return;
    }

    auto pDataDoc = new AcqDataDoc;
    if (!pDataDoc)
        return;
    
    if (load_data_document(dataFileName, pDataDoc))
    {
        render_data_plot(pDC, pDataDoc, infos);
    }
    else
    {
        CString message = _T("Data file error");
        render_error_message(pDC, infos, message);
    }
    
    // Clean up
    delete pDataDoc;
}

void CGraphImageList::render_spike_to_dc(CDC* pDC, CString& spikeFileName,
                                     const DataListCtrlInfos& infos)
{
    if (spikeFileName.IsEmpty())
    {
        CString message = _T("No spike file");
        return render_error_message(pDC, infos, message);
    }
    
    // Try to render spike data, but fall back to error message if it fails
    bool renderSuccess = false;
    
    // Attempt 1: Try with CSpikeDoc (original approach)
    CSpikeDoc* pSpikeDoc = nullptr;
    try {
        pSpikeDoc = new CSpikeDoc;
        if (pSpikeDoc && load_spike_document(spikeFileName, pSpikeDoc)) {
            render_spike_plot(pDC, pSpikeDoc, infos);
            renderSuccess = true;
        }
    } catch (...) {
        // CSpikeDoc approach failed, will try fallback
        renderSuccess = false;
    }
    
    // Clean up CSpikeDoc if it was created
    if (pSpikeDoc) {
        try {
            delete pSpikeDoc;
        } catch (...) {
            // Ignore cleanup exceptions
        }
    }
    
    // If CSpikeDoc approach failed, render a fallback message
    if (!renderSuccess) {
        // Render fallback message
        pDC->SetBkColor(col_silver);
        pDC->SetTextColor(col_blue);
        pDC->FillSolidRect(0, 0, infos.image_width, infos.image_height, col_silver);
        
        CString message = _T("Spike data: ") + spikeFileName;
        CRect rect(0, 0, infos.image_width, infos.image_height);
        pDC->DrawText(message, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        
        // Draw a simple spike-like pattern to indicate spike data
        CPen pen;
        pen.CreatePen(PS_SOLID, 1, col_black);
        CPen* pOldPen = pDC->SelectObject(&pen);
        
        int centerY = infos.image_height / 2;
        int step = infos.image_width / 20;
        
        for (int x = 10; x < infos.image_width - 10; x += step) {
            pDC->MoveTo(x, centerY - 10);
            pDC->LineTo(x, centerY + 10);
        }
        
        pDC->SelectObject(pOldPen);
    }
}

void CGraphImageList::render_empty_to_dc(CDC* pDC, int width, int height)
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

CBitmap* CGraphImageList::BuildEmptyBitmap(int width, int height, CDC* pDC)
{
    CDC* pScreenDC = nullptr;
    bool needCleanup = false;
    
    if (pDC)
    {
        // Use provided DC if available
        pScreenDC = pDC;
    }
    else
    {
        // Create screen DC if none provided
        pScreenDC = CDC::FromHandle(::GetDC(nullptr));
        needCleanup = true;
    }
    
    // Create bitmap and memory DC
    CBitmap* pBitmap = CreateBitmap(width, height, pScreenDC);
    
    CDC memDC;
    SetupMemoryDC(memDC, pBitmap, pScreenDC);
    
    // Render the empty image
    render_empty_to_dc(&memDC, width, height);
    
    // Clean up screen DC if we created it
    if (needCleanup)
    {
        ::ReleaseDC(nullptr, pScreenDC->GetSafeHdc());
    }
    
    return pBitmap;
}

bool CGraphImageList::load_data_document(CString& dataFileName, AcqDataDoc* pDataDoc)
{
    TRACE(_T("CGraphImageList::load_data_document\n"));
    if (!pDataDoc || dataFileName.IsEmpty())
        return false;
    
    // Pass the CString directly (like original code)
    return pDataDoc->open_document(dataFileName);
}

void CGraphImageList::render_data_plot(CDC* pDC, AcqDataDoc* pDataDoc, 
                                    const DataListCtrlInfos& infos)
{
    if (!pDataDoc || !pDC)
        return;
    
    // Create ChartData with exception handling
    ChartData* pTempChart = nullptr;
    try {
        TRACE(_T("CGraphImageList::RenderDataPlot - Allocating ChartData\n"));
        pTempChart = new ChartData;
        TRACE(_T("CGraphImageList::RenderDataPlot - pTempChart = %p\n"), pTempChart);
    } catch (...) {
        TRACE(_T("CGraphImageList::RenderDataPlot - Exception during ChartData allocation\n"));
        return;
    }
    
    if (!pTempChart || pTempChart == nullptr) {
        TRACE(_T("CGraphImageList::RenderDataPlot - ChartData allocation failed\n"));
        return;
    }
    
    // Create a temporary parent window for the ChartData
    CWnd tempParent;
    if (!tempParent.CreateEx(0, _T("STATIC"), _T("TEMP_PARENT"), 
                            WS_POPUP | WS_VISIBLE,
                            CRect(0, 0, 1, 1), nullptr, 0))
    {
        TRACE(_T("CGraphImageList::RenderDataPlot - Failed to create temporary parent window\n"));
        delete pTempChart;
        return;
    }
    
    // Create the ChartData window as a child of the temporary parent
    TRACE(_T("CGraphImageList::RenderDataPlot - Creating ChartData window\n"));
    if (!pTempChart->Create(_T("TEMP_DATAWND"), WS_CHILD, 
                           CRect(0, 0, infos.image_width, infos.image_height), 
                           &tempParent, 0))
    {
        TRACE(_T("CGraphImageList::RenderDataPlot - ChartData window creation failed\n"));
        tempParent.DestroyWindow();
        delete pTempChart;
        return;
    }
    TRACE(_T("CGraphImageList::RenderDataPlot - ChartData window created successfully\n"));
    
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
    tempParent.DestroyWindow();
    delete pTempChart;
}

bool CGraphImageList::load_spike_document(CString& spikeFileName, CSpikeDoc* pSpikeDoc)
{
    if (!pSpikeDoc || spikeFileName.IsEmpty())
        return false;
    
    // Pass the CString directly (like original code)
    return pSpikeDoc->OnOpenDocument(spikeFileName);
}

void CGraphImageList::render_spike_plot(CDC* pDC, CSpikeDoc* pSpikeDoc,
                                     const DataListCtrlInfos& infos)
{
    if (!pSpikeDoc || !pDC)
        return;
    
    // Create ChartSpikeBar with exception handling
    ChartSpikeBar* pTempSpike = nullptr;
    try {
        TRACE(_T("CGraphImageList::RenderSpikePlot - Allocating ChartSpikeBar\n"));
        pTempSpike = new ChartSpikeBar;
        TRACE(_T("CGraphImageList::RenderSpikePlot - pTempSpike = %p\n"), pTempSpike);
    } catch (...) {
        TRACE(_T("CGraphImageList::RenderSpikePlot - Exception during ChartSpikeBar allocation\n"));
        return;
    }
    
    if (!pTempSpike || pTempSpike == nullptr) {
        TRACE(_T("CGraphImageList::RenderSpikePlot - ChartSpikeBar allocation failed\n"));
        return;
    }
    
    // Create a temporary parent window for the ChartSpikeBar
    CWnd tempParent;
    if (!tempParent.CreateEx(0, _T("STATIC"), _T("TEMP_PARENT"), 
                            WS_POPUP | WS_VISIBLE,
                            CRect(0, 0, 1, 1), nullptr, 0))
    {
        TRACE(_T("CGraphImageList::RenderSpikePlot - Failed to create temporary parent window\n"));
        delete pTempSpike;
        return;
    }
    
    // Create the ChartSpikeBar window as a child of the temporary parent
    TRACE(_T("CGraphImageList::RenderSpikePlot - Creating ChartSpikeBar window\n"));
    if (!pTempSpike->Create(_T("TEMP_SPKWND"), WS_CHILD, 
                           CRect(0, 0, infos.image_width, infos.image_height), 
                           &tempParent, 0))
    {
        TRACE(_T("CGraphImageList::RenderSpikePlot - ChartSpikeBar window creation failed\n"));
        tempParent.DestroyWindow();
        delete pTempSpike;
        return;
    }
    TRACE(_T("CGraphImageList::RenderSpikePlot - ChartSpikeBar window created successfully\n"));
    
    pTempSpike->set_b_use_dib(FALSE);
    pTempSpike->set_display_all_files(false);
    
    // Set up the spike chart (like original code) with safety checks
    if (pSpikeDoc) {
        try {
            pTempSpike->set_spike_doc(pSpikeDoc);
            pTempSpike->set_plot_mode(infos.spike_plot_mode, infos.selected_class);
        } catch (...) {
            TRACE(_T("CGraphImageList::RenderSpikePlot - Failed to set spike doc or plot mode\n"));
        }
    }
    
    // Set time intervals if needed
    if (infos.b_set_time_span && pSpikeDoc)
    {
        try {
            long l_first = static_cast<long>(infos.t_first * pSpikeDoc->get_acq_rate());
            long l_last = static_cast<long>(infos.t_last * pSpikeDoc->get_acq_rate());
            pTempSpike->set_time_intervals(l_first, l_last);
        } catch (...) {
            // Handle time interval setting failure
            TRACE(_T("CGraphImageList::RenderSpikePlot - Failed to set time intervals\n"));
        }
    }
    
    // Render to the provided DC
    pTempSpike->plot_data_to_dc(pDC);
    
    // Clean up
    pTempSpike->DestroyWindow();
    tempParent.DestroyWindow();
    delete pTempSpike;
}
