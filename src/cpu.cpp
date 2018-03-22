/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Carmine Foggia $
   ======================================================================== */
#include "memory.h"
#include "main.h"
#include "cpu.h"
//NOTE to Carmine: You're working on the stack
void CPUClass::RunCycle(unsigned char FuncCurrentInstruction, unsigned char FuncInstructionCycle)
{
    switch(FuncCurrentInstruction)
    {
        case 0x18: //CLC
        {
            P &= 0b11111110;
            InstructionCycle = 0;
        } break;
        case 0x38: //SEC
        {
            P |= 1;
            InstructionCycle = 0;
        } break;
        case 0x58: //CLI
        {
            P &= 0b11111011;
            InstructionCycle = 0;
        } break;
        case 0x78: //SEI
        {
            P |= 4;
            InstructionCycle = 0;
        } break;
        case 0xB8: //CLV
        {
            P &= 0b10111111;
            InstructionCycle = 0;
        } break;
        case 0xD8: //CLD
        {
            P &= 0b11110111;
            InstructionCycle = 0;
        } break;
        case 0xF8: //SED
        {
            P |= 8;
            InstructionCycle = 0;
        } break;
        case 0xA0: case 0xA4: case 0xAC: case 0xB4: case 0xBC: //LDY
        {
            unsigned char addrMode = NES.RAM.ConversionTable
                [(((unsigned char)(FuncCurrentInstruction &
                                   (unsigned char)0b00011100)) >> 2)];
            NES.RAM.Read((unsigned short)0, &InstructionCycle,
                         addrMode,
                         &Y);
            if(InstructionCycle == 0)
            {
                if(Y & 128)
                {
                    P |= 128;
                }
                else
                {
                    P &= 127;
                }
                if(Y == 0)
                {
                    P |= 2;
                }
                else
                {
                    P &= 0b11111101;
                }
            }
        } break;
        case 0x4C: //JMP XXXX
        {
            if(InstructionCycle == 1)
            {
                AddressBus = NES.RAM.Read(PCTemp);
                PCTemp++;
                InstructionCycle++;
            }
            else if(InstructionCycle == 2)
            {
                AddressBus |= (NES.RAM.Read(PCTemp) << 8);
                PCTemp = AddressBus;
                InstructionCycle = 0;
            }
            break;
        }
        case 0x6C: //JMP (XXXX)
        {
            if(InstructionCycle == 1)
            {
                AddressBus = NES.RAM.Read(PCTemp);
                PCTemp++;
                InstructionCycle++;
            }
            else if(InstructionCycle == 2)
            {
                AddressBus |= (NES.RAM.Read(PCTemp) << 8);
                InstructionCycle++;
            }
            else if(InstructionCycle == 3)
            {
                temp = AddressBus;
                AddressBus &= NES.RAM.Read(temp);
                InstructionCycle++;
            }
            else if(InstructionCycle == 4)
            {
                AddressBus |=
                    (NES.RAM.Read(temp+1) << 8);
                PCTemp = AddressBus;
                InstructionCycle = 0;
            }
            break;
        }
        case 0x24: //BIT xx
        {
            unsigned char temp;
            NES.RAM.Read((unsigned short)0, &InstructionCycle,
                         ADDR_ZERO_PAGE, &temp);
            if(InstructionCycle == 0)
            {
                unsigned char a = A & temp;
                if(a & 0b10000000)
                {
                    P |= 0b10000000;
                }
                else
                {
                    P &= 0b01111111;
                }
                if(a & 0b01000000)
                {
                    P |= 0b01000000;
                }
                else
                {
                    P &= 0b10111111;
                }
                if(!a)
                {
                    P |= 0b00000010;
                }
                else
                {
                    P &= 0b11111101;
                }
            }
            break;
        }
        case 0x2c: //BIT xxxx
        {
            unsigned char temp;
            NES.RAM.Read((unsigned short)0, &InstructionCycle,
                         ADDR_ABSOLUTE, &temp);
            if(InstructionCycle == 0)
            {
                unsigned char a = A & temp;
                if(temp & 0b10000000)
                {
                    P |= 0b10000000;
                }
                else
                {
                    P &= 0b01111111;
                }
                if(a & 0b01000000)
                {
                    P |= 0b01000000;
                }
                else
                {
                    P &= 0b10111111;
                }
                if(!a)
                {
                    P |= 0b00000010;
                }
                else
                {
                    P &= 0b11111101;
                }
            }
            break;
        }
        case 0x84: //STY xx
        case 0x8C: //STY xxxx
        case 0x94: //STY xx+X
        {
            unsigned char addrMode = NES.RAM.ConversionTable
                [(((unsigned char)(FuncCurrentInstruction &
                                   (unsigned char) 0b00011100)) >> 2)];
            NES.RAM.Write(Y, &InstructionCycle, addrMode);
            break;
        }
        case 0xC0: //CPY #xx
        case 0xC4: //CPY xx
        case 0xCC: //CPY xxxx
        {
            CMP(&Y, NES.RAM.ConversionTable
                [(((unsigned char)(FuncCurrentInstruction &
                                   (unsigned char) 0b00011100)) >> 2)]);
            break;
        }
        case 0xE0: //CPX #xx
        case 0xE4: //CPX xx
        case 0xEC: //CPX xxxx
        {
            CMP(&X, NES.RAM.ConversionTable
                [(((unsigned char)(FuncCurrentInstruction &
                                   (unsigned char)0b00011100)) >> 2)]);
            break;
        }
        case 0xA8: //TAY
        {
            Y = A;
            InstructionCycle = 0;
            if(Y & 128)
            {
                P |= 128;
            }
            else
            {
                P &= 127;
            }
            if(Y == 0)
            {
                P |= 2;
            }
            else
            {
                P &= 0b11111101;
            }
            break;
        }
        case 0x98: //TYA
        {
            A = Y;
            InstructionCycle = 0;
            if(A & 128)
            {
                P |= 128;
            }
            else
            {
                P &= 127;
            }
            if(A == 0)
            {
                P |= 2;
            }
            else
            {
                P &= 0b11111101;
            }
            break;
        }
        case 0x90: //BCC
        {
            Branch(1, true);
            break;
        }
        case 0xB0: //BCS
        {
            Branch(1, false);
            break;
        }
        case 0xD0: //BNE
        {
            Branch(2, true);
            break;
        }
        case 0xF0: //BEQ
        {
            Branch(2, false);
            break;
        }
        case 0x10: //BPL
        {
            Branch(128, true);
            break;
        }
        case 0x30: //BMI
        {
            Branch(128, false);
            break;
        }
        case 0x50: //BVC
        {
            Branch(64, true);
            break;
        }
        case 0x70: //BVS
        {
            Branch(64, false);
            break;
        }
        case 0xE8: //INX
        {
            X++;
            UpdateFlags(&X);
            InstructionCycle = 0;
            break;
        }
        case 0xC8: //INY
        {
            Y++;
            UpdateFlags(&Y);
            InstructionCycle = 0;
            break;
        }
        case 0x88: //DEY
        {
            Y--;
            UpdateFlags(&Y);
            InstructionCycle = 0;
            break;
        }
        case 0x80: //NOP #xx
        {
            NOP(ADDR_IMMEDIATE);
            break;
        }
        case 0x04:
        case 0x44:
        case 0x64: //NOP xx
        {
            NOP(ADDR_ZERO_PAGE);
            break;
        }
        case 0x0C: //NOP xxxx
        {
            NOP(ADDR_ABSOLUTE);
            break;
        }
        case 0x14:
        case 0x34:
        case 0x54:
        case 0x74:
        case 0xD4:
        case 0xF4: //NOP xx+X
        {
            NOP(ADDR_ZERO_PAGE_X);
            break;
        }
        case 0x1C:
        case 0x3C:
        case 0x5C:
        case 0x7C:
        case 0xDC:
        case 0xFC: //NOP xxxx+X
        {
            NOP(ADDR_ABSOLUTE_X);
            break;
        }
        case 0x48:  //PHA
        {
            Push(A);
            break;
        }
        case 0x08: //PHP
        {
            Push(P);
            break;
        }
        case 0x68: //PLA
        {
            Pull(&A);
            if(!InstructionCycle)
            {
                UpdateFlags(&A);
            }
            break;
        }
        case 0x28: //PLP
        {
            Pull(&P);
            break;
        }
        case 0x20: //JSR xxxx
        {
            if(InstructionCycle == 1)
            {
                AddressBus = NES.RAM.Read(PCTemp);
                PCTemp++;
                InstructionCycle++;
            }
            else if(InstructionCycle == 2)
            {
                AddressBus |= (NES.RAM.Read(PCTemp) << 8);
                InstructionCycle++;
            }
            else if(InstructionCycle == 3)
            {
                NES.RAM.Write(0x100+S, (unsigned char)AddressBus);
                InstructionCycle++;
            }
            else if(InstructionCycle == 4)
            {
                NES.RAM.Write(0x100+S, (*((unsigned char*)(&PCTemp)+1)));
                S--;
                InstructionCycle++;
            }
            else if(InstructionCycle == 5)
            {
                NES.RAM.Write(0x100+S, (unsigned char)PCTemp);
                S--;
                InstructionCycle++;
            }
            else if(InstructionCycle == 6)
            {
                PCTemp = AddressBus;
                InstructionCycle = 0;
            }
            break;
        }
        case 0x60: //RTS
        {
            if(InstructionCycle < 3)
            {
                InstructionCycle++;
            }
            else if(InstructionCycle == 3)
            {
                S++;
                PCTemp = NES.RAM.Read(0x100+S);
                InstructionCycle++;
            }
            else if(InstructionCycle == 4)
            {
                S++;
                PCTemp |= (NES.RAM.Read(0x100+S) << 8);
                InstructionCycle++;
            }
            else if(InstructionCycle == 5)
            {
                PCTemp++;
                InstructionCycle = 0;
            }
            break;
        }
        case 0x00: //BRK
        {
            if(InstructionCycle == 1)
            {
                InstructionCycle++;
            }
            else if(InstructionCycle == 2)
            {
                unsigned short PCToPush = PCTemp + 1;
                NES.RAM.Write(0x100+S, (*((unsigned char*)(&PCToPush)+1)));
                S--;
                InstructionCycle++;
            }
            else if(InstructionCycle == 3)
            {
                unsigned short PCToPush = PCTemp + 1;
                NES.RAM.Write(0x100+S, (unsigned char)(PCToPush));
                S--;
                InstructionCycle++;
            }
            else if(InstructionCycle == 4)
            {
                NES.RAM.Write(0x100+S, P|0b00110000);
                S--;
                InstructionCycle++;
            }
            else if(InstructionCycle == 5)
            {
                PCTemp = NES.RAM.Read(0xFFFE);
                InstructionCycle++;
            }
            else if(InstructionCycle == 6)
            {
                PCTemp |= (unsigned short)(NES.RAM.Read(0xFFFF) << 8);
                P &= 0b11111011;
                InstructionCycle = 0;
            }
            break;
        }
        case 0x40: //RTI
        {
            if(InstructionCycle < 3)
            {
                InstructionCycle++;
            }
            else if(InstructionCycle == 3)
            {
                S++;
                P = (NES.RAM.Read(0x100+S) & 0b11001111);
                InstructionCycle++;
            }
            else if(InstructionCycle == 4)
            {
                S++;
                PCTemp = NES.RAM.Read(0x100+S);
                InstructionCycle++;
            }
            else if(InstructionCycle == 5)
            {
                S++;
                PCTemp |= (NES.RAM.Read(0x100+S) << 8);
                InstructionCycle = 0;
            }
            break;
        }
        case 0x09: //ORA #xx
        case 0x05: //ORA xx
        case 0x15: //ORA xx+X
        case 0x0D: //ORA xxxx
        case 0x1D: //ORA xxxx+X
        case 0x19: //ORA xxxx+Y
        case 0x01: //ORA (xx+X)
        case 0x11: //ORA (xx)+Y
        {
            unsigned char temp;
            NES.RAM.Read((unsigned short)0, &InstructionCycle,
                         (unsigned char)(FuncCurrentInstruction &
                                         (unsigned char)0b00011100),
                         &temp);
            if(InstructionCycle == 0)
            {
                A |= temp;
            }
            if(InstructionCycle == 0)
            {
                if(A & 128)
                {
                    P |= 128;
                }
                else
                {
                    P &= 127;
                }
                if(A == 0)
                {
                    P |= 2;
                }
                else
                {
                    P &= 0b11111101;
                }
            }
        } break;        
        case 0x29: //AND #xx
        case 0x25: //AND xx
        case 0x35: //AND xx+X
        case 0x2D: //AND xxxx
        case 0x3D: //AND xxxx+X
        case 0x39: //AND xxxx+Y
        case 0x21: //AND (xx+X)
        case 0x31: //AND (xx)+Y
        {
            unsigned char temp;
            NES.RAM.Read((unsigned short)0, &InstructionCycle,
                         (unsigned char)(FuncCurrentInstruction &
                                         (unsigned char)0b00011100),
                         &temp);
            if(InstructionCycle == 0)
            {
                A &= temp;
                if(A & 128)
                {
                    P |= 128;
                }
                else
                {
                    P &= 127;
                }
                if(A == 0)
                {
                    P |= 2;
                }
                else
                {
                    P &= 0b11111101;
                }
            }
        } break;        
        case 0x49: //EOR #xx
        case 0x45: //EOR xx
        case 0x55: //EOR xx+X
        case 0x4D: //EOR xxxx
        case 0x5D: //EOR xxxx+X
        case 0x59: //EOR xxxx+Y
        case 0x41: //EOR (xx+X)
        case 0x51: //EOR (xx)+Y
        {
            unsigned char temp;
            NES.RAM.Read((unsigned short)0, &InstructionCycle,
                         (unsigned char)(FuncCurrentInstruction &
                                         (unsigned char)0b00011100),
                         &temp);
            if(InstructionCycle == 0)
            {
                A ^= temp;
                if(A & 128)
                {
                    P |= 128;
                }
                else
                {
                    P &= 127;
                }
                if(A == 0)
                {
                    P |= 2;
                }
                else
                {
                    P &= 0b11111101;
                }
            }
        } break;        
        case 0x69: //ADC #xx
        case 0x65: //ADC xx
        case 0x75: //ADC xx+X
        case 0x6D: //ADC xxxx
        case 0x7D: //ADC xxxx+X
        case 0x79: //ADC xxxx+Y
        case 0x61: //ADC (xx+X)
        case 0x71: //ADC (xx)+Y
        {
            unsigned char temp;
            NES.RAM.Read((unsigned short)0, &InstructionCycle,
                         (unsigned char)(FuncCurrentInstruction &
                                         (unsigned char)0b00011100),
                         &temp);
            if(InstructionCycle == 0)
            {
                bool Carry = false;
                bool Overflow = false;
                if(((unsigned short)A
                    + (unsigned short)temp
                    + (unsigned short)(P & 1)) >= 256)
                {
                    Carry = true;
                }
                if((A^(A+temp+(P & 1))) &
                   (temp^(A+temp+(P & 1))) &
                   0x80)
                {
                    Overflow = true;
                }
                A += temp;
                A += (P & 1);
                if(Carry)
                {
                    P |= 1;
                }
                else
                {
                    P &= 0b11111110;
                }
                if(Overflow)
                {
                    P |= 64;
                }
                else
                {
                    P &= 0b10111111;
                }
                if(A == 0)
                {
                    P |= 2;
                }
                else
                {
                    P &= 0b11111101;
                }
                if(A & 128)
                {
                    P |= 128;
                }
                else
                {
                    P &= 127;
                }
            }
        } break;
        case 0x89: //NOP #xx
        {
            NES.RAM.Read(PCTemp);
            PCTemp++;
            InstructionCycle = 0;
            break;
        }            
        case 0x85: //STA xx
        case 0x95: //STA xx+X
        case 0x8D: //STA xxxx
        case 0x9D: //STA xxxx+X
        case 0x99: //STA xxxx+Y
        case 0x81: //STA (xx+X)
        case 0x91: //STA (xx)+Y
        {
            NES.RAM.Write(A,
                          &InstructionCycle,
                          (unsigned char)
                          (FuncCurrentInstruction &
                           (unsigned char)0b00011100));
        } break;
        case 0xA9: //LDA #xx
        case 0xA5: //LDA xx
        case 0xB5: //LDA xx+X
        case 0xAD: //LDA xxxx
        case 0xBD: //LDA xxxx+X
        case 0xB9: //LDA xxxx+Y
        case 0xA1: //LDA (xx+X)
        case 0xB1: //LDA (xx)+Y
        {                     
            NES.RAM.Read((unsigned short)0, &InstructionCycle,
                         (unsigned char)(FuncCurrentInstruction &
                                         (unsigned char)0b00011100),
                         &A);
            UpdateFlags(&A);
        } break;
        case 0xC9: //CMP #xx
        case 0xC5: //CMP xx
        case 0xD5: //CMP xx+X
        case 0xCD: //CMP xxxx
        case 0xDD: //CMP xxxx+X
        case 0xD9: //CMP xxxx+Y
        case 0xC1: //CMP (xx+X)
        case 0xD1: //CMP (xx)+Y
        {
            CMP(&A, (unsigned char)(FuncCurrentInstruction &
                                    (unsigned char)0b00011100));
        } break;
        case 0xE9: //SBC #xx
        case 0xE5: //SBC xx
        case 0xF5: //SBC xx+X
        case 0xED: //SBC xxxx
        case 0xFD: //SBC xxxx+X
        case 0xF9: //SBC xxxx+Y
        case 0xE1: //SBC (xx+X)
        case 0xF1: //SBC (xx)+Y
        {
                        
            unsigned char temp;
            NES.RAM.Read((unsigned short)0, &InstructionCycle,
                         (unsigned char)(FuncCurrentInstruction &
                                         (unsigned char)0b00011100),
                         &temp);
            if(InstructionCycle == 0)
            {
                bool Carry = false;
                bool Overflow = false;
                if((unsigned short)((unsigned short)A
                                    + (unsigned short)(~temp & 255)
                                    + (unsigned short)(P & 1))
                   >= 256)
                {
                    Carry = true;
                }
                if((A^(A+~temp+(P & 1))) &
                   (~temp^(A+~temp+(P & 1))) &
                   0x80)
                {
                    Overflow = true;
                }
                A += ~temp;
                A += (P & 1);
                if(Carry)
                {
                    P |= 1;
                }
                else
                {
                    P &= 0b11111110;
                }
                if(Overflow)
                {
                    P |= 64;
                }
                else
                {
                    P &= 0b10111111;
                }
                if(A == 0)
                {
                    P |= 2;
                }
                else
                {
                    P &= 0b11111101;
                }
                if(A & 128)
                {
                    P |= 128;
                }
                else
                {
                    P &= 127;
                }
            }
        } break;
                
        case 0xA2: //LDX #xx
        case 0xA6: //LDX xx
        case 0xAE: //LDX xxxx
        case 0xB6: //LDX xx+Y
        case 0xBE: //LDX xxxx+Y
        {
            unsigned char addrMode = NES.RAM.ConversionTable
                [(((unsigned char)(FuncCurrentInstruction &
                                   (unsigned char)0b00011100)) >> 2)];
            if(addrMode == ADDR_ZERO_PAGE_X)
            {
                addrMode = ADDR_ZERO_PAGE_Y;
            }
            if(addrMode == ADDR_ABSOLUTE_X)
            {
                addrMode = ADDR_ABSOLUTE_Y;
            }
            NES.RAM.Read((unsigned short)0, &InstructionCycle,
                         addrMode,
                         &X);
            if(InstructionCycle == 0)
            {
                if(X & 128)
                {
                    P |= 128;
                }
                else
                {
                    P &= 127;
                }
                if(X == 0)
                {
                    P |= 2;
                }
                else
                {
                    P &= 0b11111101;
                }
            }
        } break;
        case 0x86: //STX xx
        case 0x8E: //STX xxxx
        case 0x96: //STX xx+Y
        {
            unsigned char addrMode = NES.RAM.ConversionTable
                [(((unsigned char)(FuncCurrentInstruction &
                                   (unsigned char) 0b00011100)) >> 2)];
            if(addrMode == ADDR_ZERO_PAGE_X)
            {
                addrMode = ADDR_ZERO_PAGE_Y;
            }
            NES.RAM.Write(X, &InstructionCycle,
                          addrMode);
            break;
        }
        case 0xAA: //TAX
        {
            X = A;
            InstructionCycle = 0;
            if(X & 128)
            {
                P |= 128;
            }
            else
            {
                P &= 127;
            }
            if(X == 0)
            {
                P |= 2;
            }
            else
            {
                P &= 0b11111101;
            }
            break;
        }
        case 0x8A: //TXA
        {
            A = X;
            InstructionCycle = 0;
            if(A & 128)
            {
                P |= 128;
            }
            else
            {
                P &= 127;
            }
            if(A == 0)
            {
                P |= 2;
            }
            else
            {
                P &= 0b11111101;
            }
            break;
        }
        case 0xBA: //TSX
        {
            X = S;
            InstructionCycle = 0;
            if(X & 128)
            {
                P |= 128;
            }
            else
            {
                P &= 127;
            }
            if(X == 0)
            {
                P |= 2;
            }
            else
            {
                P &= 0b11111101;
            }
            break;
        }
        case 0x9A: //TXS
        {
            S = X;
            InstructionCycle = 0;
            break;
        }
        case 0x06: //ASL xx
        {
            if(InstructionCycle <= 2)
            {
                int a = InstructionCycle;
                NES.RAM.Read(0, &InstructionCycle,
                             ADDR_ZERO_PAGE,
                             &temp);
                if(a == 2)
                {
                    InstructionCycle = 3;
                }
            }
            else
            {
                ASL(&temp, 3, &InstructionCycle);
            }
            break;
        }
        case 0x0A: //ASL A
        {
            if(A & 128)
            {
                P |= 1;
            }
            else
            {
                P &= 0b11111110;
            }
            A <<= 1;
            UpdateFlags(&A);
            InstructionCycle = 0;
            break;
        }
        case 0x0E: //ASL xxxx
        {
            if(InstructionCycle <= 3)
            {
                int a = InstructionCycle;
                NES.RAM.Read(0, &InstructionCycle,
                             ADDR_ABSOLUTE,
                             &temp);
                if(a == 3)
                {
                    InstructionCycle = 4;
                }
            }
            else
            {
                ASL(&temp, 4, &InstructionCycle);
            }
            break;
        }
        case 0x16: //ASL xx+X
        {
            if(InstructionCycle <= 3)
            {
                int a = InstructionCycle;
                NES.RAM.Read(0, &InstructionCycle,
                             ADDR_ZERO_PAGE_X,
                             &temp);
                if(InstructionCycle == 0) //It can take one cycle
                    //to cross the page
                {
                    InstructionCycle = a + 1;
                    goto aaaa1;
                }
            }
            else
            {
        aaaa1:
                ASL(&temp, 4, &InstructionCycle);
            }
            break;
        }
        case 0x1E: //ASL xxxx+X
        {
            if(InstructionCycle <= 4)
            {
                int a = InstructionCycle;
                NES.RAM.Read(0, &InstructionCycle,
                             ADDR_ABSOLUTE_X,
                             &temp);
                if(InstructionCycle == 0) //Because it can take 1 cycle to
                    //cross the page
                {
                    InstructionCycle = a + 1;
                    goto aaaa2;
                }
            }
            else
            {
        aaaa2:
                ASL(&temp, 5, &InstructionCycle);
            }
            break;
        }
        case 0x46: //LSR xx
        {
            if(InstructionCycle <= 2)
            {
                int a = InstructionCycle;
                NES.RAM.Read(0, &InstructionCycle,
                             ADDR_ZERO_PAGE,
                             &temp);
                if(a == 2)
                {
                    InstructionCycle = 3;
                }
            }
            else
            {
                LSR(&temp, 3, &InstructionCycle);
            }
            break;
        }
        case 0x4A: //LSR A
        {
            if(A & 1)
            {
                P |= 1;
            }
            else
            {
                P &= 0b11111110;
            }
            A >>= 1;
            UpdateFlags(&A);
            InstructionCycle = 0;
            break;
        }
        case 0x4E: //LSR xxxx
        {
            if(InstructionCycle <= 3)
            {
                int a = InstructionCycle;
                NES.RAM.Read(0, &InstructionCycle,
                             ADDR_ABSOLUTE,
                             &temp);
                if(a == 3)
                {
                    InstructionCycle = 4;
                }
            }
            else
            {
                LSR(&temp, 4, &InstructionCycle);
            }
            break;
        }
        case 0x56: //LSR xx+X
        {
            if(InstructionCycle <= 3)
            {
                int a = InstructionCycle;
                NES.RAM.Read(0, &InstructionCycle,
                             ADDR_ZERO_PAGE_X,
                             &temp);
                if(InstructionCycle == 0) //It can take one cycle
                    //to cross the page
                {
                    InstructionCycle = a + 1;
                    goto aaaa7;
                }
            }
            else
            {
        aaaa7:
                LSR(&temp, 4, &InstructionCycle);
            }
            break;
        }
        case 0x5E: //LSR xxxx+X
        {
            if(InstructionCycle <= 4)
            {
                int a = InstructionCycle;
                NES.RAM.Read(0, &InstructionCycle,
                             ADDR_ABSOLUTE_X,
                             &temp);
                if(InstructionCycle == 0) //Because it can take 1 cycle to
                    //cross the page
                {
                    InstructionCycle = a + 1;
                    goto aaaa8;
                }
            }
            else
            {
        aaaa8:
                LSR(&temp, 5, &InstructionCycle);
            }
            break;
        }
        case 0x2A: //ROL A
        {
            unsigned char temp_ = (P & 1);
            if(A & 128)
            {
                P |= 1;
            }
            else
            {
                P &= 0b11111110;
            }
            A <<= 1;
            if(temp_)
            {
                A |= 1;
            }
            else
            {
                A &= 0b11111110;
            }
            UpdateFlags(&A);
            InstructionCycle = 0;
            break;
        }
        case 0x26: //ROL xx
        {
            if(InstructionCycle <= 2)
            {
                int a = InstructionCycle;
                NES.RAM.Read(0, &InstructionCycle,
                             ADDR_ZERO_PAGE,
                             &temp);
                if(a == 2)
                {
                    InstructionCycle = 3;
                }
            }
            else
            {
                ROL(&temp, 3, &InstructionCycle);
            }
            break;
        }
        case 0x2E: //ROL xxxx
        {
            if(InstructionCycle <= 3)
            {
                int a = InstructionCycle;
                NES.RAM.Read(0, &InstructionCycle,
                             ADDR_ABSOLUTE,
                             &temp);
                if(a == 3)
                {
                    InstructionCycle = 4;
                }
            }
            else
            {
                ROL(&temp, 4, &InstructionCycle);
            }
            break;
        }
        case 0x36: //ROL xx+X
        {
            if(InstructionCycle <= 3)
            {
                int a = InstructionCycle;
                NES.RAM.Read(0, &InstructionCycle,
                             ADDR_ZERO_PAGE_X,
                             &temp);
                if(InstructionCycle == 0) //Because it can take 1 cycle to
                    //cross the page
                {
                    InstructionCycle = a + 1;
                    goto aaaa4;
                }
            }
            else
            {
        aaaa4:
                ROL(&temp, 4, &InstructionCycle);
            }
            break;
        }
        case 0x3E: //ROL xxxx+X
        {
            if(InstructionCycle <= 4)
            {
                int a = InstructionCycle;
                NES.RAM.Read(0, &InstructionCycle,
                             ADDR_ABSOLUTE_X,
                             &temp);
                if(InstructionCycle == 0) //Because it can take 1 cycle to
                    //cross the page
                {
                    InstructionCycle = a + 1;
                    goto aaaa3;
                }
            }
            else
            {
        aaaa3:
                ROL(&temp, 5, &InstructionCycle);
            }
            break;
        }
        case 0x6A: //ROR A
        {
            unsigned char temp_ = (P & 1);
            if(A & 1)
            {
                P |= 1;
            }
            else
            {
                P &= 0b11111110;
            }
            A >>= 1;
            if(temp_)
            {
                A |= 128;
            }
            else
            {
                A &= 0b01111111;
            }
            UpdateFlags(&A);
            InstructionCycle = 0;
            break;
        }
        case 0x66: //ROR xx
        {
            if(InstructionCycle <= 2)
            {
                int a = InstructionCycle;
                NES.RAM.Read(0, &InstructionCycle,
                             ADDR_ZERO_PAGE,
                             &temp);
                if(a == 2)
                {
                    InstructionCycle = 3;
                }
            }
            else
            {
                ROR(&temp, 3, &InstructionCycle);
            }
            break;
        }
        case 0x6E: //ROR xxxx
        {
            if(InstructionCycle <= 3)
            {
                int a = InstructionCycle;
                NES.RAM.Read(0, &InstructionCycle,
                             ADDR_ABSOLUTE,
                             &temp);
                if(a == 3)
                {
                    InstructionCycle = 4;
                }
            }
            else
            {
                ROR(&temp, 4, &InstructionCycle);
            }
            break;
        }
        case 0x76: //ROR xx+X
        {
            if(InstructionCycle <= 3)
            {
                int a = InstructionCycle;
                NES.RAM.Read(0, &InstructionCycle,
                             ADDR_ZERO_PAGE_X,
                             &temp);
                if(InstructionCycle == 0) //Because it can take 1 cycle to
                    //cross the page
                {
                    InstructionCycle = a + 1;
                    goto aaaa5;
                }
            }
            else
            {
        aaaa5:
                ROR(&temp, 4, &InstructionCycle);
            }
            break;
        }
        case 0x7E: //ROR xxxx+X
        {
            if(InstructionCycle <= 4)
            {
                int a = InstructionCycle;
                NES.RAM.Read(0, &InstructionCycle,
                             ADDR_ABSOLUTE_X,
                             &temp);
                if(InstructionCycle == 0) //Because it can take 1 cycle to
                    //cross the page
                {
                    InstructionCycle = a + 1;
                    goto aaaa6;
                }
            }
            else
            {
        aaaa6:
                ROR(&temp, 5, &InstructionCycle);
            }
            break;
        }
        case 0xE6: //INC xx
        {
            int a = InstructionCycle;
            NES.RAM.Read(0, &InstructionCycle,
                         ADDR_ZERO_PAGE, &temp);
            InstructionCycle = a;
            INC(&temp, 3, &InstructionCycle);
            break;
        }
        case 0xF6: //INC xx+X
        {
            int a = InstructionCycle;
            NES.RAM.Read(0, &InstructionCycle,
                         ADDR_ZERO_PAGE_X, &temp);
            InstructionCycle = a;
            INC(&temp, 4, &InstructionCycle);
            break;
        }
        case 0xEE: //INC xxxx
        {
            int a = InstructionCycle;
            NES.RAM.Read(0, &InstructionCycle,
                         ADDR_ABSOLUTE, &temp);
            InstructionCycle = a;
            INC(&temp, 4, &InstructionCycle);
            break;
        }
        case 0xFE: //INC xxxx+X
        {
            int a = InstructionCycle;
            NES.RAM.Read(0, &InstructionCycle,
                         ADDR_ABSOLUTE_X, &temp);
            InstructionCycle = a;
            INC(&temp, 5, &InstructionCycle);
            break;
        }
        case 0xC6: //DEC xx
        {
            int a = InstructionCycle;
            NES.RAM.Read(0, &InstructionCycle,
                         ADDR_ZERO_PAGE, &temp);
            InstructionCycle = a;
            DEC(&temp, 3, &InstructionCycle);
            break;
        }
        case 0xD6: //DEC xx+X
        {
            int a = InstructionCycle;
            NES.RAM.Read(0, &InstructionCycle,
                         ADDR_ZERO_PAGE_X, &temp);
            InstructionCycle = a;
            DEC(&temp, 4, &InstructionCycle);
            break;
        }
        case 0xCE: //DEC xxxx
        {
            int a = InstructionCycle;
            NES.RAM.Read(0, &InstructionCycle,
                         ADDR_ABSOLUTE, &temp);
            InstructionCycle = a;
            DEC(&temp, 4, &InstructionCycle);
            break;
        }
        case 0xDE: //DEC xxxx+X
        {
            int a = InstructionCycle;
            NES.RAM.Read(0, &InstructionCycle,
                         ADDR_ABSOLUTE_X, &temp);
            InstructionCycle = a;
            DEC(&temp, 5, &InstructionCycle);
            break;
        }
        case 0xCA: //DEX
        {
            X--;
            UpdateFlags(&X);
            InstructionCycle = 0;
            break;
        }
        case 0x02:
        case 0x12:
        case 0x22:
        case 0x32:
        case 0x42:
        case 0x52:
        case 0x62:
        case 0x72:
        case 0x92:
        case 0xB2:
        case 0xD2:
        case 0xF2: //KIL
        {
            NES.KIL = true;
        }
        case 0x82:
        case 0xC2:
        case 0xE2: //NOP #xx
        {
            NOP(ADDR_IMMEDIATE);
            break;
        }
        case 0x1A:
        case 0x3A:
        case 0x5A:
        case 0x7A:
        case 0xDA:
        case 0xEA:
        case 0xFA: //NOP
        {
            InstructionCycle = 0;
            break;
        }
        case 0x07: //SLO xx
        {
            int a = InstructionCycle;
            NES.RAM.Read(0, &InstructionCycle,
                         ADDR_ZERO_PAGE, &temp);
            InstructionCycle = a;
            SLO(&temp, 3, &InstructionCycle);
            break;
        }
        case 0x17: //SLO xx+X
        {
            int a = InstructionCycle;
            NES.RAM.Read(0, &InstructionCycle,
                         ADDR_ZERO_PAGE_X, &temp);
            InstructionCycle = a;
            SLO(&temp, 4, &InstructionCycle);
            break;
        }
        case 0x0F: //SLO xxxx
        {
            int a = InstructionCycle;
            NES.RAM.Read(0, &InstructionCycle,
                         ADDR_ABSOLUTE, &temp);
            InstructionCycle = a;
            SLO(&temp, 4, &InstructionCycle);
            break;
        }
        case 0x1F: //SLO xxxx+X
        {
            int a = InstructionCycle;
            NES.RAM.Read(0, &InstructionCycle,
                         ADDR_ABSOLUTE_X, &temp);
            InstructionCycle = a;
            SLO(&temp, 5, &InstructionCycle);
            break;
        }
        case 0x1B: //SLO xxxx+Y
        {
            int a = InstructionCycle;
            NES.RAM.Read(0, &InstructionCycle,
                         ADDR_ABSOLUTE_Y, &temp);
            InstructionCycle = a;
            SLO(&temp, 5, &InstructionCycle);
            break;
        }
        case 0x03: //SLO (xx)+X
        {
            int a = InstructionCycle;
            NES.RAM.Read(0, &InstructionCycle,
                         ADDR_INDIRECT_X, &temp);
            InstructionCycle = a;
            SLO(&temp, 6, &InstructionCycle);
            break;
        }
        case 0x13: //SLO (xx+Y)
        {
            int a = InstructionCycle;
            NES.RAM.Read(0, &InstructionCycle,
                         ADDR_INDIRECT_Y, &temp);
            InstructionCycle = a;
            SLO(&temp, 6, &InstructionCycle);
            break;
        }
        default: //This is not supposed to happen
        {
            return;
        }
    }
    return; //TODO
}
void CPUClass::CMP(unsigned char *Register, unsigned char addrMode)
{
    unsigned char temp;
    NES.RAM.Read((unsigned short)0, &InstructionCycle,
                 addrMode,
                 &temp);
    if(InstructionCycle == 0)
    {
        if(((*Register)-(temp)) & 128)
        {
            P |= 128;
        }
        else
        {
            P &= 127;
        }
        if(*Register == temp)
        {
            P |= 2;
        }
        else
        {
            P &= 0b11111101;
        }
        if(temp <= *Register)
        {
            P |= 1;
        }
        else
        {
            P &= 0b11111110;
        }
    }
}
void CPUClass::Branch(unsigned char flag, bool clear)
{
    if(InstructionCycle == 1)
    {
        temp = NES.RAM.Read(PCTemp);
        PCTemp++;
        if(clear)
        {
            if(!(P & flag))
            {
                InstructionCycle++;
            }
            else
            {
                InstructionCycle = 0;
            }
        }
        else
        {
            if(P & flag)
            {
                InstructionCycle++;
            }
            else
            {
                InstructionCycle = 0;
            }
        }
    }
    else if(InstructionCycle == 2)
    {
        NES.RAM.Read(PCTemp);
        if((PCTemp >> 8) !=
           ((PCTemp + (char)temp) >> 8))
        {
            InstructionCycle++;
        }
        else
        {
            InstructionCycle = 0;
        }
        PCTemp += (char)temp;
    }
    else if(InstructionCycle == 3)
    {
        NES.RAM.Read(PCTemp - 256);
        InstructionCycle = 0;
    }
}
void CPUClass::ASL(unsigned char *Value, unsigned int CyclesTakenForAddressing,
                   unsigned char *InstructionCycle)
{
    if(*InstructionCycle == CyclesTakenForAddressing - 1)
    {
        (*InstructionCycle)++;
        return;
    }
    else if(*InstructionCycle == CyclesTakenForAddressing)
    {
        if(*Value & 128)
        {
            P |= 1;
        }
        else
        {
            P &= 0b11111110;
        }
        *Value <<= 1;
        (*InstructionCycle)++;
        UpdateFlags(&temp);
    }
    else if(*InstructionCycle == CyclesTakenForAddressing + 1)
    {
        NES.RAM.Write(NES.RAM.AddressBus, *Value);
        *InstructionCycle = 0;
    }
}
void CPUClass::LSR(unsigned char *Value, unsigned int CyclesTakenForAddressing,
                   unsigned char *InstructionCycle)
{
    if(*InstructionCycle == CyclesTakenForAddressing - 1)
    {
        (*InstructionCycle)++;
        return;
    }
    else if(*InstructionCycle == CyclesTakenForAddressing)
    {
        if(*Value & 1)
        {
            P |= 1;
        }
        else
        {
            P &= 0b11111110;
        }
        *Value >>= 1;
        (*InstructionCycle)++;
        UpdateFlags(&temp);
    }
    else if(*InstructionCycle == CyclesTakenForAddressing + 1)
    {
        NES.RAM.Write(NES.RAM.AddressBus, *Value);
        *InstructionCycle = 0;
    }
}
void CPUClass::ROL(unsigned char *Value, unsigned int CyclesTakenForAddressing,
                   unsigned char *InstructionCycle)
{
    if(*InstructionCycle == CyclesTakenForAddressing - 1)
    {
        (*InstructionCycle)++;
        return;
    }
    else if(*InstructionCycle == CyclesTakenForAddressing)
    {
        unsigned char temp_ = (P & 1);
        if(*Value & 128)
        {
            P |= 1;
        }
        else
        {
            P &= 0b11111110;
        }
        *Value <<= 1;
        if(temp_)
        {
            *Value |= 1;
        }
        else
        {
            *Value &= 0b11111110;
        }
        UpdateFlags(Value);
        (*InstructionCycle)++;
    }
    else if(*InstructionCycle == CyclesTakenForAddressing + 1)
    {
        NES.RAM.Write(NES.RAM.AddressBus, *Value);
        *InstructionCycle = 0;
    }
}
void CPUClass::ROR(unsigned char *Value, unsigned int CyclesTakenForAddressing,
                   unsigned char *InstructionCycle)
{
    if(*InstructionCycle == CyclesTakenForAddressing - 1)
    {
        (*InstructionCycle)++;
    }
    else if(*InstructionCycle == CyclesTakenForAddressing)
    {
        unsigned char temp_ = (P & 1);
        if(*Value & 1)
        {
            P |= 1;
        }
        else
        {
            P &= 0b11111110;
        }
        *Value >>= 1;
        if(temp_)
        {
            *Value |= 128;
        }
        else
        {
            *Value &= 0b01111111;
        }
        UpdateFlags(Value);
        (*InstructionCycle)++;
    }
    else if(*InstructionCycle == CyclesTakenForAddressing + 1)
    {
        NES.RAM.Write(NES.RAM.AddressBus, *Value);
        *InstructionCycle = 0;
    }
}
void CPUClass::INC(unsigned char* Value, unsigned int CyclesTakenForAddressing,
                   unsigned char* InstructionCycle)
{
    if(*InstructionCycle < CyclesTakenForAddressing)
    {
        (*InstructionCycle)++;
    }
    else if(*InstructionCycle == CyclesTakenForAddressing)
    {
        (*InstructionCycle)++;
        (*Value)++;
        UpdateFlags(Value);
    }
    else
    {
        NES.RAM.Write(NES.RAM.AddressBus, *Value);
        (*InstructionCycle) = 0;
    }
}
void CPUClass::DEC(unsigned char* Value, unsigned int CyclesTakenForAddressing,
                   unsigned char* InstructionCycle)
{
    if(*InstructionCycle < CyclesTakenForAddressing)
    {
        (*InstructionCycle)++;
    }
    else if(*InstructionCycle == CyclesTakenForAddressing)
    {
        (*InstructionCycle)++;
        (*Value)--;
        UpdateFlags(Value);
    }
    else
    {
        NES.RAM.Write(NES.RAM.AddressBus, *Value);
        (*InstructionCycle) = 0;
    }
}
void CPUClass::SLO(unsigned char* Value, unsigned int CyclesTakenForAddressing,
                   unsigned char* InstructionCycle)
{
    if(*InstructionCycle < CyclesTakenForAddressing)
    {
        (*InstructionCycle)++;
    }
    else if(*InstructionCycle == CyclesTakenForAddressing)
    {
        (*InstructionCycle)++;
        if(*Value & 128)
        {
            P |= 1;
        }
        else
        {
            P &= 0b11111110;
        }
        (*Value) <<= 1;
        A |= (*Value);
        UpdateFlags(&A);
    }
    else
    {
        NES.RAM.Write(NES.RAM.AddressBus, *Value);
        (*InstructionCycle) = 0;
    }
}

void CPUClass::UpdateFlags(unsigned char *Register)
{
    if(*Register)
    {
        P &= 0b11111101;
    }
    else
    {
        P |= 2;
    }
    if(*Register & 128)
    {
        P |= 128;
    }
    else
    {
        P &= 127;
    }
}
void CPUClass::NOP(unsigned char addrMode)
{
    unsigned char a;
    NES.RAM.Read(0, &InstructionCycle, addrMode, &a);
}
void CPUClass::Push(unsigned char Value)
{
    if(InstructionCycle == 1)
    {
        NES.RAM.Read(PCTemp);
        InstructionCycle++;
    }
    else if(InstructionCycle == 2)
    {
        NES.RAM.Write(((unsigned short)S)|256, Value);
        S--;
        InstructionCycle = 0;
    }
}
void CPUClass::Pull(unsigned char *Value)
{
    if(InstructionCycle == 1)
    {
        NES.RAM.Read(PCTemp);
        InstructionCycle++;
    }
    else if(InstructionCycle == 2)
    {
        S++;
        InstructionCycle++;
    }
    else if(InstructionCycle == 3)
    {
        *Value = NES.RAM.Read(0x100+S);
        InstructionCycle = 0;
    }
}
void CPUClass::Reset()
{
    NES.KIL = false;
    memcpy(NES.CartridgeMemory, NES.ROMFile + 16, NES.RAM.PRGROMSize);
    PC = NES.RAM.Read(0xFFFC);
    PC |= (((unsigned short)(NES.RAM.Read(0xFFFD))) << 8);
    PCTemp = PC;
    CurrentCycle = 7;
    InstructionCycle = 0;
    P = 0x34;
    A = 0;
    X = 0;
    Y = 0;
    S = 0xfd;
}

