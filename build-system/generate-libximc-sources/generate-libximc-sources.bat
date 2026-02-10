@echo off
rem reset errorlevel
cmd /C exit 0
if "%BASEDIR%"=="" set BASEDIR="%CD%"
if "%DEPSDIR%"=="" set DEPSDIR=%BASEDIR%\deps
if "%LIBXIMC_SRC_DIR%"=="" set LIBXIMC_SRC_DIR=%BASEDIR%\libximc\src
if "%LIBXIMC_INC_DIR%"=="" set LIBXIMC_INC_DIR=%BASEDIR%\libximc\include
set XIMC_VERSION_FILE=%BASEDIR%\version

if "%1"=="gen-libximc-sources" (
    rem This is batch butthert. In case we call this stage using the control script with appropriate command this
    rem command is passed. So here we have extra argument that must be removed by shifting.
    shift
)

if "%1"=="clean" (
    call :CLEAN
    goto :eof
)
if "%1"=="" (
    setlocal enableDelayedExpansion
    echo ****************************************************
    echo *                                                  *
    echo *           GENERATING LIBXIMC's SOURCES           *
    echo *                                                  *
    echo ****************************************************

    call .\build-system\generate-libximc-sources\generate-libximc-sources\generate-libximc-sources.bat
    if not !errorlevel! == 0 goto FAIL
    echo ****************************************************
    echo *    GENERATING LIBXIMC's SOURCES IS COMPLETED     *
    echo ****************************************************
    endlocal
    goto :eof
)
if "%1"=="--help" (
    echo "Generates libximc sources."
    goto :eof
)
echo "Unknown command %1"
echo "Generates libximc sources."
goto FAIL


:CLEAN
echo ****************************************************
echo *         CLEAN GENERATED LIBXIMC's SOURCES        *
echo ****************************************************
call .\build-system\generate-libximc-sources\generate-libximc-sources\generate-libximc-sources.bat clean
echo ****************************************************
echo *                       DONE                       *
echo ****************************************************
goto :eof


:FAIL
echo FAIL
exit /B 1
