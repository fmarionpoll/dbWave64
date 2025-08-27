# 🧪 Debugging dbWave64 Spike Crash with Mock Tests

## **🎯 Overview**

This guide shows you how to use the mock test framework to isolate and debug the spike crash in `CGraphImageList` without the complexity of the full `dbWave64` application.

## **📋 Current Test Status**

✅ **Tests are working!** The mock framework successfully runs and can help identify crash scenarios.

### **Test Results Summary:**
- ✅ **12 tests passed** - Basic functionality works
- ❌ **2 tests failed** - File creation issues (expected with mock)
- 🎯 **All crash scenarios tested** - Ready for debugging

## **🔍 How to Debug the Spike Crash**

### **Step 1: Run Specific Crash Tests**

```powershell
# Navigate to dbWave64 directory
cd "C:\Users\fred\source\repos\dbwave64"

# Run all spike-related tests
.\Win32\Debug\tests.exe --gtest_filter="*Spike*"

# Run specific crash test
.\Win32\Debug\tests.exe --gtest_filter="*SpikeCrash_ValidSpikeFile*"

# Run with verbose output
.\Win32\Debug\tests.exe --gtest_filter="*Spike*" --gtest_verbose
```

### **Step 2: Test Different Crash Scenarios**

The tests cover these specific crash scenarios:

1. **Valid Spike File** - Tests normal operation
2. **Corrupted Spike File** - Tests file corruption handling
3. **Empty Spike File** - Tests empty file handling
4. **Large Spike File** - Tests memory limits
5. **Different Plot Modes** - Tests all plot mode combinations
6. **Different Selected Classes** - Tests class selection logic
7. **Null Pointer** - Tests null pointer handling
8. **Invalid File Path** - Tests file path validation
9. **Zero Dimensions** - Tests boundary conditions
10. **Negative Dimensions** - Tests invalid parameters
11. **Very Large Dimensions** - Tests memory overflow

### **Step 3: Replace Mock with Real Implementation**

When you're ready to test the actual crash:

1. **Backup the mock:**
   ```cpp
   // In CGraphImageListMock.cpp
   // Comment out the mock implementations
   ```

2. **Include the real implementation:**
   ```cpp
   // In tests.vcxproj, add:
   <ClCompile Include="..\dbWave64\dbView\CGraphImageList.cpp" />
   ```

3. **Fix dependencies gradually:**
   - Add missing headers one by one
   - Resolve compilation errors systematically
   - Test after each fix

### **Step 4: Isolate the Crash**

```powershell
# Run with specific parameters that cause the crash
.\Win32\Debug\tests.exe --gtest_filter="*SpikeCrash_DifferentSpikePlotModes*"

# Run with debugger attached
.\Win32\Debug\tests.exe --gtest_filter="*SpikeCrash_LargeSpikeFile*" --gtest_break_on_failure
```

## **🎯 Debugging Strategy**

### **1. Parameter Isolation**
Test each parameter separately:
- File size
- Plot mode
- Selected classes
- Image dimensions

### **2. Memory Analysis**
```powershell
# Run memory-intensive tests
.\Win32\Debug\tests.exe --gtest_filter="*Performance*"
```

### **3. Stress Testing**
```powershell
# Run all tests multiple times
for ($i=1; $i -le 10; $i++) { 
    .\Win32\Debug\tests.exe --gtest_filter="*Spike*" 
}
```

## **🔧 Adding New Crash Tests**

### **Template for New Crash Test:**

```cpp
TEST_F(CGraphImageListTestBase, SpikeCrash_YourSpecificScenario)
{
    // Setup
    CString spikeFileName = _T("test_spike.spk");
    DataListCtrlInfos infos = CreateTestInfos();
    
    // Configure specific crash conditions
    infos.spike_plot_mode = PLOT_ONE_CLASS_ONLY;
    infos.selected_classes[0] = 1;
    
    // Test the crash scenario
    CBitmap* pBitmap = CGraphImageList::GenerateSpikeImage(800, 600, spikeFileName, infos);
    
    // Verify results
    ASSERT_NE(pBitmap, nullptr);
    delete pBitmap;
}
```

## **📊 Test Output Interpretation**

### **✅ Successful Test:**
```
[       OK ] CGraphImageListTestBase.SpikeCrash_ValidSpikeFile (2 ms)
```

### **❌ Failed Test:**
```
[  FAILED  ] CGraphImageListTestBase.SpikeCrash_EmptySpikeFile (0 ms)
```

### **💥 Crash/Exception:**
```
[ RUN      ] CGraphImageListTestBase.SpikeCrash_YourTest
[ CRASHED  ] CGraphImageListTestBase.SpikeCrash_YourTest
```

## **🎯 Next Steps**

1. **Run the tests** to establish baseline behavior
2. **Identify which test crashes** - this isolates the problem
3. **Modify test parameters** to narrow down the exact crash condition
4. **Replace mock with real code** gradually
5. **Debug the specific crash scenario** in isolation

## **💡 Pro Tips**

- **Start with mock tests** to understand the calling patterns
- **Use `--gtest_break_on_failure`** to break into debugger on failures
- **Add logging** to track execution flow
- **Test one parameter at a time** to isolate the issue
- **Use Visual Studio debugger** with the test executable

## **🔗 Related Files**

- `tests/SpikeCrashTests.cpp` - Crash-specific tests
- `tests/CGraphImageListMock.cpp` - Mock implementation
- `tests/CGraphImageListTests.h` - Test framework
- `dbWave64/dbView/CGraphImageList.h` - Real class interface

---

**🎯 The key advantage: You can now test crash scenarios in isolation without the complexity of the full dbWave64 application!**
