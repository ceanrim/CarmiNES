/* ========================================================================
    File: 
    Date: 
    Revision:
    Creator: Carmine Foggia 
   ======================================================================== */

#include "..\res\resource.h"
#include "debuggerclass.h"
DebuggerClass::DebuggerClass():
        isDebuggerActive(false),
        isCHRROMViewerOpen(false),
        isPaletteViewerOpen(false),
        isNametableViewerOpen(false),
        CurrentMemoryAddress(0),
        DebuggerHandle(0),
        CHRROMViewerHandle(0),
        PaletteViewerHandle(0),
        NametableViewerHandle(0),
        PatternTablesRendered(0),
        PalettesRendered(0),
        NametableRendered(0)
{
    Greys[0] = 0;
    Greys[1] = 0x555555;
    Greys[2] = 0xAAAAAA;
    Greys[3] = 0xFFFFFF;
    PTRInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    PTRInfo.bmiHeader.biWidth = 128;
    PTRInfo.bmiHeader.biHeight = -256;
    PTRInfo.bmiHeader.biPlanes = 1;
    PTRInfo.bmiHeader.biBitCount = 32;
    PTRInfo.bmiHeader.biCompression = BI_RGB;
    PTInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    PTInfo.bmiHeader.biWidth = 64;
    PTInfo.bmiHeader.biHeight = -128;
    PTInfo.bmiHeader.biPlanes = 1;
    PTInfo.bmiHeader.biBitCount = 32;
    PTInfo.bmiHeader.biCompression = BI_RGB;
    NTInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    NTInfo.bmiHeader.biWidth = 512;
    NTInfo.bmiHeader.biHeight = -480;
    NTInfo.bmiHeader.biPlanes = 1;
    NTInfo.bmiHeader.biBitCount = 32;
    NTInfo.bmiHeader.biCompression = BI_RGB;
}
