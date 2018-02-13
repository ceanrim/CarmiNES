/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Carmine Foggia $
   ======================================================================== */
#include "memory.h"
#include "cpu.h"
#include "main.h"
unsigned char Memory::Read(unsigned short Address)
/*NES memory structure:
  0x0000-0x07ff Internal Memory
  0x0800-0x1fff Mirrors three times 0x0000-0x07ff
  ...To be completed...*/
{
    if(Address < 0x2000)
    {
        return globals::InternalMemory[Address % 0x800];
    }
}
unsigned char Memory::Read(unsigned short address,
                           unsigned char cycle,
                           unsigned char addrMode)
{
    switch(addrMode)
    {
        case ADDR_IMMEDIATE:
        {
            return Memory::Read(CPU::PC);
            break;
        }
        default: //TODO
        {
            Log("Tried to read memory in a way that has not been implemented");
            return 0;
        }
    }
}
