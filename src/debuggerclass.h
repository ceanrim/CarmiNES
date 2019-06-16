#if !defined(DEBUGGERCLASS_H)
/* ========================================================================
    File: 
    Date: 
    Revision: 
    Creator: Carmine Foggia 
   ======================================================================== */

#define DEBUGGERCLASS_H
#include <windows.h>

class DebuggerClass
{
public:
    bool isDebuggerActive;
    bool isCHRROMViewerOpen;
    bool isPaletteViewerOpen;
    unsigned short CurrentMemoryAddress;
    HWND DebuggerHandle;
    HWND CHRROMViewerHandle;
    HWND PaletteViewerHandle;
    unsigned *PatternTablesRendered;
    unsigned *PalettesRendered;
    unsigned ColorRGBTable[64] =
    {
        0x00545454, 0x00001E74, 0x00081090, 0x00300088,
        0x00440064, 0x005C0030, 0x00540400, 0x003C1800,
        0x00202A00, 0x00083A00, 0x00004000, 0x00003C00,
        0x0000323C, 0x00000000, 0x00000000, 0x00000000,

        0x00989698, 0x00084CC4, 0x003032DC, 0x005C1DD4,
        0x008814B0, 0x00A01464, 0x00982220, 0x00783C00,
        0x00545A00, 0x00287200, 0x00087C00, 0x00007628,
        0x00006678, 0x00000000, 0x00000000, 0x00000000,

        0x00ECEEEC, 0x004C9AEC, 0x00787CEC, 0x00B062EC,
        0x00E454EC, 0x00EC58B4, 0x00EC6A64, 0x00D48820,
        0x00A0AA00, 0x0074C400, 0x004CD020, 0x0038CC68,
        0x0038A4BC, 0x003C3C3C, 0x00000000, 0x00000000,

        0x00ECEEEC, 0x00A8BCEC, 0x00B8B8EC, 0x00D4B2EC,
        0x00ECAEEC, 0x00ECAED4, 0x00ECB4B0, 0x00E4C490,
        0x00CCD278, 0x00B4DE78, 0x00A8E290, 0x0098E2B4,
        0x00A0D6E4, 0x00A0A2A0, 0x00000000, 0x00000000
    } ;
    BITMAPINFO PTRInfo;
    BITMAPINFO PTInfo;
    unsigned  Greys[4];
    DebuggerClass();
};

#endif
