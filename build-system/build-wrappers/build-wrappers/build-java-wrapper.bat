@echo off
if "%1"=="clean" (
    call :CLEAN
    goto :eof
)

echo ----------------------------------------------------
echo:
echo                BUILDING JAVA WRAPPER
echo:
echo ----------------------------------------------------

echo ----------------------------------------------------
echo                  System checking...
echo ----------------------------------------------------
echo Checking JDK...
if "x%JDK_HOME%"=="x" (
    echo ERROR: JDK_HOME environment variable is not set!
    goto :FAIL
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
    set JAVAH="%JDK_HOME%\bin\javah"
)

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

call :WRAPPER_JAVA win32 Win32 "%JDK32_HOME%"
if not %errorlevel% == 0 goto FAIL
call :WRAPPER_JAVA win64 x64 "%JDK_HOME%"
if not %errorlevel% == 0 goto FAIL
echo ----------------------------------------------------
echo          BUILDING JAVA WRAPPER IS COMPLETED
echo ----------------------------------------------------
echo Output:
echo * %DISTDIR%\libjximc.jar
echo * %DISTDIR%\libjximc.dll
echo ----------------------------------------------------
goto :eof


rem The code below defines functions
:CLEAN
echo Cleaning Java wrapper...
if exist %DISTDIR%\ximc\win64\wrappers\java rmdir /s/q %DISTDIR%\ximc\win64\wrappers\java
if exist %DISTDIR%\ximc\win32\wrappers\java rmdir /s/q %DISTDIR%\ximc\win32\wrappers\java
where /q git
if not %errorlevel% == 0 (
    echo WARNING: git cannot be found! Cleaning won't be complete.
) else (
    git clean -xdf %BASEDIR%\wrappers\java
)
goto :eof


:WRAPPER_JAVA
set DISTARCH=%DISTDIR%\ximc\%1
set ARCH=%2
set JPATH=%3
echo:
echo +++++++++++++ Generating java wrapper for %ARCH%
set GENDIR=wrappers\java\gen
set BINDIR=%CONFIGURATION%-%ARCH%\

if not exist %GENDIR% mkdir %GENDIR%

%XIGEN% --gen-java -x version -i "%LIBXIMC_SRC_DIR%\protocol.xi" -o wrappers\java\src\java\ru\ximc\libximc\JXimc.java -t wrappers\java\src\java\\ru\ximc\libximc\JXimc-template.java
if not %errorlevel% == 0 goto FAIL

%JAVAC% -Xlint -d wrappers\java wrappers\java\src\java\ru\ximc\libximc\JXimc.java wrappers\java\src\java\ru\ximc\libximc\XimcError.java wrappers\java\src\java\ru\ximc\libximc\XimcNoDevice.java wrappers\java\src\java\ru\ximc\libximc\XimcNotImplemented.java wrappers\java\src\java\ru\ximc\libximc\XimcValueError.java
if not %errorlevel% == 0 goto FAIL
%JAR% cf wrappers\java\libjximc.jar -C wrappers\java ru 
if not %errorlevel% == 0 goto FAIL
%JAVAH% -classpath wrappers\java\libjximc.jar -jni -d %GENDIR% ru.ximc.libximc.JXimc
if not %errorlevel% == 0 goto FAIL

%XIGEN% --gen-jni -x version -i "%LIBXIMC_SRC_DIR%\protocol.xi" -o wrappers\java\src\c\ru_ximc_libximc_JXimc-gen.c -t wrappers\java\src\c\ru_ximc_libximc_JXimc-template.c
if not %errorlevel% == 0 goto FAIL

%MSBUILD% libximc.sln /p:Configuration=%CONFIGURATION% /p:Platform=%ARCH% /t:libjximc
if not %errorlevel% == 0 goto FAIL

if not exist %DISTARCH% mkdir %DISTARCH%
copy wrappers\java\libjximc.jar %DISTARCH%
if not %errorlevel% == 0 goto FAIL
copy %BINDIR%\libjximc.dll      %DISTARCH%
if not %errorlevel% == 0 goto FAIL
goto :eof


:FAIL
echo FAIL
exit /B 1
