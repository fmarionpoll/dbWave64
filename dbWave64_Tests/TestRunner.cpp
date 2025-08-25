#include "TestRunner.h"
#include <chrono>
#include <fstream>
#include <iomanip>

TestRunner::TestRunner() 
    : pTestSuite(nullptr), debugOutputEnabled(false), breakOnFailure(false) {
    metrics = {0.0, 0, 0, 0, 0.0};
    InitializeTestEnvironment();
}

TestRunner::~TestRunner() {
    CleanupTestEnvironment();
}

void TestRunner::InitializeTestEnvironment() {
    std::cout << "=== Initializing Test Environment ===" << std::endl;
    
    // Create test suite
    pTestSuite = new CGraphImageListTest();
    
    std::cout << "Test environment initialized successfully" << std::endl;
}

void TestRunner::CleanupTestEnvironment() {
    if (pTestSuite) {
        delete pTestSuite;
        pTestSuite = nullptr;
    }
    std::cout << "Test environment cleaned up" << std::endl;
}

bool TestRunner::RunAllTests() {
    if (!pTestSuite) {
        std::cout << "ERROR: Test suite not initialized" << std::endl;
        return false;
    }
    
    std::cout << "\n=== Starting Complete Test Suite ===" << std::endl;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    bool success = pTestSuite->RunAllTests();
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    UpdateMetrics(duration.count() / 1000.0, success);
    
    PrintSummary();
    return success;
}

bool TestRunner::RunSpikeCrashTests() {
    if (!pTestSuite) {
        std::cout << "ERROR: Test suite not initialized" << std::endl;
        return false;
    }
    
    std::cout << "\n=== Starting Spike Crash Debug Tests ===" << std::endl;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    bool success = pTestSuite->RunSpikeCrashTests();
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    UpdateMetrics(duration.count() / 1000.0, success);
    
    PrintSummary();
    return success;
}

bool TestRunner::RunSpecificTest(const std::string& testName) {
    if (!pTestSuite) {
        std::cout << "ERROR: Test suite not initialized" << std::endl;
        return false;
    }
    
    std::cout << "\n=== Running Specific Test: " << testName << " ===" << std::endl;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    bool success = false;
    
    // Map test names to specific test methods
    if (testName == "EmptyImageGeneration") {
        success = pTestSuite->TestEmptyImageGeneration();
    } else if (testName == "DataImageGeneration") {
        success = pTestSuite->TestDataImageGeneration();
    } else if (testName == "SpikeImageGeneration") {
        success = pTestSuite->TestSpikeImageGeneration();
    } else if (testName == "MemoryManagement") {
        success = pTestSuite->TestMemoryManagement();
    } else if (testName == "ErrorHandling") {
        success = pTestSuite->TestErrorHandling();
    } else if (testName == "SpikeDocumentLoading") {
        success = pTestSuite->TestSpikeDocumentLoading();
    } else if (testName == "SpikeRendering") {
        success = pTestSuite->TestSpikeRendering();
    } else if (testName == "SpikeMemoryAllocation") {
        success = pTestSuite->TestSpikeMemoryAllocation();
    } else if (testName == "SpikeInvalidData") {
        success = pTestSuite->TestSpikeInvalidData();
    } else {
        std::cout << "ERROR: Unknown test name: " << testName << std::endl;
        return false;
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    UpdateMetrics(duration.count() / 1000.0, success);
    
    return success;
}

void TestRunner::PrintSummary() const {
    std::cout << "\n=== Test Runner Summary ===" << std::endl;
    std::cout << "Total Tests: " << metrics.totalTests << std::endl;
    std::cout << "Passed: " << metrics.passedTests << std::endl;
    std::cout << "Failed: " << metrics.failedTests << std::endl;
    std::cout << "Success Rate: " << std::fixed << std::setprecision(1) 
              << (metrics.totalTests > 0 ? (metrics.passedTests * 100.0 / metrics.totalTests) : 0) << "%" << std::endl;
    std::cout << "Total Time: " << std::fixed << std::setprecision(3) << metrics.totalTime << " seconds" << std::endl;
    std::cout << "Average Test Time: " << std::fixed << std::setprecision(3) << metrics.averageTestTime << " seconds" << std::endl;
}

void TestRunner::SaveResultsToFile(const std::string& filename) const {
    std::ofstream file(filename);
    if (file.is_open()) {
        file << "=== CGraphImageList Test Results ===" << std::endl;
        file << "Date: " << __DATE__ << " " << __TIME__ << std::endl;
        file << "Total Tests: " << metrics.totalTests << std::endl;
        file << "Passed: " << metrics.passedTests << std::endl;
        file << "Failed: " << metrics.failedTests << std::endl;
        file << "Success Rate: " << std::fixed << std::setprecision(1) 
             << (metrics.totalTests > 0 ? (metrics.passedTests * 100.0 / metrics.totalTests) : 0) << "%" << std::endl;
        file << "Total Time: " << std::fixed << std::setprecision(3) << metrics.totalTime << " seconds" << std::endl;
        file << "Average Test Time: " << std::fixed << std::setprecision(3) << metrics.averageTestTime << " seconds" << std::endl;
        file.close();
        std::cout << "Results saved to: " << filename << std::endl;
    } else {
        std::cout << "ERROR: Could not save results to: " << filename << std::endl;
    }
}

void TestRunner::ShowResultsInMessageBox() const {
    CString message;
    message.Format(_T("CGraphImageList Tests Completed\n\n")
                   _T("Total Tests: %d\n")
                   _T("Passed: %d\n")
                   _T("Failed: %d\n")
                   _T("Success Rate: %.1f%%\n")
                   _T("Total Time: %.3f seconds"),
                   metrics.totalTests,
                   metrics.passedTests,
                   metrics.failedTests,
                   (metrics.totalTests > 0 ? (metrics.passedTests * 100.0 / metrics.totalTests) : 0),
                   metrics.totalTime);
    
    AfxMessageBox(message, MB_OK | (metrics.failedTests > 0 ? MB_ICONWARNING : MB_ICONINFORMATION));
}

void TestRunner::RunPerformanceTests() {
    std::cout << "\n=== Running Performance Tests ===" << std::endl;
    
    // Test basic bitmap creation performance
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 100; ++i) {
        CBitmap* pBitmap = new CBitmap();
        if (pBitmap) delete pBitmap;
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    std::cout << "Performance Test: 100 basic bitmaps created in " 
              << duration.count() << " ms" << std::endl;
    std::cout << "Average time per bitmap: " << (duration.count() / 100.0) << " ms" << std::endl;
}

void TestRunner::EnableDebugOutput(bool enable) {
    debugOutputEnabled = enable;
    std::cout << "Debug output " << (enable ? "enabled" : "disabled") << std::endl;
}

void TestRunner::SetBreakOnFailure(bool breakOnFailure) {
    this->breakOnFailure = breakOnFailure;
    std::cout << "Break on failure " << (breakOnFailure ? "enabled" : "disabled") << std::endl;
}

void TestRunner::UpdateMetrics(double testTime, bool passed) {
    metrics.totalTime += testTime;
    metrics.totalTests++;
    
    if (passed) {
        metrics.passedTests++;
    } else {
        metrics.failedTests++;
    }
    
    metrics.averageTestTime = metrics.totalTime / metrics.totalTests;
}

void TestRunner::PrintPerformanceReport() const {
    std::cout << "\n=== Performance Report ===" << std::endl;
    std::cout << "Total execution time: " << std::fixed << std::setprecision(3) 
              << metrics.totalTime << " seconds" << std::endl;
    std::cout << "Average test time: " << std::fixed << std::setprecision(3) 
              << metrics.averageTestTime << " seconds" << std::endl;
    std::cout << "Tests per second: " << std::fixed << std::setprecision(1) 
              << (metrics.totalTime > 0 ? metrics.totalTests / metrics.totalTime : 0) << std::endl;
}
