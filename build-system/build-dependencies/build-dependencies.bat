@echo off
rem reset errorlevel
cmd /C exit 0
if "%BASEDIR%"=="" set BASEDIR="%CD%"
if "%DEPSDIR%"=="" set DEPSDIR=%BASEDIR%\deps
if "%DISTDIR%"=="" set DISTDIR=%BASEDIR%\ximc
if "%CONFIGURATION%"=="" set CONFIGURATION=Release

set STAGE_NAME=%0
if "%1"=="deps" (
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
echo *              BUILDING DEPENDENCIES               *
echo *                                                  *
echo ****************************************************

echo Configuration: %CONFIGURATION%

if "%1"=="xigen" (
    setlocal enableDelayedExpansion
    call ./build-system/build-dependencies/build-xigen/build-xigen.bat
    if not !errorlevel! == 0 goto FAIL
    endlocal
    goto :eof
)
if "%1"=="bindy" (
    setlocal enableDelayedExpansion
    call ./build-system/build-dependencies/build-bindy/build-bindy.bat
    if not !errorlevel! == 0 goto FAIL
    endlocal
    goto :eof
)
if "%1"=="xiwrapper" (
    setlocal enableDelayedExpansion
    call ./build-system/build-dependencies/build-xiwrapper/build-xiwrapper.bat
    if not !errorlevel! == 0 goto FAIL
    endlocal
    goto :eof
)
if "%1"=="miniupnpc" (
    setlocal enableDelayedExpansion
    call ./build-system/build-dependencies/build-miniupnpc/build-miniupnpc.bat
    if not !errorlevel! == 0 goto FAIL
    endlocal
    goto :eof
)
if "%1"=="" (
    setlocal enableDelayedExpansion
    call ./build-system/build-dependencies/build-xigen/build-xigen.bat
    if not !errorlevel! == 0 goto FAIL
    call ./build-system/build-dependencies/build-bindy/build-bindy.bat
    if not !errorlevel! == 0 goto FAIL
    call ./build-system/build-dependencies/build-xiwrapper/build-xiwrapper.bat
    if not !errorlevel! == 0 goto FAIL
    call ./build-system/build-dependencies/build-miniupnpc/build-miniupnpc.bat
    if not !errorlevel! == 0 goto FAIL
    echo ****************************************************
    echo *        BUILDING DEPENDENCIES IS COMPLETED        *
    echo ****************************************************
    endlocal
    goto :eof
)
if "%1"=="--help" (
    echo "Usage: %SCRIPT_NAME% %STAGE_NAME% [ xigen | bindy | xiwrapper | miniupnpc | clean ]"
    echo "To build all dependencies just use %SCRIPT_NAME% %STAGE_NAME%"
    echo ""
    echo "OPTIONS:"
    echo "  xigen     - build xigen"
    echo "  bindy     - build Bindy"
    echo "  xiwrapper - build xiwrapper"
    echo "  miniupnpc - build miniupnpc"
    echo "  clean     - remove all dependencies and related files"
    echo ""
    goto :eof
)
echo "Unknown command %1."
echo "Usage: %SCRIPT_NAME% %STAGE_NAME% [ xigen | bindy | xiwrapper | miniupnpc | clean ]"
echo "To build all dependencies just use %SCRIPT_NAME% %STAGE_NAME%"
echo ""
echo "OPTIONS:"
echo "  xigen     - build xigen"
echo "  bindy     - build Bindy"
echo "  xiwrapper - build xiwrapper"
echo "  miniupnpc - build miniupnpc"
echo "  clean     - remove all dependencies and related files"
echo ""
goto FAIL


:CLEAN
echo ****************************************************
echo *                CLEAN DEPENDENCIES                *
echo ****************************************************
call ./build-system/build-dependencies/build-xigen/build-xigen.bat         clean
call ./build-system/build-dependencies/build-bindy/build-bindy.bat         clean
call ./build-system/build-dependencies/build-xiwrapper/build-xiwrapper.bat clean
call ./build-system/build-dependencies/build-miniupnpc/build-miniupnpc.bat clean
if exist %BASEDIR%\deps rmdir %BASEDIR%\deps /s/q
echo ****************************************************
echo *                       DONE                       *
echo ****************************************************
goto :eof


:FAIL
echo FAIL
exit /B 1
