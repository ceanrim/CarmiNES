/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Carmine Foggia $
   ======================================================================== */
#include <windows.h>
#include "memory.h"
#include "cpu.h"
#include "main.h"

void MemoryClass::Init(unsigned short Mapper_)
{
    switch(Mapper_)
    {
        case 0: //NROM
        {
            if(PRGROMSize == 16384)
            {
                PRGROMPages[0] = NES.ROMFile + 16;
                PRGROMPages[1] = NES.ROMFile + 16;
            }
            else if(PRGROMSize == 32768)
            {
                PRGROMPages[0] = NES.ROMFile + 16;
                PRGROMPages[1] = NES.ROMFile + 16 + 16384;
            }
        }
    }
}

void MemoryClass::Destroy()
{
}

unsigned char MemoryClass::Read(unsigned short Address)
/*NES memory structure:
 0x0000-0x07ff Internal Memory
 0x0800-0x1fff Mirrors three times 0x0000-0x07ff
 ...To be completed...*/
{
    if(Address < 0x2000)
    {
        return NES.InternalMemory[Address % 0x800];
    }
    else if(Address == 0x2002)
    {
        NES.PPU.Run(NES.FrameCycle);
        unsigned char toReturn = NES.PPU.Register2002;
        NES.PPU.Register2002 &= 0b01111111;
        return toReturn;
    }
    else if((Address >= 0x6000) && (Address < 0x8000))
    {
        return NES.WRAM[Address - 0x6000];
    }
    else if((Address >= 0x8000) && (Address < 0xC000))
    {
        return NES.RAM.PRGROMPages[0][Address - 0x8000];
    }
    else if(Address >= 0xC000)
    {
        return NES.RAM.PRGROMPages[1][Address - 0xC000];
    }
    else
    {
        char ErrorMessage[] = "Tried to read at $0000.";
        UshorttoHex(Address, ErrorMessage + 18, false);
        MessageBox(0, ErrorMessage, "Error", IDOK);
        return 0;
    }
}

unsigned char MemoryClass::ReadWithNoSideEffects(unsigned short Address)
/*NES memory structure:
 0x0000-0x07ff Internal Memory
 0x0800-0x1fff Mirrors three times 0x0000-0x07ff
 0x8000-0xffff Cartridge space
 ...To be completed...*/
{
    if(Address < 0x2000)
    {
        return NES.InternalMemory[Address % 0x800];
    }
    else if((Address >= 0x6000) && (Address < 0x8000))
    {
        return NES.WRAM[Address - 0x6000];
    }
    else if((Address >= 0x8000) && (Address < 0xC000))
    {
        return NES.RAM.PRGROMPages[0][Address - 0x8000];
    }
    else if(Address >= 0xC000)
    {
        return NES.RAM.PRGROMPages[1][Address - 0xC000];
    }
    else
    {
        return 0;
    }
}
void MemoryClass::Read(unsigned char  addrMode,
                      unsigned char* valueToRewrite)
{
    switch(addrMode)
    {
        case ADDR_ZERO_PAGE:
        {
            AddressBus = Read(NES.CPU.PC);
            NES.CPU.PC++;
            NEXT_CYCLE;
            *valueToRewrite = Read(AddressBus);
            NEXT_CYCLE;
            break;
        }
        case ADDR_ZERO_PAGE_X:
        {
            AddressBus = Read(NES.CPU.PC);
            NES.CPU.PC++;
            NEXT_CYCLE;
            Read(AddressBus);
            AddressBus += NES.CPU.X;
            AddressBus &= 255;
            NEXT_CYCLE;
            *valueToRewrite = Read(AddressBus);
            NEXT_CYCLE;
            break;
        }
        case ADDR_ZERO_PAGE_Y:
        {
            AddressBus = Read(NES.CPU.PC);
            NES.CPU.PC++;
            NEXT_CYCLE;
            Read(AddressBus);
            AddressBus += NES.CPU.Y;
            AddressBus &= 255;
            NEXT_CYCLE;
            *valueToRewrite = Read(AddressBus);
            NEXT_CYCLE;
            break;
        }
        case ADDR_IMMEDIATE:
        {
            *valueToRewrite = Read(NES.CPU.PC);
            NES.CPU.PC++;
            NES.FrameCycle += (NES.Region == NTSC)? 30: 32;
            break;
        }
        case ADDR_ABSOLUTE:
        {
            AddressBus = Read(NES.CPU.PC);
            NES.CPU.PC++;
            NEXT_CYCLE;
            AddressBus |= (Read(NES.CPU.PC) << 8);
            NES.CPU.PC++;
            NEXT_CYCLE;
            *valueToRewrite = Read(AddressBus);
            NEXT_CYCLE;
        } break;
        case ADDR_ABSOLUTE_X:
        {
            AddressBus = Read(NES.CPU.PC);
            NES.CPU.PC++;
            NEXT_CYCLE;

            AddressBus |= (Read(NES.CPU.PC) << 8);
            unsigned short AddressBusChanged = AddressBus + NES.CPU.X;
            if(AddressBusChanged >> 8 != AddressBus >> 8)
            {
                AddressCarry = true;
            }
            AddressBus = AddressBusChanged;
            NES.CPU.PC++;
            NEXT_CYCLE;

            if(!AddressCarry)
            {
                *valueToRewrite = Read(AddressBus);
                NEXT_CYCLE;
                break;
            }
            Read(AddressBus - 256);
            NEXT_CYCLE;

            *valueToRewrite = Read(AddressBus);
            NEXT_CYCLE;
            break;
        }
        case ADDR_ABSOLUTE_Y:
        {
            AddressBus = Read(NES.CPU.PC);
            NES.CPU.PC++;
            NEXT_CYCLE;

            AddressBus |= (Read(NES.CPU.PC) << 8);
            unsigned short AddressBusChanged = AddressBus + NES.CPU.Y;
            if(AddressBusChanged >> 8 != AddressBus >> 8)
            {
                AddressCarry = true;
            }
            AddressBus = AddressBusChanged;
            NES.CPU.PC++;
            NEXT_CYCLE;

            if(!AddressCarry)
            {
                *valueToRewrite = Read(AddressBus);
                NEXT_CYCLE;
                break;
            }
            Read(AddressBus - 256);
            NEXT_CYCLE;

            *valueToRewrite = Read(AddressBus);
            NEXT_CYCLE;
            break;
        }
        case ADDR_INDIRECT_X:
        {
            AddressBus = Read(NES.CPU.PC);
            NES.CPU.PC++;
            NEXT_CYCLE;

            Read(AddressBus);
            AddressBus += NES.CPU.X;
            NEXT_CYCLE;

            temp = Read(AddressBus);
            AddressBus++;
            NEXT_CYCLE;

            temp |= ((Read(AddressBus)) << 8);
            AddressBus = temp;
            NEXT_CYCLE;

            *valueToRewrite = Read(AddressBus);
            NEXT_CYCLE;
            break;
        }
        case ADDR_INDIRECT_Y:
        {
            AddressBus = Read(NES.CPU.PC);
            NES.CPU.PC++;
            NEXT_CYCLE;
           
            temp = Read(AddressBus);
            AddressBus++;
            NEXT_CYCLE;
           
            temp |= (Read(AddressBus) << 8);
            AddressBus = temp;
            unsigned short AddressBusChanged = AddressBus + NES.CPU.Y;
            if((AddressBus >> 8) != ((AddressBusChanged) >> 8))
            {
                AddressCarry = true;
            }
            else
            {
                AddressCarry = false;
            }
            AddressBus = AddressBusChanged;
            NEXT_CYCLE;
           
            if(!AddressCarry)
            {
                *valueToRewrite = Read(AddressBus);
                NEXT_CYCLE;
                break;
            }
            
            Read(AddressBus - 256);
            NEXT_CYCLE;
           
            *valueToRewrite = Read(AddressBus);
            NEXT_CYCLE;
           
            break;
        }
    }
}
void MemoryClass::Write(unsigned short address,
                       unsigned char valueToWrite)
{
    if(address < 0x2000)
    {
        NES.InternalMemory[address % 0x800] = valueToWrite;
    }
    else if((address >= 0x2000) && (address < 0x4000))
    {
        NES.PPU.Run(NES.FrameCycle);
        switch(address & 7)
        {
            case 0:
            {
                NES.PPU.IODB = valueToWrite; //TODO: Implement this everywhere
                NES.PPU.NametableBase = valueToWrite & 3; //TODO: Bit 0 race condition
                NES.PPU.PatternTableBase = (valueToWrite & 0x10) >> 4; //TODO: Actually do this
                NES.PPU.PPUADDRIncrement = (valueToWrite & 4) ? 32 : 1;
                if(valueToWrite & 11101000)
                {
                    char ErrorMessage[] = "Tried to write value $00 at $0000.";
                    UchartoHex(valueToWrite, ErrorMessage + 22, false);
                    UshorttoHex(address, ErrorMessage + 29, false);
                    MessageBox(0, ErrorMessage, "Error", IDOK);
                }
                break;
            }
            case 6:
            {
                if(NES.PPU.PPUADDRWriteTick)
                {
                    NES.PPU.VRAMAddr &= 0b1111111100000000;
                    NES.PPU.VRAMAddr |= valueToWrite;
                }
                else
                {
                    NES.PPU.VRAMAddr &= 255;
                    NES.PPU.VRAMAddr |= ((unsigned short)(valueToWrite) << 8);
                }
                NES.PPU.VRAMAddr &= 0x3FFF;
                NES.PPU.PPUADDRWriteTick = (!(NES.PPU.PPUADDRWriteTick));
                break;
            }
            case 7:
            {
                NES.PPU.Write(valueToWrite);
                break;
            }
            default:
            {
                char ErrorMessage[] = "Tried to write value $00 at $0000.";
                UchartoHex(valueToWrite, ErrorMessage + 22, false);
                UshorttoHex(address, ErrorMessage + 29, false);
                MessageBox(0, ErrorMessage, "Error", IDOK);
                break;
            }
        }
    }
    else if(address == 0x4014)
    {
        //TODO: This skips a cycle if we start on an odd cycle
        //TODO: OAMADDR register
        NEXT_CYCLE;
        for(unsigned short i = 0; i < 256; i++)
        {
            unsigned short page = (unsigned short)(valueToWrite) << 8;
            unsigned char toWrite = Read(page|i);
            NEXT_CYCLE;
            NES.PPU.OAM[i] = toWrite;
            NEXT_CYCLE;
        }
    }
    else if((address >= 0x6000) && (address < 0x8000))
    {
        NES.WRAM[address - 0x6000] = valueToWrite;
    }
    else if(address >= 0x8000)
    {
    }
    else
    {
        char ErrorMessage[] = "Tried to write value $00 at $0000.";
        UchartoHex(valueToWrite, ErrorMessage + 22, false);
        UshorttoHex(address, ErrorMessage + 29, false);
        MessageBox(0, ErrorMessage, "Error", IDOK);
    }
}
void MemoryClass::WriteAddrMode(unsigned char valueToWrite,
                        unsigned char addrMode)
{
    switch(addrMode)
    {
        case ADDR_ZERO_PAGE:
        {
            AddressBus = Read(NES.CPU.PC);
            NES.CPU.PC++;
            NEXT_CYCLE;

            Write(AddressBus, valueToWrite);
            NEXT_CYCLE;
            break;
        }
        case ADDR_ZERO_PAGE_X:
        {
            AddressBus = Read(NES.CPU.PC);
            NES.CPU.PC++;
            NEXT_CYCLE;
            
            Read(AddressBus);
            AddressBus += NES.CPU.X;
            AddressBus &= 255;
            NEXT_CYCLE;
            
            Write(AddressBus, valueToWrite);
            NEXT_CYCLE;
            break;
        }
        case ADDR_ZERO_PAGE_Y:
        {
            AddressBus = Read(NES.CPU.PC);
            NES.CPU.PC++;
            NEXT_CYCLE;
            
            Read(AddressBus);
            AddressBus += NES.CPU.Y;
            AddressBus &= 255;
            NEXT_CYCLE;
            
            Write(AddressBus, valueToWrite);
            NEXT_CYCLE;
            break;
        }
        case ADDR_ABSOLUTE:
        {
            AddressBus = Read(NES.CPU.PC);
            NES.CPU.PC++;
            NEXT_CYCLE;
         
            AddressBus |= (Read(NES.CPU.PC) << 8);
            NES.CPU.PC++;
            NEXT_CYCLE;
         
            Write(AddressBus, valueToWrite);
            NEXT_CYCLE;
            break;
        }
        case ADDR_ABSOLUTE_X:
        {
            AddressBus = Read(NES.CPU.PC);
            NES.CPU.PC++;
            NEXT_CYCLE;
            
            AddressBus |= (Read(NES.CPU.PC) << 8);
            AddressBus += NES.CPU.X;
            NES.CPU.PC++;
            NEXT_CYCLE;
            
            Read(AddressBus - 256);
            NEXT_CYCLE;
                        
            Write(AddressBus, valueToWrite);
            NEXT_CYCLE;
            break;
        }
        case ADDR_ABSOLUTE_Y:
        {
            AddressBus = Read(NES.CPU.PC);
            NES.CPU.PC++;
            NEXT_CYCLE;
          
            AddressBus |= (Read(NES.CPU.PC) << 8);
            AddressBus += NES.CPU.Y;
            NES.CPU.PC++;
            NEXT_CYCLE;
          
            Read(AddressBus - 256);
            NEXT_CYCLE;
          
            Write(AddressBus, valueToWrite);
            NEXT_CYCLE;
            break;
        }
        case ADDR_INDIRECT_X:
        {
            AddressBus = Read(NES.CPU.PC);
            NES.CPU.PC++;
            NEXT_CYCLE;
          
            Read(AddressBus);
            AddressBus += NES.CPU.X;
            NEXT_CYCLE;
          
            temp = Read(AddressBus);
            AddressBus++;
            NEXT_CYCLE;
         
            temp |= ((Read(AddressBus)) << 8);
            AddressBus = temp;
            NEXT_CYCLE;
         
            Write(AddressBus, valueToWrite);
            NEXT_CYCLE;
            break;
        }
        case ADDR_INDIRECT_Y:
        {
            AddressBus = Read(NES.CPU.PC);
            NES.CPU.PC++;
            NEXT_CYCLE;

            temp = Read(AddressBus);
            AddressBus++;
            NEXT_CYCLE;

            temp |= (Read(AddressBus) << 8);
            AddressBus = temp;
            AddressBus += NES.CPU.Y;
            NEXT_CYCLE;

            Read(AddressBus - 256);
            NEXT_CYCLE;

            Write(AddressBus, valueToWrite);
            NEXT_CYCLE;
            break;
        }
    }
}
