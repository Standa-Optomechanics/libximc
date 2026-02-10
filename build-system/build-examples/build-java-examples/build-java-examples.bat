@echo off
rem Set paths
set NAME=test_Java
set EXAMPLES_SRC_DIR=%BASEDIR%\examples\%NAME%
set EXAMPLES_DIST_DIR=%DISTDIR%\examples\%NAME%

if "%1"=="clean" (
    call :CLEAN
    goto :eof
)

echo ----------------------------------------------------
echo:
echo                BUILDING JAVA EXAMPLE
echo:
echo ----------------------------------------------------

echo ----------------------------------------------------
echo                  System checking...
echo ----------------------------------------------------
echo Checking JDK...
if "x%JDK_HOME%"=="x" (
    echo ERROR: JDK_HOME environment variable is not set!
    goto FAIL
) else (
    set FILES_NOT_FOUND=false
    if not exist "%JDK_HOME%\bin\jar.exe" set FILES_NOT_FOUND=true
    if not exist "%JDK_HOME%\bin\javac.exe" set FILES_NOT_FOUND=true
    if "%FILES_NOT_FOUND%"=="true" (
        echo ERROR: Either "%JDK_HOME%\bin\javac" or "%JDK_HOME%\bin\jar" cannot be found!
        goto FAIL
    )
    echo ***** Done!
    set JAR="%JDK_HOME%\bin\jar"
    set JAVAC="%JDK_HOME%\bin\javac"
)

echo Checking libjximc.jar...
set LIBJXIMC_JAR_NOT_FOUND=false
if not exist %DISTDIR%\ximc\win32\libjximc.jar set LIBJXIMC_JAR_NOT_FOUND=true
if not exist %DISTDIR%\ximc\win64\libjximc.jar set LIBJXIMC_JAR_NOT_FOUND=true
if "%LIBJXIMC_JAR_NOT_FOUND%"=="true" (
    echo ERROR: libjximc.jar cannot be found. It must reside in "%DISTDIR%\ximc\win32\libjximc.jar" and
    echo in "%DISTDIR%\ximc\win64\libjximc.jar" for Win32 and x64 build configurations respectively.
    goto FAIL
) else (
    echo ***** Done!
    set LIBJXIMC_JAR_32=%DISTDIR%\ximc\win32\libjximc.jar
    set LIBJXIMC_DLL_32=%DISTDIR%\ximc\win32\libjximc.dll
    set LIBJXIMC_JAR_64=%DISTDIR%\ximc\win64\libjximc.jar
    set LIBJXIMC_DLL_64=%DISTDIR%\ximc\win64\libjximc.dll
)

echo Checking libximc...
set LIBXIMC_NOT_FOUND=false
if not exist %DISTDIR%\ximc\win32\libximc.dll set LIBXIMC_NOT_FOUND=true
if not exist %DISTDIR%\ximc\win32\libximc.lib set LIBXIMC_NOT_FOUND=true
if not exist %DISTDIR%\ximc\win64\libximc.dll set LIBXIMC_NOT_FOUND=true
if not exist %DISTDIR%\ximc\win64\libximc.lib set LIBXIMC_NOT_FOUND=true
if "%LIBXIMC_NOT_FOUND%"=="true" (
    echo ERROR: libximc.dll or libximc.lib cannot be found. It must reside in "%DISTDIR%\ximc\win32\" and
    echo in "%DISTDIR%\ximc\win64\" for Win32 and x64 build configurations respectively.
    goto FAIL
) else (
    echo ***** Done!
    set LIBXIMC_DLL_32=%DISTDIR%\ximc\win32\libximc.dll
    set LIBXIMC_LIB_32=%DISTDIR%\ximc\win32\libximc.lib
    set LIBXIMC_DLL_64=%DISTDIR%\ximc\win64\libximc.dll
    set LIBXIMC_LIB_64=%DISTDIR%\ximc\win64\libximc.lib
)

echo Checking xiwrapper.dll...
set XIWRAPPER_DLL_NOT_FOUND=false
if not exist %DISTDIR%\ximc\win32\xiwrapper.dll set XIWRAPPER_DLL_NOT_FOUND=true
if not exist %DISTDIR%\ximc\win64\xiwrapper.dll set XIWRAPPER_DLL_NOT_FOUND=true
if "%XIWRAPPER_DLL_NOT_FOUND%"=="true" (
    echo ERROR: xiwrapper.dll cannot be found. It must reside in "%DISTDIR%\ximc\win32\xiwrapper.dll" and
    echo in "%DISTDIR%\ximc\win64\xiwrapper.dll" for Win32 and x64 build configurations respectively.
    goto FAIL
) else (
    echo ***** Done!
    set XIWRAPPER_DLL_32=%DISTDIR%\ximc\win32\xiwrapper.dll
    set XIWRAPPER_DLL_64=%DISTDIR%\ximc\win64\xiwrapper.dll
)

echo Checking bindy...
set BINDY_NOT_FOUND=false
if not exist %DISTDIR%\ximc\win32\bindy.dll set BINDY_NOT_FOUND=true
if not exist %DISTDIR%\ximc\win64\bindy.dll set BINDY_NOT_FOUND=true
if not exist %DISTDIR%\ximc\win32\bindy.lib set BINDY_NOT_FOUND=true
if not exist %DISTDIR%\ximc\win64\bindy.lib set BINDY_NOT_FOUND=true
if "%BINDY_NOT_FOUND%"=="true" (
    echo ERROR: bindy.dll or bindy.lib cannot be found. It must reside in "%DISTDIR%\ximc\win32\" and
    echo in "%DISTDIR%\ximc\win64\" for Win32 and x64 build configurations respectively.
    goto FAIL
) else (
    echo ***** Done!
    set BINDY_DLL_32=%DISTDIR%\ximc\win32\bindy.dll
    set BINDY_LIB_32=%DISTDIR%\ximc\win32\bindy.lib
    set BINDY_DLL_64=%DISTDIR%\ximc\win64\bindy.dll
    set BINDY_LIB_64=%DISTDIR%\ximc\win64\bindy.lib
)
echo --------------- System checking done ---------------

call :BUILD_JAVA_EXAMPLE
if not %errorlevel% == 0 goto FAIL
copy %EXAMPLES_SRC_DIR%\README.txt      %EXAMPLES_DIST_DIR%
if not %errorlevel% == 0 goto FAIL
echo ----------------------------------------------------
echo          BUILDING JAVA EXAMPLE IS COMPLETED
echo ----------------------------------------------------
echo Output:
echo %EXAMPLES_DIST_DIR%
echo ----------------------------------------------------
goto :eof


rem The code below defines functions
:CLEAN
echo Cleaning Java example...
where /q git
if not %errorlevel% == 0 (
    echo WARNING: git cannot be found! Cleaning won't be complete.
) else (
    git clean -xdf %EXAMPLES_SRC_DIR%
)
if not exist %EXAMPLES_DIST_DIR%\ goto :eof
rmdir %EXAMPLES_DIST_DIR% /s/q
goto :eof


:BUILD_JAVA_EXAMPLE
echo +++++++++++++ Building example %NAME% for Win32
copy %LIBJXIMC_JAR_32% %EXAMPLES_SRC_DIR% /Y
%JAVAC% -Xlint -classpath %EXAMPLES_SRC_DIR%\libjximc.jar -d %EXAMPLES_SRC_DIR% %EXAMPLES_SRC_DIR%\ru\ximc\TestJava.java
if not %errorlevel% == 0 goto FAIL
%JAR% cmf %EXAMPLES_SRC_DIR%\MANIFEST.MF %EXAMPLES_SRC_DIR%\%NAME%.jar -C %EXAMPLES_SRC_DIR% ru
if not %errorlevel% == 0 goto FAIL
if not exist %EXAMPLES_DIST_DIR%\compiled-win32 mkdir %EXAMPLES_DIST_DIR%\compiled-win32
copy %EXAMPLES_SRC_DIR%\%NAME%.jar      %EXAMPLES_DIST_DIR%\compiled-win32
copy %LIBJXIMC_DLL_32%                  %EXAMPLES_DIST_DIR%\compiled-win32
copy %LIBJXIMC_JAR_32%                  %EXAMPLES_DIST_DIR%\compiled-win32
copy %LIBXIMC_DLL_32%                   %EXAMPLES_DIST_DIR%\compiled-win32
copy %LIBXIMC_LIB_32%                   %EXAMPLES_DIST_DIR%\compiled-win32
copy %BINDY_DLL_32%                     %EXAMPLES_DIST_DIR%\compiled-win32
copy %BINDY_LIB_32%                     %EXAMPLES_DIST_DIR%\compiled-win32
copy %XIWRAPPER_DLL_32%                 %EXAMPLES_DIST_DIR%\compiled-win32
copy %DISTDIR%\ximc\win32\libximc.def   %EXAMPLES_DIST_DIR%\compiled-win32
if not %errorlevel% == 0 goto FAIL

echo +++++++++++++ Building example %NAME% for x64
copy %LIBJXIMC_JAR_64% %EXAMPLES_SRC_DIR% /Y
%JAVAC% -Xlint -classpath %EXAMPLES_SRC_DIR%\libjximc.jar -d %EXAMPLES_SRC_DIR% %EXAMPLES_SRC_DIR%\ru\ximc\TestJava.java
if not %errorlevel% == 0 goto FAIL
%JAR% cmf %EXAMPLES_SRC_DIR%\MANIFEST.MF %EXAMPLES_SRC_DIR%\%NAME%.jar -C %EXAMPLES_SRC_DIR% ru 
if not %errorlevel% == 0 goto FAIL
if not exist %EXAMPLES_DIST_DIR%\compiled-win64 mkdir %EXAMPLES_DIST_DIR%\compiled-win64
copy %EXAMPLES_SRC_DIR%\%NAME%.jar      %EXAMPLES_DIST_DIR%\compiled-win64
copy %LIBJXIMC_DLL_64%                  %EXAMPLES_DIST_DIR%\compiled-win64
copy %LIBJXIMC_JAR_64%                  %EXAMPLES_DIST_DIR%\compiled-win64
copy %LIBXIMC_DLL_64%                   %EXAMPLES_DIST_DIR%\compiled-win64
copy %LIBXIMC_LIB_64%                   %EXAMPLES_DIST_DIR%\compiled-win64
copy %BINDY_DLL_64%                     %EXAMPLES_DIST_DIR%\compiled-win64
copy %BINDY_LIB_64%                     %EXAMPLES_DIST_DIR%\compiled-win64
copy %XIWRAPPER_DLL_64%                 %EXAMPLES_DIST_DIR%\compiled-win64
copy %DISTDIR%\ximc\win64\libximc.def   %EXAMPLES_DIST_DIR%\compiled-win64
if not %errorlevel% == 0 goto FAIL
goto :eof


:FAIL
echo FAIL
exit /B 1
