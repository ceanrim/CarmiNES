rc "..\res\NES Emulator 2017 Edition by Carmine.rc"
cl ..\lib\memcpy.cpp ..\lib\memset.cpp ..\lib\strlen.cpp -GS- -GR- -GX- -FAcsu  -Zi -Og 
cl ..\src\main.cpp ..\src\cpu.cpp ..\src\memory.cpp ..\src\nesclass.cpp ..\src\debuggerclass.cpp ..\lib\fltused.c memcpy.obj memset.obj strlen.obj "..\res\NES Emulator 2017 Edition by Carmine.res" -oNES.exe -GS- -GR- -GX- -FAcsu  -Og -O2 -Zi -link -nodefaultlib kernel32.lib gdi32.lib user32.lib comdlg32.lib winmm.lib