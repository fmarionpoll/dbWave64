@echo off
echo ========================================
echo DBWAVE64 DLL COPY VERIFICATION
echo ========================================

echo.
echo Checking source DLLs in include\toAdd:
if exist "include\toAdd\oldaapi32.dll" (
    echo [OK] oldaapi32.dll found
) else (
    echo [ERROR] oldaapi32.dll missing
)

if exist "include\toAdd\OLMEM32.dll" (
    echo [OK] OLMEM32.dll found
) else (
    echo [ERROR] OLMEM32.dll missing
)

if exist "include\toAdd\ceds64int.dll" (
    echo [OK] ceds64int.dll found
) else (
    echo [ERROR] ceds64int.dll missing
)

echo.
echo Checking output directories:
echo.

if exist "Debug\oldaapi32.dll" (
    echo [OK] Debug\oldaapi32.dll exists
) else (
    echo [MISSING] Debug\oldaapi32.dll - Run build first
)

if exist "Debug\OLMEM32.dll" (
    echo [OK] Debug\OLMEM32.dll exists
) else (
    echo [MISSING] Debug\OLMEM32.dll - Run build first
)

if exist "Debug\ceds64int.dll" (
    echo [OK] Debug\ceds64int.dll exists
) else (
    echo [MISSING] Debug\ceds64int.dll - Run build first
)

echo.
echo ========================================
echo VERIFICATION COMPLETE
echo ========================================
echo.
echo If DLLs are missing from Debug directory:
echo 1. Build the project (Debug configuration)
echo 2. Check that post-build events ran successfully
echo 3. Verify DLLs were copied to output directory
echo.
pause
