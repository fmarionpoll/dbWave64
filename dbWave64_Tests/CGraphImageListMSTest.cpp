#include "CGraphImageListMSTest.h"
#include <chrono>
#include <sstream>
#include <iomanip>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

// Static member initialization
std::vector<CString> TestDataFixture::createdFiles;

// Implementation of CGraphImageListTestBase methods
bool CGraphImageListTestBase::VerifyBitmapProperties(CBitmap* pBitmap, int expectedWidth, int expectedHeight)
{
    if (pBitmap == nullptr) {
        AddTestMessage("Bitmap is null");
        return false;
    }
    
    BITMAP bm;
    if (!pBitmap->GetBitmap(&bm)) {
        AddTestMessage("Failed to get bitmap information");
        return false;
    }
    
    bool widthMatch = (bm.bmWidth == expectedWidth);
    bool heightMatch = (bm.bmHeight == expectedHeight);
    
    if (!widthMatch || !heightMatch) {
        std::ostringstream oss;
        oss << "Bitmap dimensions mismatch. Expected: " << expectedWidth << "x" << expectedHeight 
            << ", Actual: " << bm.bmWidth << "x" << bm.bmHeight;
        AddTestMessage(oss.str());
        return false;
    }
    
    return true;
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
    pInfos->image_width = width;
    pInfos->image_height = height;
    return pInfos;
}

void CGraphImageListTestBase::AddTestMessage(const std::string& message)
{
    testMessages.push_back(message);
    Logger::WriteMessage(L"Test Message: " + std::wstring(message.begin(), message.end()).c_str());
}

// Implementation of TestDataFixture methods
CString TestDataFixture::GenerateUniqueFilename(const CString& prefix, const CString& extension)
{
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&time_t), "%Y%m%d_%H%M%S");
    oss << "_" << std::setfill('0') << std::setw(3) << ms.count();
    
    CString timestamp(oss.str().c_str());
    CString filename = prefix + _T("_") + timestamp + extension;
    
    return filename;
}

CString TestDataFixture::CreateTestDataFile()
{
    CString filename = GenerateUniqueFilename(_T("test_data"), _T(".dat"));
    
    // Create a minimal valid data file for testing
    std::ofstream file(filename.GetString());
    if (file.is_open()) {
        // Write header information
        file << "DataFile" << std::endl;
        file << "Version: 1.0" << std::endl;
        file << "Channels: 4" << std::endl;
        file << "Samples: 1000" << std::endl;
        
        // Write some sample data
        for (int i = 0; i < 1000; ++i) {
            file << i << " " << (i * 0.1) << " " << (i * 0.2) << " " << (i * 0.3) << std::endl;
        }
        
        file.close();
        createdFiles.push_back(filename);
        
        Logger::WriteMessage(L"Created test data file: " + filename);
    } else {
        Assert::Fail(L"Failed to create test data file: " + filename);
    }
    
    return filename;
}

CString TestDataFixture::CreateTestSpikeFile()
{
    CString filename = GenerateUniqueFilename(_T("test_spike"), _T(".spk"));
    
    // Create a minimal valid spike file for testing
    std::ofstream file(filename.GetString(), std::ios::binary);
    if (file.is_open()) {
        // Write spike file header
        char header[] = "SPIKE";
        file.write(header, sizeof(header));
        
        // Write version
        int version = 1;
        file.write(reinterpret_cast<char*>(&version), sizeof(version));
        
        // Write number of spikes
        int numSpikes = 100;
        file.write(reinterpret_cast<char*>(&numSpikes), sizeof(numSpikes));
        
        // Write spike data
        for (int i = 0; i < numSpikes; ++i) {
            double timestamp = i * 0.001; // 1ms intervals
            int channel = i % 4;
            double amplitude = 1.0 + (i * 0.1);
            
            file.write(reinterpret_cast<char*>(&timestamp), sizeof(timestamp));
            file.write(reinterpret_cast<char*>(&channel), sizeof(channel));
            file.write(reinterpret_cast<char*>(&amplitude), sizeof(amplitude));
        }
        
        file.close();
        createdFiles.push_back(filename);
        
        Logger::WriteMessage(L"Created test spike file: " + filename);
    } else {
        Assert::Fail(L"Failed to create test spike file: " + filename);
    }
    
    return filename;
}

CString TestDataFixture::CreateCorruptedSpikeFile()
{
    CString filename = GenerateUniqueFilename(_T("test_corrupted"), _T(".spk"));
    
    // Create a corrupted spike file for testing error handling
    std::ofstream file(filename.GetString(), std::ios::binary);
    if (file.is_open()) {
        // Write invalid header
        char header[] = "INVALID";
        file.write(header, sizeof(header));
        
        // Write corrupted data
        for (int i = 0; i < 100; ++i) {
            char randomData[16];
            for (int j = 0; j < 16; ++j) {
                randomData[j] = static_cast<char>(rand() % 256);
            }
            file.write(randomData, sizeof(randomData));
        }
        
        file.close();
        createdFiles.push_back(filename);
        
        Logger::WriteMessage(L"Created corrupted spike file: " + filename);
    } else {
        Assert::Fail(L"Failed to create corrupted spike file: " + filename);
    }
    
    return filename;
}

CString TestDataFixture::CreateLargeSpikeFile()
{
    CString filename = GenerateUniqueFilename(_T("test_large"), _T(".spk"));
    
    // Create a large spike file for stress testing
    std::ofstream file(filename.GetString(), std::ios::binary);
    if (file.is_open()) {
        // Write spike file header
        char header[] = "SPIKE";
        file.write(header, sizeof(header));
        
        // Write version
        int version = 1;
        file.write(reinterpret_cast<char*>(&version), sizeof(version));
        
        // Write number of spikes (large number for stress testing)
        int numSpikes = 10000;
        file.write(reinterpret_cast<char*>(&numSpikes), sizeof(numSpikes));
        
        // Write large amount of spike data
        for (int i = 0; i < numSpikes; ++i) {
            double timestamp = i * 0.0001; // 0.1ms intervals
            int channel = i % 8;
            double amplitude = 1.0 + (i * 0.01);
            
            file.write(reinterpret_cast<char*>(&timestamp), sizeof(timestamp));
            file.write(reinterpret_cast<char*>(&channel), sizeof(channel));
            file.write(reinterpret_cast<char*>(&amplitude), sizeof(amplitude));
        }
        
        file.close();
        createdFiles.push_back(filename);
        
        Logger::WriteMessage(L"Created large spike file: " + filename);
    } else {
        Assert::Fail(L"Failed to create large spike file: " + filename);
    }
    
    return filename;
}

CString TestDataFixture::CreateEmptySpikeFile()
{
    CString filename = GenerateUniqueFilename(_T("test_empty"), _T(".spk"));
    
    // Create an empty spike file for edge case testing
    std::ofstream file(filename.GetString(), std::ios::binary);
    if (file.is_open()) {
        // Write minimal valid header
        char header[] = "SPIKE";
        file.write(header, sizeof(header));
        
        // Write version
        int version = 1;
        file.write(reinterpret_cast<char*>(&version), sizeof(version));
        
        // Write zero spikes
        int numSpikes = 0;
        file.write(reinterpret_cast<char*>(&numSpikes), sizeof(numSpikes));
        
        file.close();
        createdFiles.push_back(filename);
        
        Logger::WriteMessage(L"Created empty spike file: " + filename);
    } else {
        Assert::Fail(L"Failed to create empty spike file: " + filename);
    }
    
    return filename;
}

void TestDataFixture::CleanupTestFiles()
{
    for (const auto& file : createdFiles) {
        if (DeleteFile(file)) {
            Logger::WriteMessage(L"Cleaned up test file: " + file);
        } else {
            Logger::WriteMessage(L"Failed to clean up test file: " + file);
        }
    }
    createdFiles.clear();
}

// Implementation of TestHelpers methods
bool TestHelpers::VerifyBitmapProperties(CBitmap* pBitmap, int expectedWidth, int expectedHeight)
{
    if (pBitmap == nullptr) {
        return false;
    }
    
    BITMAP bm;
    if (!pBitmap->GetBitmap(&bm)) {
        return false;
    }
    
    return (bm.bmWidth == expectedWidth && bm.bmHeight == expectedHeight);
}

void TestHelpers::SaveBitmapToFile(CBitmap* pBitmap, const CString& filename)
{
    if (pBitmap == nullptr) {
        Assert::Fail(L"Cannot save null bitmap to file");
        return;
    }
    
    // Implementation would save bitmap to file
    // This is a placeholder for actual implementation
    Logger::WriteMessage(L"Saving bitmap to file: " + filename);
}

CBitmap* TestHelpers::LoadBitmapFromFile(const CString& filename)
{
    // Implementation would load bitmap from file
    // This is a placeholder for actual implementation
    Logger::WriteMessage(L"Loading bitmap from file: " + filename);
    return nullptr;
}

void TestHelpers::LogBitmapInfo(CBitmap* pBitmap, const CString& context)
{
    if (pBitmap == nullptr) {
        Logger::WriteMessage(L"Bitmap is null in context: " + context);
        return;
    }
    
    BITMAP bm;
    if (pBitmap->GetBitmap(&bm)) {
        std::wostringstream oss;
        oss << L"Bitmap info (" << context << L"): " 
            << bm.bmWidth << L"x" << bm.bmHeight 
            << L", Bits per pixel: " << bm.bmBitsPixel
            << L", Bytes per row: " << bm.bmWidthBytes;
        Logger::WriteMessage(oss.str().c_str());
    } else {
        Logger::WriteMessage(L"Failed to get bitmap info for context: " + context);
    }
}

CDC* TestHelpers::CreateCompatibleDC()
{
    CDC* pDC = new CDC();
    CDC* pScreenDC = CDC::FromHandle(::GetDC(NULL));
    pDC->CreateCompatibleDC(pScreenDC);
    ::ReleaseDC(NULL, pScreenDC->GetSafeHdc());
    return pDC;
}

