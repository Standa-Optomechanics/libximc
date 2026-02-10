@echo off
if "%1"=="clean" (
    call :CLEAN
    goto :eof
)

echo ----------------------------------------------------
echo:
echo                    BUILDING BINDY
echo:
echo ----------------------------------------------------

for /f "skip=2" %%L in ( version ) do ( set BINDYVER=%%L & goto out )
:out
if "%BINDYVER%"=="" set BINDYVER=master
echo Bindy version:     %BINDYVER%

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

call :DEPS_BINDY win64 x64
if not %errorlevel% == 0 goto FAIL
call :DEPS_BINDY win32 Win32
if not %errorlevel% == 0 goto FAIL
echo ----------------------------------------------------
echo             BUILDING BINDY IS COMPLETED
echo ----------------------------------------------------
echo Output:
echo * %DISTDIR%\ximc\win32\bindy
echo * %DISTDIR%\ximc\win64\bindy
echo ----------------------------------------------------
goto :eof


rem The code below defines functions
:CLEAN
echo Cleaning bindy...
if exist %DEPSDIR%\win32\bindy rmdir   /s/q %DEPSDIR%\win32\bindy
if exist %DEPSDIR%\win64\bindy rmdir   /s/q %DEPSDIR%\win64\bindy
if exist %DISTDIR%\ximc\win32\bindy.dll del /s/q %DISTDIR%\ximc\win32\bindy.dll
if exist %DISTDIR%\ximc\win32\bindy.lib del /s/q %DISTDIR%\ximc\win32\bindy.lib
if exist %DISTDIR%\ximc\win64\bindy.dll del /s/q %DISTDIR%\ximc\win64\bindy.dll
if exist %DISTDIR%\ximc\win64\bindy.lib del /s/q %DISTDIR%\ximc\win64\bindy.lib
goto :eof


:DEPS_BINDY
set DISTARCH=%DEPSDIR%\%1
set ARCH=%2
echo:
echo +++++++++++++ Building Bindy for %ARCH%...

if exist %DISTARCH%\bindy rmdir /s/q %DISTARCH%\bindy
mkdir %DISTARCH%\bindy

if "x%URL_BINDY%" == "x" set URL_BINDY="https://github.com/EPC-MSU/Bindy.git"

"%GIT%" clone --recursive %URL_BINDY% %DISTARCH%\bindy
if not %errorlevel% == 0 goto FAIL
cd %DISTARCH%\bindy
"%GIT%" checkout %BINDYVER%
if not %errorlevel% == 0 goto FAIL
"%GIT%" submodule update --init --recursive
if not %errorlevel% == 0 goto FAIL
"%GIT%" submodule update --recursive
if not %errorlevel% == 0 goto FAIL

"%GIT%" --no-pager show --stat %BINDYVER%
set GENERATOR=Visual Studio 12 2013
if %ARCH% == x64 set GENERATOR=%GENERATOR% Win64
%CMAKE% -G "%GENERATOR%" .
set LASTERR=%errorlevel%
cd %BASEDIR%
if not %LASTERR% == 0 goto FAIL
rem remove test
echo %ARCH%
%MSBUILD% %DISTARCH%\bindy\bindy.sln /p:Configuration=%CONFIGURATION% /p:Platform=%ARCH%
if not %errorlevel% == 0 goto FAIL
if not exist %DISTARCH%\bindy\%CONFIGURATION%\bindy.dll goto FAIL

rem Copying bindy to the dist
if not exist %DISTDIR%\ximc\%1 mkdir %DISTDIR%\ximc\%1
if not %errorlevel% == 0 goto FAIL
copy %DISTARCH%\bindy\%CONFIGURATION%\bindy.dll %DISTDIR%\ximc\%1
if not %errorlevel% == 0 goto FAIL
copy %DISTARCH%\bindy\%CONFIGURATION%\bindy.lib %DISTDIR%\ximc\%1
if not %errorlevel% == 0 goto FAIL

if not "%CONFIGURATION%"=="Debug" goto SKIP_PDB_COPY_BINDY
copy %DISTARCH%\bindy\%CONFIGURATION%\bindy.pdb %DISTDIR%\ximc\%1
if not %errorlevel% == 0 goto FAIL
:SKIP_PDB_COPY_BINDY
goto :eof


:FAIL
echo FAIL
exit /B 1
