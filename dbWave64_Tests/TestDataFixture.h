#pragma once

#include <afxwin.h>
#include <string>
#include <vector>

// Test data fixture class for CGraphImageList tests
class TestDataFixture {
public:
    TestDataFixture();
    ~TestDataFixture();
    
    // Test file creation methods
    static CString CreateValidDataFile(const CString& filename = _T(""));
    static CString CreateValidSpikeFile(const CString& filename = _T(""));
    static CString CreateCorruptedDataFile(const CString& filename = _T(""));
    static CString CreateCorruptedSpikeFile(const CString& filename = _T(""));
    static CString CreateLargeDataFile(const CString& filename = _T(""), int sizeKB = 1024);
    static CString CreateLargeSpikeFile(const CString& filename = _T(""), int sizeKB = 1024);
    
    // Test data structures
    struct TestDataInfo {
        CString filename;
        bool isValid;
        int fileSize;
        CString description;
    };
    
    // Test scenarios
    static std::vector<TestDataInfo> GetStandardTestFiles();
    static std::vector<TestDataInfo> GetEdgeCaseTestFiles();
    static std::vector<TestDataInfo> GetCorruptionTestFiles();
    
    // Utility methods
    static void CleanupAllTestFiles();
    static bool FileExists(const CString& filename);
    static int GetFileSize(const CString& filename);
    static CString GetTempDirectory();
    
    // Test data validation
    static bool ValidateDataFile(const CString& filename);
    static bool ValidateSpikeFile(const CString& filename);
    
private:
    static std::vector<CString> createdFiles;
    
    // Helper methods
    static CString GenerateUniqueFilename(const CString& baseName, const CString& extension);
    static void WriteTestDataToFile(const CString& filename, const std::string& content);
    static std::string GenerateValidDataContent();
    static std::string GenerateValidSpikeContent();
    static std::string GenerateCorruptedContent();
    static std::string GenerateLargeContent(int sizeKB);
};
