#pragma once

#include <gtest/gtest.h>
#include <afxwin.h>
#include <afxext.h>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <chrono>

// Forward declarations
class CBitmap;
class CDC;
class DataListCtrlInfos;

// Base test class for CGraphImageList tests
class CGraphImageListTestBase : public ::testing::Test
{
protected:
    void SetUp() override
    {
        std::cout << "Setting up test..." << std::endl;
        testMessages.clear();
        CleanupTestFiles();
    }
    
    void TearDown() override
    {
        std::cout << "Cleaning up test..." << std::endl;
        CleanupTestFiles();
        testMessages.clear();
    }
    
    // Helper methods
    bool VerifyBitmapProperties(CBitmap* pBitmap, int expectedWidth, int expectedHeight);
    CString CreateTestDataFile();
    CString CreateTestSpikeFile();
    CString CreateCorruptedSpikeFile();
    void CleanupTestFiles();
    DataListCtrlInfos* CreateTestInfos(int width, int height);
    void AddTestMessage(const std::string& message);
    
    // Test result tracking
    std::vector<std::string> testMessages;
};

// Custom assertion macro for no-crash testing
#define ASSERT_NO_CRASH(expression) \
    ASSERT_NO_THROW({ expression; }) << "Expression should not throw or crash"

// Test data fixture class
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
