#pragma once

#include "CGraphImageListTest.h"
#include <afxwin.h>
#include <iostream>
#include <string>

// Test runner class for managing test execution
class TestRunner {
public:
    TestRunner();
    ~TestRunner();
    
    // Main test execution methods
    bool RunAllTests();
    bool RunSpikeCrashTests();
    bool RunSpecificTest(const std::string& testName);
    
    // Test reporting
    void PrintSummary() const;
    void SaveResultsToFile(const std::string& filename) const;
    
    // Integration with Visual Studio
    void ShowResultsInMessageBox() const;
    
    // Performance testing
    void RunPerformanceTests();
    
    // Debugging support
    void EnableDebugOutput(bool enable);
    void SetBreakOnFailure(bool breakOnFailure);
    
private:
    CGraphImageListTest* pTestSuite;
    bool debugOutputEnabled;
    bool breakOnFailure;
    
    // Performance metrics
    struct PerformanceMetrics {
        double totalTime;
        int totalTests;
        int passedTests;
        int failedTests;
        double averageTestTime;
    };
    
    PerformanceMetrics metrics;
    
    // Helper methods
    void InitializeTestEnvironment();
    void CleanupTestEnvironment();
    void UpdateMetrics(double testTime, bool passed);
    void PrintPerformanceReport() const;
};
