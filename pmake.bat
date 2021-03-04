@echo off


SETLOCAL
SET linktype=exe

CLS

ECHO **************************************
ECHO *     %DATE%-%TIME%     *
ECHO **************************************

GOTO clean

:clean

    IF EXIST *.dll DEL *.dll
    IF EXIST *.exe DEL *.exe
    IF EXIST *.exp DEL *.exp
    IF EXIST *.lib DEL *.lib
    IF EXIST *.pdb DEL *.pdb
    IF EXIST *.obj DEL *.obj

    GOTO arg_parse

:arg_parse

    IF "%~1"=="/DLL" GOTO set_dll
    IF "%~1"=="/dll" GOTO set_dll
    
    IF "%~x1"==".cpp" GOTO ext_matched
    IF "%~x1"==".cxx" GOTO ext_matched
    IF "%~x1"==".c" GOTO ext_matched
    
    GOTO default

:set_dll
    
    SET linktype=dll

    GOTO next

:ext_matched
    
    ECHO:
    ECHO ----------------------------
    ECHO     Compiling %~1
    ECHO ----------------------------

    rem compile all with matched extension (*.c, *.cpp, *.cxx)
    CALL cl %~1
    GOTO link_%linktype%

:link_dll
    
    ECHO:
    ECHO ----------------------------
    ECHO     Linking   %~1
    ECHO ----------------------------

    CALL uflink UFDLL=%~n1.dll *.obj

    GOTO next

:link_exe
    
    ECHO:
    ECHO ----------------------------
    ECHO     Linking   %~1
    ECHO ----------------------------

    
    CALL uflink UFEXE=%~n1.exe *.obj

    ECHO:
    ECHO ----------------------------
    ECHO     Executing %~1
    ECHO ----------------------------

    CALL %~n1.exe

    GOTO end

:default
    ECHO unmatched file extension: %~1
    GOTO end

:next
    SHIFT
    IF NOT "%~1"=="" GOTO arg_parse

    GOTO end

:end
    ECHO:
    ECHO ----------------------------
    ECHO     Done
    ECHO ----------------------------

    ENDLOCAL

@echo on
