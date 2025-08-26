#include "pch.h"
#include "CGraphImageListTests.h"
#include <afxwin.h>
#include <afxext.h>
#include <gdiplus.h>
#include <filesystem>

// Static member initialization
std::vector<CString> TestDataFixture::createdFiles;

// CGraphImageListTestBase implementation
bool CGraphImageListTestBase::VerifyBitmapProperties(CBitmap* pBitmap, int expectedWidth, int expectedHeight)
{
    if (!pBitmap) return false;
    
    BITMAP bm;
    if (pBitmap->GetBitmap(&bm))
    {
        return (bm.bmWidth == expectedWidth && bm.bmHeight == expectedHeight);
    }
    return false;
}

CString CGraphImageListTestBase::CreateTestDataFile()
{
    return TestDataFixture::CreateTestDataFile();
}

CString CGraphImageListTestBase::CreateTestSpikeFile()
{
    return TestDataFixture::CreateTestSpikeFile();
}

CString CGraphImageListTestBase::CreateCorruptedSpikeFile()
{
    return TestDataFixture::CreateCorruptedSpikeFile();
}

void CGraphImageListTestBase::CleanupTestFiles()
{
    TestDataFixture::CleanupTestFiles();
}

DataListCtrlInfos* CGraphImageListTestBase::CreateTestInfos(int width, int height)
{
    DataListCtrlInfos* pInfos = new DataListCtrlInfos();
    // Initialize with test data
    pInfos->image_width = width;
    pInfos->image_height = height;
    pInfos->parent = nullptr;
    pInfos->p_empty_bitmap = nullptr;
    pInfos->data_transform = 0;
    pInfos->display_mode = 1;
    pInfos->spike_plot_mode = PLOT_BLACK;
    pInfos->selected_class = 0;
    pInfos->t_first = 0.0f;
    pInfos->t_last = 0.0f;
    pInfos->mv_span = 0.0f;
    pInfos->b_set_time_span = false;
    pInfos->b_set_mv_span = false;
    pInfos->b_display_file_name = false;
    return pInfos;
}

void CGraphImageListTestBase::AddTestMessage(const std::string& message)
{
    testMessages.push_back(message);
    std::cout << "Test Message: " << message << std::endl;
}

// TestDataFixture implementation
CString TestDataFixture::GenerateUniqueFilename(const CString& prefix, const CString& extension)
{
    static int counter = 0;
    CString filename;
    filename.Format(_T("%s_test_%d%s"), prefix, ++counter, extension);
    return filename;
}

CString TestDataFixture::CreateTestDataFile()
{
    CString filename = GenerateUniqueFilename(_T("data"), _T(".dat"));
    CString fullPath = _T("test_data\\") + filename;
    
    // Create directory if it doesn't exist
    CreateDirectory(_T("test_data"), NULL);
    
    std::ofstream file(fullPath);
    if (file.is_open())
    {
        // Write test data content
        file << "Test data file content" << std::endl;
        file << "Width: 640" << std::endl;
        file << "Height: 480" << std::endl;
        file << "Channels: 3" << std::endl;
        file.close();
        
        createdFiles.push_back(fullPath);
        return fullPath;
    }
    
    return _T("");
}

CString TestDataFixture::CreateTestSpikeFile()
{
    CString filename = GenerateUniqueFilename(_T("spike"), _T(".spk"));
    CString fullPath = _T("test_data\\") + filename;
    
    // Create directory if it doesn't exist
    CreateDirectory(_T("test_data"), NULL);
    
    std::ofstream file(fullPath, std::ios::binary);
    if (file.is_open())
    {
        // Write spike file header
        int header[] = { 0x12345678, 100, 200, 300 }; // Magic number, width, height, channels
        file.write(reinterpret_cast<const char*>(header), sizeof(header));
        
        // Write some spike data
        for (int i = 0; i < 1000; ++i)
        {
            float spikeValue = static_cast<float>(i) * 0.1f;
            file.write(reinterpret_cast<const char*>(&spikeValue), sizeof(float));
        }
        
        file.close();
        createdFiles.push_back(fullPath);
        return fullPath;
    }
    
    return _T("");
}

CString TestDataFixture::CreateCorruptedSpikeFile()
{
    CString filename = GenerateUniqueFilename(_T("corrupted"), _T(".spk"));
    CString fullPath = _T("test_data\\") + filename;
    
    // Create directory if it doesn't exist
    CreateDirectory(_T("test_data"), NULL);
    
    std::ofstream file(fullPath, std::ios::binary);
    if (file.is_open())
    {
        // Write corrupted header
        int header[] = { 0xDEADBEEF, -1, -1, 999 }; // Invalid magic number and dimensions
        file.write(reinterpret_cast<const char*>(header), sizeof(header));
        
        // Write some random corrupted data
        for (int i = 0; i < 100; ++i)
        {
            char randomData[] = { 0xFF, 0x00, 0xAA, 0x55 };
            file.write(randomData, sizeof(randomData));
        }
        
        file.close();
        createdFiles.push_back(fullPath);
        return fullPath;
    }
    
    return _T("");
}

CString TestDataFixture::CreateLargeSpikeFile()
{
    CString filename = GenerateUniqueFilename(_T("large"), _T(".spk"));
    CString fullPath = _T("test_data\\") + filename;
    
    // Create directory if it doesn't exist
    CreateDirectory(_T("test_data"), NULL);
    
    std::ofstream file(fullPath, std::ios::binary);
    if (file.is_open())
    {
        // Write large spike file header
        int header[] = { 0x12345678, 1920, 1080, 3 }; // Large dimensions
        file.write(reinterpret_cast<const char*>(header), sizeof(header));
        
        // Write large amount of spike data
        for (int i = 0; i < 100000; ++i)
        {
            float spikeValue = static_cast<float>(i) * 0.01f;
            file.write(reinterpret_cast<const char*>(&spikeValue), sizeof(float));
        }
        
        file.close();
        createdFiles.push_back(fullPath);
        return fullPath;
    }
    
    return _T("");
}

CString TestDataFixture::CreateEmptySpikeFile()
{
    CString filename = GenerateUniqueFilename(_T("empty"), _T(".spk"));
    CString fullPath = _T("test_data\\") + filename;
    
    // Create directory if it doesn't exist
    CreateDirectory(_T("test_data"), NULL);
    
    std::ofstream file(fullPath, std::ios::binary);
    if (file.is_open())
    {
        // Write minimal header only
        int header[] = { 0x12345678, 0, 0, 0 };
        file.write(reinterpret_cast<const char*>(header), sizeof(header));
        
        file.close();
        createdFiles.push_back(fullPath);
        return fullPath;
    }
    
    return _T("");
}

CString CGraphImageListTestBase::CreateEmptySpikeFile()
{
    CString filename = _T("test_data\\empty_spike_test.spk");
    CString fullPath = _T("test_data\\") + filename;
    
    // Create directory if it doesn't exist
    CreateDirectory(_T("test_data"), NULL);
    
    std::ofstream file(fullPath, std::ios::binary);
    if (file.is_open())
    {
        // Create an empty file
        file.close();
        return fullPath;
    }
    
    return _T("");
}

CString CGraphImageListTestBase::CreateLargeSpikeFile()
{
    CString filename = _T("test_data\\large_spike_test.spk");
    CString fullPath = _T("test_data\\") + filename;
    
    // Create directory if it doesn't exist
    CreateDirectory(_T("test_data"), NULL);
    
    std::ofstream file(fullPath, std::ios::binary);
    if (file.is_open())
    {
        // Write a larger spike file with more data
        int header[] = { 0x12345678, 1000, 0, 0 }; // 1000 spikes
        file.write(reinterpret_cast<const char*>(header), sizeof(header));
        
        // Write some spike data
        for (int i = 0; i < 1000; ++i)
        {
            float time = static_cast<float>(i) * 0.001f;
            float amplitude = static_cast<float>(i % 100) * 0.1f;
            int class_id = i % 5;
            
            file.write(reinterpret_cast<const char*>(&time), sizeof(float));
            file.write(reinterpret_cast<const char*>(&amplitude), sizeof(float));
            file.write(reinterpret_cast<const char*>(&class_id), sizeof(int));
        }
        
        file.close();
        return fullPath;
    }
    
    return _T("");
}

void TestDataFixture::CleanupTestFiles()
{
    for (const auto& file : createdFiles)
    {
        DeleteFile(file);
    }
    createdFiles.clear();
    
    // Remove test directory if empty
    RemoveDirectory(_T("test_data"));
}

// TestHelpers implementation
bool TestHelpers::VerifyBitmapProperties(CBitmap* pBitmap, int expectedWidth, int expectedHeight)
{
    if (!pBitmap) return false;
    
    BITMAP bm;
    if (pBitmap->GetBitmap(&bm))
    {
        return (bm.bmWidth == expectedWidth && bm.bmHeight == expectedHeight);
    }
    return false;
}

void TestHelpers::SaveBitmapToFile(CBitmap* pBitmap, const CString& filename)
{
    if (!pBitmap) return;
    
    CDC* pDC = CreateCompatibleDC();
    if (pDC)
    {
        CBitmap* pOldBitmap = pDC->SelectObject(pBitmap);
        
        BITMAP bm;
        pBitmap->GetBitmap(&bm);
        
        // Save bitmap to file using GDI+
        // This is a simplified version - in practice you'd use GDI+ or other methods
        std::wcout << L"Saving bitmap " << filename << L" (" << bm.bmWidth << L"x" << bm.bmHeight << L")" << std::endl;
        
        pDC->SelectObject(pOldBitmap);
        delete pDC;
    }
}

CBitmap* TestHelpers::LoadBitmapFromFile(const CString& filename)
{
    // This is a placeholder - in practice you'd implement bitmap loading
    CBitmap* pBitmap = new CBitmap();
    // Load bitmap from file
    return pBitmap;
}

void TestHelpers::LogBitmapInfo(CBitmap* pBitmap, const CString& context)
{
    if (!pBitmap)
    {
        std::wcout << context << L": NULL bitmap" << std::endl;
        return;
    }
    
    BITMAP bm;
    if (pBitmap->GetBitmap(&bm))
    {
        std::wcout << context << L": " << bm.bmWidth << L"x" << bm.bmHeight 
                   << L", " << bm.bmBitsPixel << L" bits per pixel" << std::endl;
    }
    else
    {
        std::wcout << context << L": Failed to get bitmap info" << std::endl;
    }
}

CDC* TestHelpers::CreateCompatibleDC()
{
    CDC* pDC = new CDC();
    pDC->CreateCompatibleDC(NULL);
    return pDC;
}
