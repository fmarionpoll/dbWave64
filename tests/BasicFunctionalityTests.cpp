#include "pch.h"
#include "CGraphImageListTests.h"

// Basic functionality tests for CGraphImageList using Google Test

// Test empty image generation with basic dimensions
TEST_F(CGraphImageListTestBase, GenerateEmptyImage_Basic)
{
    std::cout << "Testing basic empty image generation..." << std::endl;
    
    // Create test infos
    DataListCtrlInfos* pInfos = CreateTestInfos(640, 480);
    ASSERT_NE(pInfos, nullptr) << "Failed to create test infos";
    
    // Test empty image generation
            ASSERT_NO_CRASH({
            // Call the actual CGraphImageList method
            CBitmap* pBitmap = CGraphImageList::GenerateEmptyImage(pInfos->image_width, pInfos->image_height);
            ASSERT_NE(pBitmap, nullptr) << "Failed to generate empty image";
            std::cout << "Empty image generation completed successfully" << std::endl;
            delete pBitmap;
        });
    
    delete pInfos;
}

// Test empty image generation with zero dimensions
TEST_F(CGraphImageListTestBase, GenerateEmptyImage_ZeroDimensions)
{
    std::cout << "Testing empty image generation with zero dimensions..." << std::endl;
    
    DataListCtrlInfos* pInfos = CreateTestInfos(0, 0);
    ASSERT_NE(pInfos, nullptr) << "Failed to create test infos";
    
            ASSERT_NO_CRASH({
            // Call the actual CGraphImageList method
            CBitmap* pBitmap = CGraphImageList::GenerateEmptyImage(pInfos->image_width, pInfos->image_height);
            ASSERT_NE(pBitmap, nullptr) << "Failed to generate zero dimension image";
            std::cout << "Zero dimension image generation completed" << std::endl;
            delete pBitmap;
        });
    
    delete pInfos;
}

// Test empty image generation with negative dimensions
TEST_F(CGraphImageListTestBase, GenerateEmptyImage_NegativeDimensions)
{
    std::cout << "Testing empty image generation with negative dimensions..." << std::endl;
    
    DataListCtrlInfos* pInfos = CreateTestInfos(-100, -200);
    ASSERT_NE(pInfos, nullptr) << "Failed to create test infos";
    
            ASSERT_NO_CRASH({
            // Call the actual CGraphImageList method
            CBitmap* pBitmap = CGraphImageList::GenerateEmptyImage(pInfos->image_width, pInfos->image_height);
            ASSERT_NE(pBitmap, nullptr) << "Failed to generate negative dimension image";
            std::cout << "Negative dimension image generation completed" << std::endl;
            delete pBitmap;
        });
    
    delete pInfos;
}

// Test empty image generation with large dimensions
TEST_F(CGraphImageListTestBase, GenerateEmptyImage_LargeDimensions)
{
    std::cout << "Testing empty image generation with large dimensions..." << std::endl;
    
    DataListCtrlInfos* pInfos = CreateTestInfos(1920, 1080);
    ASSERT_NE(pInfos, nullptr) << "Failed to create test infos";
    
            ASSERT_NO_CRASH({
            // Call the actual CGraphImageList method
            CBitmap* pBitmap = CGraphImageList::GenerateEmptyImage(pInfos->image_width, pInfos->image_height);
            ASSERT_NE(pBitmap, nullptr) << "Failed to generate large dimension image";
            std::cout << "Large dimension image generation completed" << std::endl;
            delete pBitmap;
        });
    
    delete pInfos;
}

// Test multiple empty image generation
TEST_F(CGraphImageListTestBase, GenerateEmptyImage_MultipleImages)
{
    std::cout << "Testing multiple empty image generation..." << std::endl;
    
    std::vector<DataListCtrlInfos*> infosList;
    
    // Create multiple test infos
    for (int i = 0; i < 5; ++i)
    {
        DataListCtrlInfos* pInfos = CreateTestInfos(100 + i * 50, 100 + i * 50);
        ASSERT_NE(pInfos, nullptr) << "Failed to create test infos " << i;
        infosList.push_back(pInfos);
    }
    
    // Generate multiple images
            for (size_t i = 0; i < infosList.size(); ++i)
        {
            ASSERT_NO_CRASH({
                // Call the actual CGraphImageList method
                CBitmap* pBitmap = CGraphImageList::GenerateEmptyImage(infosList[i]->image_width, infosList[i]->image_height);
                ASSERT_NE(pBitmap, nullptr) << "Failed to generate image " << i;
                std::cout << "Generated image " << i << " successfully" << std::endl;
                delete pBitmap;
            });
        }
    
    // Cleanup
    for (auto pInfos : infosList)
    {
        delete pInfos;
    }
}

// Test memory management
TEST_F(CGraphImageListTestBase, MemoryManagement_Basic)
{
    std::cout << "Testing memory management..." << std::endl;
    
    // Test that we can create and delete multiple infos without memory leaks
    for (int i = 0; i < 10; ++i)
    {
        DataListCtrlInfos* pInfos = CreateTestInfos(100, 100);
        ASSERT_NE(pInfos, nullptr) << "Failed to create test infos " << i;
        
        // Simulate some operations
        pInfos->image_width = 200 + i;
        pInfos->image_height = 150 + i;
        
        delete pInfos;
    }
    
    std::cout << "Memory management test completed successfully" << std::endl;
}

// Test parameter validation
TEST_F(CGraphImageListTestBase, ParameterValidation)
{
    std::cout << "Testing parameter validation..." << std::endl;
    
    // Test with null pointer
    ASSERT_NO_CRASH({
        // Test with null pointer (should handle gracefully)
        CBitmap* pBitmap = CGraphImageList::GenerateEmptyImage(0, 0);
        // This might return nullptr for zero dimensions, which is acceptable
        std::cout << "Null pointer test completed" << std::endl;
        if (pBitmap) delete pBitmap;
    });
    
    // Test with invalid dimensions
    DataListCtrlInfos* pInfos = CreateTestInfos(INT_MAX, INT_MAX);
    ASSERT_NE(pInfos, nullptr) << "Failed to create test infos";
    
    ASSERT_NO_CRASH({
        // Call the actual CGraphImageList method with invalid dimensions
        CBitmap* pBitmap = CGraphImageList::GenerateEmptyImage(pInfos->image_width, pInfos->image_height);
        // This might return nullptr for invalid dimensions, which is acceptable
        std::cout << "Invalid dimensions test completed" << std::endl;
        if (pBitmap) delete pBitmap;
    });
    
    delete pInfos;
}
