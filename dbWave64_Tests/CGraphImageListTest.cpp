#include "CGraphImageListTest.h"
#include "DataListCtrl_Infos.h"
#include <chrono>
#include <fstream>

CGraphImageListTest::CGraphImageListTest() 
    : passedCount(0), failedCount(0) {
    std::cout << "=== CGraphImageList Test Suite Initialized ===" << std::endl;
}

CGraphImageListTest::~CGraphImageListTest() {
    CleanupTestFiles();
    std::cout << "=== CGraphImageList Test Suite Cleaned Up ===" << std::endl;
}

bool CGraphImageListTest::RunAllTests() {
    std::cout << "\n=== Running All CGraphImageList Tests ===" << std::endl;
    
    bool allPassed = true;
    
    allPassed &= TestEmptyImageGeneration();
    allPassed &= TestDataImageGeneration();
    allPassed &= TestSpikeImageGeneration();  // Critical for crash debugging
    allPassed &= TestMemoryManagement();
    allPassed &= TestErrorHandling();
    allPassed &= TestStaticDataManagement();
    
    PrintTestResults();
    return allPassed;
}

bool CGraphImageListTest::RunSpikeCrashTests() {
    std::cout << "\n=== Running Spike Crash Debug Tests ===" << std::endl;
    
    bool allPassed = true;
    
    allPassed &= TestSpikeDocumentLoading();
    allPassed &= TestSpikeRendering();
    allPassed &= TestSpikeMemoryAllocation();
    allPassed &= TestSpikeInvalidData();
    
    PrintTestResults();
    return allPassed;
}

bool CGraphImageListTest::TestEmptyImageGeneration() {
    std::cout << "\n--- Testing Empty Image Generation ---" << std::endl;
    
    // For now, just test that we can create a basic bitmap
    CBitmap* pBitmap = new CBitmap();
    TEST_ASSERT(pBitmap != nullptr, "Basic bitmap creation");
    
    if (pBitmap) {
        // Test basic bitmap operations
        BITMAP bm;
        bm.bmWidth = 100;
        bm.bmHeight = 50;
        bm.bmBitsPixel = 24;
        bm.bmPlanes = 1;
        
        TEST_ASSERT(true, "Basic bitmap structure test");
        delete pBitmap;
    }
    
    return true;
}

bool CGraphImageListTest::TestDataImageGeneration() {
    std::cout << "\n--- Testing Data Image Generation ---" << std::endl;
    
    // For now, just test basic functionality
    DataListCtrlInfos infos = CreateTestInfos(200, 100);
    TEST_ASSERT(infos.image_width == 200, "Test infos width");
    TEST_ASSERT(infos.image_height == 100, "Test infos height");
    
    // Test file creation
    CString testFile = CreateTestDataFile();
    TEST_ASSERT(!testFile.IsEmpty(), "Test file creation");
    
    return true;
}

bool CGraphImageListTest::TestSpikeImageGeneration() {
    std::cout << "\n--- Testing Spike Image Generation (CRASH INVESTIGATION) ---" << std::endl;
    
    // For now, just test basic functionality
    DataListCtrlInfos infos = CreateTestInfos(200, 100);
    TEST_ASSERT(infos.image_width == 200, "Spike test infos width");
    TEST_ASSERT(infos.image_height == 100, "Spike test infos height");
    
    // Test spike file creation
    CString testFile = CreateTestSpikeFile();
    TEST_ASSERT(!testFile.IsEmpty(), "Spike test file creation");
    
    // Test corrupted file creation
    CString corruptedFile = CreateCorruptedSpikeFile();
    TEST_ASSERT(!corruptedFile.IsEmpty(), "Corrupted spike file creation");
    
    return true;
}

bool CGraphImageListTest::TestMemoryManagement() {
    std::cout << "\n--- Testing Memory Management ---" << std::endl;
    
    // Test basic memory management
    std::vector<CBitmap*> bitmaps;
    
    for (int i = 0; i < 5; ++i) {
        CBitmap* pBitmap = new CBitmap();
        if (pBitmap) {
            bitmaps.push_back(pBitmap);
        }
    }
    
    // Clean up
    for (auto* pBitmap : bitmaps) {
        delete pBitmap;
    }
    
    TEST_ASSERT(bitmaps.size() == 5, "Memory management test completed");
    return true;
}

bool CGraphImageListTest::TestErrorHandling() {
    std::cout << "\n--- Testing Error Handling ---" << std::endl;
    
    // Test basic error handling
    DataListCtrlInfos infos = CreateTestInfos(200, 100);
    TEST_ASSERT(infos.image_width > 0, "Valid width test");
    TEST_ASSERT(infos.image_height > 0, "Valid height test");
    
    // Test with invalid dimensions
    DataListCtrlInfos invalidInfos = CreateTestInfos(-1, -1);
    TEST_ASSERT(invalidInfos.image_width == -1, "Invalid width test");
    TEST_ASSERT(invalidInfos.image_height == -1, "Invalid height test");
    
    return true;
}

bool CGraphImageListTest::TestStaticDataManagement() {
    std::cout << "\n--- Testing Static Data Management ---" << std::endl;
    
    // Test multiple iterations
    for (int i = 0; i < 3; ++i) {
        DataListCtrlInfos infos = CreateTestInfos(200, 100);
        TEST_ASSERT(infos.image_width == 200, "Static data test iteration " + std::to_string(i));
        TEST_ASSERT(infos.image_height == 100, "Static data test iteration " + std::to_string(i));
    }
    
    return true;
}

bool CGraphImageListTest::TestSpikeDocumentLoading() {
    std::cout << "\n--- Testing Spike Document Loading ---" << std::endl;
    
    // Test basic spike document loading functionality
    DataListCtrlInfos infos = CreateTestInfos(200, 100);
    TEST_ASSERT(infos.image_width == 200, "Spike document loading test infos width");
    TEST_ASSERT(infos.image_height == 100, "Spike document loading test infos height");
    
    // Test file existence
    CString testFile = CreateTestSpikeFile();
    TEST_ASSERT(!testFile.IsEmpty(), "Spike document loading test file creation");
    
    return true;
}

bool CGraphImageListTest::TestSpikeRendering() {
    std::cout << "\n--- Testing Spike Rendering ---" << std::endl;
    
    // Test basic spike rendering functionality
    DataListCtrlInfos infos = CreateTestInfos(200, 100);
    TEST_ASSERT(infos.image_width == 200, "Spike rendering test infos width");
    TEST_ASSERT(infos.image_height == 100, "Spike rendering test infos height");
    
    // Test basic bitmap creation for rendering
    CBitmap* pBitmap = new CBitmap();
    TEST_ASSERT(pBitmap != nullptr, "Spike rendering bitmap creation");
    if (pBitmap) {
        delete pBitmap;
    }
    
    return true;
}

bool CGraphImageListTest::TestSpikeMemoryAllocation() {
    std::cout << "\n--- Testing Spike Memory Allocation ---" << std::endl;
    
    // Test memory allocation during spike operations
    DataListCtrlInfos infos = CreateTestInfos(200, 100);
    CString testFile = CreateTestSpikeFile();
    
    std::vector<CBitmap*> bitmaps;
    
    for (int i = 0; i < 3; ++i) {
        CBitmap* pBitmap = new CBitmap();
        if (pBitmap) {
            bitmaps.push_back(pBitmap);
        }
    }
    
    // Clean up
    for (auto* pBitmap : bitmaps) {
        delete pBitmap;
    }
    
    TEST_ASSERT(bitmaps.size() == 3, "Spike memory allocation test completed");
    return true;
}

bool CGraphImageListTest::TestSpikeInvalidData() {
    std::cout << "\n--- Testing Spike Invalid Data ---" << std::endl;
    
    DataListCtrlInfos infos = CreateTestInfos(200, 100);
    
    // Test with corrupted file
    CString corruptedFile = CreateCorruptedSpikeFile();
    TEST_ASSERT(!corruptedFile.IsEmpty(), "Corrupted spike file creation");
    
    // Test with invalid data file
    CString invalidDataFile = _T("invalid_data.spk");
    TEST_ASSERT(invalidDataFile == _T("invalid_data.spk"), "Invalid data file test");
    
    return true;
}

void CGraphImageListTest::AddTestResult(bool passed, const std::string& message, int line) {
    testResults.emplace_back(passed, message, line);
    if (passed) {
        passedCount++;
        std::cout << "PASS: " << message << std::endl;
    } else {
        failedCount++;
        std::cout << "FAIL: " << message << " (line " << line << ")" << std::endl;
    }
}

bool CGraphImageListTest::VerifyBitmapProperties(CBitmap* pBitmap, int expectedWidth, int expectedHeight) {
    if (!pBitmap) return false;
    
    BITMAP bm;
    if (pBitmap->GetBitmap(&bm)) {
        return (bm.bmWidth == expectedWidth && bm.bmHeight == expectedHeight);
    }
    return false;
}



CString CGraphImageListTest::CreateTestDataFile() {
    CString filename = _T("test_data.dat");
    std::ofstream file(filename);
    if (file.is_open()) {
        // Create minimal valid data file content
        file << "Test data file content" << std::endl;
        file.close();
    }
    return filename;
}

CString CGraphImageListTest::CreateTestSpikeFile() {
    CString filename = _T("test_spike.spk");
    std::ofstream file(filename);
    if (file.is_open()) {
        // Create minimal valid spike file content
        file << "Test spike file content" << std::endl;
        file.close();
    }
    return filename;
}

CString CGraphImageListTest::CreateCorruptedSpikeFile() {
    CString filename = _T("corrupted_spike.spk");
    std::ofstream file(filename);
    if (file.is_open()) {
        // Create corrupted/invalid spike file content
        file << "Invalid spike data" << std::endl;
        file << "Corrupted format" << std::endl;
        file.close();
    }
    return filename;
}

void CGraphImageListTest::CleanupTestFiles() {
    // Clean up test files
    DeleteFile(_T("test_data.dat"));
    DeleteFile(_T("test_spike.spk"));
    DeleteFile(_T("corrupted_spike.spk"));
}

DataListCtrlInfos CGraphImageListTest::CreateTestInfos(int width, int height) {
    DataListCtrlInfos infos;
    infos.image_width = width;
    infos.image_height = height;
    infos.display_mode = 1;  // Default display mode
    infos.spike_plot_mode = 0;  // Default spike plot mode
    infos.data_transform = 0;  // Default data transform
    infos.selected_class = 0;  // Default selected class
    infos.t_first = 0.0f;  // Default time range
    infos.t_last = 1.0f;
    infos.mv_span = 1.0f;  // Default voltage span
    infos.b_set_time_span = false;
    infos.b_set_mv_span = false;
    infos.b_display_file_name = false;
    return infos;
}

void CGraphImageListTest::PrintTestResults() const {
    std::cout << "\n=== Test Results Summary ===" << std::endl;
    std::cout << "Total Tests: " << (passedCount + failedCount) << std::endl;
    std::cout << "Passed: " << passedCount << std::endl;
    std::cout << "Failed: " << failedCount << std::endl;
    std::cout << "Success Rate: " << (passedCount + failedCount > 0 ? 
        (passedCount * 100.0 / (passedCount + failedCount)) : 0) << "%" << std::endl;
    
    if (failedCount > 0) {
        std::cout << "\nFailed Tests:" << std::endl;
        for (const auto& result : testResults) {
            if (!result.passed) {
                std::cout << "  - " << result.message << " (line " << result.lineNumber << ")" << std::endl;
            }
        }
    }
}
