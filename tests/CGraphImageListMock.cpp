#include "pch.h"
#include "CGraphImageListTests.h"

// Mock implementation of CGraphImageList methods for testing
// This avoids the complex dependencies of the real implementation

CBitmap* CGraphImageList::GenerateEmptyImage(int width, int height)
{
    // Create a simple empty bitmap for testing
    CDC* pScreenDC = CDC::FromHandle(::GetDC(nullptr));
    CBitmap* pBitmap = new CBitmap;
    
    if (pBitmap->CreateBitmap(width, height, 
                             pScreenDC->GetDeviceCaps(PLANES),
                             pScreenDC->GetDeviceCaps(BITSPIXEL), nullptr))
    {
        // Fill with white background
        CDC memDC;
        memDC.CreateCompatibleDC(pScreenDC);
        CBitmap* pOldBitmap = memDC.SelectObject(pBitmap);
        memDC.FillSolidRect(0, 0, width, height, RGB(255, 255, 255));
        memDC.SelectObject(pOldBitmap);
    }
    
    ::ReleaseDC(nullptr, pScreenDC->GetSafeHdc());
    return pBitmap;
}

CBitmap* CGraphImageList::GenerateDataImage(int width, int height, CString& dataFileName, const DataListCtrlInfos& infos)
{
    // Create a simple data image for testing
    CDC* pScreenDC = CDC::FromHandle(::GetDC(nullptr));
    CBitmap* pBitmap = new CBitmap;
    
    if (pBitmap->CreateBitmap(width, height, 
                             pScreenDC->GetDeviceCaps(PLANES),
                             pScreenDC->GetDeviceCaps(BITSPIXEL), nullptr))
    {
        // Fill with light blue background to simulate data
        CDC memDC;
        memDC.CreateCompatibleDC(pScreenDC);
        CBitmap* pOldBitmap = memDC.SelectObject(pBitmap);
        memDC.FillSolidRect(0, 0, width, height, RGB(200, 220, 255));
        memDC.SelectObject(pOldBitmap);
    }
    
    ::ReleaseDC(nullptr, pScreenDC->GetSafeHdc());
    return pBitmap;
}

CBitmap* CGraphImageList::GenerateSpikeImage(int width, int height, CString& spikeFileName, const DataListCtrlInfos& infos)
{
    // Create a simple spike image for testing
    CDC* pScreenDC = CDC::FromHandle(::GetDC(nullptr));
    CBitmap* pBitmap = new CBitmap;
    
    if (pBitmap->CreateBitmap(width, height, 
                             pScreenDC->GetDeviceCaps(PLANES),
                             pScreenDC->GetDeviceCaps(BITSPIXEL), nullptr))
    {
        // Fill with light red background to simulate spikes
        CDC memDC;
        memDC.CreateCompatibleDC(pScreenDC);
        CBitmap* pOldBitmap = memDC.SelectObject(pBitmap);
        memDC.FillSolidRect(0, 0, width, height, RGB(255, 200, 200));
        memDC.SelectObject(pOldBitmap);
    }
    
    ::ReleaseDC(nullptr, pScreenDC->GetSafeHdc());
    return pBitmap;
}
