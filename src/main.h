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
#define PAL_CYCLE_COUNT 33247 //Cycles every frame for a PAL system
#define NTSC_CYCLE_COUNT 29830 //Cycles every frame for a NTSC system
extern const char ToHex[]; //To convert easily from binary to hexadecimal
extern bool Debugger;
extern HWND DebuggerHandle;
extern NESClass NES;
#endif
