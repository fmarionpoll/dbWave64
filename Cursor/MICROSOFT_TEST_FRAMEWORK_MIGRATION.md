# Microsoft Test Framework Migration Guide

## Overview

This document outlines the migration from Google Test to Microsoft Test Framework (MSTest) for the dbWave64 project. Microsoft Test Framework provides better integration with Visual Studio's Test Explorer and offers native C++ testing capabilities.

## Why Microsoft Test Framework?

### Advantages
1. **Native Visual Studio Integration** - Seamless integration with Test Explorer
2. **C++ Support** - Native C++ test framework with full language support
3. **Debugging Support** - Better debugging experience within Visual Studio
4. **Performance Profiling** - Built-in performance testing capabilities
5. **Code Coverage** - Integrated code coverage analysis
6. **Test Data** - Built-in data-driven testing support
7. **Parallel Execution** - Native parallel test execution

### Migration Benefits
- Better IDE integration
- Improved debugging experience
- Native C++ testing patterns
- Enhanced reporting capabilities
- Built-in performance testing

## Migration Strategy

### Phase 1: Framework Setup
1. Add Microsoft Test Framework NuGet package
2. Update project configuration
3. Create base test classes

### Phase 2: Test Migration
1. Convert Google Test fixtures to MSTest test classes
2. Migrate test methods and assertions
3. Update test data management

### Phase 3: Advanced Features
1. Implement data-driven tests
2. Add performance tests
3. Configure code coverage

## Project Configuration

### NuGet Package
```xml
<PackageReference Include="Microsoft.VisualStudio.TestTools.UnitTesting" Version="1.0.0" />
```

### Project Settings
- Enable C++ Unit Testing
- Configure test discovery
- Set up code coverage

## Test Class Structure

### Base Test Class
```cpp
#include <CppUnitTest.h>
#include <afxwin.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

TEST_CLASS(CGraphImageListTestBase)
{
protected:
    TEST_CLASS_INITIALIZE(ClassInitialize)
    {
        // Initialize MFC
        if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0)) {
            Assert::Fail(L"MFC initialization failed");
        }
    }
    
    TEST_CLASS_CLEANUP(ClassCleanup)
    {
        // Cleanup MFC
    }
    
    TEST_METHOD_INITIALIZE(TestInitialize)
    {
        // Setup before each test
    }
    
    TEST_METHOD_CLEANUP(TestCleanup)
    {
        // Cleanup after each test
    }
};
```

### Test Method Structure
```cpp
TEST_CLASS(CGraphImageListBasicTests)
{
    TEST_METHOD(GenerateEmptyImage_Basic)
    {
        // Arrange
        int width = 100;
        int height = 50;
        
        // Act
        CBitmap* pBitmap = CGraphImageList::GenerateEmptyImage(width, height);
        
        // Assert
        Assert::IsNotNull(pBitmap, L"Bitmap should not be null");
        
        BITMAP bm;
        pBitmap->GetBitmap(&bm);
        Assert::AreEqual(width, (int)bm.bmWidth, L"Width should match");
        Assert::AreEqual(height, (int)bm.bmHeight, L"Height should match");
        
        delete pBitmap;
    }
};
```

## Assertion Migration

### Google Test to MSTest Mapping

| Google Test | MSTest | Description |
|-------------|--------|-------------|
| `EXPECT_EQ(a, b)` | `Assert::AreEqual(a, b)` | Equality assertion |
| `EXPECT_NE(a, b)` | `Assert::AreNotEqual(a, b)` | Inequality assertion |
| `EXPECT_TRUE(expr)` | `Assert::IsTrue(expr)` | Boolean true assertion |
| `EXPECT_FALSE(expr)` | `Assert::IsFalse(expr)` | Boolean false assertion |
| `EXPECT_NULL(ptr)` | `Assert::IsNull(ptr)` | Null pointer assertion |
| `EXPECT_NOT_NULL(ptr)` | `Assert::IsNotNull(ptr)` | Non-null pointer assertion |
| `EXPECT_THROW(expr, type)` | `Assert::ExpectException<type>(expr)` | Exception assertion |
| `EXPECT_NO_THROW(expr)` | `Assert::ExpectNoException(expr)` | No exception assertion |

### Custom Assertions
```cpp
// Custom bitmap assertion
void AssertBitmapProperties(CBitmap* pBitmap, int expectedWidth, int expectedHeight)
{
    Assert::IsNotNull(pBitmap, L"Bitmap should not be null");
    
    BITMAP bm;
    pBitmap->GetBitmap(&bm);
    Assert::AreEqual(expectedWidth, (int)bm.bmWidth, L"Bitmap width mismatch");
    Assert::AreEqual(expectedHeight, (int)bm.bmHeight, L"Bitmap height mismatch");
}

// Usage
TEST_METHOD(GenerateEmptyImage_ValidDimensions)
{
    CBitmap* pBitmap = CGraphImageList::GenerateEmptyImage(100, 50);
    AssertBitmapProperties(pBitmap, 100, 50);
    delete pBitmap;
}
```

## Test Categories

### Basic Functionality Tests
```cpp
TEST_CLASS(CGraphImageListBasicTests)
{
    TEST_METHOD(GenerateEmptyImage_Basic)
    {
        // Test basic empty image generation
    }
    
    TEST_METHOD(GenerateEmptyImage_ZeroDimensions)
    {
        // Test edge cases with zero dimensions
    }
    
    TEST_METHOD(GenerateEmptyImage_NegativeDimensions)
    {
        // Test edge cases with negative dimensions
    }
};
```

### Data Image Tests
```cpp
TEST_CLASS(CGraphImageListDataTests)
{
    TEST_METHOD(GenerateDataImage_ValidFile)
    {
        // Test data image generation with valid file
    }
    
    TEST_METHOD(GenerateDataImage_InvalidFile)
    {
        // Test data image generation with invalid file
    }
    
    TEST_METHOD(GenerateDataImage_EmptyFile)
    {
        // Test data image generation with empty file
    }
};
```

### Spike Image Tests (Critical for Crash Debugging)
```cpp
TEST_CLASS(CGraphImageListSpikeTests)
{
    TEST_METHOD(GenerateSpikeImage_ValidFile)
    {
        // Test spike image generation with valid file
    }
    
    TEST_METHOD(GenerateSpikeImage_CrashInvestigation)
    {
        // Test various spike file formats to isolate crash
    }
    
    TEST_METHOD(GenerateSpikeImage_CorruptedFile)
    {
        // Test with corrupted spike files
    }
    
    TEST_METHOD(GenerateSpikeImage_LargeFile)
    {
        // Test with large spike files
    }
};
```

### Memory Management Tests
```cpp
TEST_CLASS(CGraphImageListMemoryTests)
{
    TEST_METHOD(MemoryLeakDetection)
    {
        // Test for memory leaks
    }
    
    TEST_METHOD(StaticDataThreadSafety)
    {
        // Test concurrent access to static data
    }
};
```

## Data-Driven Tests

### Test Data Attributes
```cpp
TEST_CLASS(CGraphImageListDataDrivenTests)
{
    TEST_METHOD_DATA(GenerateEmptyImage_VariousDimensions, 
        L"Data/EmptyImageDimensions.csv")
    void GenerateEmptyImage_VariousDimensions(int width, int height, bool shouldSucceed)
    {
        if (shouldSucceed) {
            CBitmap* pBitmap = CGraphImageList::GenerateEmptyImage(width, height);
            Assert::IsNotNull(pBitmap, L"Bitmap should be created");
            delete pBitmap;
        } else {
            CBitmap* pBitmap = CGraphImageList::GenerateEmptyImage(width, height);
            Assert::IsNull(pBitmap, L"Bitmap should not be created for invalid dimensions");
        }
    }
};
```

### Test Data File Format
```csv
width,height,shouldSucceed
100,50,true
0,0,false
-1,-1,false
1000,1000,true
```

## Performance Tests

### Basic Performance Test
```cpp
TEST_CLASS(CGraphImageListPerformanceTests)
{
    TEST_METHOD(GenerateEmptyImage_Performance)
    {
        auto start = std::chrono::high_resolution_clock::now();
        
        for (int i = 0; i < 1000; ++i) {
            CBitmap* pBitmap = CGraphImageList::GenerateEmptyImage(100, 100);
            delete pBitmap;
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        Assert::IsTrue(duration.count() < 5000, L"Performance test should complete within 5 seconds");
    }
};
```

### Performance Test with Attributes
```cpp
TEST_CLASS(CGraphImageListPerformanceTests)
{
    BEGIN_TEST_METHOD_ATTRIBUTE(GenerateEmptyImage_Performance)
        TEST_OWNER(L"Performance Team")
        TEST_PRIORITY(1)
        TEST_WORKITEM(12345)
    END_TEST_METHOD_ATTRIBUTE()
    
    TEST_METHOD(GenerateEmptyImage_Performance)
    {
        // Performance test implementation
    }
};
```

## Test Utilities

### Test Data Fixture
```cpp
class TestDataFixture
{
public:
    static CString CreateTestDataFile();
    static CString CreateTestSpikeFile();
    static CString CreateCorruptedSpikeFile();
    static void CleanupTestFiles();
    
private:
    static std::vector<CString> createdFiles;
};

// Implementation
CString TestDataFixture::CreateTestDataFile()
{
    CString filename = _T("test_data_") + std::to_wstring(GetTickCount()).c_str() + _T(".dat");
    // Create test file with valid data format
    createdFiles.push_back(filename);
    return filename;
}

void TestDataFixture::CleanupTestFiles()
{
    for (const auto& file : createdFiles) {
        DeleteFile(file);
    }
    createdFiles.clear();
}
```

### Test Helper Methods
```cpp
class TestHelpers
{
public:
    static bool VerifyBitmapProperties(CBitmap* pBitmap, int expectedWidth, int expectedHeight);
    static void SaveBitmapToFile(CBitmap* pBitmap, const CString& filename);
    static CBitmap* LoadBitmapFromFile(const CString& filename);
    
private:
    static CDC* CreateCompatibleDC();
};
```

## Integration with Visual Studio

### Test Explorer Configuration
```xml
<!-- In .vcxproj file -->
<ItemGroup>
  <PackageReference Include="Microsoft.VisualStudio.TestTools.UnitTesting" Version="1.0.0" />
</ItemGroup>

<PropertyGroup>
  <EnableCppUnitTest>true</EnableCppUnitTest>
  <CppUnitTestRoot>$(ProjectDir)</CppUnitTestRoot>
</PropertyGroup>
```

### Test Discovery
- Tests are automatically discovered by Test Explorer
- Test methods must be decorated with `TEST_METHOD`
- Test classes must be decorated with `TEST_CLASS`

### Debugging Support
```cpp
TEST_METHOD(GenerateSpikeImage_DebugMode)
{
    // Enable debug output
    Logger::WriteMessage(L"Starting spike image generation test");
    
    // Test implementation
    CBitmap* pBitmap = CGraphImageList::GenerateSpikeImage(200, 100, _T("test.spk"), infos);
    
    // Debug output
    Logger::WriteMessage(L"Spike image generation completed");
    
    Assert::IsNotNull(pBitmap, L"Spike image should be generated");
    delete pBitmap;
}
```

## Migration Checklist

### Phase 1: Framework Setup
- [ ] Add Microsoft Test Framework NuGet package
- [ ] Update project configuration
- [ ] Create base test classes
- [ ] Verify Test Explorer integration

### Phase 2: Test Migration
- [ ] Convert Google Test fixtures to MSTest test classes
- [ ] Migrate test methods and assertions
- [ ] Update test data management
- [ ] Verify all tests pass

### Phase 3: Advanced Features
- [ ] Implement data-driven tests
- [ ] Add performance tests
- [ ] Configure code coverage
- [ ] Set up continuous integration

### Phase 4: Validation
- [ ] Run all existing tests
- [ ] Verify spike crash debugging capabilities
- [ ] Test performance benchmarks
- [ ] Validate Test Explorer integration

## Troubleshooting

### Common Issues
1. **Test Discovery Problems**
   - Ensure `TEST_METHOD` and `TEST_CLASS` attributes are used
   - Check project configuration for C++ Unit Testing
   - Verify NuGet package installation

2. **MFC Initialization Issues**
   - Ensure MFC is properly initialized in test setup
   - Check for proper cleanup in test teardown

3. **Memory Leak Detection**
   - Use Visual Studio's built-in memory leak detection
   - Configure debug heap settings

4. **Performance Test Failures**
   - Adjust performance thresholds based on hardware
   - Consider system load during testing

## Best Practices

### Test Organization
1. **Logical Grouping** - Group related tests in test classes
2. **Clear Naming** - Use descriptive test method names
3. **Test Data Management** - Centralize test data creation and cleanup
4. **Assertion Clarity** - Use clear, descriptive assertion messages

### Performance Considerations
1. **Test Isolation** - Ensure tests don't interfere with each other
2. **Resource Cleanup** - Properly clean up resources after each test
3. **Test Data Size** - Use appropriate test data sizes for performance tests

### Maintenance
1. **Regular Updates** - Keep test framework updated
2. **Documentation** - Maintain test documentation
3. **Code Coverage** - Monitor and improve test coverage

## Conclusion

Migrating to Microsoft Test Framework provides better integration with Visual Studio and enhanced testing capabilities. The migration should be done incrementally, starting with framework setup and progressing through test migration and advanced features.

The new framework will provide better debugging support for the spike crash investigation and improved overall testing experience within the Visual Studio environment.

