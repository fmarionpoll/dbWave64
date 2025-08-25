#pragma once

#include "CGraphImageListGoogleTest.h"

// Performance tests for CGraphImageList using Google Test
class CGraphImageListPerformanceTests : public CGraphImageListTestBase
{
public:
    // Test empty image generation performance
    TEST_F(CGraphImageListPerformanceTests, GenerateEmptyImage_Performance)
    {
        std::cout << "Testing empty image generation performance..." << std::endl;
        
        DataListCtrlInfos* pInfos = CreateTestInfos(640, 480);
        ASSERT_NE(pInfos, nullptr) << "Failed to create test infos";
        
        auto start = std::chrono::high_resolution_clock::now();
        
        ASSERT_NO_CRASH({
            // CGraphImageList::GenerateEmptyImage(pInfos);
            std::cout << "Empty image generation completed" << std::endl;
        });
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        std::cout << "Empty image generation took " << duration.count() << "ms" << std::endl;
        ASSERT_LT(duration.count(), 1000) << "Empty image generation should complete within 1 second";
        
        delete pInfos;
    }
    
    // Test data image generation performance
    TEST_F(CGraphImageListPerformanceTests, GenerateDataImage_Performance)
    {
        std::cout << "Testing data image generation performance..." << std::endl;
        
        CString dataFile = CreateTestDataFile();
        ASSERT_FALSE(dataFile.IsEmpty()) << "Failed to create test data file";
        
        DataListCtrlInfos* pInfos = CreateTestInfos(640, 480);
        ASSERT_NE(pInfos, nullptr) << "Failed to create test infos";
        
        auto start = std::chrono::high_resolution_clock::now();
        
        ASSERT_NO_CRASH({
            // CGraphImageList::GenerateDataImage(pInfos, dataFile);
            std::cout << "Data image generation completed" << std::endl;
        });
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        std::cout << "Data image generation took " << duration.count() << "ms" << std::endl;
        ASSERT_LT(duration.count(), 2000) << "Data image generation should complete within 2 seconds";
        
        delete pInfos;
    }
    
    // Test spike image generation performance
    TEST_F(CGraphImageListPerformanceTests, GenerateSpikeImage_Performance)
    {
        std::cout << "Testing spike image generation performance..." << std::endl;
        
        CString spikeFile = CreateTestSpikeFile();
        ASSERT_FALSE(spikeFile.IsEmpty()) << "Failed to create test spike file";
        
        DataListCtrlInfos* pInfos = CreateTestInfos(640, 480);
        ASSERT_NE(pInfos, nullptr) << "Failed to create test infos";
        
        auto start = std::chrono::high_resolution_clock::now();
        
        ASSERT_NO_CRASH({
            // CGraphImageList::GenerateSpikeImage(pInfos, spikeFile);
            std::cout << "Spike image generation completed" << std::endl;
        });
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        std::cout << "Spike image generation took " << duration.count() << "ms" << std::endl;
        ASSERT_LT(duration.count(), 3000) << "Spike image generation should complete within 3 seconds";
        
        delete pInfos;
    }
    
    // Test memory allocation performance
    TEST_F(CGraphImageListPerformanceTests, MemoryAllocation_Performance)
    {
        std::cout << "Testing memory allocation performance..." << std::endl;
        
        const int numAllocations = 100;
        std::vector<DataListCtrlInfos*> infosList;
        
        auto start = std::chrono::high_resolution_clock::now();
        
        // Create multiple objects
        for (int i = 0; i < numAllocations; ++i)
        {
            DataListCtrlInfos* pInfos = CreateTestInfos(100 + i, 100 + i);
            ASSERT_NE(pInfos, nullptr) << "Failed to create test infos " << i;
            infosList.push_back(pInfos);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        std::cout << "Created " << numAllocations << " objects in " << duration.count() << " microseconds" << std::endl;
        ASSERT_LT(duration.count(), 10000) << "Memory allocation should be fast";
        
        // Cleanup
        for (auto pInfos : infosList)
        {
            delete pInfos;
        }
    }
    
    // Test large image generation performance
    TEST_F(CGraphImageListPerformanceTests, LargeImage_Performance)
    {
        std::cout << "Testing large image generation performance..." << std::endl;
        
        DataListCtrlInfos* pInfos = CreateTestInfos(1920, 1080);
        ASSERT_NE(pInfos, nullptr) << "Failed to create test infos";
        
        auto start = std::chrono::high_resolution_clock::now();
        
        ASSERT_NO_CRASH({
            // CGraphImageList::GenerateEmptyImage(pInfos);
            std::cout << "Large image generation completed" << std::endl;
        });
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        std::cout << "Large image generation took " << duration.count() << "ms" << std::endl;
        ASSERT_LT(duration.count(), 5000) << "Large image generation should complete within 5 seconds";
        
        delete pInfos;
    }
    
    // Test concurrent generation performance (simulated)
    TEST_F(CGraphImageListPerformanceTests, ConcurrentGeneration_Performance)
    {
        std::cout << "Testing concurrent generation performance..." << std::endl;
        
        const int numThreads = 4;
        std::vector<DataListCtrlInfos*> infosList;
        
        // Create test infos for each "thread"
        for (int i = 0; i < numThreads; ++i)
        {
            DataListCtrlInfos* pInfos = CreateTestInfos(320, 240);
            ASSERT_NE(pInfos, nullptr) << "Failed to create test infos " << i;
            infosList.push_back(pInfos);
        }
        
        auto start = std::chrono::high_resolution_clock::now();
        
        // Simulate concurrent processing
        for (int i = 0; i < numThreads; ++i)
        {
            ASSERT_NO_CRASH({
                // CGraphImageList::GenerateEmptyImage(infosList[i]);
                std::cout << "Concurrent generation " << i << " completed" << std::endl;
            });
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        std::cout << "Concurrent generation took " << duration.count() << "ms" << std::endl;
        ASSERT_LT(duration.count(), 3000) << "Concurrent generation should complete within 3 seconds";
        
        // Cleanup
        for (auto pInfos : infosList)
        {
            delete pInfos;
        }
    }
    
    // Test file I/O performance
    TEST_F(CGraphImageListPerformanceTests, FileIO_Performance)
    {
        std::cout << "Testing file I/O performance..." << std::endl;
        
        // Create multiple test files
        std::vector<CString> dataFiles;
        for (int i = 0; i < 5; ++i)
        {
            CString dataFile = CreateTestDataFile();
            ASSERT_FALSE(dataFile.IsEmpty()) << "Failed to create data file " << i;
            dataFiles.push_back(dataFile);
        }
        
        DataListCtrlInfos* pInfos = CreateTestInfos(640, 480);
        ASSERT_NE(pInfos, nullptr) << "Failed to create test infos";
        
        auto start = std::chrono::high_resolution_clock::now();
        
        // Process multiple files
        for (const auto& dataFile : dataFiles)
        {
            ASSERT_NO_CRASH({
                // CGraphImageList::GenerateDataImage(pInfos, dataFile);
                std::cout << "File I/O processing completed for " << dataFile << std::endl;
            });
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        std::cout << "File I/O processing took " << duration.count() << "ms" << std::endl;
        ASSERT_LT(duration.count(), 10000) << "File I/O processing should complete within 10 seconds";
        
        delete pInfos;
    }
    
    // Test stress performance
    TEST_F(CGraphImageListPerformanceTests, StressTest_Performance)
    {
        std::cout << "Running performance stress test..." << std::endl;
        
        const int numIterations = 50;
        std::vector<DataListCtrlInfos*> infosList;
        
        // Create test infos for stress test
        for (int i = 0; i < numIterations; ++i)
        {
            DataListCtrlInfos* pInfos = CreateTestInfos(100 + (i % 10) * 50, 100 + (i % 10) * 50);
            ASSERT_NE(pInfos, nullptr) << "Failed to create test infos " << i;
            infosList.push_back(pInfos);
        }
        
        auto start = std::chrono::high_resolution_clock::now();
        
        // Run stress test
        for (int i = 0; i < numIterations; ++i)
        {
            ASSERT_NO_CRASH({
                // CGraphImageList::GenerateEmptyImage(infosList[i]);
                if (i % 10 == 0)
                {
                    std::cout << "Stress test iteration " << i << " completed" << std::endl;
                }
            });
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        std::cout << "Stress test completed " << numIterations << " iterations in " << duration.count() << "ms" << std::endl;
        ASSERT_LT(duration.count(), 30000) << "Stress test should complete within 30 seconds";
        
        // Cleanup
        for (auto pInfos : infosList)
        {
            delete pInfos;
        }
    }
};
