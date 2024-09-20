@echo Using jenkins at %JENKINS_HOME%, node %NODE_NAME%
@echo Using parameters %NODE_LABELS%
@echo Use git revision %XIMC_REVISION%
@echo Use JDK %JDK_HOME%

@if "x%GIT%"=='x' set GIT=git
@if "x%JDK_HOME%"=='x' goto FAIL

set BASEDIR=%CD%
:: reset errorrevel
@cmd /C exit 0

:: Build hash could be specified by jenkins
@if 'x%XIMC_REVISION%' NEQ 'x' @set GIT_SUFFIX=%XIMC_REVISION%
@if 'x%SKIP_CLEAN_CHECKOUT%' NEQ 'x' goto SKIP_CLEAN_CHECKOUT
"%GIT%" checkout -f %GIT_SUFFIX%
@if not %errorlevel% == 0 goto FAIL

:SKIP_CLEAN_CHECKOUT
:: drop all non-added files
"%GIT%" clean -xdf
@if not %errorlevel% == 0 goto FAIL
@if exist %DISTDIR% rmdir /S /Q %DISTDIR%
@if not %errorlevel% == 0 goto FAIL

@echo Launching build itself
call build.bat
@if not %errorlevel% == 0 goto FAIL

@echo Packaging
@if "%NODE_NAME%" == "" goto SUCCESS
c:\cygwin\bin\bash.exe --login -c "cd `cygpath '%BASEDIR%'` && tar cf results-dist-%NODE_NAME%.tar ximc"
@if not %errorlevel% == 0 goto FAIL

:: it is an exit
:SUCCESS
@echo CI build succeeded
@goto :eof

:FAIL
@echo CI build failed
exit /B 1

