#pragma once

// Google Test Framework for Visual Studio 2022
// Using the standard Google Test package available in VS2022

#include <gtest/gtest.h>
#include <afxwin.h>
#include <afxext.h>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <chrono>
#include <sstream>
#include <iomanip>
#include "DataListCtrl_Infos.h"

// Forward declarations
class CBitmap;
class CDC;

// Google Test base class for CGraphImageList tests
class CGraphImageListTestBase : public ::testing::Test
{
protected:
    // SetUp method (runs before each test)
    void SetUp() override
    {
        std::wcout << L"Setting up individual test" << std::endl;
        
        // Reset test data
        testMessages.clear();
        
        // Create fresh test environment
        TestDataFixture::CleanupTestFiles();
    }
    
    // TearDown method (runs after each test)
    void TearDown() override
    {
        std::wcout << L"Cleaning up individual test" << std::endl;
        
        // Cleanup test files
        TestDataFixture::CleanupTestFiles();
        
        // Clear test messages
        testMessages.clear();
    }
    
    // Helper methods
    bool VerifyBitmapProperties(CBitmap* pBitmap, int expectedWidth, int expectedHeight);
    
    // Test data creation helpers
    CString CreateTestDataFile();
    CString CreateTestSpikeFile();
    CString CreateCorruptedSpikeFile();
    void CleanupTestFiles();
    
    // Test fixtures
    DataListCtrlInfos* CreateTestInfos(int width, int height);
    
    // Test result tracking
    std::vector<std::string> testMessages;
    void AddTestMessage(const std::string& message);
};

// Custom assertion macro for bitmap verification
#define ASSERT_BITMAP_PROPERTIES(bitmap, width, height) \
    ASSERT_TRUE(VerifyBitmapProperties((bitmap), (width), (height))) \
    << "Bitmap properties verification failed for " << width << "x" << height

// Custom assertion macro for no-crash testing
#define ASSERT_NO_CRASH(expression) \
    ASSERT_NO_THROW({ expression; }) << "Expression should not throw or crash"

// Test data fixture class for Google Test
class TestDataFixture
{
public:
    static CString CreateTestDataFile();
    static CString CreateTestSpikeFile();
    static CString CreateCorruptedSpikeFile();
    static CString CreateLargeSpikeFile();
    static CString CreateEmptySpikeFile();
    static void CleanupTestFiles();
    
private:
    static std::vector<CString> createdFiles;
    static CString GenerateUniqueFilename(const CString& prefix, const CString& extension);
};

// Test helper utilities
class TestHelpers
{
public:
    static bool VerifyBitmapProperties(CBitmap* pBitmap, int expectedWidth, int expectedHeight);
    static void SaveBitmapToFile(CBitmap* pBitmap, const CString& filename);
    static CBitmap* LoadBitmapFromFile(const CString& filename);
    static void LogBitmapInfo(CBitmap* pBitmap, const CString& context);
    
private:
    static CDC* CreateCompatibleDC();
};
