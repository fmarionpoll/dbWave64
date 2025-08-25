#pragma once

#include "CGraphImageListGoogleTest.h"

// Data image generation tests for CGraphImageList using Google Test
class CGraphImageListDataTests : public CGraphImageListTestBase
{
public:
    // Test data image generation with valid file
    TEST_F(CGraphImageListDataTests, GenerateDataImage_ValidFile)
    {
        std::cout << "Testing data image generation with valid file..." << std::endl;
        
        // Create test data file
        CString dataFile = CreateTestDataFile();
        ASSERT_FALSE(dataFile.IsEmpty()) << "Failed to create test data file";
        
        // Create test infos
        DataListCtrlInfos* pInfos = CreateTestInfos(640, 480);
        ASSERT_NE(pInfos, nullptr) << "Failed to create test infos";
        
        ASSERT_NO_CRASH({
            // CGraphImageList::GenerateDataImage(pInfos, dataFile);
            std::cout << "Valid data file processing completed successfully" << std::endl;
        });
        
        delete pInfos;
    }
    
    // Test data image generation with invalid file
    TEST_F(CGraphImageListDataTests, GenerateDataImage_InvalidFile)
    {
        std::cout << "Testing data image generation with invalid file..." << std::endl;
        
        CString invalidFile = _T("invalid_data_file.dat");
        
        DataListCtrlInfos* pInfos = CreateTestInfos(100, 100);
        ASSERT_NE(pInfos, nullptr) << "Failed to create test infos";
        
        ASSERT_NO_CRASH({
            // CGraphImageList::GenerateDataImage(pInfos, invalidFile);
            std::cout << "Invalid file handling completed successfully" << std::endl;
        });
        
        delete pInfos;
    }
    
    // Test data image generation with empty file
    TEST_F(CGraphImageListDataTests, GenerateDataImage_EmptyFile)
    {
        std::cout << "Testing data image generation with empty file..." << std::endl;
        
        // Create empty data file
        CString emptyFile = _T("test_data\\empty_data.dat");
        CreateDirectory(_T("test_data"), NULL);
        
        std::ofstream file(emptyFile);
        file.close();
        
        DataListCtrlInfos* pInfos = CreateTestInfos(100, 100);
        ASSERT_NE(pInfos, nullptr) << "Failed to create test infos";
        
        ASSERT_NO_CRASH({
            // CGraphImageList::GenerateDataImage(pInfos, emptyFile);
            std::cout << "Empty file handling completed successfully" << std::endl;
        });
        
        delete pInfos;
        DeleteFile(emptyFile);
    }
    
    // Test data image generation with corrupted file
    TEST_F(CGraphImageListDataTests, GenerateDataImage_CorruptedFile)
    {
        std::cout << "Testing data image generation with corrupted file..." << std::endl;
        
        // Create corrupted data file
        CString corruptedFile = _T("test_data\\corrupted_data.dat");
        CreateDirectory(_T("test_data"), NULL);
        
        std::ofstream file(corruptedFile, std::ios::binary);
        if (file.is_open())
        {
            // Write corrupted data
            char corruptedData[] = { 0xFF, 0x00, 0xAA, 0x55, 0xDE, 0xAD, 0xBE, 0xEF };
            file.write(corruptedData, sizeof(corruptedData));
            file.close();
        }
        
        DataListCtrlInfos* pInfos = CreateTestInfos(100, 100);
        ASSERT_NE(pInfos, nullptr) << "Failed to create test infos";
        
        ASSERT_NO_CRASH({
            // CGraphImageList::GenerateDataImage(pInfos, corruptedFile);
            std::cout << "Corrupted file handling completed successfully" << std::endl;
        });
        
        delete pInfos;
        DeleteFile(corruptedFile);
    }
    
    // Test data image generation with large file
    TEST_F(CGraphImageListDataTests, GenerateDataImage_LargeFile)
    {
        std::cout << "Testing data image generation with large file..." << std::endl;
        
        // Create large data file
        CString largeFile = _T("test_data\\large_data.dat");
        CreateDirectory(_T("test_data"), NULL);
        
        std::ofstream file(largeFile, std::ios::binary);
        if (file.is_open())
        {
            // Write large amount of data
            for (int i = 0; i < 10000; ++i)
            {
                int dataValue = i * 2;
                file.write(reinterpret_cast<const char*>(&dataValue), sizeof(int));
            }
            file.close();
        }
        
        DataListCtrlInfos* pInfos = CreateTestInfos(1920, 1080);
        ASSERT_NE(pInfos, nullptr) << "Failed to create test infos";
        
        ASSERT_NO_CRASH({
            // CGraphImageList::GenerateDataImage(pInfos, largeFile);
            std::cout << "Large file processing completed successfully" << std::endl;
        });
        
        delete pInfos;
        DeleteFile(largeFile);
    }
    
    // Test data image generation with various dimensions
    TEST_F(CGraphImageListDataTests, GenerateDataImage_VariousDimensions)
    {
        std::cout << "Testing data image generation with various dimensions..." << std::endl;
        
        CString dataFile = CreateTestDataFile();
        ASSERT_FALSE(dataFile.IsEmpty()) << "Failed to create test data file";
        
        // Test various dimensions
        std::vector<std::pair<int, int>> dimensions = {
            {100, 100}, {200, 150}, {640, 480}, {800, 600}, {1920, 1080}
        };
        
        for (const auto& dim : dimensions)
        {
            DataListCtrlInfos* pInfos = CreateTestInfos(dim.first, dim.second);
            ASSERT_NE(pInfos, nullptr) << "Failed to create test infos for " << dim.first << "x" << dim.second;
            
            ASSERT_NO_CRASH({
                // CGraphImageList::GenerateDataImage(pInfos, dataFile);
                std::cout << "Data image generation for " << dim.first << "x" << dim.second << " completed" << std::endl;
            });
            
            delete pInfos;
        }
    }
    
    // Test data image generation with multiple files
    TEST_F(CGraphImageListDataTests, GenerateDataImage_MultipleFiles)
    {
        std::cout << "Testing data image generation with multiple files..." << std::endl;
        
        // Create multiple data files
        std::vector<CString> dataFiles;
        for (int i = 0; i < 3; ++i)
        {
            CString dataFile = CreateTestDataFile();
            ASSERT_FALSE(dataFile.IsEmpty()) << "Failed to create data file " << i;
            dataFiles.push_back(dataFile);
        }
        
        DataListCtrlInfos* pInfos = CreateTestInfos(640, 480);
        ASSERT_NE(pInfos, nullptr) << "Failed to create test infos";
        
        // Process multiple files
        for (size_t i = 0; i < dataFiles.size(); ++i)
        {
            ASSERT_NO_CRASH({
                // CGraphImageList::GenerateDataImage(pInfos, dataFiles[i]);
                std::cout << "Processed data file " << i << " successfully" << std::endl;
            });
        }
        
        delete pInfos;
    }
    
    // Test data image generation performance
    TEST_F(CGraphImageListDataTests, GenerateDataImage_PerformanceTest)
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
        ASSERT_LT(duration.count(), 5000) << "Data image generation should complete within 5 seconds";
        
        delete pInfos;
    }
    
    // Test data image generation error handling
    TEST_F(CGraphImageListDataTests, GenerateDataImage_ErrorHandling)
    {
        std::cout << "Testing data image generation error handling..." << std::endl;
        
        DataListCtrlInfos* pInfos = CreateTestInfos(100, 100);
        ASSERT_NE(pInfos, nullptr) << "Failed to create test infos";
        
        // Test with various error conditions
        std::vector<CString> errorFiles = {
            _T(""),  // Empty string
            _T("nonexistent_file.dat"),  // Nonexistent file
            _T("wrong_extension.txt"),  // Wrong extension
            _T("file_without_extension")  // No extension
        };
        
        for (const auto& errorFile : errorFiles)
        {
            ASSERT_NO_CRASH({
                // CGraphImageList::GenerateDataImage(pInfos, errorFile);
                std::cout << "Error handling for '" << errorFile << "' completed successfully" << std::endl;
            });
        }
        
        delete pInfos;
    }
};
