#include "TestDataFixture.h"
#include <fstream>
#include <sstream>
#include <random>
#include <chrono>

std::vector<CString> TestDataFixture::createdFiles;

TestDataFixture::TestDataFixture() {
    // Initialize test data fixture
}

TestDataFixture::~TestDataFixture() {
    CleanupAllTestFiles();
}

CString TestDataFixture::CreateValidDataFile(const CString& filename) {
    CString actualFilename = filename.IsEmpty() ? 
        GenerateUniqueFilename(_T("valid_data"), _T(".dat")) : filename;
    
    std::string content = GenerateValidDataContent();
    WriteTestDataToFile(actualFilename, content);
    createdFiles.push_back(actualFilename);
    
    return actualFilename;
}

CString TestDataFixture::CreateValidSpikeFile(const CString& filename) {
    CString actualFilename = filename.IsEmpty() ? 
        GenerateUniqueFilename(_T("valid_spike"), _T(".spk")) : filename;
    
    std::string content = GenerateValidSpikeContent();
    WriteTestDataToFile(actualFilename, content);
    createdFiles.push_back(actualFilename);
    
    return actualFilename;
}

CString TestDataFixture::CreateCorruptedDataFile(const CString& filename) {
    CString actualFilename = filename.IsEmpty() ? 
        GenerateUniqueFilename(_T("corrupted_data"), _T(".dat")) : filename;
    
    std::string content = GenerateCorruptedContent();
    WriteTestDataToFile(actualFilename, content);
    createdFiles.push_back(actualFilename);
    
    return actualFilename;
}

CString TestDataFixture::CreateCorruptedSpikeFile(const CString& filename) {
    CString actualFilename = filename.IsEmpty() ? 
        GenerateUniqueFilename(_T("corrupted_spike"), _T(".spk")) : filename;
    
    std::string content = GenerateCorruptedContent();
    WriteTestDataToFile(actualFilename, content);
    createdFiles.push_back(actualFilename);
    
    return actualFilename;
}

CString TestDataFixture::CreateLargeDataFile(const CString& filename, int sizeKB) {
    CString actualFilename = filename.IsEmpty() ? 
        GenerateUniqueFilename(_T("large_data"), _T(".dat")) : filename;
    
    std::string content = GenerateLargeContent(sizeKB);
    WriteTestDataToFile(actualFilename, content);
    createdFiles.push_back(actualFilename);
    
    return actualFilename;
}

CString TestDataFixture::CreateLargeSpikeFile(const CString& filename, int sizeKB) {
    CString actualFilename = filename.IsEmpty() ? 
        GenerateUniqueFilename(_T("large_spike"), _T(".spk")) : filename;
    
    std::string content = GenerateLargeContent(sizeKB);
    WriteTestDataToFile(actualFilename, content);
    createdFiles.push_back(actualFilename);
    
    return actualFilename;
}

std::vector<TestDataFixture::TestDataInfo> TestDataFixture::GetStandardTestFiles() {
    std::vector<TestDataInfo> files;
    
    // Valid files
    TestDataInfo validData;
    validData.filename = CreateValidDataFile();
    validData.isValid = true;
    validData.fileSize = GetFileSize(validData.filename);
    validData.description = _T("Valid data file");
    files.push_back(validData);
    
    TestDataInfo validSpike;
    validSpike.filename = CreateValidSpikeFile();
    validSpike.isValid = true;
    validSpike.fileSize = GetFileSize(validSpike.filename);
    validSpike.description = _T("Valid spike file");
    files.push_back(validSpike);
    
    // Invalid files
    TestDataInfo invalidData;
    invalidData.filename = _T("nonexistent_data.dat");
    invalidData.isValid = false;
    invalidData.fileSize = 0;
    invalidData.description = _T("Non-existent data file");
    files.push_back(invalidData);
    
    TestDataInfo invalidSpike;
    invalidSpike.filename = _T("nonexistent_spike.spk");
    invalidSpike.isValid = false;
    invalidSpike.fileSize = 0;
    invalidSpike.description = _T("Non-existent spike file");
    files.push_back(invalidSpike);
    
    return files;
}

std::vector<TestDataFixture::TestDataInfo> TestDataFixture::GetEdgeCaseTestFiles() {
    std::vector<TestDataInfo> files;
    
    // Empty files
    TestDataInfo emptyData;
    emptyData.filename = CreateValidDataFile(_T("empty_data.dat"));
    emptyData.isValid = true;
    emptyData.fileSize = 0;
    emptyData.description = _T("Empty data file");
    files.push_back(emptyData);
    
    TestDataInfo emptySpike;
    emptySpike.filename = CreateValidSpikeFile(_T("empty_spike.spk"));
    emptySpike.isValid = true;
    emptySpike.fileSize = 0;
    emptySpike.description = _T("Empty spike file");
    files.push_back(emptySpike);
    
    // Large files
    TestDataInfo largeData;
    largeData.filename = CreateLargeDataFile(_T("large_data.dat"), 1024);
    largeData.isValid = true;
    largeData.fileSize = GetFileSize(largeData.filename);
    largeData.description = _T("Large data file (1MB)");
    files.push_back(largeData);
    
    TestDataInfo largeSpike;
    largeSpike.filename = CreateLargeSpikeFile(_T("large_spike.spk"), 1024);
    largeSpike.isValid = true;
    largeSpike.fileSize = GetFileSize(largeSpike.filename);
    largeSpike.description = _T("Large spike file (1MB)");
    files.push_back(largeSpike);
    
    return files;
}

std::vector<TestDataFixture::TestDataInfo> TestDataFixture::GetCorruptionTestFiles() {
    std::vector<TestDataInfo> files;
    
    // Corrupted files
    TestDataInfo corruptedData;
    corruptedData.filename = CreateCorruptedDataFile();
    corruptedData.isValid = false;
    corruptedData.fileSize = GetFileSize(corruptedData.filename);
    corruptedData.description = _T("Corrupted data file");
    files.push_back(corruptedData);
    
    TestDataInfo corruptedSpike;
    corruptedSpike.filename = CreateCorruptedSpikeFile();
    corruptedSpike.isValid = false;
    corruptedSpike.fileSize = GetFileSize(corruptedSpike.filename);
    corruptedSpike.description = _T("Corrupted spike file");
    files.push_back(corruptedSpike);
    
    // Files with invalid format
    TestDataInfo invalidFormatData;
    invalidFormatData.filename = CreateValidDataFile(_T("invalid_format_data.txt"));
    invalidFormatData.isValid = false;
    invalidFormatData.fileSize = GetFileSize(invalidFormatData.filename);
    invalidFormatData.description = _T("Data file with wrong extension");
    files.push_back(invalidFormatData);
    
    TestDataInfo invalidFormatSpike;
    invalidFormatSpike.filename = CreateValidSpikeFile(_T("invalid_format_spike.txt"));
    invalidFormatSpike.isValid = false;
    invalidFormatSpike.fileSize = GetFileSize(invalidFormatSpike.filename);
    invalidFormatSpike.description = _T("Spike file with wrong extension");
    files.push_back(invalidFormatSpike);
    
    return files;
}

void TestDataFixture::CleanupAllTestFiles() {
    for (const auto& filename : createdFiles) {
        DeleteFile(filename);
    }
    createdFiles.clear();
}

bool TestDataFixture::FileExists(const CString& filename) {
    DWORD fileAttributes = GetFileAttributes(filename);
    return (fileAttributes != INVALID_FILE_ATTRIBUTES && 
            !(fileAttributes & FILE_ATTRIBUTE_DIRECTORY));
}

int TestDataFixture::GetFileSize(const CString& filename) {
    if (!FileExists(filename)) {
        return 0;
    }
    
    HANDLE hFile = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, 
                             nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hFile == INVALID_HANDLE_VALUE) {
        return 0;
    }
    
    DWORD fileSize = ::GetFileSize(hFile, NULL);
    CloseHandle(hFile);
    
    return static_cast<int>(fileSize);
}

CString TestDataFixture::GetTempDirectory() {
    TCHAR tempPath[MAX_PATH];
    DWORD result = GetTempPath(MAX_PATH, tempPath);
    if (result > 0 && result < MAX_PATH) {
        return CString(tempPath);
    }
    return _T(".");
}

bool TestDataFixture::ValidateDataFile(const CString& filename) {
    if (!FileExists(filename)) {
        return false;
    }
    
    // Basic validation - check if file has some content
    int fileSize = GetFileSize(filename);
    return fileSize > 0;
}

bool TestDataFixture::ValidateSpikeFile(const CString& filename) {
    if (!FileExists(filename)) {
        return false;
    }
    
    // Basic validation - check if file has some content
    int fileSize = GetFileSize(filename);
    return fileSize > 0;
}

CString TestDataFixture::GenerateUniqueFilename(const CString& baseName, const CString& extension) {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    CString uniqueFilename;
    uniqueFilename.Format(_T("%s_%lld_%d%s"), 
                         baseName, 
                         static_cast<long long>(time_t), 
                         static_cast<int>(milliseconds.count()),
                         extension);
    
    return uniqueFilename;
}

void TestDataFixture::WriteTestDataToFile(const CString& filename, const std::string& content) {
    std::ofstream file(filename);
    if (file.is_open()) {
        file << content;
        file.close();
    }
}

std::string TestDataFixture::GenerateValidDataContent() {
    std::ostringstream content;
    content << "# Data file for testing CGraphImageList\n";
    content << "# Format: timestamp, value\n";
    content << "0.0, 1.234\n";
    content << "0.001, 2.345\n";
    content << "0.002, 3.456\n";
    content << "0.003, 4.567\n";
    content << "0.004, 5.678\n";
    return content.str();
}

std::string TestDataFixture::GenerateValidSpikeContent() {
    std::ostringstream content;
    content << "# Spike file for testing CGraphImageList\n";
    content << "# Format: timestamp, channel, amplitude\n";
    content << "0.0, 1, 0.123\n";
    content << "0.001, 1, 0.234\n";
    content << "0.002, 2, 0.345\n";
    content << "0.003, 1, 0.456\n";
    content << "0.004, 2, 0.567\n";
    return content.str();
}

std::string TestDataFixture::GenerateCorruptedContent() {
    std::ostringstream content;
    content << "# Corrupted file for testing error handling\n";
    content << "Invalid data format\n";
    content << "Missing required fields\n";
    content << "Corrupted binary data: \x00\x01\x02\x03\n";
    content << "Unexpected characters: !@#$%^&*()\n";
    return content.str();
}

std::string TestDataFixture::GenerateLargeContent(int sizeKB) {
    std::ostringstream content;
    content << "# Large test file (" << sizeKB << "KB)\n";
    
    // Generate random data to fill the file
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 100.0);
    
    int targetSize = sizeKB * 1024;
    int currentSize = content.str().length();
    
    while (currentSize < targetSize) {
        content << dis(gen) << ", " << dis(gen) << ", " << dis(gen) << "\n";
        currentSize = content.str().length();
    }
    
    return content.str();
}
