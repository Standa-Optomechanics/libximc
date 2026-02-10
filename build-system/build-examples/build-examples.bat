@echo off
rem reset errorlevel
cmd /C exit 0
if "%BASEDIR%"=="" set BASEDIR="%CD%"
if "%DEPSDIR%"=="" set DEPSDIR=%BASEDIR%\deps
if "%DISTDIR%"=="" set DISTDIR=%BASEDIR%\dist
if "%CONFIGURATION%"=="" set CONFIGURATION=Release

set STAGE_NAME=%0
if "%1"=="examples" (
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
echo *           BUILDING LIBXIMC's EXAMPLES            *
echo *                                                  *
echo ****************************************************

echo Configuration: %CONFIGURATION%
if "%1"=="C" (
    setlocal enableDelayedExpansion
    call ./build-system/build-examples/build-c-examples/build-c-examples.bat
    if not !errorlevel! == 0 goto FAIL
    endlocal
    goto :eof
)
if "%1"=="C#" (
    setlocal enableDelayedExpansion
    call ./build-system/build-examples/build-csharp-examples/build-csharp-examples.bat
    if not !errorlevel! == 0 goto FAIL
    endlocal
    goto :eof
)
if "%1"=="VB.NET" (
    setlocal enableDelayedExpansion
    call ./build-system/build-examples/build-vbnet-examples/build-vbnet-examples.bat
    if not !errorlevel! == 0 goto FAIL
    endlocal
    goto :eof
)
if "%1"=="Java" (
    setlocal enableDelayedExpansion
    call ./build-system/build-examples/build-java-examples/build-java-examples.bat
    if not !errorlevel! == 0 goto FAIL
    endlocal
    goto :eof
)
if "%1"=="" (
    setlocal enableDelayedExpansion
    call ./build-system/build-examples/build-c-examples/build-c-examples.bat
    if not !errorlevel! == 0 goto FAIL
    call ./build-system/build-examples/build-csharp-examples/build-csharp-examples.bat
    if not !errorlevel! == 0 goto FAIL
    call ./build-system/build-examples/build-vbnet-examples/build-vbnet-examples.bat
    if not !errorlevel! == 0 goto FAIL
    call ./build-system/build-examples/build-java-examples/build-java-examples.bat
    if not !errorlevel! == 0 goto FAIL
    echo ****************************************************
    echo *     BUILDING LIBXIMC's EXAMPLES IS COMPLETED     *
    echo ****************************************************
    endlocal
    goto :eof
)
if "%1"=="--help" (
    echo "Usage: %SCRIPT_NAME% %STAGE_NAME% [ C | C# | VB.NET | Java | clean ]"
    echo "To build all dependencies just use %SCRIPT_NAME% %STAGE_NAME%."
    echo:
    echo "OPTIONS:"
    echo "  C      - C example"
    echo "  C#     - C# example"
    echo "  VB.NET - VB.NET example"
    echo "  Java   - Java example"
    echo "  clean  - remove all built examples"
    echo ""
    goto :eof
)
echo "Unknown command %1."
echo "Usage: %SCRIPT_NAME% %STAGE_NAME% [ C | C# | VB.NET | Java | clean ]"
echo "To build all dependencies just use %SCRIPT_NAME% %STAGE_NAME%."
echo:
echo "OPTIONS:"
echo "  C      - C example"
echo "  C#     - C# example"
echo "  VB.NET - VB.NET example"
echo "  Java   - Java example"
echo "  clean  - remove all built examples"
echo ""
goto FAIL



:CLEAN
echo ****************************************************
echo *                  CLEAN EXAMPLES                  *
echo ****************************************************
call ./build-system/build-examples/build-c-examples/build-c-examples.bat           clean
call ./build-system/build-examples/build-csharp-examples/build-csharp-examples.bat clean
call ./build-system/build-examples/build-vbnet-examples/build-vbnet-examples.bat   clean
call ./build-system/build-examples/build-java-examples/build-java-examples.bat     clean
if exist %DISTDIR%\examples rmdir %DISTDIR%\examples /s/q
echo ****************************************************
echo *                       DONE                       *
echo ****************************************************
goto :eof


:FAIL
echo FAIL
exit /B 1
