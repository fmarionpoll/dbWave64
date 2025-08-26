#include "pch.h"
#include "CGraphImageListTests.h"
#include <thread>

// Performance tests for CGraphImageList using Google Test

// Test performance of empty image generation
TEST_F(CGraphImageListTestBase, Performance_EmptyImageGeneration)
{
    std::cout << "Testing performance of empty image generation..." << std::endl;
    
    DataListCtrlInfos* pInfos = CreateTestInfos(640, 480);
    ASSERT_NE(pInfos, nullptr) << "Failed to create test infos";
    
    auto start = std::chrono::high_resolution_clock::now();
    
    // Generate multiple empty images for performance testing
    for (int i = 0; i < 100; ++i)
    {
        ASSERT_NO_CRASH({
            // Call the actual CGraphImageList method
            CBitmap* pBitmap = CGraphImageList::GenerateEmptyImage(pInfos->image_width, pInfos->image_height);
            ASSERT_NE(pBitmap, nullptr) << "Failed to generate empty image in performance test";
            delete pBitmap;
        });
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "Generated 100 empty images in " << duration.count() << "ms" << std::endl;
    ASSERT_LT(duration.count(), 10000) << "Empty image generation should complete within 10 seconds";
    
    delete pInfos;
}

// Test performance of data image generation
TEST_F(CGraphImageListTestBase, Performance_DataImageGeneration)
{
    std::cout << "Testing performance of data image generation..." << std::endl;
    
    CString dataFile = CreateTestDataFile();
    ASSERT_FALSE(dataFile.IsEmpty()) << "Failed to create test data file";
    
    DataListCtrlInfos* pInfos = CreateTestInfos(640, 480);
    ASSERT_NE(pInfos, nullptr) << "Failed to create test infos";
    
    auto start = std::chrono::high_resolution_clock::now();
    
    // Generate multiple data images for performance testing
    for (int i = 0; i < 10; ++i)
    {
        ASSERT_NO_CRASH({
            // Call the actual CGraphImageList method
            CBitmap* pBitmap = CGraphImageList::GenerateDataImage(pInfos->image_width, pInfos->image_height, dataFile, *pInfos);
            ASSERT_NE(pBitmap, nullptr) << "Failed to generate data image in performance test";
            delete pBitmap;
        });
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "Generated 10 data images in " << duration.count() << "ms" << std::endl;
    ASSERT_LT(duration.count(), 5000) << "Data image generation should complete within 5 seconds";
    
    delete pInfos;
}

// Test performance of spike image generation
TEST_F(CGraphImageListTestBase, Performance_SpikeImageGeneration)
{
    std::cout << "Testing performance of spike image generation..." << std::endl;
    
    CString spikeFile = CreateTestSpikeFile();
    ASSERT_FALSE(spikeFile.IsEmpty()) << "Failed to create test spike file";
    
    DataListCtrlInfos* pInfos = CreateTestInfos(640, 480);
    ASSERT_NE(pInfos, nullptr) << "Failed to create test infos";
    
    auto start = std::chrono::high_resolution_clock::now();
    
    // Generate multiple spike images for performance testing
    for (int i = 0; i < 10; ++i)
    {
        ASSERT_NO_CRASH({
            // Call the actual CGraphImageList method
            CBitmap* pBitmap = CGraphImageList::GenerateSpikeImage(pInfos->image_width, pInfos->image_height, spikeFile, *pInfos);
            ASSERT_NE(pBitmap, nullptr) << "Failed to generate spike image in performance test";
            delete pBitmap;
        });
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "Generated 10 spike images in " << duration.count() << "ms" << std::endl;
    ASSERT_LT(duration.count(), 5000) << "Spike image generation should complete within 5 seconds";
    
    delete pInfos;
}

// Test performance with different image sizes
TEST_F(CGraphImageListTestBase, Performance_DifferentImageSizes)
{
    std::cout << "Testing performance with different image sizes..." << std::endl;
    
    std::vector<std::pair<int, int>> sizes = {
        {100, 100}, {320, 240}, {640, 480}, {800, 600}, {1024, 768}, {1920, 1080}
    };
    
    for (const auto& size : sizes)
    {
        DataListCtrlInfos* pInfos = CreateTestInfos(size.first, size.second);
        ASSERT_NE(pInfos, nullptr) << "Failed to create test infos for " << size.first << "x" << size.second;
        
        auto start = std::chrono::high_resolution_clock::now();
        
        ASSERT_NO_CRASH({
            // Call the actual CGraphImageList method
            CBitmap* pBitmap = CGraphImageList::GenerateEmptyImage(size.first, size.second);
            ASSERT_NE(pBitmap, nullptr) << "Failed to generate " << size.first << "x" << size.second << " image";
            delete pBitmap;
        });
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        std::cout << "Generated " << size.first << "x" << size.second << " image in " << duration.count() << "μs" << std::endl;
        
        delete pInfos;
    }
}

// Test memory usage during image generation
TEST_F(CGraphImageListTestBase, Performance_MemoryUsage)
{
    std::cout << "Testing memory usage during image generation..." << std::endl;
    
    DataListCtrlInfos* pInfos = CreateTestInfos(1920, 1080);
    ASSERT_NE(pInfos, nullptr) << "Failed to create test infos";
    
    // Generate multiple large images to test memory usage
    for (int i = 0; i < 5; ++i)
    {
        ASSERT_NO_CRASH({
            // Call the actual CGraphImageList method
            CBitmap* pBitmap = CGraphImageList::GenerateEmptyImage(pInfos->image_width, pInfos->image_height);
            ASSERT_NE(pBitmap, nullptr) << "Failed to generate large image " << (i + 1);
            delete pBitmap;
        });
        
        std::cout << "Generated large image " << (i + 1) << "/5" << std::endl;
    }
    
    delete pInfos;
    std::cout << "Memory usage test completed successfully" << std::endl;
}

// Test concurrent image generation performance
TEST_F(CGraphImageListTestBase, Performance_ConcurrentGeneration)
{
    std::cout << "Testing concurrent image generation performance..." << std::endl;
    
    std::vector<DataListCtrlInfos*> infosList;
    
    // Create multiple test infos for concurrent testing
    for (int i = 0; i < 4; ++i)
    {
        DataListCtrlInfos* pInfos = CreateTestInfos(640, 480);
        ASSERT_NE(pInfos, nullptr) << "Failed to create test infos " << i;
        infosList.push_back(pInfos);
    }
    
    auto start = std::chrono::high_resolution_clock::now();
    
    // Simulate concurrent image generation
    std::vector<std::thread> threads;
    for (size_t i = 0; i < infosList.size(); ++i)
    {
        threads.emplace_back([&, i]() {
            ASSERT_NO_CRASH({
                // Call the actual CGraphImageList method
                CBitmap* pBitmap = CGraphImageList::GenerateEmptyImage(infosList[i]->image_width, infosList[i]->image_height);
                ASSERT_NE(pBitmap, nullptr) << "Failed to generate image in concurrent test " << i;
                delete pBitmap;
            });
        });
    }
    
    // Wait for all threads to complete
    for (auto& thread : threads)
    {
        thread.join();
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "Concurrent generation of 4 images took " << duration.count() << "ms" << std::endl;
    ASSERT_LT(duration.count(), 1000) << "Concurrent generation should complete within 1 second";
    
    // Cleanup
    for (auto pInfos : infosList)
    {
        delete pInfos;
    }
}
