/* ========================================================================
    File: 
    Date: 
    Revision:
    Creator: Carmine Foggia 
   ======================================================================== */
/*TODO:
 *Simulate PPU registers completely and properly
 *Emulate read/write buffering
 *Emulate sprites
 *Emulate scanline timing
 *PAL
 */
#include <windows.h>
#include "main.h"
#include "nesclass.h"
#include "ppu.h"

PPUClass::PPUClass()
        :LastEmulatedCycle(0),
         Scanline(261),
         Dot(0),
         VRAMAddr(0),
         PPUADDRWriteTick(0)
{
    memset(OAM, 0, 256);
    Register2002 = 0;
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
    //memset(BackgroundPalettes, 0, 16);
    //FOR DEBUG:
    BackgroundPalettes[0][0] = 0x0F;
    BackgroundPalettes[0][1] = 0x16;
    BackgroundPalettes[0][2] = 0x30;
    BackgroundPalettes[0][3] = 0x37;
    memset(BackgroundPalettes[1], 0, 12);
    memset(SpritePalettes, 0, 16);
}

//TODO: The PPU processes pixels 8 by 8, implement this

void PPUClass::Run(unsigned long long CycleToGet) //Only NTSC for now
{
    bool RollingOver = false;
    if(LastEmulatedCycle > CycleToGet) //Let's avoid infinite loops because of
                                       //cycle rollover
    {
        RollingOver = true;
        CycleToGet += NTSC_CYCLE_COUNT;
    }
    /*if((LastEmulatedCycle < NTSC_VBLANK_CYCLE) && (CycleToGet >= NTSC_VBLANK_CYCLE))
    {
        Register2002 |= 0b10000000;
        NES.NMI = true;
    }
    if((LastEmulatedCycle < NTSC_VBLANK_UNSET_CYCLE) &&
       (CycleToGet >= NTSC_VBLANK_UNSET_CYCLE))
    {
        Register2002 &= 0b01111111;
        }*/
    while(LastEmulatedCycle < CycleToGet)
    {
        LastEmulatedCycle += 5;
        if(Scanline == 261) //Pre-render scanline
        {
            if(Dot == 0)
            {
                Register2002 &= 0b01111111;
                Dot++;
            }
            else if(Dot < (338 + (NES.FrameCount & 1)?0:1))
            {
                Dot++;
            }
            else
            {
                Dot = -1;
                Scanline = 0;
            }
        }
        else if(Scanline <= 239)
        {
            if(Dot == -1)
            {
                Dot++;
            }
            else if(Dot <= 255)
            {
                unsigned char BackgroundTile =
                    Nametables[NametableBase][((Scanline >> 3) << 5) +
                                              Dot >> 3];
                unsigned char Palette =
                    Nametables[NametableBase]
                    [960 + ((Scanline >> 5) << 3) + (Dot >> 5)];
                if(Scanline & 16)
                {
                    if(Dot & 16)
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
                    if(Dot & 16)
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
                     [(BackgroundTile << 4) + (Scanline & 7)] &
                     (1 << (7 - (Dot & 7)))) >> (7 - (Dot & 7));
                Color |=
                    ((PatternTables[PatternTableBase]
                      [(BackgroundTile << 4) + (Scanline & 7) + 8] &
                      (1 << (7 - (Dot & 7)))) >> (7 - (Dot & 7)) << 1);
                NES.RenderBuffer.Memory[(Scanline << 8) + Dot] =
                    NES.Debugger.ColorRGBTable
                    [NES.PPU.BackgroundPalettes[Palette][Color]];
                Dot++;
            }
            else if(Dot < 339)
            {
                Dot++;
            }
            else
            {
                Scanline++;
                Dot = -1;
            }
        }
        else if(Scanline == 240)
        {
            if(Dot < 339)
            {
                Dot++;
            }
            else
            {
                Scanline++;
                Dot = -1;
            }
        }
        else if(Scanline == 241)
        {
            if(Dot == 0)
            {
                Register2002 |= 0b10000000;
                if(NES.NMIEnabled)
                {
                    NES.NMI = true;
                }
                Dot++;
            }
            else if(Dot < 339)
            {
                Dot++;
            }
            else
            {
                Scanline++;
                Dot = -1;
            }
        }
        else if(Scanline < 261)
        {
            if(Dot < 339)
            {
                Dot++;
            }
            else
            {
                Scanline++;
                Dot = -1;
            }
        }
    }
    if(RollingOver)
    {
        LastEmulatedCycle -= NTSC_CYCLE_COUNT;
    }
}

void PPUClass::Write(unsigned char valueToWrite) //TODO: Buffering
{
    if(VRAMAddr < 0x1FFF)
    {
        PatternTables[(VRAMAddr & 0x1000) >> 12][VRAMAddr & 0x0FFF] = valueToWrite;
    }
    else if((VRAMAddr >= 0x2000) && (VRAMAddr < 0x3F00))
    {
        Nametables[(VRAMAddr & 0x0C00) >> 14][VRAMAddr & 0x03FF] = valueToWrite;
    }
    else if(VRAMAddr >= 0x3F00)
    {
        if(VRAMAddr & 3)
        {
            if(VRAMAddr & 16) //Sprite palettes
            {
                SpritePalettes[(VRAMAddr & 12) >> 2][VRAMAddr & 3] = valueToWrite;
            }
            else
            {
                BackgroundPalettes[(VRAMAddr & 12) >> 2][VRAMAddr & 3] = valueToWrite;
            }
        }
        else
        {
            BackgroundPalettes[0][0] = valueToWrite;
            BackgroundPalettes[1][0] = valueToWrite;
            BackgroundPalettes[2][0] = valueToWrite;
            BackgroundPalettes[3][0] = valueToWrite;
            SpritePalettes[0][0]     = valueToWrite;
            SpritePalettes[1][0]     = valueToWrite;
            SpritePalettes[2][0]     = valueToWrite;
            SpritePalettes[3][0]     = valueToWrite;
        }
    }
    else
    {
        char ErrorMessage[] = "Tried to write value $00 at $0000 in PPU.";
        UchartoHex(valueToWrite, ErrorMessage + 22, false);
        UshorttoHex(VRAMAddr, ErrorMessage + 29, false);
        MessageBox(0, ErrorMessage, "Error", IDOK);
    }
    VRAMAddr += PPUADDRIncrement;
}
