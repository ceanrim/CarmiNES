/* ========================================================================
    File: 
    Date: 
    Revision:
    Creator: Carmine Foggia 
   ======================================================================== */
#include "main.h"
NESClass::NESClass():
        CPU(), RAM(),
        Region(PAL), Speed(1.f),
        Running(true),
        InternalMemory(0),
        CartridgeMemory(0),
        WRAM(0),
        Pause(false),
        RenderBuffer(),
        Window(0),
        MainWindowDC(0),
        LogFileHandle(INVALID_HANDLE_VALUE),
        Debugger(),
        ROMFile(0),
        KIL(false),
        PerformanceCounterFrequency({0})
{
    QueryPerformanceFrequency(&PerformanceCounterFrequency);
    PerformanceCounterFrequency.QuadPart /= 1000;
}

  
