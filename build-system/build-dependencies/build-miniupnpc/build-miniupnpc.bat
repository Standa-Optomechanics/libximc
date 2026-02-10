@echo off
if "%1"=="clean" (
    call :CLEAN
    goto :eof
)

echo ----------------------------------------------------
echo:
echo                  BUILDING MINIUPNPC
echo:
echo ----------------------------------------------------

set MINIUPNPCVER=8ddbb71
echo miniupnpc version: %MINIUPNPCVER%

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

call :DEPS_MINIUPNPC win64 x64
if not %errorlevel% == 0 goto FAIL
call :DEPS_MINIUPNPC win32 Win32
if not %errorlevel% == 0 goto FAIL
echo ----------------------------------------------------
echo           BUILDING MINIUPNPC IS COMPLETED
echo ----------------------------------------------------
echo Output:
echo * %DEPSDIR%\win32\miniupnpc
echo * %DEPSDIR%\win64\miniupnpc
echo ----------------------------------------------------
goto :eof


rem The code below defines functions
:CLEAN
echo Cleaning miniupnpc...
if exist %DEPSDIR%\win32\miniupnpc rmdir /s/q %DEPSDIR%\win32\miniupnpc
if exist %DEPSDIR%\win64\miniupnpc rmdir /s/q %DEPSDIR%\win64\miniupnpc
goto :eof

:DEPS_MINIUPNPC
set DISTARCH=%DEPSDIR%\%1
set ARCH=%2
echo:
echo +++++++++++++ Building miniupnpc for %ARCH%...

rmdir /S /Q %DISTARCH%\miniupnpc-dist
rmdir /S /Q %DISTARCH%\miniupnpc
mkdir %DISTARCH%\miniupnpc

if "x%URL_MINIUPNPC%" == "x" set URL_MINIUPNPC="https://github.com/EPC-MSU/miniupnpc.git"

"%GIT%" clone --recursive %URL_MINIUPNPC% %DISTARCH%\miniupnpc-dist
if not %errorlevel% == 0 goto FAIL
cd %DISTARCH%\miniupnpc-dist
"%GIT%" checkout %MINIUPNPCVER%
if not %errorlevel% == 0 goto FAIL

set GENERATOR=Visual Studio 12 2013
if %ARCH% == x64 set GENERATOR=%GENERATOR% Win64
%CMAKE% -G "%GENERATOR%" -DUPNPC_BUILD_TESTS=OFF -DUPNPC_BUILD_SAMPLE=OFF -DUPNPC_BUILD_SHARED=OFF -DCMAKE_INSTALL_PREFIX=%DISTARCH%\miniupnpc .
set LASTERR=%errorlevel%
cd %BASEDIR%
if not %LASTERR% == 0 goto FAIL

%MSBUILD% %DISTARCH%\miniupnpc-dist\ALL_BUILD.vcxproj
if not %errorlevel% == 0 goto FAIL
%MSBUILD% %DISTARCH%\miniupnpc-dist\INSTALL.vcxproj
if not %errorlevel% == 0 goto FAIL
goto :eof


:FAIL
echo FAIL
exit /B 1
