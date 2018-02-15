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
bool Memory::AddressCarry = false;
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
                AddressBus = Read(CPU::PC);
                CPU::PC++;
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
                AddressBus = Read(CPU::PC);
                CPU::PC++;
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
        case ADDR_IMMEDIATE:
        {
            *valueToRewrite = Memory::Read(CPU::PC);
            CPU::InstructionCycle = 0;
            CPU::PC++;
            break;
        }
        case ADDR_ABSOLUTE:
        {
            if(*cycle == 1)
            {
                AddressBus = Memory::Read(CPU::PC);
                CPU::PC++;
                (*cycle)++;
                break;
            }
            if(*cycle == 2)
            {
                AddressBus |= (Memory::Read(CPU::PC) << 8);
                CPU::PC++;
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
                AddressBus = Memory::Read(CPU::PC);
                CPU::PC++;
                (*cycle)++;
                break;
            }
            if(*cycle == 2)
            {
                AddressBus |= (Memory::Read(CPU::PC) << 8);
                if((AddressBus >> 8) != ((AddressBus+CPU::X) >> 8))
                {
                    AddressCarry = true; //Crossing pages takes one clock cycle
                }
                else
                {
                    AddressCarry = false;
                }
                AddressBus += CPU::X;
                CPU::PC++;
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
                AddressBus = Memory::Read(CPU::PC);
                CPU::PC++;
                (*cycle)++;
                break;
            }
            if(*cycle == 2)
            {
                AddressBus |= (Memory::Read(CPU::PC) << 8);
                if((AddressBus >> 8) != ((AddressBus+CPU::Y) >> 8))
                {
                    AddressCarry = true; //Crossing pages takes one clock cycle
                }
                else
                {
                    AddressCarry = false;
                }
                AddressBus += CPU::Y;
                CPU::PC++;
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
                AddressBus = Memory::Read(CPU::PC);
                CPU::PC++;
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
                AddressBus = Memory::Read(CPU::PC);
                CPU::PC++;
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
        default: //TODO
        {
            Log("Tried to read memory in a way that has not been implemented");
            return;
        }
    }
}
