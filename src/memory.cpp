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
        NES.PPU.Run(NES.MasterCycle);
        NES.PPU.IODB = valueToWrite;
        switch(address & 8)
        {
            case 0:
            {
                NES.PPU.NametableBase = valueToWrite & 3;
                NES.PPU.PatternTableBase = (valueToWrite & 0x10) >> 4;
                if(valueToWrite & 11101100)
                {
                    char ErrorMessage[] = "Tried to write value $00 at $0000.";
                    UchartoHex(valueToWrite, ErrorMessage + 22, false);
                    UshorttoHex(address, ErrorMessage + 29, false);
                    MessageBox(0, ErrorMessage, "Error", IDOK);
                }
                break;
            }
            default:
            {
                char ErrorMessage[] = "Tried to write value $00 at $0000.";
                UchartoHex(valueToWrite, ErrorMessage + 22, false);
                UshorttoHex(address, ErrorMessage + 29, false);
                MessageBox(0, ErrorMessage, "Error", IDOK);
            }
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
