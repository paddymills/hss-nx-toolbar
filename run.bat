@echo off

set

echo "Compiling..."
ufcomp Test.cpp

echo "Linking..."
uflink UFEXE=Test.exe

echo "Executing..."
Test.exe

@echo on
