#if !defined(NESCLASS_H)
/* ========================================================================
    File: 
    Date: 
    Revision: 
    Creator: Carmine Foggia 
   ======================================================================== */
#define NESCLASS_H
#include "cpu.h"
#include "debuggerclass.h"
class RenderBufferClass
{
public:
    BITMAPINFO Info;
    void *Memory;
    int Width;
    int Height;
};
class NESClass
{
public:
    CPUClass           CPU;
    MemoryClass        RAM;
    int                Region;
    float              Speed;
    bool               Running;
    bool               Pause;
    bool               KIL;
    unsigned char     *InternalMemory;
    unsigned char     *CartridgeMemory;
    unsigned char     *WRAM;
    RenderBufferClass  RenderBuffer;
    HWND               Window;
    HDC                MainWindowDC;
    HANDLE             LogFileHandle;
    DebuggerClass      Debugger;
    unsigned char     *ROMFile;
    DWORD              ROMFileSize;
    LARGE_INTEGER      PerformanceCounterFrequency;
    LARGE_INTEGER      LastFrameTime;
    LARGE_INTEGER      CurrentFrameTime;
    unsigned           FrameCount;
    long long          FrameTimes[64];
    unsigned long long FrameCycle;
    NESClass();
};
#endif