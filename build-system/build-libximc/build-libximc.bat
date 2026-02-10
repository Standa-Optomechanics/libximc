@echo off
rem reset errorlevel
cmd /C exit 0
if "%BASEDIR%"=="" set BASEDIR="%CD%"
if "%DEPSDIR%"=="" set DEPSDIR=%BASEDIR%\deps
if "%DISTDIR%"=="" set DISTDIR=%BASEDIR%\ximc
if "%LIBSDIR_x86%"=="" set LIBSDIR_x86=%BASEDIR%\ximc\win32
if "%LIBSDIR_x64%"=="" set LIBSDIR_x64=%BASEDIR%\ximc\win64
if "%CONFIGURATION%"=="" set CONFIGURATION=Release
if "%LIBXIMC_SRC_DIR%"=="" set LIBXIMC_SRC_DIR=%BASEDIR%\libximc\src
if "%LIBXIMC_INC_DIR%"=="" set LIBXIMC_INC_DIR=%BASEDIR%\libximc\include

set STAGE_NAME=%0
if "%1"=="libximc" (
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
    echo *                 BUILDING LIBXIMC                 *
    echo *                                                  *
    echo ****************************************************

    echo Configuration: %CONFIGURATION%

    call ./build-system/build-libximc/build-libximc/build-libximc.bat
    if not !errorlevel! == 0 goto FAIL
    echo ****************************************************
    echo *           BUILDING LIBXIMC IS COMPLETED          *
    echo ****************************************************
    endlocal
    goto :eof
)
if "%1"=="--help" (
    echo "Builds libximc."
    goto :eof
)
echo "Builds libximc."
goto FAIL


:CLEAN
echo ****************************************************
echo *                  CLEAN LIBXIMC                   *
echo ****************************************************
call ./build-system/build-libximc/build-libximc/build-libximc.bat clean
echo ****************************************************
echo *                       DONE                       *
echo ****************************************************
goto :eof


:FAIL
echo FAIL
exit /B 1
