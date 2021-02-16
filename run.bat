@echo off

cls

set cpp_file=%1
set exe_file=%cpp_file:cpp=exe%

DEL %exe_file%

rem COMPILE
echo:
echo ----------------------------
echo         Compiling...
echo           %cpp_file%
echo ----------------------------
call cl %cpp_file%

rem LINK
echo:
echo ----------------------------
echo         Linking...
echo           %exe_file%
echo ----------------------------
call uflink UFEXE=%exe_file%

rem Execute
echo:
echo ----------------------------
echo         Executing...
echo           %exe_file%
echo ----------------------------
call %exe_file%

@echo on
