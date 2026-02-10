@echo off
if "%1"=="clean" (
    call :CLEAN
    goto :eof
)

echo ----------------------------------------------------
echo:
echo                    BUILDING XIGEN
echo:
echo ----------------------------------------------------

set XIGENVER=v1.1.0
echo xigen version:     %XIGENVER%

echo ----------------------------------------------------
echo                  System checking...
echo ----------------------------------------------------
echo Checking Git...
where /q git
if not %errorlevel% == 0 (
    echo ERROR: git is not found! Check the installation and/or PATH.
    goto FAIL
) else (
    echo ***** Done!
    set GIT=git
)

echo Checking CMake...
where /q cmake
if not %errorlevel% == 0 (
    echo ERROR: cmake is not found! Check the installation and/or PATH.
    goto FAIL
) else (
    echo ***** Done!
    set CMAKE=cmake
)

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
echo --------------- System checking done ---------------

call :DEPS_XIGEN win32 Win32
if not %errorlevel% == 0 goto FAIL
call :DEPS_XIGEN win64 x64
if not %errorlevel% == 0 goto FAIL
echo ----------------------------------------------------
echo             BUILDING XIGEN IS COMPLETED
echo ----------------------------------------------------
echo Output:
echo * %DEPSDIR%\win32\xigen
echo * %DEPSDIR%\win64\xigen
echo ----------------------------------------------------
goto :eof


rem The code below defines functions
:CLEAN
echo Cleaning xigen...
if exist %DEPSDIR%\win32\xigen rmdir /s/q %DEPSDIR%\win32\xigen
if exist %DEPSDIR%\win64\xigen rmdir /s/q %DEPSDIR%\win64\xigen
goto :eof


:DEPS_XIGEN
set DISTARCH=%DEPSDIR%\%1
set ARCH=%2
echo:
echo +++++++++++++ Building xigen for %ARCH%...

if exist %DISTARCH%\xigen rmdir /S /Q %DISTARCH%\xigen
mkdir %DISTARCH%\xigen

set URL_XIGEN="https://github.com/Standa-Optomechanics/xigen.git"

"%GIT%" clone --recursive %URL_XIGEN% %DISTARCH%\xigen -b %XIGENVER%
if not %errorlevel% == 0 goto FAIL
cd %DISTARCH%\xigen
if not %errorlevel% == 0 goto FAIL
set GENERATOR=Visual Studio 12 2013
if %ARCH% == x64 set GENERATOR=%GENERATOR% Win64
%CMAKE% -G "%GENERATOR%" -DBUILD_TESTING=OFF -DCMAKE_INSTALL_PREFIX=%DISTARCH%\xigen
if not %errorlevel% == 0 goto FAIL
%MSBUILD% ALL_BUILD.vcxproj /p:Configuration=%CONFIGURATION% /p:Platform=%ARCH%
if not %errorlevel% == 0 goto FAIL
%MSBUILD% INSTALL.vcxproj /p:Configuration=%CONFIGURATION% /p:Platform=%ARCH%
if not %errorlevel% == 0 goto FAIL
cd %BASEDIR%
goto :eof

:FAIL
echo FAIL
exit /B 1
