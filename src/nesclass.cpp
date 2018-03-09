/* ========================================================================
    File: 
    Date: 
    Revision:
    Creator: Carmine Foggia 
   ======================================================================== */
#include "main.h"
NESClass::NESClass():
        CPU(), RAM(), Region(PAL), Speed(1.f), Running(true),
        InternalMemory(0),
        CartridgeMemory(0)
{
}

  
