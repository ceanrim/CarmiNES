cl ..\src\main.cpp ..\src\cpu.cpp ..\src\memory.cpp ..\src\fltused.c ..\lib\memcpy.obj ..\lib\memset.obj ..\lib\strlen.obj "..\res\NES Emulator 2017 Edition by Carmine.aps" -oNES.exe -GS- -GR- -GX- -FAcsu -Zi /I"..\..\users\carmine\downloads\sdl2-devel-2.0.5-vc\sdl2-2.0.5\include" -link -nodefaultlib -LIBPATH:"C:\Users\Carmine\Downloads\SDL2-devel-2.0.5-VC\SDL2-2.0.5\lib\x64" kernel32.lib gdi32.lib user32.lib SDL2.lib
