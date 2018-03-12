#if !defined(DEBUGGERCLASS_H)
/* ========================================================================
    File: 
    Date: 
    Revision: 
    Creator: Carmine Foggia 
   ======================================================================== */

#define DEBUGGERCLASS_H

class DebuggerClass
{
public:
    bool isDebuggerActive;
    unsigned short CurrentMemoryAddress;
    HWND DebuggerHandle;
};

#endif
