@echo off
set FILE="%1"
for /F "delims=" %%i in (%FILE%) do set basename="%%~ni"

set JAVA_HOME_TEMP=%3

set "JAVA_HOME=%JAVA_HOME_TEMP:"=%"

call ballerina build --dump-bir-file=%basename%-bir-dump %FILE%
%2.exe %basename%-bir-dump -o %basename%-bir-dump.ll

clang -o %basename%.exe %basename%-bir-dump.ll -L..\..\..\runtime\target\release

SET LD_LIBRARY_PATH=..\..\..\runtime\target\release

%basename%.exe

echo RETVAL=%errorlevel%

del %basename%.exe %basename%-bir-dump.ll %basename%.jar %basename%-bir-dump
