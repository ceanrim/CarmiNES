#if !defined(MAIN_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Carmine Foggia $
   ======================================================================== */
#include <windows.h>
#include "..\res\resource.h"
#include "memory.h"
#include "nesclass.h"
#define MAIN_H
#define PAL 1
#define NTSC 0
#define PAL_CYCLE_COUNT 531952 //Cycles every frame for a PAL system
#define NTSC_CYCLE_COUNT 447450 //Cycles every frame for a NTSC system
#define NEXT_CYCLE NES.FrameCycle += (NES.Region == NTSC)? 15: 16; \
    NES.MasterCycle += (NES.Region == NTSC)? 15: 16;
#define NEXT_CYCLES(a) NES.FrameCycle += (NES.Region == NTSC)? 15*a: 16*a; \
    NES.MasterCycle += (NES.Region == NTSC)? 15*a: 16*a;
extern const char ToHex[]; //To convert easily from binary to hexadecimal
extern bool Debugger;
extern HWND DebuggerHandle;
extern NESClass NES;
extern void UchartoHex(unsigned char, char*, bool);
extern void UshorttoHex(unsigned short, char*, bool);
#endif
