@echo off
rem Set paths
set NAME=test_VBNET
set EXAMPLES_SRC_DIR=%BASEDIR%\examples\%NAME%
set EXAMPLES_DIST_DIR=%DISTDIR%\examples\%NAME%

if "%1"=="clean" (
    call :CLEAN
    goto :eof
)

echo ----------------------------------------------------
echo:
echo               BUILDING VB.NET EXAMPLE
echo:
echo ----------------------------------------------------

echo ----------------------------------------------------
echo                  System checking...
echo ----------------------------------------------------
echo Checking VS120COMNTOOLS environmental variable...
if "%VS120COMNTOOLS%"=="" (
    echo ERROR: VS120COMNTOOLS env.variable is empty! Check Visual Studio installation and/or env.variables.
    goto FAIL
) else (
    echo ***** Done!
    rem TODO: remove an architectural kludge!
    rem Prepare environmental variables
    if "%VSINSTALLDIR%" == "" call "%VS120COMNTOOLS%/vsvars32.bat"
    if not %errorlevel% == 0 goto FAIL
    if "%VSINSTALLDIR%" == "" call "%VCINSTALLDIR%/vcvarsall.bat" x86_amd64
    if not %errorlevel% == 0 goto FAIL
)

echo Checking MSBuild...
where /q msbuild
if not %errorlevel% == 0 (
    echo ERROR: msbuild is not found! Check the installation and/or PATH.
    goto FAIL
) else (
    echo ***** Done!
    rem TODO: remove an architectural kludge!
    set MSBUILD=msbuild /nr:false
    set MSBUILDDISABLENODEREUSE=1
)

echo Checking ximcnet.dll...
set XIMCNET_DLL_NOT_FOUND=false
if not exist %DISTDIR%\ximc\win32\wrappers\csharp\ximcnet.dll set XIMCNET_DLL_NOT_FOUND=true
if not exist %DISTDIR%\ximc\win64\wrappers\csharp\ximcnet.dll set XIMCNET_DLL_NOT_FOUND=true
if "%XIMCNET_DLL_NOT_FOUND%"=="true" (
    echo ERROR: ximcnet.dll cannot be found. It must reside in "%DISTDIR%\ximc\win32\wrappers\csharp\ximcnet.dll" and
    echo in "%DISTDIR%\ximc\win64\wrappers\csharp\ximcnet.dll" for Win32 and x64 build configurations respectively.
    goto FAIL
) else (
    echo ***** Done!
    set XIMCNET_DLL_32=%DISTDIR%\ximc\win32\wrappers\csharp\ximcnet.dll
    set XIMCNET_DLL_64=%DISTDIR%\ximc\win64\wrappers\csharp\ximcnet.dll
)
echo --------------- System checking done ---------------

call :BUILD_VBNET_EXAMPLE
if not %errorlevel% == 0 goto FAIL
echo ----------------------------------------------------
echo        BUILDING VB.NET EXAMPLE IS COMPLETED
echo ----------------------------------------------------
echo Output:
echo %EXAMPLES_DIST_DIR%
echo ----------------------------------------------------
goto :eof


rem The code below defines functions
:CLEAN
echo Cleaning VB.NET example...
where /q git
if not %errorlevel% == 0 (
    echo WARNING: git cannot be found! Cleaning won't be complete.
) else (
    git clean -xdf %EXAMPLES_SRC_DIR%
)
if not exist %EXAMPLES_DIST_DIR% goto :eof
rmdir %EXAMPLES_DIST_DIR% /s/q
goto :eof


:BUILD_VBNET_EXAMPLE
echo +++++++++++++ Building example %NAME% for Win32
copy %XIMCNET_DLL_32% %EXAMPLES_SRC_DIR%
%MSBUILD% %EXAMPLES_SRC_DIR%\%NAME%.sln /p:Configuration=%CONFIGURATION% /p:Platform=Win32
if not %errorlevel% == 0 goto FAIL
mkdir %EXAMPLES_DIST_DIR%\%NAME%-compiled-win32
copy %EXAMPLES_SRC_DIR%\compiled-win32\* %EXAMPLES_DIST_DIR%\%NAME%-compiled-win32\*
if not %errorlevel% == 0 goto FAIL

del %EXAMPLES_DIST_DIR%\%NAME%-compiled-win32\test_VBNET.pdb
del %EXAMPLES_DIST_DIR%\%NAME%-compiled-win32\test_VBNET.xml
echo +++++++++++++ Building example %NAME% for x64
copy %XIMCNET_DLL_64% %EXAMPLES_SRC_DIR%
%MSBUILD% %EXAMPLES_SRC_DIR%\%NAME%.sln /p:Configuration=%CONFIGURATION% /p:Platform=x64
if not %errorlevel% == 0 goto FAIL
mkdir %EXAMPLES_DIST_DIR%\%NAME%-compiled-win64
copy %EXAMPLES_SRC_DIR%\compiled-win64\* %EXAMPLES_DIST_DIR%\%NAME%-compiled-win64\*
if not %errorlevel% == 0 goto FAIL

del %EXAMPLES_DIST_DIR%\%NAME%-compiled-win64\test_VBNET.pdb
del %EXAMPLES_DIST_DIR%\%NAME%-compiled-win64\test_VBNET.xml
goto :eof

:FAIL
echo FAIL
exit /B 1
