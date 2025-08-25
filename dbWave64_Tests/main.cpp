#include "TestRunner.h"
#include <iostream>
#include <string>
#include <afxwin.h>

// Main entry point for the test project
int main(int argc, char* argv[]) {
    // Initialize MFC
    if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0)) {
        std::cout << "ERROR: MFC initialization failed" << std::endl;
        return 1;
    }
    
    std::cout << "=== CGraphImageList Test Suite ===" << std::endl;
    std::cout << "Build Date: " << __DATE__ << " " << __TIME__ << std::endl;
    std::cout << "==================================" << std::endl;
    
    TestRunner testRunner;
    
    // Parse command line arguments
    bool runAllTests = true;
    bool runSpikeTests = false;
    bool runSpecificTest = false;
    std::string specificTestName;
    bool saveResults = false;
    std::string resultsFile = "test_results.txt";
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--spike-only" || arg == "-s") {
            runAllTests = false;
            runSpikeTests = true;
        } else if (arg == "--test" || arg == "-t") {
            if (i + 1 < argc) {
                runAllTests = false;
                runSpecificTest = true;
                specificTestName = argv[++i];
            }
        } else if (arg == "--save-results" || arg == "-r") {
            saveResults = true;
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                resultsFile = argv[++i];
            }
        } else if (arg == "--help" || arg == "-h") {
            std::cout << "Usage: dbWave64_Tests [options]" << std::endl;
            std::cout << "Options:" << std::endl;
            std::cout << "  --spike-only, -s     Run only spike crash tests" << std::endl;
            std::cout << "  --test <name>, -t    Run specific test" << std::endl;
            std::cout << "  --save-results, -r   Save results to file" << std::endl;
            std::cout << "  --help, -h           Show this help" << std::endl;
            std::cout << std::endl;
            std::cout << "Available tests:" << std::endl;
            std::cout << "  EmptyImageGeneration" << std::endl;
            std::cout << "  DataImageGeneration" << std::endl;
            std::cout << "  SpikeImageGeneration" << std::endl;
            std::cout << "  MemoryManagement" << std::endl;
            std::cout << "  ErrorHandling" << std::endl;
            std::cout << "  SpikeDocumentLoading" << std::endl;
            std::cout << "  SpikeRendering" << std::endl;
            std::cout << "  SpikeMemoryAllocation" << std::endl;
            std::cout << "  SpikeInvalidData" << std::endl;
            return 0;
        }
    }
    
    bool success = false;
    
    try {
        if (runSpikeTests) {
            std::cout << "Running spike crash tests only..." << std::endl;
            success = testRunner.RunSpikeCrashTests();
        } else if (runSpecificTest) {
            std::cout << "Running specific test: " << specificTestName << std::endl;
            success = testRunner.RunSpecificTest(specificTestName);
        } else {
            std::cout << "Running all tests..." << std::endl;
            success = testRunner.RunAllTests();
        }
        
        // Save results if requested
        if (saveResults) {
            testRunner.SaveResultsToFile(resultsFile);
        }
        
        // Show results in message box if running in Windows environment
        testRunner.ShowResultsInMessageBox();
        
    } catch (const std::exception& e) {
        std::cout << "ERROR: Exception occurred during testing: " << e.what() << std::endl;
        success = false;
    } catch (...) {
        std::cout << "ERROR: Unknown exception occurred during testing" << std::endl;
        success = false;
    }
    
    std::cout << "\n=== Test Suite Completed ===" << std::endl;
    std::cout << "Result: " << (success ? "SUCCESS" : "FAILURE") << std::endl;
    
    if (saveResults) {
        std::cout << "Results saved to: " << resultsFile << std::endl;
    }
    
    std::cout << "\nPress Enter to exit..." << std::endl;
    std::cin.get();
    
    return success ? 0 : 1;
}

// Alternative entry point for integration with main application
extern "C" __declspec(dllexport) void RunCGraphImageListTests() {
    TestRunner testRunner;
    bool success = testRunner.RunAllTests();
    
    CString message;
    message.Format(_T("CGraphImageList Tests: %s"), success ? _T("ALL PASSED") : _T("SOME FAILED"));
    AfxMessageBox(message);
}

// Performance test entry point
extern "C" __declspec(dllexport) void RunCGraphImageListPerformanceTests() {
    TestRunner testRunner;
    testRunner.RunPerformanceTests();
}
