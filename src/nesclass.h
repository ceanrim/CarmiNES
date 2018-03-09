#if !defined(NESCLASS_H)
/* ========================================================================
    File: 
    Date: 
    Revision: 
    Creator: Carmine Foggia 
   ======================================================================== */
#define NESCLASS_H
#include "cpu.h"
class NESClass
{
public:
    CPUClass       CPU;
    MemoryClass    RAM;
    int            Region;
    float          Speed;
    bool           Running;
    unsigned char *InternalMemory;
    unsigned char *CartridgeMemory;
    NESClass();
    //DebuggerClass Debugger;
};
#endif
