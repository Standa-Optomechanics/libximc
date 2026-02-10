@echo off
if "%1"=="clean" (
    call :CLEAN
    goto :eof
)

echo ----------------------------------------------------
echo:
echo                  BUILDING XIWRAPPER
echo:
echo ----------------------------------------------------

for /f "skip=3" %%L in ( version ) do ( set XIWRAPPERVER=%%L & goto out )
:out
if "%XIWRAPPERVER%"=="" set XIWRAPPERVER=master
echo xiwrapper version: %XIWRAPPERVER%

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

call :DEPS_XIWRAPPER win64 x64
if not %errorlevel% == 0 goto FAIL
call :DEPS_XIWRAPPER win32 Win32
if not %errorlevel% == 0 goto FAIL
echo ----------------------------------------------------
echo           BUILDING XIWRAPPER IS COMPLETED
echo ----------------------------------------------------
echo Output:
echo * %DISTDIR%\ximc\win32\xiwrapper.dll
echo * %DISTDIR%\ximc\win64\xiwrapper.dll
echo ----------------------------------------------------
goto :eof


rem The code below defines functions
:CLEAN
echo Cleaning xiwrapper...
if exist %DEPSDIR%\win32\xiwrapper rmdir   /s/q %DEPSDIR%\win32\xiwrapper
if exist %DEPSDIR%\win64\xiwrapper rmdir   /s/q %DEPSDIR%\win64\xiwrapper
if exist %DISTDIR%\win32\xiwrapper.dll del /s/q %DISTDIR%\win32\xiwrapper.dll
if exist %DISTDIR%\win64\xiwrapper.dll del /s/q %DISTDIR%\win64\xiwrapper.dll
goto :eof


:DEPS_XIWRAPPER
set DISTARCH=%DEPSDIR%\%1
set ARCH=%2
echo:
echo +++++++++++++ Building xiwrapper for %ARCH%...

rmdir /S /Q %DISTARCH%\xiwrapper
mkdir %DISTARCH%\xiwrapper

if "x%URL_XIWRAPPER%" == "x" set URL_XIWRAPPER="https://github.com/Standa-Optomechanics/libxiwrapper.git"

"%GIT%" clone %URL_XIWRAPPER% %DISTARCH%\xiwrapper
if not %errorlevel% == 0 goto FAIL
cd %DISTARCH%\xiwrapper
%GIT% checkout %XIWRAPPERVER%
if not %errorlevel% == 0 goto FAIL
"%GIT%" --no-pager show --stat %XIWRAPPERVER%
if not %errorlevel% == 0 goto FAIL
set GENERATOR=Visual Studio 12 2013
if %ARCH% == x64 set GENERATOR=%GENERATOR% Win64
%CMAKE% -G "%GENERATOR%" -DBINDY_PATH=%DISTARCH%\bindy .
set LASTERR=%errorlevel%
cd %BASEDIR%
if not %LASTERR% == 0 goto FAIL
%MSBUILD% %DISTARCH%\xiwrapper\xiwrapper.sln /p:Configuration=%CONFIGURATION% /p:Platform=%ARCH%
if not %errorlevel% == 0 goto FAIL
if not exist %DISTARCH%\xiwrapper\%CONFIGURATION%\xiwrapper.dll goto FAIL

rem Copying xiwrapper to the dist
if not exist %DISTDIR%\ximc\%1 mkdir %DISTDIR%\ximc\%1
if not %errorlevel% == 0 goto FAIL
copy %DISTARCH%\xiwrapper\%CONFIGURATION%\xiwrapper.dll %DISTDIR%\ximc\%1
if not %errorlevel% == 0 goto FAIL

if not "%CONFIGURATION%"=="Debug" goto SKIP_PDB_COPY_XIWRAPPER
copy %DISTARCH%\xiwrapper\%CONFIGURATION%\xiwrapper.pdb %DISTDIR%\ximc\%1
if not %errorlevel% == 0 goto FAIL
:SKIP_PDB_COPY_XIWRAPPER
goto :eof


:FAIL
echo FAIL
exit /B 1
