@echo off
rem reset errorlevel
cmd /C exit 0
set BASEDIR="%CD%"
set DISTDIR=%BASEDIR%\dist
set DEPSDIR=%BASEDIR%\deps
set LIBSDIR_x86=%BASEDIR%\ximc\win32
set LIBSDIR_x64=%BASEDIR%\ximc\win64
set LIBXIMC_SRC_DIR=%BASEDIR%\libximc\src
set LIBXIMC_INC_DIR=%BASEDIR%\libximc\include
set CONFIGURATION=Debug
if "%DEBUG%"=="true" goto :CONF_DEBUG
set CONFIGURATION=Release
:CONF_DEBUG

set SCRIPT_NAME=%0

if "%1"=="clean" (
    call :CLEAN
    goto :eof
)
if "%1"=="deps" (
    setlocal enableDelayedExpansion
    call ./build-system/build-dependencies/build-dependencies.bat %*
    if not !errorlevel! == 0 goto FAIL
    endlocal
    goto :eof    
)
if "%1"=="gen-libximc-sources" (
    setlocal enableDelayedExpansion
    call ./build-system/generate-libximc-sources/generate-libximc-sources.bat %*
    if not !errorlevel! == 0 goto FAIL
    endlocal
    goto :eof
)
if "%1"=="libximc" (
    setlocal enableDelayedExpansion
    call ./build-system/build-libximc/build-libximc.bat %*
    if not !errorlevel! == 0 goto FAIL
    endlocal
    goto :eof
)
if "%1"=="wrappers" (
    setlocal enableDelayedExpansion
    call ./build-system/build-wrappers/build-wrappers.bat %*
    if not !errorlevel! == 0 goto FAIL
    endlocal
    goto :eof
)
if "%1"=="examples" (
    setlocal enableDelayedExpansion
    call ./build-system/build-examples/build-examples.bat %*
    if not !errorlevel! == 0 goto FAIL
    endlocal
    goto :eof
)
if "%1"=="" (
    setlocal enableDelayedExpansion
    echo $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
    echo $                     BUILD ALL                    $
    echo $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

    echo Configuration: %CONFIGURATION%


    call ./build-system/build-dependencies/build-dependencies.bat
    if not !errorlevel! == 0 goto FAIL
    call ./build-system/generate-libximc-sources/generate-libximc-sources.bat
    if not !errorlevel! == 0 goto FAIL
    call ./build-system/build-libximc/build-libximc.bat
    if not !errorlevel! == 0 goto FAIL
    call ./build-system/build-wrappers/build-wrappers.bat
    if not !errorlevel! == 0 goto FAIL
    call ./build-system/build-examples/build-examples.bat
    if not !errorlevel! == 0 goto FAIL
    endlocal
    goto :eof
)
if "%1"=="--help" (
    echo Libximc build script.
    echo.
    call :PRINT_HELP
    goto :eof
)
echo Unknown command %1.
echo.
call :PRINT_HELP
goto FAIL


:CLEAN
echo $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
echo $                     CLEAN ALL                    $
echo $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
where /q git
if not %errorlevel% == 0 (
    echo WARNING: git cannot be found! Cleaning won't be complete.
) else (
    git clean -xdf
)
call ./build-system/build-dependencies/build-dependencies.bat             clean
call ./build-system/generate-libximc-sources/generate-libximc-sources.bat clean
call ./build-system/build-libximc/build-libximc.bat                       clean
call ./build-system/build-wrappers/build-wrappers.bat                     clean
call ./build-system/build-examples/build-examples.bat                     clean
echo $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
echo $                       DONE                       $
echo $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
goto :eof

:PRINT_HELP
echo Usage: %SCRIPT_NAME% ^[ clean ^| deps ^| gen-libximc-sources ^| libximc ^| wrappers ^| examples ^| --help ^]
echo For full build just use %SCRIPT_NAME%.
echo Use %SCRIPT_NAME% COMMAND --help to see detailed info.
echo.
echo COMMAND:
echo   clean               - full cleaning
echo   deps                - build dependencies
echo   gen-libximc-sources - generate libximc source code
echo   libximc             - build the library
echo   wrappers            - build wrappers
echo   examples            - build examples
echo.
goto :eof

:FAIL
echo FAIL
exit /B 1
