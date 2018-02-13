#if !defined(MEMORY_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Carmine Foggia $
   ======================================================================== */
#define MEMORY_H
#define ADDR_INDIRECT_X  0
#define ADDR_ZERO_PAGE   1
#define ADDR_IMMEDIATE   2
#define ADDR_ABSOLUTE    3
#define ADDR_INDIRECT_Y  4
#define ADDR_ZERO_PAGE_X 5
#define ADDR_ABSOLUTE_Y  6
#define ADDR_ABSOLUTE_X  7

namespace Memory
{
    extern unsigned char Read(unsigned short); //Reads the memory
                                               //at the corresponding address
    
    extern unsigned char Read(unsigned short address,  //Processes addressing modes
                              unsigned char cycle,     //in a cycle-accurate way
                              unsigned char addrMode); 
                                                       
}

#endif
