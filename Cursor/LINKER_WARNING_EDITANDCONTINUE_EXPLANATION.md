# Linker Warning LNK4075: EDITANDCONTINUE vs INCREMENTAL Explanation

## Warning Message
```
1>chanlistitem.obj : warning LNK4075: ignoring '/EDITANDCONTINUE' due to '/INCREMENTAL:NO' specification
```

## What This Warning Means

This warning occurs because there's a **conflict between two linker settings** in your Visual Studio project:

1. **`/EDITANDCONTINUE`** - Enables Edit and Continue debugging feature
2. **`/INCREMENTAL:NO`** - Disables incremental linking

## Root Cause Analysis

### Project Configuration Conflict

Looking at your `dbWave64.vcxproj` file, the issue is in the Debug configuration:

```xml
<!-- Line 567: LinkIncremental is set to FALSE -->
<LinkIncremental>false</LinkIncremental>

<!-- Line 605: DebugInformationFormat is set to EditAndContinue -->
<DebugInformationFormat>EditAndContinue</DebugInformationFormat>
```

### Why This Conflict Occurs

- **Edit and Continue** requires **incremental linking** to work properly
- When `LinkIncremental` is set to `false`, the linker uses `/INCREMENTAL:NO`
- This disables the incremental linking that Edit and Continue depends on
- The linker automatically ignores the Edit and Continue setting when incremental linking is disabled

## Solutions

### Option 1: Enable Incremental Linking (Recommended for Debug)

Change the Debug configuration in `dbWave64.vcxproj`:

```xml
<PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Debug|Win32'">
    <OutDir>.\Debug\</OutDir>
    <IntDir>.\Debug\</IntDir>
    <LinkIncremental>true</LinkIncremental>  <!-- Change from false to true -->
    <!-- ... rest of configuration -->
</PropertyGroup>
```

### Option 2: Disable Edit and Continue

If you prefer to keep incremental linking disabled, change the debug information format:

```xml
<ClCompile>
    <!-- ... other settings ... -->
    <DebugInformationFormat>ProgramDatabase</DebugInformationFormat>  <!-- Change from EditAndContinue -->
</ClCompile>
```

### Option 3: Use Different Settings for Different Configurations

- **Debug**: Enable both incremental linking and Edit and Continue
- **Release**: Disable both for faster builds

## Impact of Each Setting

### Edit and Continue (`/EDITANDCONTINUE`)
- **Pros**: Allows code changes during debugging without restarting
- **Cons**: Slightly larger executable and slower linking
- **Best for**: Development and debugging

### Incremental Linking (`/INCREMENTAL`)
- **Pros**: Faster linking for small changes
- **Cons**: Larger executable size
- **Best for**: Development builds

### Non-Incremental Linking (`/INCREMENTAL:NO`)
- **Pros**: Smaller executable, potentially faster for full rebuilds
- **Cons**: Slower linking for incremental changes
- **Best for**: Release builds

## Recommended Configuration

For your Debug x86 configuration, I recommend:

```xml
<PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Debug|Win32'">
    <LinkIncremental>true</LinkIncremental>
</PropertyGroup>

<ItemDefinitionGroup Condition="'$(Configuration)|$(Platform)'=='Debug|Win32'">
    <ClCompile>
        <DebugInformationFormat>EditAndContinue</DebugInformationFormat>
    </ClCompile>
</ItemDefinitionGroup>
```

## Verification

After making changes:
1. Clean the solution (`Build > Clean Solution`)
2. Rebuild the solution
3. The warning should disappear
4. Edit and Continue should work during debugging

## Additional Notes

- This warning is **harmless** - it doesn't affect functionality
- The linker automatically chooses the most restrictive setting
- For production builds, keeping `LinkIncremental=false` is often preferred
- The warning appears for each object file, but it's the same underlying issue

## Related Files

- `dbwave64/dbWave64/dbWave64.vcxproj` - Project configuration file
- `chanlistitem.cpp` - One of the files triggering the warning (but not the cause)
