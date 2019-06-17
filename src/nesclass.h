#if !defined(NESCLASS_H)
/* ========================================================================
    File: 
    Date: 
    Revision: 
    Creator: Carmine Foggia 
   ======================================================================== */
#define NESCLASS_H
#include "memory.h"
#include "cpu.h"
#include "debuggerclass.h"
#include "ppu.h"
class RenderBufferClass
{
public:
    BITMAPINFO Info;
    unsigned *Memory;
    int Width;
    int Height;
};
class NESClass
{
public:
    CPUClass           CPU;
    MemoryClass        RAM;
    PPUClass           PPU;
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
    unsigned long long MasterCycle;
    unsigned long long FrameCycle;
    bool               NMI;
    bool               NMIEnabled;
    NESClass();
};
#endif
