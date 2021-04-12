@echo off

SETLOCAL

ECHO **************************************
ECHO *     %DATE%-%TIME%     *
ECHO **************************************

IF EXIST ..\logs\*.log DEL ..\logs\*.log

IF EXIST *.dll DEL *.dll
IF EXIST *.exe DEL *.exe
IF EXIST *.exp DEL *.exp
IF EXIST *.lib DEL *.lib
IF EXIST *.pdb DEL *.pdb
IF EXIST *.obj DEL *.obj

ECHO:
ECHO ----------------------------
ECHO     Compiling
ECHO ----------------------------

rem compile all with matched extension (*.c, *.cpp, *.cxx)
IF EXIST ..\src\*.c   CALL cl ..\src\*.c
IF EXIST ..\src\*.cpp CALL cl ..\src\*.cpp
IF EXIST ..\src\*.cxx CALL cl ..\src\*.cxx

ECHO:
ECHO ----------------------------
ECHO     Linking
ECHO ----------------------------

CALL uflink UFDLL=HssDxfExport.dll *.obj

ECHO:
ECHO ----------------------------
ECHO     Done
ECHO ----------------------------

ENDLOCAL

@echo on
