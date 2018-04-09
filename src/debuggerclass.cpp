/* ========================================================================
    File: 
    Date: 
    Revision:
    Creator: Carmine Foggia 
   ======================================================================== */

#include "debuggerclass.h"

DebuggerClass::DebuggerClass():
        isDebuggerActive(false),
        isCHRROMViewerOpen(false),
        CurrentMemoryAddress(0),
        DebuggerHandle(0),
        CHRROMViewerHandle(0),
        PatternTablesRendered(0)
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
}
