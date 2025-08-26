#include "pch.h"
#include "CGraphImageListTests.h"

// Data image generation tests for CGraphImageList using Google Test

// Test data image generation with valid data file
TEST_F(CGraphImageListTestBase, GenerateDataImage_ValidFile)
{
    std::cout << "Testing data image generation with valid file..." << std::endl;
    
    // Create test data file
    CString dataFile = CreateTestDataFile();
    ASSERT_FALSE(dataFile.IsEmpty()) << "Failed to create test data file";
    
    // Create test infos
    DataListCtrlInfos* pInfos = CreateTestInfos(640, 480);
    ASSERT_NE(pInfos, nullptr) << "Failed to create test infos";
    
            ASSERT_NO_CRASH({
            // Call the actual CGraphImageList method
            CBitmap* pBitmap = CGraphImageList::GenerateDataImage(pInfos->image_width, pInfos->image_height, dataFile, *pInfos);
            ASSERT_NE(pBitmap, nullptr) << "Failed to generate data image with valid file";
            std::cout << "Data image generation with valid file completed" << std::endl;
            delete pBitmap;
        });
    
    delete pInfos;
}

// Test data image generation with empty file
TEST_F(CGraphImageListTestBase, GenerateDataImage_EmptyFile)
{
    std::cout << "Testing data image generation with empty file..." << std::endl;
    
    // Create empty data file
    CString emptyFile = CreateTestDataFile();
    // Make it empty by truncating
    std::ofstream file(emptyFile, std::ios::trunc);
    file.close();
    
    DataListCtrlInfos* pInfos = CreateTestInfos(640, 480);
    ASSERT_NE(pInfos, nullptr) << "Failed to create test infos";
    
            ASSERT_NO_CRASH({
            // Call the actual CGraphImageList method
            CBitmap* pBitmap = CGraphImageList::GenerateDataImage(pInfos->image_width, pInfos->image_height, emptyFile, *pInfos);
            // This might return nullptr for empty file, which is acceptable
            std::cout << "Data image generation with empty file completed" << std::endl;
            if (pBitmap) delete pBitmap;
        });
    
    delete pInfos;
}

// Test data image generation with corrupted file
TEST_F(CGraphImageListTestBase, GenerateDataImage_CorruptedFile)
{
    std::cout << "Testing data image generation with corrupted file..." << std::endl;
    
    // Create corrupted data file
    CString corruptedFile = CreateCorruptedSpikeFile();
    ASSERT_FALSE(corruptedFile.IsEmpty()) << "Failed to create corrupted file";
    
    DataListCtrlInfos* pInfos = CreateTestInfos(640, 480);
    ASSERT_NE(pInfos, nullptr) << "Failed to create test infos";
    
            ASSERT_NO_CRASH({
            // Call the actual CGraphImageList method
            CBitmap* pBitmap = CGraphImageList::GenerateDataImage(pInfos->image_width, pInfos->image_height, corruptedFile, *pInfos);
            // This might return nullptr for corrupted file, which is acceptable
            std::cout << "Data image generation with corrupted file completed" << std::endl;
            if (pBitmap) delete pBitmap;
        });
    
    delete pInfos;
}

// Test data image generation with non-existent file
TEST_F(CGraphImageListTestBase, GenerateDataImage_NonExistentFile)
{
    std::cout << "Testing data image generation with non-existent file..." << std::endl;
    
    CString nonExistentFile = _T("non_existent_file.dat");
    
    DataListCtrlInfos* pInfos = CreateTestInfos(640, 480);
    ASSERT_NE(pInfos, nullptr) << "Failed to create test infos";
    
            ASSERT_NO_CRASH({
            // Call the actual CGraphImageList method
            CBitmap* pBitmap = CGraphImageList::GenerateDataImage(pInfos->image_width, pInfos->image_height, nonExistentFile, *pInfos);
            // This might return nullptr for non-existent file, which is acceptable
            std::cout << "Data image generation with non-existent file completed" << std::endl;
            if (pBitmap) delete pBitmap;
        });
    
    delete pInfos;
}

// Test data image generation with large file
TEST_F(CGraphImageListTestBase, GenerateDataImage_LargeFile)
{
    std::cout << "Testing data image generation with large file..." << std::endl;
    
    // Create large data file
    CString largeFile = CreateTestDataFile();
    // Append more data to make it larger
    std::ofstream file(largeFile, std::ios::app);
    for (int i = 0; i < 1000; ++i)
    {
        file << "Additional data line " << i << "\n";
    }
    file.close();
    
    DataListCtrlInfos* pInfos = CreateTestInfos(640, 480);
    ASSERT_NE(pInfos, nullptr) << "Failed to create test infos";
    
            ASSERT_NO_CRASH({
            // Call the actual CGraphImageList method
            CBitmap* pBitmap = CGraphImageList::GenerateDataImage(pInfos->image_width, pInfos->image_height, largeFile, *pInfos);
            ASSERT_NE(pBitmap, nullptr) << "Failed to generate data image with large file";
            std::cout << "Data image generation with large file completed" << std::endl;
            delete pBitmap;
        });
    
    delete pInfos;
}

// Test data image generation with different display modes
TEST_F(CGraphImageListTestBase, GenerateDataImage_DifferentDisplayModes)
{
    std::cout << "Testing data image generation with different display modes..." << std::endl;
    
    CString dataFile = CreateTestDataFile();
    ASSERT_FALSE(dataFile.IsEmpty()) << "Failed to create test data file";
    
    // Test different display modes
    for (int mode = 0; mode < 5; ++mode)
    {
        DataListCtrlInfos* pInfos = CreateTestInfos(640, 480);
        ASSERT_NE(pInfos, nullptr) << "Failed to create test infos for mode " << mode;
        
        pInfos->display_mode = mode;
        
        ASSERT_NO_CRASH({
            // Call the actual CGraphImageList method
            CBitmap* pBitmap = CGraphImageList::GenerateDataImage(pInfos->image_width, pInfos->image_height, dataFile, *pInfos);
            ASSERT_NE(pBitmap, nullptr) << "Failed to generate data image with display mode " << mode;
            std::cout << "Data image generation with display mode " << mode << " completed" << std::endl;
            delete pBitmap;
        });
        
        delete pInfos;
    }
}

// Test data image generation with different data transforms
TEST_F(CGraphImageListTestBase, GenerateDataImage_DifferentDataTransforms)
{
    std::cout << "Testing data image generation with different data transforms..." << std::endl;
    
    CString dataFile = CreateTestDataFile();
    ASSERT_FALSE(dataFile.IsEmpty()) << "Failed to create test data file";
    
    // Test different data transforms
    for (int transform = 0; transform < 3; ++transform)
    {
        DataListCtrlInfos* pInfos = CreateTestInfos(640, 480);
        ASSERT_NE(pInfos, nullptr) << "Failed to create test infos for transform " << transform;
        
        pInfos->data_transform = transform;
        
        ASSERT_NO_CRASH({
            // Call the actual CGraphImageList method
            CBitmap* pBitmap = CGraphImageList::GenerateDataImage(pInfos->image_width, pInfos->image_height, dataFile, *pInfos);
            ASSERT_NE(pBitmap, nullptr) << "Failed to generate data image with data transform " << transform;
            std::cout << "Data image generation with data transform " << transform << " completed" << std::endl;
            delete pBitmap;
        });
        
        delete pInfos;
    }
}

// Test data image generation with time span settings
TEST_F(CGraphImageListTestBase, GenerateDataImage_TimeSpanSettings)
{
    std::cout << "Testing data image generation with time span settings..." << std::endl;
    
    CString dataFile = CreateTestDataFile();
    ASSERT_FALSE(dataFile.IsEmpty()) << "Failed to create test data file";
    
    DataListCtrlInfos* pInfos = CreateTestInfos(640, 480);
    ASSERT_NE(pInfos, nullptr) << "Failed to create test infos";
    
    // Set time span
    pInfos->b_set_time_span = true;
    pInfos->t_first = 0.0f;
    pInfos->t_last = 100.0f;
    
            ASSERT_NO_CRASH({
            // Call the actual CGraphImageList method
            CBitmap* pBitmap = CGraphImageList::GenerateDataImage(pInfos->image_width, pInfos->image_height, dataFile, *pInfos);
            ASSERT_NE(pBitmap, nullptr) << "Failed to generate data image with time span settings";
            std::cout << "Data image generation with time span settings completed" << std::endl;
            delete pBitmap;
        });
    
    delete pInfos;
}

// Test data image generation with voltage span settings
TEST_F(CGraphImageListTestBase, GenerateDataImage_VoltageSpanSettings)
{
    std::cout << "Testing data image generation with voltage span settings..." << std::endl;
    
    CString dataFile = CreateTestDataFile();
    ASSERT_FALSE(dataFile.IsEmpty()) << "Failed to create test data file";
    
    DataListCtrlInfos* pInfos = CreateTestInfos(640, 480);
    ASSERT_NE(pInfos, nullptr) << "Failed to create test infos";
    
    // Set voltage span
    pInfos->b_set_mv_span = true;
    pInfos->mv_span = 50.0f;
    
            ASSERT_NO_CRASH({
            // Call the actual CGraphImageList method
            CBitmap* pBitmap = CGraphImageList::GenerateDataImage(pInfos->image_width, pInfos->image_height, dataFile, *pInfos);
            ASSERT_NE(pBitmap, nullptr) << "Failed to generate data image with voltage span settings";
            std::cout << "Data image generation with voltage span settings completed" << std::endl;
            delete pBitmap;
        });
    
    delete pInfos;
}
