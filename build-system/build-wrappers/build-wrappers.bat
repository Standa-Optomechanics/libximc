@echo off
rem reset errorlevel
cmd /C exit 0
if "%BASEDIR%"=="" set BASEDIR="%CD%"
if "%DEPSDIR%"=="" set DEPSDIR=%BASEDIR%\deps
if "%DISTDIR%"=="" set DISTDIR=%BASEDIR%\ximc
if "%CONFIGURATION%"=="" set CONFIGURATION=Release
if "%LIBXIMC_SRC_DIR%"=="" set LIBXIMC_SRC_DIR=%BASEDIR%\libximc\src
if "%LIBXIMC_INC_DIR%"=="" set LIBXIMC_INC_DIR=%BASEDIR%\libximc\include
set XIMC_VERSION_FILE=%BASEDIR%\version

set STAGE_NAME=%0
if "%1"=="wrappers" (
    rem This is batch butthert. In case we call this stage using the control script with appropriate command this
    rem command is passed. So here we have extra argument that must be removed by shifting.
    shift
)

if "%1"=="clean" (
    call :CLEAN
    goto :eof
)
echo ****************************************************
echo *                                                  *
echo *                BUILDING WRAPPERS                 *
echo *                                                  *
echo ****************************************************

echo Configuration: %CONFIGURATION%

if "%1"=="C#" (
    setlocal enableDelayedExpansion
    call ./build-system/build-wrappers/build-wrappers/build-csharp-wrapper.bat
    if not !errorlevel! == 0 goto FAIL
    endlocal
    goto :eof
)
if "%1"=="Java" (
    setlocal enableDelayedExpansion
    call ./build-system/build-wrappers/build-wrappers/build-java-wrapper.bat
    if not !errorlevel! == 0 goto FAIL
    endlocal
    goto :eof
)
if "%1"=="Matlab" (
    setlocal enableDelayedExpansion
    call ./build-system/build-wrappers/build-wrappers/build-matlab-wrapper.bat
    if not !errorlevel! == 0 goto FAIL
    endlocal
    goto :eof
)
if "%1"=="" (
    setlocal enableDelayedExpansion
    call ./build-system/build-wrappers/build-wrappers/build-csharp-wrapper.bat
    if not !errorlevel! == 0 goto FAIL
    call ./build-system/build-wrappers/build-wrappers/build-java-wrapper.bat
    if not !errorlevel! == 0 goto FAIL
    call ./build-system/build-wrappers/build-wrappers/build-matlab-wrapper.bat
    if not !errorlevel! == 0 goto FAIL
    echo ****************************************************
    echo *          BUILDING WRAPPERS IS COMPLETED          *
    echo ****************************************************
    endlocal
    goto :eof
)
if "%1"=="--help" (
    call :PRINT_HELP
    goto :eof
)
echo Unknown command %1.
echo.
call :PRINT_HELP
goto FAIL


:CLEAN
echo ****************************************************
echo *                  CLEAN WRAPPERS                  *
echo ****************************************************
call ./build-system/build-wrappers/build-wrappers/build-csharp-wrapper.bat clean
call ./build-system/build-wrappers/build-wrappers/build-java-wrapper.bat   clean
call ./build-system/build-wrappers/build-wrappers/build-matlab-wrapper.bat clean
if exist %DISTDIR%\win32\wrappers rmdir /s/q %DISTDIR%\win32\wrappers
if exist %DISTDIR%\win64\wrappers rmdir /s/q %DISTDIR%\win64\wrappers
echo ****************************************************
echo *                       DONE                       *
echo ****************************************************
goto :eof

:PRINT_HELP
echo Usage: %SCRIPT_NAME% %STAGE_NAME% ^[ C# ^| Java ^| Matlab ^]
echo To build all wrappers just use %SCRIPT_NAME% %STAGE_NAME%
echo.
echo OPTIONS:
echo   C#        - build C# wrapper
echo   Java      - build Java wrapper
echo   Matlab    - build Matlab wrapper
echo   clean     - remove all wrappers and related files
echo.
goto :eof


:FAIL
echo FAIL
exit /B 1
