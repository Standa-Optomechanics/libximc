@echo off
if "%1"=="clean" (
    call :CLEAN
    goto :eof
)

echo ----------------------------------------------------
echo:
echo                BUILDING MATLAB WRAPPER
echo:
echo ----------------------------------------------------

echo ----------------------------------------------------
echo                  System checking...
echo ----------------------------------------------------
echo Checking matlab.exe...
where /q matlab.exe
if not %errorlevel% == 0 (
    echo ERROR: matlab.exe cannot be found! Check its installation and PATH.
    goto FAIL
) else (
    echo ***** Done!
)
echo --------------- System checking done ---------------

call :WRAPPER_MATLAB
if not %errorlevel% == 0 goto FAIL
echo ----------------------------------------------------
echo         BUILDING MATLAB WRAPPER IS COMPLETED
echo ----------------------------------------------------
echo Output:
echo * %DISTDIR%\ximc\win64\wrappers\matlab
echo ----------------------------------------------------
goto :eof


rem The code below defines functions
:CLEAN
echo Cleaning Matlab wrapper...
if exist %DISTDIR%\ximc\win64\wrappers\matlab rmdir /s/q %DISTDIR%\ximc\win64\wrappers\java
goto :eof


:WRAPPER_MATLAB
echo:
echo +++++++++++++ Building matlab wrapper
set DISTARCH=%DISTDIR%\ximc\win64
set BINDIR=%DISTARCH%\wrappers\matlab

if not exist %BINDIR% mkdir %BINDIR%
if not %errorlevel% == 0 goto FAIL

copy %DISTARCH%\bindy.dll     %BASEDIR%\wrappers\matlab\
copy %DISTARCH%\xiwrapper.dll %BASEDIR%\wrappers\matlab\
copy %DISTARCH%\libximc.dll   %BASEDIR%\wrappers\matlab\

if not %errorlevel% == 0 goto FAIL
cd %BASEDIR%\wrappers\matlab
matlab.exe -nodesktop -wait -r buildm -logfile buildm.log
set LASTERR=%errorlevel%
cd %BASEDIR%
if not %LASTERR% == 0 goto FAIL

move wrappers\matlab\ximcm.m %BINDIR%
if not %errorlevel% == 0 goto FAIL

move wrappers\matlab\libximc_thunk_pcwin64.dll %BINDIR%
if not %errorlevel% == 0 goto FAIL

copy wrappers\matlab\ximcm.h %BINDIR%
if not %errorlevel% == 0 goto FAIL
goto :eof


:FAIL
echo FAIL
exit /B 1
