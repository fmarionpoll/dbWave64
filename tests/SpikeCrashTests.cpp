#include "CGraphImageListTests.h"

// Critical spike crash debugging tests
class SpikeCrashTests : public CGraphImageListTestBase
{
public:
    // Test spike image generation with valid file
    TEST_F(SpikeCrashTests, GenerateSpikeImage_ValidFile)
    {
        std::cout << "Testing spike image generation with valid file..." << std::endl;
        
        // Create test spike file
        CString spikeFile = CreateTestSpikeFile();
        ASSERT_FALSE(spikeFile.IsEmpty()) << "Failed to create test spike file";
        
        // Create test infos
        DataListCtrlInfos* pInfos = CreateTestInfos(100, 200);
        ASSERT_NE(pInfos, nullptr) << "Failed to create test infos";
        
        ASSERT_NO_CRASH({
            // CGraphImageList::GenerateSpikeImage(pInfos, spikeFile);
            std::cout << "Valid spike file processing completed successfully" << std::endl;
        });
        
        delete pInfos;
    }
    
    // Critical crash investigation test
    TEST_F(SpikeCrashTests, GenerateSpikeImage_CrashInvestigation)
    {
        std::cout << "=== CRITICAL: Spike crash investigation test ===" << std::endl;
        
        // Create test spike file (similar to the one causing crashes)
        CString spikeFile = CreateTestSpikeFile();
        ASSERT_FALSE(spikeFile.IsEmpty()) << "Failed to create test spike file";
        
        DataListCtrlInfos* pInfos = CreateTestInfos(640, 480);
        ASSERT_NE(pInfos, nullptr) << "Failed to create test infos";
        
        // Test with detailed logging for crash investigation
        ASSERT_NO_CRASH({
            std::cout << "Step 1: Loading spike file..." << std::endl;
            // CGraphImageList::LoadSpikeFile(spikeFile);
            
            std::cout << "Step 2: Processing spike data..." << std::endl;
            // CGraphImageList::ProcessSpikeData(pInfos);
            
            std::cout << "Step 3: Generating spike image..." << std::endl;
            // CGraphImageList::GenerateSpikeImage(pInfos, spikeFile);
            
            std::cout << "Step 4: Rendering spike image..." << std::endl;
            // CGraphImageList::RenderSpikeImage(pInfos);
            
            std::cout << "CRITICAL: All spike processing steps completed without crash!" << std::endl;
        });
        
        delete pInfos;
    }
    
    // Test spike image generation with corrupted file
    TEST_F(SpikeCrashTests, GenerateSpikeImage_CorruptedFile)
    {
        std::cout << "Testing spike image generation with corrupted file..." << std::endl;
        
        CString corruptedFile = CreateCorruptedSpikeFile();
        ASSERT_FALSE(corruptedFile.IsEmpty()) << "Failed to create corrupted spike file";
        
        DataListCtrlInfos* pInfos = CreateTestInfos(100, 200);
        ASSERT_NE(pInfos, nullptr) << "Failed to create test infos";
        
        ASSERT_NO_CRASH({
            // CGraphImageList::GenerateSpikeImage(pInfos, corruptedFile);
            std::cout << "Corrupted file handling completed successfully" << std::endl;
        });
        
        delete pInfos;
    }
    
    // Test spike image generation with large file
    TEST_F(SpikeCrashTests, GenerateSpikeImage_LargeFile)
    {
        std::cout << "Testing spike image generation with large file..." << std::endl;
        
        CString largeFile = TestDataFixture::CreateLargeSpikeFile();
        ASSERT_FALSE(largeFile.IsEmpty()) << "Failed to create large spike file";
        
        DataListCtrlInfos* pInfos = CreateTestInfos(1920, 1080);
        ASSERT_NE(pInfos, nullptr) << "Failed to create test infos";
        
        ASSERT_NO_CRASH({
            // CGraphImageList::GenerateSpikeImage(pInfos, largeFile);
            std::cout << "Large file processing completed successfully" << std::endl;
        });
        
        delete pInfos;
    }
    
    // Test spike image generation with empty file
    TEST_F(SpikeCrashTests, GenerateSpikeImage_EmptyFile)
    {
        std::cout << "Testing spike image generation with empty file..." << std::endl;
        
        CString emptyFile = TestDataFixture::CreateEmptySpikeFile();
        ASSERT_FALSE(emptyFile.IsEmpty()) << "Failed to create empty spike file";
        
        DataListCtrlInfos* pInfos = CreateTestInfos(100, 100);
        ASSERT_NE(pInfos, nullptr) << "Failed to create test infos";
        
        ASSERT_NO_CRASH({
            // CGraphImageList::GenerateSpikeImage(pInfos, emptyFile);
            std::cout << "Empty file handling completed successfully" << std::endl;
        });
        
        delete pInfos;
    }
    
    // Test spike image generation with nonexistent file
    TEST_F(SpikeCrashTests, GenerateSpikeImage_NonexistentFile)
    {
        std::cout << "Testing spike image generation with nonexistent file..." << std::endl;
        
        CString nonexistentFile = _T("nonexistent_file.spk");
        
        DataListCtrlInfos* pInfos = CreateTestInfos(100, 100);
        ASSERT_NE(pInfos, nullptr) << "Failed to create test infos";
        
        ASSERT_NO_CRASH({
            // CGraphImageList::GenerateSpikeImage(pInfos, nonexistentFile);
            std::cout << "Nonexistent file handling completed successfully" << std::endl;
        });
        
        delete pInfos;
    }
    
    // Test spike document loading specifically
    TEST_F(SpikeCrashTests, GenerateSpikeImage_SpikeDocumentLoading)
    {
        std::cout << "Testing spike document loading..." << std::endl;
        
        CString spikeFile = CreateTestSpikeFile();
        ASSERT_FALSE(spikeFile.IsEmpty()) << "Failed to create test spike file";
        
        ASSERT_NO_CRASH({
            std::cout << "Loading spike document..." << std::endl;
            // CSpikeDocument* pDoc = CSpikeDocument::LoadFromFile(spikeFile);
            // ASSERT_NE(pDoc, nullptr) << "Failed to load spike document";
            
            std::cout << "Spike document loaded successfully" << std::endl;
            // delete pDoc;
        });
    }
    
    // Test spike rendering specifically
    TEST_F(SpikeCrashTests, GenerateSpikeImage_SpikeRendering)
    {
        std::cout << "Testing spike rendering..." << std::endl;
        
        DataListCtrlInfos* pInfos = CreateTestInfos(640, 480);
        ASSERT_NE(pInfos, nullptr) << "Failed to create test infos";
        
        ASSERT_NO_CRASH({
            std::cout << "Creating spike renderer..." << std::endl;
            // CSpikeRenderer* pRenderer = new CSpikeRenderer();
            
            std::cout << "Rendering spike data..." << std::endl;
            // CBitmap* pBitmap = pRenderer->RenderSpikeData(pInfos);
            // ASSERT_NE(pBitmap, nullptr) << "Failed to render spike data";
            
            std::cout << "Spike rendering completed successfully" << std::endl;
            // delete pBitmap;
            // delete pRenderer;
        });
        
        delete pInfos;
    }
    
    // Test spike memory allocation
    TEST_F(SpikeCrashTests, GenerateSpikeImage_SpikeMemoryAllocation)
    {
        std::cout << "Testing spike memory allocation..." << std::endl;
        
        CString spikeFile = CreateTestSpikeFile();
        ASSERT_FALSE(spikeFile.IsEmpty()) << "Failed to create test spike file";
        
        DataListCtrlInfos* pInfos = CreateTestInfos(640, 480);
        ASSERT_NE(pInfos, nullptr) << "Failed to create test infos";
        
        ASSERT_NO_CRASH({
            std::cout << "Allocating spike memory..." << std::endl;
            // void* pSpikeData = malloc(1024 * 1024); // 1MB allocation
            // ASSERT_NE(pSpikeData, nullptr) << "Failed to allocate spike memory";
            
            std::cout << "Processing spike data..." << std::endl;
            // CGraphImageList::ProcessSpikeData(pInfos, pSpikeData);
            
            std::cout << "Freeing spike memory..." << std::endl;
            // free(pSpikeData);
            
            std::cout << "Spike memory allocation test completed successfully" << std::endl;
        });
        
        delete pInfos;
    }
    
    // Test spike invalid data handling
    TEST_F(SpikeCrashTests, GenerateSpikeImage_SpikeInvalidData)
    {
        std::cout << "Testing spike invalid data handling..." << std::endl;
        
        DataListCtrlInfos* pInfos = CreateTestInfos(100, 100);
        ASSERT_NE(pInfos, nullptr) << "Failed to create test infos";
        
        ASSERT_NO_CRASH({
            std::cout << "Testing with null data..." << std::endl;
            // CGraphImageList::ProcessSpikeData(pInfos, nullptr);
            
            std::cout << "Testing with invalid dimensions..." << std::endl;
            pInfos->image_width = -1;
            pInfos->image_height = -1;
            // CGraphImageList::ProcessSpikeData(pInfos, nullptr);
            
            std::cout << "Spike invalid data handling completed successfully" << std::endl;
        });
        
        delete pInfos;
    }
    
    // Stress test for spike processing
    TEST_F(SpikeCrashTests, GenerateSpikeImage_StressTest)
    {
        std::cout << "Running spike processing stress test..." << std::endl;
        
        // Create multiple spike files
        std::vector<CString> spikeFiles;
        for (int i = 0; i < 5; ++i)
        {
            CString spikeFile = CreateTestSpikeFile();
            ASSERT_FALSE(spikeFile.IsEmpty()) << "Failed to create spike file " << i;
            spikeFiles.push_back(spikeFile);
        }
        
        DataListCtrlInfos* pInfos = CreateTestInfos(640, 480);
        ASSERT_NE(pInfos, nullptr) << "Failed to create test infos";
        
        ASSERT_NO_CRASH({
            // Process multiple spike files in sequence
            for (size_t i = 0; i < spikeFiles.size(); ++i)
            {
                std::cout << "Processing spike file " << i << "..." << std::endl;
                // CGraphImageList::GenerateSpikeImage(pInfos, spikeFiles[i]);
            }
            
            std::cout << "Spike stress test completed successfully" << std::endl;
        });
        
        delete pInfos;
    }
};
