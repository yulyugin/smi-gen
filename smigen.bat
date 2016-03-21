echo off

SC STOP smigen
SC DELETE smigen
SC CREATE smigen binPath= "%~dp0build-win7-fre\amd64\smigen.sys" type= kernel depend= EventLog
SC START smigen

IF ERRORLEVEL 1 ECHO *** Failed to load smigen kernel driver ***
IF ERRORLEVEL 1 GOTO EXIT

smigen.exe

:EXIT
