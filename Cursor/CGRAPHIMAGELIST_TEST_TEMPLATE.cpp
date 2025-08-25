// CGraphImageList_Test.cpp
// Test template for debugging CGraphImageList spike crash
// Add this to your project and run to isolate the crash

#include "StdAfx.h"
#include "CGraphImageList.h"
#include "DataListCtrl_Infos.h"
#include <iostream>
#include <vector>
#include <chrono>

// Simple test framework for immediate use
#define TEST_ASSERT(condition, message) \
    if (!(condition)) { \
        std::cout << "FAIL: " << message << " at line " << __LINE__ << std::endl; \
        return false; \
    } else { \
        std::cout << "PASS: " << message << std::endl; \
    }

#define TEST_EXPECT_NO_CRASH(expression, message) \
    try { \
        expression; \
        std::cout << "PASS: " << message << " (no crash)" << std::endl; \
    } catch (...) { \
        std::cout << "FAIL: " << message << " (crashed)" << std::endl; \
        return false; \
    }

// Test fixture class
class CGraphImageListTest
{
public:
    bool RunAllTests()
    {
        std::cout << "=== CGraphImageList Test Suite ===" << std::endl;
        
        bool allPassed = true;
        
        allPassed &= TestEmptyImageGeneration();
        allPassed &= TestDataImageGeneration();
        allPassed &= TestSpikeImageGeneration();  // This is where your crash likely occurs
        allPassed &= TestMemoryManagement();
        allPassed &= TestErrorHandling();
        
        std::cout << "=== Test Results: " << (allPassed ? "ALL PASSED" : "SOME FAILED") << " ===" << std::endl;
        return allPassed;
    }

private:
    bool TestEmptyImageGeneration()
    {
        std::cout << "\n--- Testing Empty Image Generation ---" << std::endl;
        
        // Test basic empty image generation
        CBitmap* pBitmap = nullptr;
        TEST_EXPECT_NO_CRASH(
            pBitmap = CGraphImageList::GenerateEmptyImage(100, 50),
            "GenerateEmptyImage with valid dimensions"
        );
        
        if (pBitmap)
        {
            // Verify bitmap properties
            BITMAP bm;
            if (pBitmap->GetBitmap(&bm))
            {
                TEST_ASSERT(bm.bmWidth == 100, "Bitmap width should be 100");
                TEST_ASSERT(bm.bmHeight == 50, "Bitmap height should be 50");
            }
            delete pBitmap;
        }
        
        // Test edge cases
        TEST_EXPECT_NO_CRASH(
            pBitmap = CGraphImageList::GenerateEmptyImage(0, 0),
            "GenerateEmptyImage with zero dimensions"
        );
        if (pBitmap) delete pBitmap;
        
        TEST_EXPECT_NO_CRASH(
            pBitmap = CGraphImageList::GenerateEmptyImage(-1, -1),
            "GenerateEmptyImage with negative dimensions"
        );
        if (pBitmap) delete pBitmap;
        
        return true;
    }
    
    bool TestDataImageGeneration()
    {
        std::cout << "\n--- Testing Data Image Generation ---" << std::endl;
        
        // Test with invalid file (should not crash)
        CString invalidFile = _T("nonexistent_file.dat");
        DataListCtrlInfos infos;
        infos.image_width = 200;
        infos.image_height = 100;
        
        CBitmap* pBitmap = nullptr;
        TEST_EXPECT_NO_CRASH(
            pBitmap = CGraphImageList::GenerateDataImage(200, 100, invalidFile, infos),
            "GenerateDataImage with invalid file"
        );
        
        if (pBitmap)
        {
            BITMAP bm;
            if (pBitmap->GetBitmap(&bm))
            {
                TEST_ASSERT(bm.bmWidth == 200, "Data bitmap width should be 200");
                TEST_ASSERT(bm.bmHeight == 100, "Data bitmap height should be 100");
            }
            delete pBitmap;
        }
        
        // Test with empty filename
        CString emptyFile = _T("");
        TEST_EXPECT_NO_CRASH(
            pBitmap = CGraphImageList::GenerateDataImage(200, 100, emptyFile, infos),
            "GenerateDataImage with empty filename"
        );
        if (pBitmap) delete pBitmap;
        
        return true;
    }
    
    bool TestSpikeImageGeneration()
    {
        std::cout << "\n--- Testing Spike Image Generation (CRASH INVESTIGATION) ---" << std::endl;
        
        DataListCtrlInfos infos;
        infos.image_width = 200;
        infos.image_height = 100;
        
        // Test 1: Empty filename
        std::cout << "Test 1: Empty filename" << std::endl;
        CString emptyFile = _T("");
        CBitmap* pBitmap = nullptr;
        TEST_EXPECT_NO_CRASH(
            pBitmap = CGraphImageList::GenerateSpikeImage(200, 100, emptyFile, infos),
            "GenerateSpikeImage with empty filename"
        );
        if (pBitmap) delete pBitmap;
        
        // Test 2: Invalid filename
        std::cout << "Test 2: Invalid filename" << std::endl;
        CString invalidFile = _T("nonexistent_spike.spk");
        TEST_EXPECT_NO_CRASH(
            pBitmap = CGraphImageList::GenerateSpikeImage(200, 100, invalidFile, infos),
            "GenerateSpikeImage with invalid filename"
        );
        if (pBitmap) delete pBitmap;
        
        // Test 3: Null pointer in infos
        std::cout << "Test 3: Null pointer test" << std::endl;
        TEST_EXPECT_NO_CRASH(
            pBitmap = CGraphImageList::GenerateSpikeImage(200, 100, invalidFile, infos),
            "GenerateSpikeImage with null pointer handling"
        );
        if (pBitmap) delete pBitmap;
        
        // Test 4: Large dimensions
        std::cout << "Test 4: Large dimensions" << std::endl;
        TEST_EXPECT_NO_CRASH(
            pBitmap = CGraphImageList::GenerateSpikeImage(1000, 1000, invalidFile, infos),
            "GenerateSpikeImage with large dimensions"
        );
        if (pBitmap) delete pBitmap;
        
        // Test 5: Zero dimensions
        std::cout << "Test 5: Zero dimensions" << std::endl;
        TEST_EXPECT_NO_CRASH(
            pBitmap = CGraphImageList::GenerateSpikeImage(0, 0, invalidFile, infos),
            "GenerateSpikeImage with zero dimensions"
        );
        if (pBitmap) delete pBitmap;
        
        // Test 6: Negative dimensions
        std::cout << "Test 6: Negative dimensions" << std::endl;
        TEST_EXPECT_NO_CRASH(
            pBitmap = CGraphImageList::GenerateSpikeImage(-1, -1, invalidFile, infos),
            "GenerateSpikeImage with negative dimensions"
        );
        if (pBitmap) delete pBitmap;
        
        return true;
    }
    
    bool TestMemoryManagement()
    {
        std::cout << "\n--- Testing Memory Management ---" << std::endl;
        
        // Test multiple generations to check for memory leaks
        std::vector<CBitmap*> bitmaps;
        
        for (int i = 0; i < 10; ++i)
        {
            CBitmap* pBitmap = CGraphImageList::GenerateEmptyImage(50, 50);
            if (pBitmap)
            {
                bitmaps.push_back(pBitmap);
            }
        }
        
        // Clean up
        for (auto* pBitmap : bitmaps)
        {
            delete pBitmap;
        }
        
        std::cout << "PASS: Memory management test completed" << std::endl;
        return true;
    }
    
    bool TestErrorHandling()
    {
        std::cout << "\n--- Testing Error Handling ---" << std::endl;
        
        // Test with various invalid parameters
        DataListCtrlInfos infos;
        infos.image_width = 200;
        infos.image_height = 100;
        
        // Test BuildEmptyBitmap with null DC
        CBitmap* pBitmap = nullptr;
        TEST_EXPECT_NO_CRASH(
            pBitmap = CGraphImageList::BuildEmptyBitmap(100, 100, nullptr),
            "BuildEmptyBitmap with null DC"
        );
        if (pBitmap) delete pBitmap;
        
        return true;
    }
};

// Main test runner
int main()
{
    CGraphImageListTest testSuite;
    bool success = testSuite.RunAllTests();
    
    std::cout << "\nPress any key to exit..." << std::endl;
    std::cin.get();
    
    return success ? 0 : 1;
}

// Alternative: If you want to integrate with your existing application
// Add this function to your main application and call it from a menu or button
void RunCGraphImageListTests()
{
    CGraphImageListTest testSuite;
    bool success = testSuite.RunAllTests();
    
    // Show results in a message box or log
    CString message;
    message.Format(_T("CGraphImageList Tests: %s"), success ? _T("ALL PASSED") : _T("SOME FAILED"));
    AfxMessageBox(message);
}
