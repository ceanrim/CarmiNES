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
    unsigned char     *InternalMemory;
    unsigned char     *CartridgeMemory;
    RenderBufferClass  RenderBuffer;
    HWND               Window;
    HDC                MainWindowDC;
    HANDLE             LogFileHandle;
    DebuggerClass      Debugger;
    NESClass();
};
#endif
