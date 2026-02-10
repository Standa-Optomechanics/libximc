@echo off
if "%1"=="clean" (
    call :CLEAN
    goto :eof
)

echo ----------------------------------------------------
echo:
echo             GENERATING LIBXIMC's SOURCES
echo:
echo ----------------------------------------------------

echo ----------------------------------------------------
echo                  System checking...
echo ----------------------------------------------------
echo Checking xigen...
if not exist %DEPSDIR%\win32\xigen\bin\xigen.exe (
    echo ERROR: xigen for win32 cannot be found!
    goto :FAIL
) else (
    echo ***** Done!
    set XIGEN=%DEPSDIR%\win32\xigen\bin\xigen.exe
    set XIGEN_SRC_DIR=%DEPSDIR%\win32\xigen\src
)
echo --------------- System checking done ---------------


call :BUILD_SOURCES
if not %errorlevel% == 0 goto :FAIL
echo ----------------------------------------------------
echo       GENERATING LIBXIMC's SOURCES IS COMPLETED
echo ----------------------------------------------------
echo Output:
echo * %LIBXIMC_INC_DIR%\ximc.h
echo * %LIBXIMC_SRC_DIR%\ximc-gen.h
echo * %LIBXIMC_SRC_DIR%\ximc-gen.c
echo * %LIBXIMC_SRC_DIR%\libximc.def
echo * %LIBXIMC_SRC_DIR%\fwprotocol.h
echo * %LIBXIMC_SRC_DIR%\fwprotocol.c
echo ----------------------------------------------------
goto :eof


rem The code below defines functions
:CLEAN
echo Cleaning libximc's generated sources...
where /q git
if not %errorlevel% == 0 (
    echo WARNING: git cannot be found! Cleaning won't be done
) else (
    git clean -xdf %BASEDIR%\libximc
)
goto :eof


:BUILD_SOURCES
echo ++++++++++ Generating libximc's sources...
%XIGEN% --gen-header          -x %XIMC_VERSION_FILE% -i "%LIBXIMC_SRC_DIR%\protocol.xi" -o "%LIBXIMC_INC_DIR%\ximc.h"       -t "%LIBXIMC_SRC_DIR%\ximc-template.h"
%XIGEN% --gen-internal-header -x %XIMC_VERSION_FILE% -i "%LIBXIMC_SRC_DIR%\protocol.xi" -o "%LIBXIMC_SRC_DIR%\ximc-gen.h"   -t "%LIBXIMC_SRC_DIR%\ximc-gen-template.h"
%XIGEN% --gen-code            -x %XIMC_VERSION_FILE% -i "%LIBXIMC_SRC_DIR%\protocol.xi" -o "%LIBXIMC_SRC_DIR%\ximc-gen.c"   -t "%LIBXIMC_SRC_DIR%\ximc-gen-template.c"
%XIGEN% --gen-def             -x %XIMC_VERSION_FILE% -i "%LIBXIMC_SRC_DIR%\protocol.xi" -o "%LIBXIMC_SRC_DIR%\libximc.def"  -t "%LIBXIMC_SRC_DIR%\libximc-template.def"
%XIGEN% --gen-fw-header       -x %XIMC_VERSION_FILE% -i "%LIBXIMC_SRC_DIR%\protocol.xi" -o "%LIBXIMC_SRC_DIR%\fwprotocol.h" -t "%XIGEN_SRC_DIR%\fwprotocol-template.h"
%XIGEN% --gen-fw-lib          -x %XIMC_VERSION_FILE% -i "%LIBXIMC_SRC_DIR%\protocol.xi" -o "%LIBXIMC_SRC_DIR%\fwprotocol.c" -t "%XIGEN_SRC_DIR%\fwprotocol-template.c"
goto :eof


:FAIL
echo FAIL
exit /B 1
