#if !defined(MAIN_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Carmine Foggia $
   ======================================================================== */
#include "..\lib\include.h"
#define NULL 0
#define MAIN_H
#define PAL 1
#define NTSC 0
#define PAL_CYCLE_COUNT 33247 //Cycles every frame for a PAL system
#define NTSC_CYCLE_COUNT 29830 //Cycles every frame for a NTSC system
namespace globals
{
    extern int Region;
    extern float Speed;
    extern bool Running; // if this is false the program closes
    extern unsigned char *InternalMemory; //Memory addresses between 0x0000 and 0x07FF
    extern unsigned char *CartridgeMemory; //For now only mapper 0
    extern const char ToHex[]; //To convert easily from binary to hexadecimal
}

#endif
