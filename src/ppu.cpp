/* ========================================================================
    File: 
    Date: 
    Revision:
    Creator: Carmine Foggia 
   ======================================================================== */
#include <windows.h>
#include "main.h"
#include "nesclass.h"
#include "ppu.h"

PPUClass::PPUClass()
        :LastEmulatedCycle(0),
         Scanline(261),
         Dot(0),
         EvenFrame(true)
{
    NextFrameBuffer = (unsigned char *)VirtualAlloc(0, 256 * 262, MEM_COMMIT, PAGE_READWRITE);
    memset(NextFrameBuffer, 0, 240*256);
}

void PPUClass::Init(unsigned short Mapper)
{
    switch(Mapper)
    {
        case 0:
        {
            CHRROM = NES.ROMFile + 16 + NES.RAM.PRGROMSize;
            PatternTables[0] = CHRROM;
            PatternTables[1] = CHRROM + 4096;
            if(Mirroring & 8) //Four screen VRAM/No mirroring
            {
                Nametables[0] = Nametable0;
                Nametables[1] = Nametable1;
                Nametables[2] = Nametable2;
                Nametables[3] = Nametable3; 
            }
            else if(Mirroring == 0) //Horizontal mirroring
            {
                Nametables[0] = Nametables[1] = Nametable0;
                Nametables[2] = Nametables[3] = Nametable2;
            }
            else if(Mirroring == 1) //Vertical mirroring
            {
                Nametables[0] = Nametables[2] = Nametable0;
                Nametables[1] = Nametables[3] = Nametable1;
            }
            break;
        }
    }
    memset(Nametable0, 0, 1024);
    memset(Nametable1, 0, 1024);
    memset(Nametable2, 0, 1024);
    memset(Nametable3, 0, 1024);
}

void PPUClass::Run(unsigned long long CycleToGet) //Only NTSC for now
{
    while(LastEmulatedCycle < CycleToGet)
    {
        /*LastEmulatedCycle++;
        if(Scanline == 261) //Pre-render scanline
        {
            if(Dot < (339 + EvenFrame))
            {
                Dot++;
            }
            else
            {
                Dot = 0;
                Scanline = 0;
            }
        }
        else if(Scanline <= 239)
        {
            if(Dot == 0)
            {
                Dot++;
            }
            else if(Dot <= 256)
            {
                unsigned char BackgroundTile =
                    Nametables[NametableBase][Dot >> 3];
                unsigned char Palette =
                    NameTables[NametableBase]
                    [960 + ((Scanline >> 5) << 3) + (Dot >> 3)];
                if(Scanline & 4)
                {
                    if(Dot & 4)
                    {
                        Palette >>= 6;
                    }
                    else
                    {
                        Palette >>= 4;
                        Palette &= 3;
                    }
                }
                else
                {
                    if(Dot & 4)
                    {
                        Palette >>= 2;
                        Palette &= 3;
                    }
                    else
                    {
                        Palette &= 3;
                    }
                }
                unsigned char Color = 0;
                Color =
                    (PatternTables[PatternTableBase]
                     [(BackgroundTile << 4) + (Scanline & 7)] >>
                     (6 - (Dot & 7)));
                Color |=
                    (PatternTables[PatternTableBase]
                     [(BackgroundTile << 4) + (Scanline & 7) + 8] >>
                     (7 - (Dot & 7)));
                Color |= (Palette << 2);
                NextFrameBuffer[(Scanline << 8) + Dot] = Color;
            }
            }*/
    }
}
