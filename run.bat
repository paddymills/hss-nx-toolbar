@echo off


SETLOCAL
SET linktype=exe

GOTO main

:main
    IF "%~1"=="/DLL" GOTO set_dll
    IF "%~1"=="/dll" GOTO set_dll
    
    IF "%~1"=="/CLEAN" GOTO clean
    IF "%~1"=="/clean" GOTO clean

    CLS

    ECHO **************************************
    ECHO *     %DATE%-%TIME%     *
    ECHO **************************************
    
    IF "%~x1"==".cpp" GOTO ext_matched
    IF "%~x1"==".cxx" GOTO ext_matched
    IF "%~x1"==".c" GOTO ext_matched
    
    GOTO default

:clean

    IF EXIST *.dll DEL *.dll
    IF EXIST *.exe DEL *.exe
    IF EXIST *.exp DEL *.exp
    IF EXIST *.lib DEL *.lib
    IF EXIST *.pdb DEL *.pdb
    IF EXIST *.obj DEL *.obj

    GOTO next

:set_dll
    
    SET linktype=dll

    GOTO next

:ext_matched
    
    ECHO:
    ECHO ----------------------------
    ECHO     Compiling %~1
    ECHO ----------------------------

    CALL cl %~1
    GOTO link_%linktype%

:link_dll
    
    IF EXIST %~n1.dll DEL %~n1.dll

    ECHO:
    ECHO ----------------------------
    ECHO     Linking   %~1
    ECHO ----------------------------
    CALL uflink UFDLL=%~n1.dll

    GOTO next

:link_exe
    
    IF EXIST %~n1.exe DEL %~n1.exe
    
    ECHO:
    ECHO ----------------------------
    ECHO     Linking   %~1
    ECHO ----------------------------
    
    CALL uflink UFEXE=%~n1.exe

    ECHO:
    ECHO ----------------------------
    ECHO     Executing %~1
    ECHO ----------------------------

    CALL %~n1.exe

    GOTO next

:default
    ECHO unmatched file extension: %~1
    GOTO end

:next
    SHIFT
    IF NOT "%~1"=="" GOTO main

    GOTO end

:end
    ENDLOCAL

@echo on
