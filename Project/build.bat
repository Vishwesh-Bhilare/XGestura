mkdir bin
cls
@echo off

echo === clearing binaries ===
del /F bin\*.obj bin\*.res
del /F *.exe

echo === compiling the source code ===
cl.exe /c /EHsc src/win32-handler.c /Fo:bin/win32-handler.obj

echo === compiling the resouces ===
rc.exe /fo bin/window.res window.rc

echo === linking files ===
link.exe bin/win32-handler.obj bin/window.res user32.lib gdi32.lib /SUBSYSTEM:WINDOWS
