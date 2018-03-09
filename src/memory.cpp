/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Carmine Foggia $
   ======================================================================== */
#include "memory.h"
#include "cpu.h"
#include "main.h"
MemoryClass::MemoryClass():
        AddressCarry(false),
        ConversionTable{ADDR_IMMEDIATE, ADDR_ZERO_PAGE, 0,
                        ADDR_ABSOLUTE, 0, ADDR_ZERO_PAGE_X,
                        0, ADDR_ABSOLUTE_X}
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
    if((Address >= 0x8000) && (Address < 0xC000))
    {
        return NES.CartridgeMemory[Address - 0x8000];
    }
    if(Address >= 0xC000)
    {
        if(this->Mapper == 0)
        {
            if(PRGROMSize == 16384)
            {
                return NES.CartridgeMemory[Address - 0xC000];
            }
            if(PRGROMSize == 32768)
            {
                return NES.CartridgeMemory[Address - 0x8000];
            }
        }
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
    if((Address >= 0x8000) && (Address < 0xC000))
    {
        return NES.CartridgeMemory[Address - 0x8000];
    }
    if(Address >= 0xC000)
    {
        if(this->Mapper == 0)
        {
            if(PRGROMSize == 16384)
            {
                return NES.CartridgeMemory[Address - 0xC000];
            }
            if(PRGROMSize == 32768)
            {
                return NES.CartridgeMemory[Address - 0x8000];
            }
        }
    }
}
void MemoryClass::Read(unsigned short address,
                       unsigned char* cycle,
                       unsigned char  addrMode,
                       unsigned char* valueToRewrite)
{
    switch(addrMode)
    {
        case ADDR_ZERO_PAGE:
        {
            if(*cycle == 1)
            {
                AddressBus = Read(NES.CPU.PCTemp);
                NES.CPU.PCTemp++;
                (*cycle)++;
                break;
            }
            if(*cycle == 2)
            {
                *valueToRewrite = Read(AddressBus);
                (*cycle) = 0;
                break;
            }
        }
        case ADDR_ZERO_PAGE_X:
        {
            if(*cycle == 1)
            {
                AddressBus = Read(NES.CPU.PCTemp);
                NES.CPU.PCTemp++;
                (*cycle)++;
                break;
            }
            if(*cycle == 2)
            {
                Read(AddressBus);
                AddressBus += NES.CPU.X;
                AddressBus &= 255;
                (*cycle)++;
                break;
            }
            if(*cycle == 3)
            {
                *valueToRewrite = Read(AddressBus);
                (*cycle) = 0;
                break;
            }
        }
        case ADDR_ZERO_PAGE_Y:
        {
            if(*cycle == 1)
            {
                AddressBus = Read(NES.CPU.PCTemp);
                NES.CPU.PCTemp++;
                (*cycle)++;
                break;
            }
            if(*cycle == 2)
            {
                Read(AddressBus);
                AddressBus += NES.CPU.Y;
                AddressBus &= 255;
                (*cycle)++;
                break;
            }
            if(*cycle == 3)
            {
                *valueToRewrite = Read(AddressBus);
                (*cycle) = 0;
                break;
            }
        }
        case ADDR_IMMEDIATE:
        {
            *valueToRewrite = Read(NES.CPU.PCTemp);
            NES.CPU.InstructionCycle = 0;
            NES.CPU.PCTemp++;
            break;
        }
        case ADDR_ABSOLUTE:
        {
            if(*cycle == 1)
            {
                AddressBus = Read(NES.CPU.PCTemp);
                NES.CPU.PCTemp++;
                (*cycle)++;
                break;
            }
            if(*cycle == 2)
            {
                AddressBus |= (Read(NES.CPU.PCTemp) << 8);
                NES.CPU.PCTemp++;
                (*cycle)++;
                break;
            }
            if(*cycle == 3)
            {
                *valueToRewrite = Read(AddressBus);
                (*cycle) = 0;
                break;
            }
        } break;
        case ADDR_ABSOLUTE_X:
        {
            if(*cycle == 1)
            {
                AddressBus = Read(NES.CPU.PCTemp);
                NES.CPU.PCTemp++;
                (*cycle)++;
                break;
            }
            if(*cycle == 2)
            {
                AddressBus |= (Read(NES.CPU.PCTemp) << 8);
                if((AddressBus >> 8) != ((AddressBus+NES.CPU.X) >> 8))
                {
                    AddressCarry = true; //Crossing pages takes one clock cycle
                }
                else
                {
                    AddressCarry = false;
                }
                AddressBus += NES.CPU.X;
                NES.CPU.PCTemp++;
                (*cycle)++;
                break;
            }
            if(*cycle == 3)
            {
                if(!AddressCarry)
                {
                    *valueToRewrite = Read(AddressBus);
                    (*cycle) = 0;
                    break;
                }
                Read(AddressBus - 256);
                (*cycle)++;
                break;
            }
            if(*cycle == 4)
            {
                *valueToRewrite = Read(AddressBus);
                (*cycle) = 0;
                break;
            }
        }
        case ADDR_ABSOLUTE_Y:
        {
            if(*cycle == 1)
            {
                AddressBus = Read(NES.CPU.PCTemp);
                NES.CPU.PCTemp++;
                (*cycle)++;
                break;
            }
            if(*cycle == 2)
            {
                AddressBus |= (Read(NES.CPU.PCTemp) << 8);
                if((AddressBus >> 8) != ((AddressBus+NES.CPU.Y) >> 8))
                {
                    AddressCarry = true; //Crossing pages takes one clock cycle
                }
                else
                {
                    AddressCarry = false;
                }
                AddressBus += NES.CPU.Y;
                NES.CPU.PCTemp++;
                (*cycle)++;
                break;
            }
            if(*cycle == 3)
            {
                if(!AddressCarry)
                {
                    *valueToRewrite = Read(AddressBus);
                    (*cycle) = 0;
                    break;
                }
                Read(AddressBus - 256);
                (*cycle)++;
                break;
            }
            if(*cycle == 4)
            {
                *valueToRewrite = Read(AddressBus);
                (*cycle) = 0;
                break;
            }
        }
        case ADDR_INDIRECT_X:
        {
            if(*cycle == 1)
            {
                AddressBus = Read(NES.CPU.PCTemp);
                NES.CPU.PCTemp++;
                (*cycle)++;
                break;
            }
            if(*cycle == 2)
            {
                Read(AddressBus);
                AddressBus += NES.CPU.X;
                (*cycle)++;
                break;
            }
            if(*cycle == 3)
            {
                temp = Read(AddressBus);
                AddressBus++;
                (*cycle)++;
                break;
            }
            if(*cycle == 4)
            {
                temp |= ((Read(AddressBus)) << 8);
                AddressBus = temp;
                (*cycle)++;
                break;
            }
            if(*cycle == 5)
            {
                *valueToRewrite = Read(AddressBus);
                (*cycle) = 0;
                break;
            }
        }
        case ADDR_INDIRECT_Y:
        {
            if(*cycle == 1)
            {
                AddressBus = Read(NES.CPU.PCTemp);
                NES.CPU.PCTemp++;
                (*cycle)++;
                break;
            }
            if(*cycle == 2)
            {
                temp = Read(AddressBus);
                AddressBus++;
                (*cycle)++;
                break;
            }
            if(*cycle == 3)
            {
                temp |= (Read(AddressBus) << 8);
                AddressBus = temp;
                if((AddressBus >> 8) != ((AddressBus + NES.CPU.Y) >> 8))
                {
                    AddressCarry = true;
                }
                else
                {
                    AddressCarry = false;
                }
                AddressBus += NES.CPU.Y;
                (*cycle)++;
                break;
            }
            if(*cycle == 4)
            {
                if(!AddressCarry)
                {
                    *valueToRewrite = Read(AddressBus);
                    (*cycle) = 0;
                    break;
                }
                Read(AddressBus - 256);
                (*cycle)++;
                break;
            }
            if(*cycle == 5)
            {
                *valueToRewrite = Read(AddressBus);
                (*cycle) = 0;
                break;
            }
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
    if((address >= 0x8000) && (address < 0xC000))
    {
        NES.CartridgeMemory[address - 0x8000] = valueToWrite;
    }
    if(address >= 0xC000)
    {
        if(this->Mapper == 0)
        {
            if(PRGROMSize == 16384)
            {
                NES.CartridgeMemory[address - 0xC000] = valueToWrite;
            }
            if(PRGROMSize == 32768)
            {
                NES.CartridgeMemory[address - 0x8000] = valueToWrite;
            }
        }
    }
}
void MemoryClass::Write(unsigned char valueToWrite,
                        unsigned char* cycle,
                        unsigned char addrMode)
{
    switch(addrMode)
    {
        case ADDR_IMMEDIATE:
        {
        } break;
        case ADDR_ZERO_PAGE:
        {
            if(*cycle == 1)
            {
                AddressBus = Read(NES.CPU.PCTemp);
                NES.CPU.PCTemp++;
                (*cycle)++;
                break;
            }
            if(*cycle == 2)
            {
                Write(AddressBus, valueToWrite);
                (*cycle) = 0;
                break;
            }
        } break;
        case ADDR_ZERO_PAGE_X:
        {
            if(*cycle == 1)
            {
                AddressBus = Read(NES.CPU.PCTemp);
                NES.CPU.PCTemp++;
                (*cycle)++;
                break;
            }
            if(*cycle == 2)
            {
                Read(AddressBus);
                AddressBus += NES.CPU.X;
                AddressBus &= 255;
                (*cycle)++;
                break;
            }
            if(*cycle == 3)
            {
                Write(AddressBus, valueToWrite);
                (*cycle) = 0;
                break;
            }
        }
        case ADDR_ZERO_PAGE_Y:
        {
            if(*cycle == 1)
            {
                AddressBus = Read(NES.CPU.PCTemp);
                NES.CPU.PCTemp++;
                (*cycle)++;
                break;
            }
            if(*cycle == 2)
            {
                Read(AddressBus);
                AddressBus += NES.CPU.Y;
                AddressBus &= 255;
                (*cycle)++;
                break;
            }
            if(*cycle == 3)
            {
                Write(AddressBus, valueToWrite);
                (*cycle) = 0;
                break;
            }
        }
        case ADDR_ABSOLUTE:
        {
            if(*cycle == 1)
            {
                AddressBus = Read(NES.CPU.PCTemp);
                NES.CPU.PCTemp++;
                (*cycle)++;
                break;
            }
            if(*cycle == 2)
            {
                AddressBus |= (Read(NES.CPU.PCTemp) << 8);
                NES.CPU.PCTemp++;
                (*cycle)++;
                break;
            }
            if(*cycle == 3)
            {
                Write(AddressBus, valueToWrite);
                (*cycle) = 0;
                break;
            }
        }
        case ADDR_ABSOLUTE_X:
        {
            if(*cycle == 1)
            {
                AddressBus = Read(NES.CPU.PCTemp);
                NES.CPU.PCTemp++;
                (*cycle)++;
                break;
            }
            if(*cycle == 2)
            {
                AddressBus |= (Read(NES.CPU.PCTemp) << 8);
                AddressBus += NES.CPU.X;
                NES.CPU.PCTemp++;
                (*cycle)++;
                break;
            }
            if(*cycle == 3)
            {
                Read(AddressBus - 256);
                (*cycle)++;
                break;
            }
            if(*cycle == 4)
            {
                Write(AddressBus, valueToWrite);
                (*cycle) = 0;
                break;
            }
        }
        case ADDR_ABSOLUTE_Y:
        {
            if(*cycle == 1)
            {
                AddressBus = Read(NES.CPU.PCTemp);
                NES.CPU.PCTemp++;
                (*cycle)++;
                break;
            }
            if(*cycle == 2)
            {
                AddressBus |= (Read(NES.CPU.PCTemp) << 8);
                AddressBus += NES.CPU.Y;
                NES.CPU.PCTemp++;
                (*cycle)++;
                break;
            }
            if(*cycle == 3)
            {
                Read(AddressBus - 256);
                (*cycle)++;
                break;
            }
            if(*cycle == 4)
            {
                Write(AddressBus, valueToWrite);
                (*cycle) = 0;
                break;
            }
        }
        case ADDR_INDIRECT_X:
        {
            if(*cycle == 1)
            {
                AddressBus = Read(NES.CPU.PCTemp);
                NES.CPU.PCTemp++;
                (*cycle)++;
                break;
            }
            if(*cycle == 2)
            {
                Read(AddressBus);
                AddressBus += NES.CPU.X;
                (*cycle)++;
                break;
            }
            if(*cycle == 3)
            {
                temp = Read(AddressBus);
                AddressBus++;
                (*cycle)++;
                break;
            }
            if(*cycle == 4)
            {
                temp |= ((Read(AddressBus)) << 8);
                AddressBus = temp;
                (*cycle)++;
                break;
            }
            if(*cycle == 5)
            {
                Write(AddressBus, valueToWrite);
                (*cycle) = 0;
                break;
            }
        }
        case ADDR_INDIRECT_Y:
        {
            if(*cycle == 1)
            {
                AddressBus = Read(NES.CPU.PCTemp);
                NES.CPU.PCTemp++;
                (*cycle)++;
                break;
            }
            if(*cycle == 2)
            {
                temp = Read(AddressBus);
                AddressBus++;
                (*cycle)++;
                break;
            }
            if(*cycle == 3)
            {
                temp |= (Read(AddressBus) << 8);
                AddressBus = temp;
                AddressBus += NES.CPU.Y;
                (*cycle)++;
                break;
            }
            if(*cycle == 4)
            {
                Read(AddressBus - 256);
                (*cycle)++;
                break;
            }
            if(*cycle == 5)
            {
                Write(AddressBus, valueToWrite);
                (*cycle) = 0;
                break;
            }
        }
    }
}
