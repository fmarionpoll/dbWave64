#pragma once

#include <afxwin.h>
#include <afxext.h>
#include <iostream>
#include <vector>
#include <string>

// Forward declarations
class CBitmap;
class CDC;
struct DataListCtrlInfos;

// Test result structure
struct TestResult {
    bool passed;
    std::string message;
    int lineNumber;
    
    TestResult(bool p, const std::string& msg, int line) 
        : passed(p), message(msg), lineNumber(line) {}
};

// Test categories for CGraphImageList
class CGraphImageListTest {
public:
    CGraphImageListTest();
    ~CGraphImageListTest();
    
    // Main test runner
    bool RunAllTests();
    bool RunSpikeCrashTests();  // Focus on spike crash debugging
    
    // Individual test categories
    bool TestEmptyImageGeneration();
    bool TestDataImageGeneration();
    bool TestSpikeImageGeneration();  // Critical for crash debugging
    bool TestMemoryManagement();
    bool TestErrorHandling();
    bool TestStaticDataManagement();
    
    // Spike crash specific tests
    bool TestSpikeDocumentLoading();
    bool TestSpikeRendering();
    bool TestSpikeMemoryAllocation();
    bool TestSpikeInvalidData();
    
    // Utility methods
    void PrintTestResults() const;
    int GetPassedCount() const { return passedCount; }
    int GetFailedCount() const { return failedCount; }
    
private:
    // Test data
    std::vector<TestResult> testResults;
    int passedCount;
    int failedCount;
    
    // Helper methods
    void AddTestResult(bool passed, const std::string& message, int line);
    bool VerifyBitmapProperties(CBitmap* pBitmap, int expectedWidth, int expectedHeight);
    
    // Test data creation helpers
    CString CreateTestDataFile();
    CString CreateTestSpikeFile();
    CString CreateCorruptedSpikeFile();
    void CleanupTestFiles();
    
    // Test fixtures
    DataListCtrlInfos CreateTestInfos(int width, int height);
};

// Test macros for easy use
#define TEST_ASSERT(condition, message) \
    AddTestResult((condition), (message), __LINE__)

#define TEST_EXPECT_NO_CRASH(expression, message) \
    try { expression; AddTestResult(true, (message) + " (no crash)", __LINE__); } catch (...) { AddTestResult(false, (message) + " (crashed)", __LINE__); }

#define TEST_VERIFY_BITMAP(bitmap, width, height, message) \
    AddTestResult(VerifyBitmapProperties((bitmap), (width), (height)), (message), __LINE__)
