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
    unsigned short CurrentMemoryAddress;
    HWND DebuggerHandle;
    HWND CHRROMViewerHandle;
    unsigned *PatternTablesRendered;
    BITMAPINFO PTRInfo;
    unsigned  Greys[4];
    DebuggerClass();
};

#endif
