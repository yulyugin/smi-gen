echo off

IF NOT EXIST C:\WinDDK\7600.16385.1 GOTO UNKNOWN_WDK

call C:\WinDDK\7600.16385.1\bin\setenv.bat C:\WinDDK\7600.16385.1\ fre x64 WIN7 2>&1 > nul

cd /d %~dp0\smigen
build /F /w /c

cd /d %~dp0

gcc -Wall -Werror -Ismigen smigen.c -o smigen.exe

goto EXIT

:UNKNOWN_WDK
ECHO *** Can't find useful Windows Driver Kit. ***
ECHO *** Only Windows Driver Kit Version 7.1.0 is supported. ***

:EXIT
