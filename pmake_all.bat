@echo off

CLS

ECHO **************************************
ECHO *     %DATE%-%TIME%     *
ECHO **************************************

GOTO clean

:clean

    @REM IF EXIST *.dll DEL *.dll
    @REM IF EXIST *.exe DEL *.exe

    IF EXIST *.dll DEL %~n1.dll
    IF EXIST *.exe DEL %~n1.exe
    IF EXIST *.exp DEL *.exp
    IF EXIST *.lib DEL *.lib
    IF EXIST *.pdb DEL *.pdb
    IF EXIST *.obj DEL *.obj

    GOTO compile

:compile
    
    ECHO:
    ECHO ----------------------------
    ECHO     Compiling
    ECHO ----------------------------

    rem compile all with matched extension (*.c, *.cpp, *.cxx)
    IF EXIST *.cxx CALL cl *.cxx
    IF EXIST *.cpp CALL cl *.cpp
    IF EXIST *.c   CALL cl *.c

    GOTO link

:link
    
    ECHO:
    ECHO ----------------------------
    ECHO     Linking
    ECHO ----------------------------

    CALL uflink UFDLL=%~n1.dll *.obj

@echo on