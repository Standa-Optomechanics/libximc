@echo off
rem Set paths
set NAME=test_C
set EXAMPLES_SRC_DIR=%BASEDIR%\examples\%NAME%
set EXAMPLES_DIST_DIR=%DISTDIR%\examples\%NAME%

if "%1"=="clean" (
    call :CLEAN
    goto :eof
)

echo ----------------------------------------------------
echo:
echo                 BUILDING C EXAMPLES
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

echo Checking mingw32...
set MINGW32_NOT_FOUND=false
if not exist C:\MSYS2\mingw32\bin\ set MINGW32_NOT_FOUND=true
if not exist C:\MSYS2\mingw64\bin\ set MINGW32_NOT_FOUND=true
if "%MINGW32_NOT_FOUND%"=="true" (
    setlocal enabledelayedexpansion
    echo WARRNING: mingw32 cannot be found. It must reside in "C:\MSYS2\mingw32\bin\" and 
    echo in "C:\MSYS2\mingw64\bin\" for Win32 and x64 respectively. So, we try to use a kludge:
    echo search binaries in "C:\Program Files (x86)\mingw-w64\i686-7.3.0-posix-dwarf-rt_v5-rev0\mingw32\bin"
    echo and "C:\Program Files\mingw-w64\x86_64-7.3.0-posix-seh-rt_v5-rev0\mingw64\bin".
    set MINGW32_NOT_FOUND=false
    if not exist "C:\Program Files (x86)\mingw-w64\i686-7.3.0-posix-dwarf-rt_v5-rev0\mingw32\bin" set MINGW32_NOT_FOUND=true
    if not exist "C:\Program Files\mingw-w64\x86_64-7.3.0-posix-seh-rt_v5-rev0\mingw64\bin" set MINGW32_NOT_FOUND=true
    if "!MINGW32_NOT_FOUND!"=="true" (
        echo ERROR: even the kludge doesn't work...
        goto FAIL
    )
    endlocal
    set "MINGW32=C:\Program Files (x86)\mingw-w64\i686-7.3.0-posix-dwarf-rt_v5-rev0\mingw32\bin"
    set "MINGW64=C:\Program Files\mingw-w64\x86_64-7.3.0-posix-seh-rt_v5-rev0\mingw64\bin"
) else (
    echo ***** Done!
    set MINGW32=C:\MSYS2\mingw32\bin\
    set MINGW64=C:\MSYS2\mingw64\bin\
)

echo Checking CodeBlocks...
if not exist "C:\Program Files (x86)\CodeBlocks\cbp2make.exe" (
    echo ERROR: CodeBlocks cannot be found! Its location must be "C:\Program Files (x86)\CodeBlocks\cbp2make.exe"
    goto FAIL
) else (
    echo ***** Done!
    set CBP2MAKE="C:\Program Files (x86)\CodeBlocks\cbp2make.exe"
)

echo Checking libximc.dll...
set LIBXIMC_DLL_NOT_FOUND=false
if not exist %DISTDIR%\ximc\win32\libximc.dll set LIBXIMC_DLL_NOT_FOUND=true
if not exist %DISTDIR%\ximc\win64\libximc.dll set LIBXIMC_DLL_NOT_FOUND=true
if "%LIBXIMC_DLL_NOT_FOUND%"=="true" (
    echo ERROR: libximc.dll cannot be found! It must reside in "%DISTDIR%\ximc\win32\"
    echo and in "%DISTDIR%\ximc\win64\".
    goto FAIL
) else (
    echo ***** Done!
    set LIBXIMC_DLL_32=%DISTDIR%\ximc\win32\libximc.dll
    set LIBXIMC_DLL_64=%DISTDIR%\ximc\win64\libximc.dll
)

echo Checking bindy.dll...
set BINDY_DLL_NOT_FOUND=false
if not exist %DISTDIR%\ximc\win32\bindy.dll set BINDY_DLL_NOT_FOUND=true
if not exist %DISTDIR%\ximc\win64\bindy.dll set BINDY_DLL_NOT_FOUND=true
if "%BINDY_DLL_NOT_FOUND%"=="true" (
    echo ERROR: bindy.dll cannot be found! It must reside in "%DISTDIR%\ximc\win32\"
    echo and in "%DISTDIR%\ximc\win64\".
    goto FAIL
) else (
    echo ***** Done!
    set BINDY_DLL_32=%DISTDIR%\ximc\win32\bindy.dll
    set BINDY_DLL_64=%DISTDIR%\ximc\win64\bindy.dll
)

echo Checking xiwrapper.dll...
set XIWRAPPER_DLL_NOT_FOUND=false
if not exist %DISTDIR%\ximc\win32\xiwrapper.dll set XIWRAPPER_DLL_NOT_FOUND=true
if not exist %DISTDIR%\ximc\win64\xiwrapper.dll set XIWRAPPER_DLL_NOT_FOUND=true
if "%XIWRAPPER_DLL_NOT_FOUND%"=="true" (
    echo ERROR: xiwrapper.dll cannot be found! It must reside in "%DISTDIR%\ximc\win32\"
    echo and in "%DISTDIR%\ximc\win64\".
    goto FAIL
) else (
    echo ***** Done!
    set XIWRAPPER_DLL_32=%DISTDIR%\ximc\win32\xiwrapper.dll
    set XIWRAPPER_DLL_64=%DISTDIR%\ximc\win64\xiwrapper.dll
)

echo Checking libximc.lib...
set LIBXIMC_LIB_NOT_FOUND=false
if not exist %DISTDIR%\ximc\win32\libximc.lib set LIBXIMC_LIB_NOT_FOUND=true
if not exist %DISTDIR%\ximc\win64\libximc.lib set LIBXIMC_LIB_NOT_FOUND=true
if "%LIBXIMC_LIB_NOT_FOUND%"=="true" (
    echo ERROR: libximc.lib cannot be found! It must reside in "%DISTDIR%\ximc\win32\"
    echo and in "%DISTDIR%\ximc\win64\".
    goto FAIL
) else (
    echo ***** Done!
    set LIBXIMC_LIB_32=%DISTDIR%\ximc\win32\libximc.lib
    set LIBXIMC_LIB_64=%DISTDIR%\ximc\win64\libximc.lib
)

echo Checking ximc.h...
set XIMC_H_NOT_FOUND=false
if not exist %DISTDIR%\ximc\win32\ximc.h set XIMC_H_NOT_FOUND=true
if not exist %DISTDIR%\ximc\win64\ximc.h set XIMC_H_NOT_FOUND=true
if "%XIMC_H_NOT_DOUND%"=="true" (
    echo ERROR: ximc.h cannot be found! It must reside in "%DISTDIR%\ximc\win32\"
    echo and in "%DISTDIR%\ximc\win64\" for Win32 and x64 respectively.
    goto FAIL
) else (
    echo ***** Done!
    set XIMC_H_32=%DISTDIR%\ximc\win32\ximc.h
    set XIMC_H_64=%DISTDIR%\ximc\win64\ximc.h
)
echo --------------- System checking done ---------------

call :BUILD_TESTAPP_C_EXAMPLE
if not %errorlevel% == 0 goto FAIL
call :BUILD_TESTAPPEASY_C_EXAMPLE
if not %errorlevel% == 0 goto FAIL
call :BUILD_TESTPROFILE_C_EXAMPLE
if not %errorlevel% == 0 goto FAIL
echo ----------------------------------------------------
echo           BUILDING C EXAMPLES IS COMPLETED
echo ----------------------------------------------------
echo Output:
echo %EXAMPLES_DIST_DIR%
echo ----------------------------------------------------
goto :eof


rem The code below defines functions
:CLEAN
echo Cleaning C examples...
where /q git
if not %errorlevel% == 0 (
    echo WARNING: git cannot be found! Cleaning won't be complete.
) else (
    git clean -xdf %EXAMPLES_SRC_DIR%
)
if not exist %EXAMPLES_DIST_DIR%\ goto :eof
rmdir %EXAMPLES_DIST_DIR% /s/q
goto :eof


:BUILD_TESTAPP_C_EXAMPLE
setlocal
set SUBNAME=testapp_C
echo Building example %SUBNAME% with VS...

echo +++++++++++++ Building example %SUBNAME% for Win32 (VS)
mkdir %EXAMPLES_SRC_DIR%\%SUBNAME%\win32
copy %LIBXIMC_DLL_32%   %EXAMPLES_SRC_DIR%\%SUBNAME%\win32
copy %LIBXIMC_LIB_32%   %EXAMPLES_SRC_DIR%\%SUBNAME%\win32
copy %BINDY_DLL_32%     %EXAMPLES_SRC_DIR%\%SUBNAME%\win32
copy %XIWRAPPER_DLL_32% %EXAMPLES_SRC_DIR%\%SUBNAME%\win32
copy %XIMC_H_32%        %EXAMPLES_SRC_DIR%\%SUBNAME%\win32
%MSBUILD% %EXAMPLES_SRC_DIR%\%SUBNAME%\%SUBNAME%.sln /p:Configuration=%CONFIGURATION% /p:Platform=Win32
if not %errorlevel% == 0 goto FAIL
mkdir %EXAMPLES_DIST_DIR%\%SUBNAME%\compiled-win32
copy %EXAMPLES_SRC_DIR%\%SUBNAME%\compiled-win32\*       %EXAMPLES_DIST_DIR%\%SUBNAME%\compiled-win32\*
copy %EXAMPLES_SRC_DIR%\%SUBNAME%\Corr_table_example.tbl %EXAMPLES_DIST_DIR%\%SUBNAME%\compiled-win32\Corr_table_example.tbl
if not %errorlevel% == 0 goto FAIL

echo +++++++++++++ Building example %SUBNAME% for x64 (VS)
mkdir %EXAMPLES_SRC_DIR%\%SUBNAME%\win64
copy %LIBXIMC_DLL_64%   %EXAMPLES_SRC_DIR%\%SUBNAME%\win64
copy %LIBXIMC_LIB_64%   %EXAMPLES_SRC_DIR%\%SUBNAME%\win64
copy %BINDY_DLL_64%     %EXAMPLES_SRC_DIR%\%SUBNAME%\win64
copy %XIWRAPPER_DLL_64% %EXAMPLES_SRC_DIR%\%SUBNAME%\win64
copy %XIMC_H_64%        %EXAMPLES_SRC_DIR%\%SUBNAME%\win64
%MSBUILD% %EXAMPLES_SRC_DIR%\%SUBNAME%\%SUBNAME%.sln /p:Configuration=%CONFIGURATION% /p:Platform=x64
if not %errorlevel% == 0 goto FAIL
mkdir %EXAMPLES_DIST_DIR%\%SUBNAME%\compiled-win64
copy %EXAMPLES_SRC_DIR%\%SUBNAME%\compiled-win64\*       %EXAMPLES_DIST_DIR%\%SUBNAME%\compiled-win64\*
copy %EXAMPLES_SRC_DIR%\%SUBNAME%\Corr_table_example.tbl %EXAMPLES_DIST_DIR%\%SUBNAME%\compiled-win64\Corr_table_example.tbl
if not %errorlevel% == 0 goto FAIL

rem Remove debug files produced by VS
del %EXAMPLES_DIST_DIR%\%SUBNAME%\compiled-win32\%SUBNAME%.pdb
del %EXAMPLES_DIST_DIR%\%SUBNAME%\compiled-win64\%SUBNAME%.pdb

echo +++++++++++++ Building example %SUBNAME% for Win32 (CodeBlocks)
%CBP2MAKE% -in %EXAMPLES_SRC_DIR%\%SUBNAME%\%SUBNAME%.cbp -out %EXAMPLES_SRC_DIR%\%SUBNAME%\makefile -windows -targets "win32" 
if not %errorlevel% == 0 goto FAIL
set PATH=%MINGW32%;%PATH%
mingw32-make --directory %EXAMPLES_SRC_DIR%\%SUBNAME%
if not %errorlevel% == 0 goto FAIL
del %EXAMPLES_SRC_DIR%\%SUBNAME%\cb_obj /Q
mkdir %EXAMPLES_DIST_DIR%\%SUBNAME%\cb_compiled-win32
copy %EXAMPLES_SRC_DIR%\%SUBNAME%\cb_compiled-win32\*    %EXAMPLES_DIST_DIR%\%SUBNAME%\cb_compiled-win32\*
copy %EXAMPLES_SRC_DIR%\%SUBNAME%\Corr_table_example.tbl %EXAMPLES_DIST_DIR%\%SUBNAME%\cb_compiled-win32\Corr_table_example.tbl
if not %errorlevel% == 0 goto FAIL

echo +++++++++++++ Building example %SUBNAME% for x64 (CodeBlocks)
%CBP2MAKE% -in %EXAMPLES_SRC_DIR%\%SUBNAME%\%SUBNAME%.cbp -out %EXAMPLES_SRC_DIR%\%SUBNAME%\makefile -windows -targets "win64"
if not %errorlevel% == 0 goto FAIL
set PATH=%MINGW64%;%PATH%
mingw32-make --directory %EXAMPLES_SRC_DIR%\%SUBNAME%
if not %errorlevel% == 0 goto FAIL
del %EXAMPLES_SRC_DIR%\%SUBNAME%\cb_obj /Q
mkdir %EXAMPLES_DIST_DIR%\%SUBNAME%\cb_compiled-win64
copy %EXAMPLES_SRC_DIR%\%SUBNAME%\cb_compiled-win64\*    %EXAMPLES_DIST_DIR%\%SUBNAME%\cb_compiled-win64\*
copy %EXAMPLES_SRC_DIR%\%SUBNAME%\Corr_table_example.tbl %EXAMPLES_DIST_DIR%\%SUBNAME%\cb_compiled-win64\Corr_table_example.tbl
if not %errorlevel% == 0 goto FAIL
endlocal
goto :eof


:BUILD_TESTAPPEASY_C_EXAMPLE
setlocal
set SUBNAME=testappeasy_C

echo +++++++++++++ Building example %SUBNAME% for Win32 (VS)
mkdir %EXAMPLES_SRC_DIR%\%SUBNAME%\win32
copy %LIBXIMC_DLL_32%   %EXAMPLES_SRC_DIR%\%SUBNAME%\win32
copy %LIBXIMC_LIB_32%   %EXAMPLES_SRC_DIR%\%SUBNAME%\win32
copy %BINDY_DLL_32%     %EXAMPLES_SRC_DIR%\%SUBNAME%\win32
copy %XIWRAPPER_DLL_32% %EXAMPLES_SRC_DIR%\%SUBNAME%\win32
copy %XIMC_H_32%        %EXAMPLES_SRC_DIR%\%SUBNAME%\win32
%MSBUILD% %EXAMPLES_SRC_DIR%\%SUBNAME%\%SUBNAME%.sln /p:Configuration=%CONFIGURATION% /p:Platform=Win32
if not %errorlevel% == 0 goto FAIL
mkdir %EXAMPLES_DIST_DIR%\%SUBNAME%\compiled-win32
copy %EXAMPLES_SRC_DIR%\%SUBNAME%\compiled-win32\* %EXAMPLES_DIST_DIR%\%SUBNAME%\compiled-win32\*
if not %errorlevel% == 0 goto FAIL

echo +++++++++++++ Building example %SUBNAME% for x64 (VS)
mkdir %EXAMPLES_SRC_DIR%\%SUBNAME%\win64
copy %LIBXIMC_DLL_64%   %EXAMPLES_SRC_DIR%\%SUBNAME%\win64
copy %LIBXIMC_LIB_64%   %EXAMPLES_SRC_DIR%\%SUBNAME%\win64
copy %BINDY_DLL_64%     %EXAMPLES_SRC_DIR%\%SUBNAME%\win64
copy %XIWRAPPER_DLL_64% %EXAMPLES_SRC_DIR%\%SUBNAME%\win64
copy %XIMC_H_64%        %EXAMPLES_SRC_DIR%\%SUBNAME%\win64
%MSBUILD% %EXAMPLES_SRC_DIR%\%SUBNAME%\%SUBNAME%.sln /p:Configuration=%CONFIGURATION% /p:Platform=x64
if not %errorlevel% == 0 goto FAIL
mkdir %EXAMPLES_DIST_DIR%\%SUBNAME%\compiled-win64
copy %EXAMPLES_SRC_DIR%\%SUBNAME%\compiled-win64\* %EXAMPLES_DIST_DIR%\%SUBNAME%\compiled-win64\*
if not %errorlevel% == 0 goto FAIL

rem Remove debug files produced by VS
del %EXAMPLES_DIST_DIR%\%SUBNAME%\compiled-win32\%SUBNAME%.pdb
del %EXAMPLES_DIST_DIR%\%SUBNAME%\compiled-win64\%SUBNAME%.pdb

echo +++++++++++++ Building example %SUBNAME% for Win32 (CodeBlocks)
set PATH=%MINGW32%;%PATH%
%CBP2MAKE% -in %EXAMPLES_SRC_DIR%\%SUBNAME%\%SUBNAME%.cbp -out %EXAMPLES_SRC_DIR%\%SUBNAME%\makefile -windows -targets "win32" 
if not %errorlevel% == 0 goto FAIL
mingw32-make --directory %EXAMPLES_SRC_DIR%\%SUBNAME%
if not %errorlevel% == 0 goto FAIL
del %EXAMPLES_SRC_DIR%\%SUBNAME%\cb_obj /Q
mkdir %EXAMPLES_DIST_DIR%\%SUBNAME%\cb_compiled-win32
copy %EXAMPLES_SRC_DIR%\%SUBNAME%\cb_compiled-win32\* %EXAMPLES_DIST_DIR%\%SUBNAME%\cb_compiled-win32\*
if not %errorlevel% == 0 goto FAIL

echo +++++++++++++ Building example %SUBNAME% for x64 (CodeBlocks)
set PATH=%MINGW64%;%PATH%
%CBP2MAKE% -in %EXAMPLES_SRC_DIR%\%SUBNAME%\%SUBNAME%.cbp -out %EXAMPLES_SRC_DIR%\%SUBNAME%\makefile -windows -targets "win64"
if not %errorlevel% == 0 goto FAIL
mingw32-make --directory %EXAMPLES_SRC_DIR%\%SUBNAME%
if not %errorlevel% == 0 goto FAIL
del %EXAMPLES_SRC_DIR%\%SUBNAME%\cb_obj /Q
mkdir %EXAMPLES_DIST_DIR%\%SUBNAME%\cb_compiled-win64
copy %EXAMPLES_SRC_DIR%\%SUBNAME%\cb_compiled-win64\* %EXAMPLES_DIST_DIR%\%SUBNAME%\cb_compiled-win64\*
if not %errorlevel% == 0 goto FAIL
endlocal
goto :eof


:BUILD_TESTPROFILE_C_EXAMPLE
setlocal
set SUBNAME=testprofile_C

echo +++++++++++++ Building example %SUBNAME% for Win32 (VS)
mkdir %EXAMPLES_SRC_DIR%\%SUBNAME%\win32
copy %LIBXIMC_DLL_32%   %EXAMPLES_SRC_DIR%\%SUBNAME%\win32
copy %LIBXIMC_LIB_32%   %EXAMPLES_SRC_DIR%\%SUBNAME%\win32
copy %BINDY_DLL_32%     %EXAMPLES_SRC_DIR%\%SUBNAME%\win32
copy %XIWRAPPER_DLL_32% %EXAMPLES_SRC_DIR%\%SUBNAME%\win32
copy %XIMC_H_32%        %EXAMPLES_SRC_DIR%\%SUBNAME%\win32
%MSBUILD% %EXAMPLES_SRC_DIR%\%SUBNAME%\%SUBNAME%.sln /p:Configuration=%CONFIGURATION% /p:Platform=Win32
if not %errorlevel% == 0 goto FAIL
mkdir %EXAMPLES_DIST_DIR%\%SUBNAME%\compiled-win32
copy %EXAMPLES_SRC_DIR%\%SUBNAME%\compiled-win32\* %EXAMPLES_DIST_DIR%\%SUBNAME%\compiled-win32\*
if not %errorlevel% == 0 goto FAIL

echo +++++++++++++ Building example %SUBNAME% for x64 (VS)
mkdir %EXAMPLES_SRC_DIR%\%SUBNAME%\win64
copy %LIBXIMC_DLL_64%   %EXAMPLES_SRC_DIR%\%SUBNAME%\win64
copy %LIBXIMC_LIB_64%   %EXAMPLES_SRC_DIR%\%SUBNAME%\win64
copy %BINDY_DLL_64%     %EXAMPLES_SRC_DIR%\%SUBNAME%\win64
copy %XIWRAPPER_DLL_64% %EXAMPLES_SRC_DIR%\%SUBNAME%\win64
copy %XIMC_H_64%        %EXAMPLES_SRC_DIR%\%SUBNAME%\win64
%MSBUILD% %EXAMPLES_SRC_DIR%\%SUBNAME%\%SUBNAME%.sln /p:Configuration=%CONFIGURATION% /p:Platform=x64
if not %errorlevel% == 0 goto FAIL
mkdir %EXAMPLES_DIST_DIR%\%SUBNAME%\compiled-win64
copy %EXAMPLES_SRC_DIR%\%SUBNAME%\compiled-win64\* %EXAMPLES_DIST_DIR%\%SUBNAME%\compiled-win64\*
if not %errorlevel% == 0 goto FAIL

rem Remove debug files produced by VS
del %EXAMPLES_DIST_DIR%\%SUBNAME%\compiled-win32\testprofile_C.pdb
del %EXAMPLES_DIST_DIR%\%SUBNAME%\compiled-win64\testprofile_C.pdb
endlocal
goto :eof

:FAIL
echo FAIL
exit /B 1
