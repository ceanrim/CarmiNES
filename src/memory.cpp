/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Carmine Foggia $
   ======================================================================== */
#include "memory.h"
#include "cpu.h"
#include "main.h"
unsigned short Memory::AddressBus;
unsigned short Memory::temp;
unsigned short Memory::Mapper;
unsigned short Memory::PRGROMSize;
bool Memory::AddressCarry = false;
unsigned char Memory::ConversionTable[] = {ADDR_IMMEDIATE, ADDR_ZERO_PAGE, 0,
                                           ADDR_ABSOLUTE, 0, ADDR_ZERO_PAGE_X,
                                           0, ADDR_ABSOLUTE_X};
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
    if((Address >= 0x8000) && (Address < 0xC000))
    {
        return globals::CartridgeMemory[Address - 0x8000];
    }
    if(Address >= 0xC000)
    {
        if(Memory::Mapper == 0)
        {
            if(PRGROMSize == 16384)
            {
                return globals::CartridgeMemory[Address - 0xC000];
            }
            if(PRGROMSize == 32768)
            {
                return globals::CartridgeMemory[Address - 0x8000];
            }
        }
    }
}

unsigned char Memory::ReadWithNoSideEffects(unsigned short Address)
/*NES memory structure:
  0x0000-0x07ff Internal Memory
  0x0800-0x1fff Mirrors three times 0x0000-0x07ff
  0x8000-0xffff Cartridge space
  ...To be completed...*/
{
    if(Address < 0x2000)
    {
        return globals::InternalMemory[Address % 0x800];
    }
    if((Address >= 0x8000) && (Address < 0xC000))
    {
        return globals::CartridgeMemory[Address - 0x8000];
    }
    if(Address >= 0xC000)
    {
        if(Memory::Mapper == 0)
        {
            if(PRGROMSize == 16384)
            {
                return globals::CartridgeMemory[Address - 0xC000];
            }
            if(PRGROMSize == 32768)
            {
                return globals::CartridgeMemory[Address - 0x8000];
            }
        }
    }
}
void Memory::Read(unsigned short address,
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
                AddressBus = Read(CPU::PCTemp);
                CPU::PCTemp++;
                (*cycle)++;
                break;
            }
            if(*cycle == 2)
            {
                *valueToRewrite = Memory::Read(AddressBus);
                (*cycle) = 0;
                break;
            }
        }
        case ADDR_ZERO_PAGE_X:
        {
            if(*cycle == 1)
            {
                AddressBus = Read(CPU::PCTemp);
                CPU::PCTemp++;
                (*cycle)++;
                break;
            }
            if(*cycle == 2)
            {
                Read(AddressBus);
                AddressBus += CPU::X;
                AddressBus &= 255;
                (*cycle)++;
                break;
            }
            if(*cycle == 3)
            {
                *valueToRewrite = Memory::Read(AddressBus);
                (*cycle) = 0;
                break;
            }
        }
        case ADDR_ZERO_PAGE_Y:
        {
            if(*cycle == 1)
            {
                AddressBus = Read(CPU::PCTemp);
                CPU::PCTemp++;
                (*cycle)++;
                break;
            }
            if(*cycle == 2)
            {
                Read(AddressBus);
                AddressBus += CPU::Y;
                AddressBus &= 255;
                (*cycle)++;
                break;
            }
            if(*cycle == 3)
            {
                *valueToRewrite = Memory::Read(AddressBus);
                (*cycle) = 0;
                break;
            }
        }
        case ADDR_IMMEDIATE:
        {
            *valueToRewrite = Memory::Read(CPU::PCTemp);
            CPU::InstructionCycle = 0;
            CPU::PCTemp++;
            break;
        }
        case ADDR_ABSOLUTE:
        {
            if(*cycle == 1)
            {
                AddressBus = Memory::Read(CPU::PCTemp);
                CPU::PCTemp++;
                (*cycle)++;
                break;
            }
            if(*cycle == 2)
            {
                AddressBus |= (Memory::Read(CPU::PCTemp) << 8);
                CPU::PCTemp++;
                (*cycle)++;
                break;
            }
            if(*cycle == 3)
            {
                *valueToRewrite = Memory::Read(AddressBus);
                (*cycle) = 0;
                break;
            }
        } break;
        case ADDR_ABSOLUTE_X:
        {
            if(*cycle == 1)
            {
                AddressBus = Memory::Read(CPU::PCTemp);
                CPU::PCTemp++;
                (*cycle)++;
                break;
            }
            if(*cycle == 2)
            {
                AddressBus |= (Memory::Read(CPU::PCTemp) << 8);
                if((AddressBus >> 8) != ((AddressBus+CPU::X) >> 8))
                {
                    AddressCarry = true; //Crossing pages takes one clock cycle
                }
                else
                {
                    AddressCarry = false;
                }
                AddressBus += CPU::X;
                CPU::PCTemp++;
                (*cycle)++;
                break;
            }
            if(*cycle == 3)
            {
                if(!AddressCarry)
                {
                    *valueToRewrite = Memory::Read(AddressBus);
                    (*cycle) = 0;
                    break;
                }
                Memory::Read(AddressBus - 256);
                (*cycle)++;
                break;
            }
            if(*cycle == 4)
            {
                *valueToRewrite = Memory::Read(AddressBus);
                (*cycle) = 0;
                break;
            }
        }
        case ADDR_ABSOLUTE_Y:
        {
            if(*cycle == 1)
            {
                AddressBus = Memory::Read(CPU::PCTemp);
                CPU::PCTemp++;
                (*cycle)++;
                break;
            }
            if(*cycle == 2)
            {
                AddressBus |= (Memory::Read(CPU::PCTemp) << 8);
                if((AddressBus >> 8) != ((AddressBus+CPU::Y) >> 8))
                {
                    AddressCarry = true; //Crossing pages takes one clock cycle
                }
                else
                {
                    AddressCarry = false;
                }
                AddressBus += CPU::Y;
                CPU::PCTemp++;
                (*cycle)++;
                break;
            }
            if(*cycle == 3)
            {
                if(!AddressCarry)
                {
                    *valueToRewrite = Memory::Read(AddressBus);
                    (*cycle) = 0;
                    break;
                }
                Memory::Read(AddressBus - 256);
                (*cycle)++;
                break;
            }
            if(*cycle == 4)
            {
                *valueToRewrite = Memory::Read(AddressBus);
                (*cycle) = 0;
                break;
            }
        }
        case ADDR_INDIRECT_X:
        {
            if(*cycle == 1)
            {
                AddressBus = Memory::Read(CPU::PCTemp);
                CPU::PCTemp++;
                (*cycle)++;
                break;
            }
            if(*cycle == 2)
            {
                Memory::Read(AddressBus);
                AddressBus += CPU::X;
                (*cycle)++;
                break;
            }
            if(*cycle == 3)
            {
                temp = Memory::Read(AddressBus);
                AddressBus++;
                (*cycle)++;
                break;
            }
            if(*cycle == 4)
            {
                temp |= ((Memory::Read(AddressBus)) << 8);
                AddressBus = temp;
                (*cycle)++;
                break;
            }
            if(*cycle == 5)
            {
                *valueToRewrite = Memory::Read(AddressBus);
                (*cycle) = 0;
                break;
            }
        }
        case ADDR_INDIRECT_Y:
        {
            if(*cycle == 1)
            {
                AddressBus = Memory::Read(CPU::PCTemp);
                CPU::PCTemp++;
                (*cycle)++;
                break;
            }
            if(*cycle == 2)
            {
                temp = Memory::Read(AddressBus);
                AddressBus++;
                (*cycle)++;
                break;
            }
            if(*cycle == 3)
            {
                temp |= (Memory::Read(AddressBus) << 8);
                AddressBus = temp;
                if((AddressBus >> 8) != ((AddressBus + CPU::Y) >> 8))
                {
                    AddressCarry = true;
                }
                else
                {
                    AddressCarry = false;
                }
                AddressBus += CPU::Y;
                (*cycle)++;
                break;
            }
            if(*cycle == 4)
            {
                if(!AddressCarry)
                {
                    *valueToRewrite = Memory::Read(AddressBus);
                    (*cycle) = 0;
                    break;
                }
                Memory::Read(AddressBus - 256);
                (*cycle)++;
                break;
            }
            if(*cycle == 5)
            {
                *valueToRewrite = Memory::Read(AddressBus);
                (*cycle) = 0;
                break;
            }
        }
    }
}
void Memory::Write(unsigned short address,
                   unsigned char valueToWrite)
{
    if(address < 0x2000)
    {
        globals::InternalMemory[address % 0x800] = valueToWrite;
    }
    if((address >= 0x8000) && (address < 0xC000))
    {
        globals::CartridgeMemory[address - 0x8000] = valueToWrite;
    }
    if(address >= 0xC000)
    {
        if(Memory::Mapper == 0)
        {
            if(PRGROMSize == 16384)
            {
                globals::CartridgeMemory[address - 0xC000] = valueToWrite;
            }
            if(PRGROMSize == 32768)
            {
                globals::CartridgeMemory[address - 0x8000] = valueToWrite;
            }
        }
    }
}
void Memory::Write(unsigned char valueToWrite,
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
                AddressBus = Read(CPU::PCTemp);
                CPU::PCTemp++;
                (*cycle)++;
                break;
            }
            if(*cycle == 2)
            {
                Memory::Write(AddressBus, valueToWrite);
                (*cycle) = 0;
                break;
            }
        } break;
        case ADDR_ZERO_PAGE_X:
        {
            if(*cycle == 1)
            {
                AddressBus = Read(CPU::PCTemp);
                CPU::PCTemp++;
                (*cycle)++;
                break;
            }
            if(*cycle == 2)
            {
                Read(AddressBus);
                AddressBus += CPU::X;
                AddressBus &= 255;
                (*cycle)++;
                break;
            }
            if(*cycle == 3)
            {
                Memory::Write(AddressBus, valueToWrite);
                (*cycle) = 0;
                break;
            }
        }
        case ADDR_ZERO_PAGE_Y:
        {
            if(*cycle == 1)
            {
                AddressBus = Read(CPU::PCTemp);
                CPU::PCTemp++;
                (*cycle)++;
                break;
            }
            if(*cycle == 2)
            {
                Read(AddressBus);
                AddressBus += CPU::Y;
                AddressBus &= 255;
                (*cycle)++;
                break;
            }
            if(*cycle == 3)
            {
                Memory::Write(AddressBus, valueToWrite);
                (*cycle) = 0;
                break;
            }
        }
        case ADDR_ABSOLUTE:
        {
            if(*cycle == 1)
            {
                AddressBus = Memory::Read(CPU::PCTemp);
                CPU::PCTemp++;
                (*cycle)++;
                break;
            }
            if(*cycle == 2)
            {
                AddressBus |= (Memory::Read(CPU::PCTemp) << 8);
                CPU::PCTemp++;
                (*cycle)++;
                break;
            }
            if(*cycle == 3)
            {
                Memory::Write(AddressBus, valueToWrite);
                (*cycle) = 0;
                break;
            }
        }
        case ADDR_ABSOLUTE_X:
        {
            if(*cycle == 1)
            {
                AddressBus = Memory::Read(CPU::PCTemp);
                CPU::PCTemp++;
                (*cycle)++;
                break;
            }
            if(*cycle == 2)
            {
                AddressBus |= (Memory::Read(CPU::PCTemp) << 8);
                AddressBus += CPU::X;
                CPU::PCTemp++;
                (*cycle)++;
                break;
            }
            if(*cycle == 3)
            {
                Memory::Read(AddressBus - 256);
                (*cycle)++;
                break;
            }
            if(*cycle == 4)
            {
                Memory::Write(AddressBus, valueToWrite);
                (*cycle) = 0;
                break;
            }
        }
        case ADDR_ABSOLUTE_Y:
        {
            if(*cycle == 1)
            {
                AddressBus = Memory::Read(CPU::PCTemp);
                CPU::PCTemp++;
                (*cycle)++;
                break;
            }
            if(*cycle == 2)
            {
                AddressBus |= (Memory::Read(CPU::PCTemp) << 8);
                AddressBus += CPU::Y;
                CPU::PCTemp++;
                (*cycle)++;
                break;
            }
            if(*cycle == 3)
            {
                Memory::Read(AddressBus - 256);
                (*cycle)++;
                break;
            }
            if(*cycle == 4)
            {
                Memory::Write(AddressBus, valueToWrite);
                (*cycle) = 0;
                break;
            }
        }
        case ADDR_INDIRECT_X:
        {
            if(*cycle == 1)
            {
                AddressBus = Memory::Read(CPU::PCTemp);
                CPU::PCTemp++;
                (*cycle)++;
                break;
            }
            if(*cycle == 2)
            {
                Memory::Read(AddressBus);
                AddressBus += CPU::X;
                (*cycle)++;
                break;
            }
            if(*cycle == 3)
            {
                temp = Memory::Read(AddressBus);
                AddressBus++;
                (*cycle)++;
                break;
            }
            if(*cycle == 4)
            {
                temp |= ((Memory::Read(AddressBus)) << 8);
                AddressBus = temp;
                (*cycle)++;
                break;
            }
            if(*cycle == 5)
            {
                Memory::Write(AddressBus, valueToWrite);
                (*cycle) = 0;
                break;
            }
        }
        case ADDR_INDIRECT_Y:
        {
            if(*cycle == 1)
            {
                AddressBus = Memory::Read(CPU::PCTemp);
                CPU::PCTemp++;
                (*cycle)++;
                break;
            }
            if(*cycle == 2)
            {
                temp = Memory::Read(AddressBus);
                AddressBus++;
                (*cycle)++;
                break;
            }
            if(*cycle == 3)
            {
                temp |= (Memory::Read(AddressBus) << 8);
                AddressBus = temp;
                AddressBus += CPU::Y;
                (*cycle)++;
                break;
            }
            if(*cycle == 4)
            {
                Memory::Read(AddressBus - 256);
                (*cycle)++;
                break;
            }
            if(*cycle == 5)
            {
                Memory::Write(AddressBus, valueToWrite);
                (*cycle) = 0;
                break;
            }
        }
    }
}
