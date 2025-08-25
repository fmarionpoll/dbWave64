#include <gtest/gtest.h>
#include <afxwin.h>
#include <afxext.h>
#include <iostream>

// Initialize MFC for testing
class MFCApp : public CWinApp
{
public:
    virtual BOOL InitInstance()
    {
        CWinApp::InitInstance();
        return TRUE;
    }
};

MFCApp theApp;

int main(int argc, char** argv)
{
    // Initialize MFC
    if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
    {
        std::cerr << "MFC initialization failed!" << std::endl;
        return -1;
    }
    
    std::cout << "=== dbWave64 Test Suite ===" << std::endl;
    std::cout << "Initializing Google Test..." << std::endl;
    
    // Initialize Google Test
    ::testing::InitGoogleTest(&argc, argv);
    
    // Run all tests
    int result = RUN_ALL_TESTS();
    
    std::cout << "=== Test Suite Complete ===" << std::endl;
    return result;
}

// Export functions for potential integration with main application
extern "C" {
    __declspec(dllexport) int RunCGraphImageListTests() {
        // This function can be called from the main application
        // to run tests programmatically
        return 0; // Placeholder - would need to implement test runner
    }
    
    __declspec(dllexport) int RunCGraphImageListPerformanceTests() {
        // This function can be called from the main application
        // to run performance tests programmatically
        return 0; // Placeholder - would need to implement performance runner
    }
}
