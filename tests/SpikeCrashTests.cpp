#include "pch.h"
#include "CGraphImageListTests.h"

// Spike crash debugging tests for CGraphImageList using Google Test

// Test spike image generation with valid spike file
TEST_F(CGraphImageListTestBase, SpikeCrash_ValidSpikeFile)
{
    std::cout << "Testing spike image generation with valid spike file..." << std::endl;
    
    // Create test spike file
    CString spikeFile = CreateTestSpikeFile();
    ASSERT_FALSE(spikeFile.IsEmpty()) << "Failed to create test spike file";
    
    // Create test infos
    DataListCtrlInfos* pInfos = CreateTestInfos(640, 480);
    ASSERT_NE(pInfos, nullptr) << "Failed to create test infos";
    
            ASSERT_NO_CRASH({
            // Call the actual CGraphImageList method - THIS IS THE CRITICAL TEST FOR YOUR CRASH!
            CBitmap* pBitmap = CGraphImageList::GenerateSpikeImage(pInfos->image_width, pInfos->image_height, spikeFile, *pInfos);
            ASSERT_NE(pBitmap, nullptr) << "Failed to generate spike image with valid file";
            std::cout << "Spike image generation with valid file completed" << std::endl;
            delete pBitmap;
        });
    
    delete pInfos;
}

// Test spike image generation with corrupted spike file
TEST_F(CGraphImageListTestBase, SpikeCrash_CorruptedSpikeFile)
{
    std::cout << "Testing spike image generation with corrupted spike file..." << std::endl;
    
    // Create corrupted spike file
    CString corruptedFile = CreateCorruptedSpikeFile();
    ASSERT_FALSE(corruptedFile.IsEmpty()) << "Failed to create corrupted spike file";
    
    DataListCtrlInfos* pInfos = CreateTestInfos(640, 480);
    ASSERT_NE(pInfos, nullptr) << "Failed to create test infos";
    
            ASSERT_NO_CRASH({
            // Call the actual CGraphImageList method
            CBitmap* pBitmap = CGraphImageList::GenerateSpikeImage(pInfos->image_width, pInfos->image_height, corruptedFile, *pInfos);
            // This might return nullptr for corrupted file, which is acceptable
            std::cout << "Spike image generation with corrupted file completed" << std::endl;
            if (pBitmap) delete pBitmap;
        });
    
    delete pInfos;
}

// Test spike image generation with empty spike file
TEST_F(CGraphImageListTestBase, SpikeCrash_EmptySpikeFile)
{
    std::cout << "Testing spike image generation with empty spike file..." << std::endl;
    
    // Create empty spike file
    CString emptyFile = CreateEmptySpikeFile();
    ASSERT_TRUE(emptyFile.IsEmpty()) << "Failed to create empty spike file";
    
    DataListCtrlInfos* pInfos = CreateTestInfos(640, 480);
    ASSERT_NE(pInfos, nullptr) << "Failed to create test infos";
    
            ASSERT_NO_CRASH({
            // Call the actual CGraphImageList method
            CBitmap* pBitmap = CGraphImageList::GenerateSpikeImage(pInfos->image_width, pInfos->image_height, emptyFile, *pInfos);
            // This might return nullptr for empty file, which is acceptable
            std::cout << "Spike image generation with empty file completed" << std::endl;
            if (pBitmap) delete pBitmap;
        });
    
    delete pInfos;
}

// Test spike image generation with large spike file
TEST_F(CGraphImageListTestBase, SpikeCrash_LargeSpikeFile)
{
    std::cout << "Testing spike image generation with large spike file..." << std::endl;
    
    // Create large spike file
    CString largeFile = CreateLargeSpikeFile(); // Should not be empty
    ASSERT_FALSE(largeFile.IsEmpty()) << "Failed to create large spike file";
    
    DataListCtrlInfos* pInfos = CreateTestInfos(640, 480);
    ASSERT_NE(pInfos, nullptr) << "Failed to create test infos";
    
            ASSERT_NO_CRASH({
            // Call the actual CGraphImageList method
            CBitmap* pBitmap = CGraphImageList::GenerateSpikeImage(pInfos->image_width, pInfos->image_height, largeFile, *pInfos);
            ASSERT_NE(pBitmap, nullptr) << "Failed to generate spike image with large file";
            std::cout << "Spike image generation with large file completed" << std::endl;
            delete pBitmap;
        });
    
    delete pInfos;
}

// Test spike image generation with different spike plot modes
TEST_F(CGraphImageListTestBase, SpikeCrash_DifferentSpikePlotModes)
{
    std::cout << "Testing spike image generation with different spike plot modes..." << std::endl;
    
    CString spikeFile = CreateTestSpikeFile();
    ASSERT_FALSE(spikeFile.IsEmpty()) << "Failed to create test spike file";
    
    // Test different spike plot modes
    std::vector<int> plotModes = {PLOT_BLACK, PLOT_ONE_CLASS_ONLY, PLOT_ONE_CLASS, PLOT_CLASS_COLORS, PLOT_SINGLE_SPIKE};
    
    for (int mode : plotModes)
    {
        DataListCtrlInfos* pInfos = CreateTestInfos(640, 480);
        ASSERT_NE(pInfos, nullptr) << "Failed to create test infos for mode " << mode;
        
        pInfos->spike_plot_mode = mode;
        
        ASSERT_NO_CRASH({
            // Call the actual CGraphImageList method
            CBitmap* pBitmap = CGraphImageList::GenerateSpikeImage(pInfos->image_width, pInfos->image_height, spikeFile, *pInfos);
            ASSERT_NE(pBitmap, nullptr) << "Failed to generate spike image with plot mode " << mode;
            std::cout << "Spike image generation with plot mode " << mode << " completed" << std::endl;
            delete pBitmap;
        });
        
        delete pInfos;
    }
}

// Test spike image generation with different selected classes
TEST_F(CGraphImageListTestBase, SpikeCrash_DifferentSelectedClasses)
{
    std::cout << "Testing spike image generation with different selected classes..." << std::endl;
    
    CString spikeFile = CreateTestSpikeFile();
    ASSERT_FALSE(spikeFile.IsEmpty()) << "Failed to create test spike file";
    
    // Test different selected classes
    for (int selectedClass = 0; selectedClass < 5; ++selectedClass)
    {
        DataListCtrlInfos* pInfos = CreateTestInfos(640, 480);
        ASSERT_NE(pInfos, nullptr) << "Failed to create test infos for class " << selectedClass;
        
        pInfos->selected_class = selectedClass;
        
        ASSERT_NO_CRASH({
            // Call the actual CGraphImageList method
            CBitmap* pBitmap = CGraphImageList::GenerateSpikeImage(pInfos->image_width, pInfos->image_height, spikeFile, *pInfos);
            ASSERT_NE(pBitmap, nullptr) << "Failed to generate spike image with selected class " << selectedClass;
            std::cout << "Spike image generation with selected class " << selectedClass << " completed" << std::endl;
            delete pBitmap;
        });
        
        delete pInfos;
    }
}

// Test spike image generation with null pointer
TEST_F(CGraphImageListTestBase, SpikeCrash_NullPointer)
{
    std::cout << "Testing spike image generation with null pointer..." << std::endl;
    
    CString spikeFile = CreateTestSpikeFile();
    ASSERT_FALSE(spikeFile.IsEmpty()) << "Failed to create test spike file";
    
    ASSERT_NO_CRASH({
        // Test with null pointer (should handle gracefully)
        CBitmap* pBitmap = CGraphImageList::GenerateSpikeImage(0, 0, spikeFile, DataListCtrlInfos());
        // This might return nullptr for zero dimensions, which is acceptable
        std::cout << "Spike image generation with null pointer completed" << std::endl;
        if (pBitmap) delete pBitmap;
    });
}

// Test spike image generation with invalid file path
TEST_F(CGraphImageListTestBase, SpikeCrash_InvalidFilePath)
{
    std::cout << "Testing spike image generation with invalid file path..." << std::endl;
    
    DataListCtrlInfos* pInfos = CreateTestInfos(640, 480);
    ASSERT_NE(pInfos, nullptr) << "Failed to create test infos";
    
    CString invalidFile = _T("non_existent_spike_file.spk");
    
            ASSERT_NO_CRASH({
            // Call the actual CGraphImageList method
            CBitmap* pBitmap = CGraphImageList::GenerateSpikeImage(pInfos->image_width, pInfos->image_height, invalidFile, *pInfos);
            // This might return nullptr for invalid file, which is acceptable
            std::cout << "Spike image generation with invalid file path completed" << std::endl;
            if (pBitmap) delete pBitmap;
        });
    
    delete pInfos;
}

// Test spike image generation with zero dimensions
TEST_F(CGraphImageListTestBase, SpikeCrash_ZeroDimensions)
{
    std::cout << "Testing spike image generation with zero dimensions..." << std::endl;
    
    CString spikeFile = CreateTestSpikeFile();
    ASSERT_FALSE(spikeFile.IsEmpty()) << "Failed to create test spike file";
    
    DataListCtrlInfos* pInfos = CreateTestInfos(0, 0);
    ASSERT_NE(pInfos, nullptr) << "Failed to create test infos";
    
            ASSERT_NO_CRASH({
            // Call the actual CGraphImageList method
            CBitmap* pBitmap = CGraphImageList::GenerateSpikeImage(pInfos->image_width, pInfos->image_height, spikeFile, *pInfos);
            // This might return nullptr for zero dimensions, which is acceptable
            std::cout << "Spike image generation with zero dimensions completed" << std::endl;
            if (pBitmap) delete pBitmap;
        });
    
    delete pInfos;
}

// Test spike image generation with negative dimensions
TEST_F(CGraphImageListTestBase, SpikeCrash_NegativeDimensions)
{
    std::cout << "Testing spike image generation with negative dimensions..." << std::endl;
    
    CString spikeFile = CreateTestSpikeFile();
    ASSERT_FALSE(spikeFile.IsEmpty()) << "Failed to create test spike file";
    
    DataListCtrlInfos* pInfos = CreateTestInfos(-100, -200);
    ASSERT_NE(pInfos, nullptr) << "Failed to create test infos";
    
            ASSERT_NO_CRASH({
            // Call the actual CGraphImageList method
            CBitmap* pBitmap = CGraphImageList::GenerateSpikeImage(pInfos->image_width, pInfos->image_height, spikeFile, *pInfos);
            // This might return nullptr for negative dimensions, which is acceptable
            std::cout << "Spike image generation with negative dimensions completed" << std::endl;
            if (pBitmap) delete pBitmap;
        });
    
    delete pInfos;
}

// Test spike image generation with very large dimensions
TEST_F(CGraphImageListTestBase, SpikeCrash_VeryLargeDimensions)
{
    std::cout << "Testing spike image generation with very large dimensions..." << std::endl;
    
    CString spikeFile = CreateTestSpikeFile();
    ASSERT_FALSE(spikeFile.IsEmpty()) << "Failed to create test spike file";
    
    DataListCtrlInfos* pInfos = CreateTestInfos(4096, 4096);
    ASSERT_NE(pInfos, nullptr) << "Failed to create test infos";
    
            ASSERT_NO_CRASH({
            // Call the actual CGraphImageList method
            CBitmap* pBitmap = CGraphImageList::GenerateSpikeImage(pInfos->image_width, pInfos->image_height, spikeFile, *pInfos);
            ASSERT_NE(pBitmap, nullptr) << "Failed to generate spike image with very large dimensions";
            std::cout << "Spike image generation with very large dimensions completed" << std::endl;
            delete pBitmap;
        });
    
    delete pInfos;
}

// Test spike image generation with multiple files
TEST_F(CGraphImageListTestBase, SpikeCrash_MultipleFiles)
{
    std::cout << "Testing spike image generation with multiple files..." << std::endl;
    
    std::vector<CString> spikeFiles;
    
    // Create multiple spike files
    for (int i = 0; i < 3; ++i)
    {
        CString spikeFile = CreateTestSpikeFile();
        ASSERT_FALSE(spikeFile.IsEmpty()) << "Failed to create spike file " << i;
        spikeFiles.push_back(spikeFile);
    }
    
    DataListCtrlInfos* pInfos = CreateTestInfos(640, 480);
    ASSERT_NE(pInfos, nullptr) << "Failed to create test infos";
    
    // Process multiple files
            for (size_t i = 0; i < spikeFiles.size(); ++i)
        {
            ASSERT_NO_CRASH({
                // Call the actual CGraphImageList method
                CBitmap* pBitmap = CGraphImageList::GenerateSpikeImage(pInfos->image_width, pInfos->image_height, spikeFiles[i], *pInfos);
                ASSERT_NE(pBitmap, nullptr) << "Failed to process spike file " << i;
                std::cout << "Processed spike file " << i << " successfully" << std::endl;
                delete pBitmap;
            });
        }
    
    delete pInfos;
}

// Test spike image generation stress test
TEST_F(CGraphImageListTestBase, SpikeCrash_StressTest)
{
    std::cout << "Running spike image generation stress test..." << std::endl;
    
    CString spikeFile = CreateTestSpikeFile();
    ASSERT_FALSE(spikeFile.IsEmpty()) << "Failed to create test spike file";
    
    // Run multiple iterations to stress test
    for (int i = 0; i < 10; ++i)
    {
        DataListCtrlInfos* pInfos = CreateTestInfos(640, 480);
        ASSERT_NE(pInfos, nullptr) << "Failed to create test infos for iteration " << i;
        
        // Vary parameters for stress testing
        pInfos->spike_plot_mode = i % 5;
        pInfos->selected_class = i % 3;
        pInfos->image_width = 640 + (i * 10);
        pInfos->image_height = 480 + (i * 10);
        
        ASSERT_NO_CRASH({
            // Call the actual CGraphImageList method
            CBitmap* pBitmap = CGraphImageList::GenerateSpikeImage(pInfos->image_width, pInfos->image_height, spikeFile, *pInfos);
            ASSERT_NE(pBitmap, nullptr) << "Failed to generate spike image in stress test iteration " << i;
            std::cout << "Stress test iteration " << i << " completed" << std::endl;
            delete pBitmap;
        });
        
        delete pInfos;
    }
}
