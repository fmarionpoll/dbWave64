# CGraphImageList Testing Strategy

## Why Testing is Critical

Given the crash you're experiencing with spike display and the recent refactoring, comprehensive testing is essential to:

1. **Identify the spike crash root cause** - isolate whether it's in rendering, document loading, or memory management
2. **Validate refactoring correctness** - ensure the wrapper function approach works correctly
3. **Prevent regressions** - catch issues before they reach production
4. **Document expected behavior** - create living documentation of how the class should work

## Testing Approach

### 1. Unit Tests (Recommended Priority)

#### Test Framework Options
- **Google Test** - Most popular, excellent C++ support
- **Catch2** - Header-only, easy to integrate
- **Boost.Test** - If already using Boost
- **Custom test framework** - Simple assert-based tests

#### Core Test Categories

**A. Basic Functionality Tests**
```cpp
TEST(CGraphImageListTest, GenerateEmptyImage_Basic)
{
    // Test basic empty image generation
    CBitmap* pBitmap = CGraphImageList::GenerateEmptyImage(100, 50);
    ASSERT_NE(pBitmap, nullptr);
    
    // Verify bitmap properties
    BITMAP bm;
    pBitmap->GetBitmap(&bm);
    EXPECT_EQ(bm.bmWidth, 100);
    EXPECT_EQ(bm.bmHeight, 50);
    
    delete pBitmap;
}

TEST(CGraphImageListTest, GenerateEmptyImage_ZeroDimensions)
{
    // Test edge cases
    CBitmap* pBitmap = CGraphImageList::GenerateEmptyImage(0, 0);
    // Should handle gracefully or return nullptr
    delete pBitmap;
}
```

**B. Data Image Tests**
```cpp
TEST(CGraphImageListTest, GenerateDataImage_ValidFile)
{
    // Create test data file
    CString testFile = CreateTestDataFile();
    DataListCtrlInfos infos;
    infos.image_width = 200;
    infos.image_height = 100;
    
    CBitmap* pBitmap = CGraphImageList::GenerateDataImage(200, 100, testFile, infos);
    ASSERT_NE(pBitmap, nullptr);
    
    // Verify bitmap was created
    BITMAP bm;
    pBitmap->GetBitmap(&bm);
    EXPECT_EQ(bm.bmWidth, 200);
    EXPECT_EQ(bm.bmHeight, 100);
    
    delete pBitmap;
    DeleteTestFile(testFile);
}

TEST(CGraphImageListTest, GenerateDataImage_InvalidFile)
{
    CString invalidFile = _T("nonexistent_file.dat");
    DataListCtrlInfos infos;
    infos.image_width = 200;
    infos.image_height = 100;
    
    CBitmap* pBitmap = CGraphImageList::GenerateDataImage(200, 100, invalidFile, infos);
    ASSERT_NE(pBitmap, nullptr);
    
    // Should generate error image, not crash
    delete pBitmap;
}
```

**C. Spike Image Tests (Critical for Your Crash)**
```cpp
TEST(CGraphImageListTest, GenerateSpikeImage_ValidFile)
{
    // Create test spike file
    CString testFile = CreateTestSpikeFile();
    DataListCtrlInfos infos;
    infos.image_width = 200;
    infos.image_height = 100;
    
    CBitmap* pBitmap = CGraphImageList::GenerateSpikeImage(200, 100, testFile, infos);
    ASSERT_NE(pBitmap, nullptr);
    
    // Verify bitmap was created
    BITMAP bm;
    pBitmap->GetBitmap(&bm);
    EXPECT_EQ(bm.bmWidth, 200);
    EXPECT_EQ(bm.bmHeight, 100);
    
    delete pBitmap;
    DeleteTestFile(testFile);
}

TEST(CGraphImageListTest, GenerateSpikeImage_CrashInvestigation)
{
    // Test with various spike file formats
    std::vector<CString> testFiles = {
        CreateEmptySpikeFile(),
        CreateCorruptedSpikeFile(),
        CreateLargeSpikeFile(),
        CreateSpikeFileWithInvalidData()
    };
    
    DataListCtrlInfos infos;
    infos.image_width = 200;
    infos.image_height = 100;
    
    for (const auto& testFile : testFiles)
    {
        CBitmap* pBitmap = nullptr;
        
        // Should not crash, even with invalid data
        EXPECT_NO_THROW({
            pBitmap = CGraphImageList::GenerateSpikeImage(200, 100, testFile, infos);
        });
        
        if (pBitmap)
        {
            delete pBitmap;
        }
        
        DeleteTestFile(testFile);
    }
}
```

**D. Memory Management Tests**
```cpp
TEST(CGraphImageListTest, MemoryLeakDetection)
{
    // Generate multiple images and verify no memory leaks
    for (int i = 0; i < 100; ++i)
    {
        CBitmap* pBitmap = CGraphImageList::GenerateEmptyImage(50, 50);
        ASSERT_NE(pBitmap, nullptr);
        delete pBitmap;
    }
    
    // Use memory leak detection tools to verify
}

TEST(CGraphImageListTest, StaticDataThreadSafety)
{
    // Test concurrent access to static data
    std::vector<std::thread> threads;
    std::atomic<int> successCount(0);
    
    for (int i = 0; i < 10; ++i)
    {
        threads.emplace_back([&successCount]() {
            CString testFile = CreateTestDataFile();
            DataListCtrlInfos infos;
            infos.image_width = 100;
            infos.image_height = 50;
            
            CBitmap* pBitmap = CGraphImageList::GenerateDataImage(100, 50, testFile, infos);
            if (pBitmap)
            {
                delete pBitmap;
                successCount++;
            }
            
            DeleteTestFile(testFile);
        });
    }
    
    for (auto& thread : threads)
    {
        thread.join();
    }
    
    EXPECT_EQ(successCount.load(), 10);
}
```

### 2. Integration Tests

**A. DataListCtrl Integration**
```cpp
TEST(CGraphImageListIntegrationTest, DataListCtrlRowIntegration)
{
    // Test the full integration with DataListCtrl_Row
    DataListCtrl_Row row;
    DataListCtrlInfos infos;
    infos.image_width = 200;
    infos.image_height = 100;
    
    // Test all display modes
    row.set_display_parameters(&infos, 0);
    
    // Verify image list was updated correctly
}
```

**B. Document Loading Integration**
```cpp
TEST(CGraphImageListIntegrationTest, DocumentLoading)
{
    // Test integration with AcqDataDoc and CSpikeDoc
    // Verify document loading works correctly
    // Test error handling when documents fail to load
}
```

### 3. Performance Tests

```cpp
TEST(CGraphImageListPerformanceTest, GenerationSpeed)
{
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 1000; ++i)
    {
        CBitmap* pBitmap = CGraphImageList::GenerateEmptyImage(100, 100);
        delete pBitmap;
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    // Should complete within reasonable time
    EXPECT_LT(duration.count(), 5000); // 5 seconds
}
```

### 4. Error Handling Tests

```cpp
TEST(CGraphImageListErrorTest, InvalidParameters)
{
    // Test with invalid dimensions
    CBitmap* pBitmap = CGraphImageList::GenerateEmptyImage(-1, -1);
    // Should handle gracefully
    
    // Test with null pointers
    // Test with empty strings
    // Test with corrupted data
}
```

## Test Infrastructure Setup

### 1. Test Data Creation
```cpp
class CGraphImageListTestFixture
{
protected:
    CString CreateTestDataFile()
    {
        // Create a minimal valid data file for testing
        CString filename = _T("test_data.dat");
        // ... create file with valid data format
        return filename;
    }
    
    CString CreateTestSpikeFile()
    {
        // Create a minimal valid spike file for testing
        CString filename = _T("test_spike.spk");
        // ... create file with valid spike format
        return filename;
    }
    
    void DeleteTestFile(const CString& filename)
    {
        // Clean up test files
        DeleteFile(filename);
    }
    
    void SetUp() override
    {
        // Initialize test environment
    }
    
    void TearDown() override
    {
        // Clean up test environment
    }
};
```

### 2. Mock Objects
```cpp
class MockAcqDataDoc : public AcqDataDoc
{
public:
    MOCK_METHOD(bool, open_document, (const CString&), (override));
    MOCK_METHOD(void, read_data_infos, (), (override));
    // ... other methods
};

class MockCSpikeDoc : public CSpikeDoc
{
public:
    MOCK_METHOD(bool, OnOpenDocument, (const CString&), (override));
    // ... other methods
};
```

## Debugging the Spike Crash

### 1. Isolate the Problem
```cpp
TEST(CGraphImageListDebugTest, SpikeCrashIsolation)
{
    // Test each component separately
    TEST(CGraphImageListDebugTest, SpikeDocumentLoading)
    {
        // Test just document loading
    }
    
    TEST(CGraphImageListDebugTest, SpikeRendering)
    {
        // Test just rendering with valid document
    }
    
    TEST(CGraphImageListDebugTest, SpikeMemoryManagement)
    {
        // Test memory allocation/deallocation
    }
}
```

### 2. Add Debug Logging
```cpp
// Add TRACE statements to identify crash location
void CGraphImageList::render_spike_to_dc(CDC* pDC, CString& spikeFileName, const DataListCtrlInfos& infos)
{
    TRACE(_T("CGraphImageList::render_spike_to_dc - Start\n"));
    
    if (spikeFileName.IsEmpty())
    {
        TRACE(_T("CGraphImageList::render_spike_to_dc - Empty filename\n"));
        CString message = _T("No spike file");
        render_error_message(pDC, infos, message);
        return;
    }
    
    TRACE(_T("CGraphImageList::render_spike_to_dc - Creating CSpikeDoc\n"));
    CSpikeDoc* pSpikeDoc = new CSpikeDoc;
    
    TRACE(_T("CGraphImageList::render_spike_to_dc - Loading document\n"));
    if (!pSpikeDoc->OnOpenDocument(spikeFileName))
    {
        TRACE(_T("CGraphImageList::render_spike_to_dc - Document load failed\n"));
        // ... error handling
    }
    
    // ... continue with more TRACE statements
}
```

## Implementation Priority

### Phase 1: Critical Tests (Implement First)
1. **Spike crash isolation tests** - Identify the root cause
2. **Basic functionality tests** - Ensure refactoring didn't break anything
3. **Memory leak tests** - Verify proper cleanup

### Phase 2: Comprehensive Tests
1. **Error handling tests** - Test edge cases and invalid inputs
2. **Performance tests** - Ensure acceptable performance
3. **Integration tests** - Test with real components

### Phase 3: Advanced Tests
1. **Thread safety tests** - If multi-threading is needed
2. **Stress tests** - Test with large files and high load
3. **Regression tests** - Prevent future issues

## Tools and Setup

### Recommended Testing Tools
- **Google Test** - Primary test framework
- **Valgrind** - Memory leak detection
- **AddressSanitizer** - Memory error detection
- **Visual Studio Test Explorer** - Integrated testing

### Continuous Integration
- Set up automated testing in your build pipeline
- Run tests on every commit
- Generate test coverage reports

## Conclusion

Implementing comprehensive tests for `CGraphImageList` will:
1. **Help identify and fix the spike crash** quickly
2. **Validate the refactoring** was successful
3. **Prevent future regressions** as you continue development
4. **Provide confidence** in the code quality

Start with the spike crash isolation tests to identify the root cause, then expand to comprehensive testing coverage.
