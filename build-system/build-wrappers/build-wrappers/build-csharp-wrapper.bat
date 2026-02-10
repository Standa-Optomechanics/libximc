@echo off
if "%1"=="clean" (
    call :CLEAN
    goto :eof
)

echo ----------------------------------------------------
echo:
echo                  BUILDING C# WRAPPER
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

echo Checking xigen...
if not exist %DEPSDIR%\win32\xigen\bin\xigen.exe (
    echo ERROR: xigen for win32 cannot be found!
    goto :FAIL
) else (
    echo ***** Done!
    set XIGEN=%DEPSDIR%\win32\xigen\bin\xigen.exe
)
echo --------------- System checking done ---------------

call :WRAPPER_CSHARP win32 Win32
if not %errorlevel% == 0 goto FAIL
call :WRAPPER_CSHARP win64 x64
if not %errorlevel% == 0 goto FAIL
echo ----------------------------------------------------
echo           BUILDING C# WRAPPER IS COMPLETED          
echo ----------------------------------------------------
echo Output:
echo * %DISTDIR%\ximc\win32\wrappers\csharp
echo * %DISTDIR%\ximc\win64\wrappers\csharp
echo ----------------------------------------------------
goto :eof


rem The code below defines functions
:CLEAN
echo Cleaning C# wrapper...
if exist %DISTDIR%\ximc\win32\wrappers\csharp rmdir /s/q %DISTDIR%\ximc\win32\wrappers\csharp
if exist %DISTDIR%\ximc\win64\wrappers\csharp rmdir /s/q %DISTDIR%\ximc\win64\wrappers\csharp
where /q git
if not %errorlevel% == 0 (
    echo WARNING: git cannot be found! Cleaning won't be complete.
) else (
    git clean -xdf %BASEDIR%\wrappers\csharp
)
goto :eof


:WRAPPER_CSHARP
echo:
echo +++++++++++++ Building csharp wrapper for %2
set DISTARCH=%DISTDIR%\ximc\%1
set BINDIR=wrappers\csharp\bin\%CONFIGURATION%-%2

:: allow msbuild processes to finish, sometimes they lock build dir
waitfor XimcBuildLock /t 30
cmd /C exit 0

if not exist %DISTARCH% mkdir %DISTARCH%
if not %errorlevel% == 0 goto FAIL

%XIGEN% --gen-csharp -x %XIMC_VERSION_FILE% -i %LIBXIMC_SRC_DIR%\protocol.xi -o %BASEDIR%\wrappers\csharp\src\ximcnet.cs -t %BASEDIR%\wrappers\csharp\src\ximcnet-template.cs
%MSBUILD% wrappers\csharp\ximcnet.sln /p:Configuration=%CONFIGURATION% /p:Platform=%2
if not %errorlevel% == 0 goto FAIL

if not exist %DISTARCH%\wrappers\csharp\ mkdir %DISTARCH%\wrappers\csharp\
copy %BINDIR%\ximcnet.dll           %DISTARCH%\wrappers\csharp\
if not %errorlevel% == 0 goto FAIL
copy wrappers\csharp\src\ximcnet.cs %DISTARCH%\wrappers\csharp\
if not %errorlevel% == 0 goto FAIL

waitfor XimcBuildLock /t 30
cmd /C exit 0
goto :eof


:FAIL
echo FAIL
exit /B 1
