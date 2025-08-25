#pragma once

// Simple test framework for Visual Studio 2022
// Since Microsoft Test Framework headers are not available, we'll create a simple framework

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

// Simple test framework macros
#define TEST_CLASS(name) class name
#define TEST_METHOD(name) void name()
#define TEST_CLASS_INITIALIZE(name) static void name()
#define TEST_CLASS_CLEANUP(name) static void name()
#define TEST_METHOD_INITIALIZE(name) void name()
#define TEST_METHOD_CLEANUP(name) void name()

// Simple assertion macros
#define Assert::IsNotNull(ptr, message) if (!(ptr)) { std::wcout << L"Assertion failed: " << message << std::endl; return; }
#define Assert::AreEqual(expected, actual, message) if ((expected) != (actual)) { std::wcout << L"Assertion failed: " << message << std::endl; return; }
#define Assert::IsTrue(condition, message) if (!(condition)) { std::wcout << L"Assertion failed: " << message << std::endl; return; }
#define Assert::Fail(message) { std::wcout << L"Test failed: " << message << std::endl; return; }
#define Assert::ExpectNoException(expression, message) try { expression; } catch (...) { std::wcout << L"Exception caught: " << message << std::endl; return; }

// Simple logging
#define Logger::WriteMessage(message) std::wcout << message << std::endl

namespace Microsoft::VisualStudio::CppUnitTestFramework {
    class Assert {
    public:
        static void IsNotNull(void* ptr, const wchar_t* message) {
            if (!ptr) {
                std::wcout << L"Assertion failed: " << message << std::endl;
                throw std::runtime_error("Assertion failed");
            }
        }
        
        static void AreEqual(int expected, int actual, const wchar_t* message) {
            if (expected != actual) {
                std::wcout << L"Assertion failed: " << message << std::endl;
                throw std::runtime_error("Assertion failed");
            }
        }
        
        static void IsTrue(bool condition, const wchar_t* message) {
            if (!condition) {
                std::wcout << L"Assertion failed: " << message << std::endl;
                throw std::runtime_error("Assertion failed");
            }
        }
        
        static void Fail(const wchar_t* message) {
            std::wcout << L"Test failed: " << message << std::endl;
            throw std::runtime_error("Test failed");
        }
        
        template<typename Func>
        static void ExpectNoException(Func func, const wchar_t* message) {
            try {
                func();
            } catch (...) {
                std::wcout << L"Exception caught: " << message << std::endl;
                throw std::runtime_error("Exception caught");
            }
        }
    };
    
    class Logger {
    public:
        static void WriteMessage(const wchar_t* message) {
            std::wcout << message << std::endl;
        }
    };
}

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

// Forward declarations
class CBitmap;
class CDC;

// Microsoft Test Framework base class for CGraphImageList tests
TEST_CLASS(CGraphImageListTestBase)
{
protected:
    // Class-level initialization (runs once before all tests)
    TEST_CLASS_INITIALIZE(ClassInitialize)
    {
        Logger::WriteMessage(L"=== Initializing CGraphImageList Test Suite ===");
        
        // Initialize MFC
        if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0)) {
            Assert::Fail(L"MFC initialization failed");
        }
        
        Logger::WriteMessage(L"MFC initialized successfully");
    }
    
    // Class-level cleanup (runs once after all tests)
    TEST_CLASS_CLEANUP(ClassCleanup)
    {
        Logger::WriteMessage(L"=== Cleaning up CGraphImageList Test Suite ===");
        
        // Cleanup any remaining test files
        TestDataFixture::CleanupTestFiles();
        
        Logger::WriteMessage(L"Test suite cleanup completed");
    }
    
    // Test-level initialization (runs before each test)
    TEST_METHOD_INITIALIZE(TestInitialize)
    {
        Logger::WriteMessage(L"Setting up individual test");
        
        // Reset test data
        testMessages.clear();
        
        // Create fresh test environment
        TestDataFixture::CleanupTestFiles();
    }
    
    // Test-level cleanup (runs after each test)
    TEST_METHOD_CLEANUP(TestCleanup)
    {
        Logger::WriteMessage(L"Cleaning up individual test");
        
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
    
    // Test result tracking (for custom assertions)
    std::vector<std::string> testMessages;
    void AddTestMessage(const std::string& message);
};

// Custom assertion macro for bitmap verification
#define ASSERT_BITMAP_PROPERTIES(bitmap, width, height) \
    Assert::IsTrue(VerifyBitmapProperties((bitmap), (width), (height)), \
    L"Bitmap properties verification failed for " + std::to_wstring(width) + L"x" + std::to_wstring(height))

// Custom assertion macro for no-crash testing
#define ASSERT_NO_CRASH(expression) \
    Assert::ExpectNoException([&]() { expression; }, L"Expression should not throw or crash")

// Test data fixture class for Microsoft Test Framework
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
