/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Carmine Foggia $
   ======================================================================== */
#include "memory.h"
#include "main.h"
#include "cpu.h"
//NOTE to Carmine: You're refactoring the push instructions
void CPUClass::Run()
{
    CurrentInstruction = NES.RAM.Read(PC);
    PC++;
    NEXT_CYCLE;
    switch(CurrentInstruction)
    {
        case 0x18: //CLC
        {
            P &= 0b11111110;
            NEXT_CYCLE;
        } break;
        case 0x38: //SEC
        {
            P |= 1;
            NEXT_CYCLE;
        } break;
        case 0x58: //CLI
        {
            P &= 0b11111011;
            NEXT_CYCLE;
        } break;
        case 0x78: //SEI
        {
            P |= 4;
            NEXT_CYCLE;
        } break;
        case 0xB8: //CLV
        {
            P &= 0b10111111;
            NEXT_CYCLE;
        } break;
        case 0xD8: //CLD
        {
            P &= 0b11110111;
            NEXT_CYCLE;
        } break;
        case 0xF8: //SED
        {
            P |= 8;
            NEXT_CYCLE;
        } break;


        
        case 0xA0: //LDY #xx
        {
            addrMode = ADDR_IMMEDIATE;
            goto LDYAll;
        }
        case 0xA4: //LDY xx
        {
            addrMode = ADDR_ZERO_PAGE;
            goto LDYAll;
        }
        case 0xAC: //LDY xxxx
        {
            addrMode = ADDR_ABSOLUTE;
            goto LDYAll;
        }
        case 0xB4: //LDY xx+X
        {
            addrMode = ADDR_ZERO_PAGE_X;
            goto LDYAll;
        }
        case 0xBC: //LDY xxxx+X
        {
            addrMode = ADDR_ABSOLUTE_X;
            goto LDYAll;
        }
        LDYAll:
        {
            NES.RAM.Read(addrMode, &Y);
            UpdateFlags(Y);
        } break;



        case 0x4C: //JMP XXXX
        {
            AddressBus = NES.RAM.Read(PC);
            PC++;
            NEXT_CYCLE;
         
            AddressBus |= (NES.RAM.Read(PC) << 8);
            PC = AddressBus;
            NEXT_CYCLE;
            break;
        }
        case 0x6C: //JMP (XXXX)
        {
            AddressBus = NES.RAM.Read(PC);
            PC++;
            NEXT_CYCLE;

            AddressBus |= (NES.RAM.Read(PC) << 8);
            NEXT_CYCLE;

            temp = AddressBus;
            AddressBus = NES.RAM.Read(temp);
            NEXT_CYCLE;

            AddressBus |=
                (NES.RAM.Read(temp+1) << 8);
            PC = AddressBus;
            NEXT_CYCLE;
            
            break;
        }


        
        case 0x24: //BIT xx
        {
            unsigned char temp;
            NES.RAM.Read(ADDR_ZERO_PAGE, &temp);
            unsigned char a = A & temp;
            UpdateFlags(a);
            if(a & 0b01000000)
            {
                P |= 0b01000000;
            }
            else
            {
                P &= 0b10111111;
            }
            break;
        }
        case 0x2c: //BIT xxxx
        {
            unsigned char temp;
            NES.RAM.Read(ADDR_ABSOLUTE, &temp);
            unsigned char a = A & temp;
            UpdateFlags(a);
            if(a & 0b01000000)
            {
                P |= 0b01000000;
            }
            else
            {
                P &= 0b10111111;
            }
            break;
        }


        
        case 0x84: //STY xx
        {
            addrMode = ADDR_ZERO_PAGE;
            goto STYAll;
        }
        case 0x8C: //STY xxxx
        {
            addrMode = ADDR_ABSOLUTE;
            goto STYAll;
        }
        case 0x94: //STY xx+X
        {
            addrMode = ADDR_ZERO_PAGE_X;
            goto STYAll;
        }
        STYAll:
        {
            NES.RAM.WriteAddrMode(Y, addrMode);
            break;
        }



        
        case 0xC0: //CPY #xx
        {
            addrMode = ADDR_IMMEDIATE;
            goto CPYAll;
        }
        case 0xC4: //CPY xx
        {
            addrMode = ADDR_ZERO_PAGE;
            goto CPYAll;
        }
        case 0xCC: //CPY xxxx
        {
            addrMode = ADDR_ABSOLUTE;
            goto CPYAll;
        }
        CPYAll:
        {
            CMP(&Y, addrMode);
            break;
        }


        
        case 0xE0: //CPX #xx
        {
            addrMode = ADDR_IMMEDIATE;
            goto CPXAll;
        }
        case 0xE4: //CPX xx
        {
            addrMode = ADDR_ZERO_PAGE;
            goto CPXAll;
        }
        case 0xEC: //CPX xxxx
        {
            addrMode = ADDR_ABSOLUTE;
            goto CPXAll;
        }
        CPXAll:
        {
            CMP(&X, addrMode);
            break;
        }


        
        case 0xA8: //TAY
        {
            Y = A;
            UpdateFlags(Y);
            NEXT_CYCLE;
            break;
        }
        case 0x98: //TYA
        {
            A = Y;
            UpdateFlags(A);
            NEXT_CYCLE;
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
            UpdateFlags(X);
            NEXT_CYCLE;
            break;
        }
        case 0xC8: //INY
        {
            Y++;
            UpdateFlags(Y);
            NEXT_CYCLE;
            break;
        }
        case 0x88: //DEY
        {
            Y--;
            UpdateFlags(Y);
            NEXT_CYCLE;
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
            UpdateFlags(A);
            break;
        }
        case 0x28: //PLP
        {
            Pull(&P);
            break;
        }


        
        case 0x20: //JSR xxxx
        {
            AddressBus = NES.RAM.Read(PC);
            PC++;
            NEXT_CYCLE;
            
            AddressBus |= (NES.RAM.Read(PC) << 8);
            NEXT_CYCLE;
            
            NES.RAM.Write(0x100+S, (unsigned char)AddressBus);
            NEXT_CYCLE;
            
            NES.RAM.Write(0x100+S, (*((unsigned char*)(&PC)+1)));
            S--;
            NEXT_CYCLE;
            
            NES.RAM.Write(0x100+S, (unsigned char)PC);
            S--;
            PC = AddressBus;
            NEXT_CYCLES(2);
            break;
        }
        case 0x60: //RTS
        {
            NEXT_CYCLES(2);
            
            S++;
            PC = NES.RAM.Read(0x100+S);
            NEXT_CYCLE;
            
            S++;
            PC |= (NES.RAM.Read(0x100+S) << 8);
            PC++;
            NEXT_CYCLES(2);
            break;
        }
        case 0x00: //BRK
        {
            NEXT_CYCLE;
            
            unsigned short PCToPush = PC + 1;
            NES.RAM.Write(0x100+S,
                          (unsigned char)((unsigned short)PCToPush >> 8));
            S--;
            NEXT_CYCLE;
            
            NES.RAM.Write(0x100+S, (unsigned char)(PCToPush));
            S--;
            NEXT_CYCLE;
            
            NES.RAM.Write(0x100+S, P|0b00110000);
            S--;
            NEXT_CYCLE;
            
            PC = NES.RAM.Read(0xFFFE);
            NEXT_CYCLE;
            
            PC |= (unsigned short)(NES.RAM.Read(0xFFFF) << 8);
            P &= 0b11111011;
            NEXT_CYCLE;
            
            break;
        }
        case 0x40: //RTI
        {
            NEXT_CYCLES(2);
                        
            S++;
            P = (NES.RAM.Read(0x100+S) & 0b11001111);
            NEXT_CYCLE;
            
            S++;
            PC = NES.RAM.Read(0x100+S);
            NEXT_CYCLE;
            
            S++;
            PC |= (NES.RAM.Read(0x100+S) << 8);
            NEXT_CYCLE;
            
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
            NES.RAM.Read((unsigned char)(CurrentInstruction &
                                         (unsigned char)0b00011100),
                         &temp);
            A |= temp;
            UpdateFlags(A);
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
            NES.RAM.Read((unsigned char)(CurrentInstruction &
                                         (unsigned char)0b00011100),
                         &temp);
            A &= temp;
            UpdateFlags(A);
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
            NES.RAM.Read((unsigned char)(CurrentInstruction &
                                         (unsigned char)0b00011100),
                         &temp);
            A ^= temp;
            UpdateFlags(A);
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
            NES.RAM.Read((unsigned char)(CurrentInstruction &
                                         (unsigned char)0b00011100),
                         &temp);
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
        } break;



        case 0x89: //NOP #xx
        {
            NES.RAM.Read(PC);
            PC++;
            NEXT_CYCLE;
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
            NES.RAM.WriteAddrMode(A, (unsigned char)(CurrentInstruction &
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
            NES.RAM.Read((unsigned char)(CurrentInstruction &
                                         (unsigned char)0b00011100),
                         &A);
            UpdateFlags(A);
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
            CMP(&A, (unsigned char)(CurrentInstruction &
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
            NES.RAM.Read((unsigned char)(CurrentInstruction &
                                         (unsigned char)0b00011100),
                         &temp);
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
        } break;
                


        case 0xA2: //LDX #xx
        {
            addrMode = ADDR_IMMEDIATE;
            goto LDXAll;
        }
        case 0xA6: //LDX xx
        {
            addrMode = ADDR_ZERO_PAGE;
            goto LDXAll;
        }
        case 0xAE: //LDX xxxx
        {
            addrMode = ADDR_ABSOLUTE;
            goto LDXAll;
        }
        case 0xB6: //LDX xx+Y
        {
            addrMode = ADDR_ZERO_PAGE_Y;
            goto LDXAll;
        }
        case 0xBE: //LDX xxxx+Y
        {
            addrMode = ADDR_ABSOLUTE_Y;
            goto LDXAll;
        }
        LDXAll:
        {        
            NES.RAM.Read(addrMode, &X);
            UpdateFlags(X);
        } break;


        
        case 0x86: //STX xx
        {
            addrMode = ADDR_ZERO_PAGE;
            goto STXAll;
        }
        case 0x8E: //STX xxxx
        {
            addrMode = ADDR_ABSOLUTE;
            goto STXAll;
        }
        case 0x96: //STX xx+Y
        {
            addrMode = ADDR_ZERO_PAGE_Y;
            goto STXAll;
        }
        STXAll:
        {
            NES.RAM.WriteAddrMode(X, addrMode);
            break;
        }


        
        case 0xAA: //TAX
        {
            X = A;
            UpdateFlags(X);
            NEXT_CYCLE;
            break;
        }
        case 0x8A: //TXA
        {
            A = X;
            UpdateFlags(A);
            NEXT_CYCLE;
            break;
        }
        case 0xBA: //TSX
        {
            X = S;
            UpdateFlags(X);
            NEXT_CYCLE;
            break;
        }
        case 0x9A: //TXS
        {
            S = X;
            NEXT_CYCLE;
            break;
        }


        
        case 0x06: //ASL xx
        {
            NES.RAM.Read(ADDR_ZERO_PAGE, &temp);
            ASL(&temp);
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
            UpdateFlags(A);
            NEXT_CYCLE;
            break;
        }
        case 0x0E: //ASL xxxx
        {
            NES.RAM.Read(ADDR_ABSOLUTE, &temp);
            ASL(&temp);
            break;
        }
        case 0x16: //ASL xx+X
        {
            NES.RAM.Read(ADDR_ZERO_PAGE_X, &temp);
            if(!NES.RAM.AddressCarry)
            {
                NEXT_CYCLE;
            }
            ASL(&temp);
            break;
        }
        case 0x1E: //ASL xxxx+X
        {
            NES.RAM.Read(ADDR_ABSOLUTE_X, &temp);
            if(!NES.RAM.AddressCarry)
            {
                NEXT_CYCLE;
            }
            ASL(&temp);
            break;
        }


        
        case 0x46: //LSR xx
        {
            NES.RAM.Read(ADDR_ZERO_PAGE, &temp);
            LSR(&temp);
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
            UpdateFlags(A);
            NEXT_CYCLE;
            break;
        }
        case 0x4E: //LSR xxxx
        {
            NES.RAM.Read(ADDR_ABSOLUTE, &temp);
            LSR(&temp);
            break;
        }
        case 0x56: //LSR xx+X
        {
            NES.RAM.Read(ADDR_ZERO_PAGE_X, &temp);
            if(!NES.RAM.AddressCarry)
            {
                NEXT_CYCLE;
            }
            LSR(&temp);
            break;
        }
        case 0x5E: //LSR xxxx+X
        {
            NES.RAM.Read(ADDR_ABSOLUTE_X, &temp);
            if(!NES.RAM.AddressCarry)
            {
                NEXT_CYCLE;
            }
            LSR(&temp);
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
            UpdateFlags(A);
            NEXT_CYCLE;
            break;
        }
        case 0x26: //ROL xx
        {
            NES.RAM.Read(ADDR_ZERO_PAGE, &temp);
            ROL(&temp);
            break;
        }
        case 0x2E: //ROL xxxx
        {
            NES.RAM.Read(ADDR_ABSOLUTE, &temp);
            ROL(&temp);
            break;
        }
        case 0x36: //ROL xx+X
        {
            NES.RAM.Read(ADDR_ZERO_PAGE_X, &temp);
            if(!NES.RAM.AddressCarry)
            {
                NEXT_CYCLE;
            }
            ROL(&temp);
            break;
        }
        case 0x3E: //ROL xxxx+X
        {
            NES.RAM.Read(ADDR_ABSOLUTE_X, &temp);
            if(!NES.RAM.AddressCarry)
            {
                NEXT_CYCLE;
            }
            ROL(&temp);
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
            UpdateFlags(A);
            NEXT_CYCLE;
            break;
        }
        case 0x66: //ROR xx
        {
            NES.RAM.Read(ADDR_ZERO_PAGE, &temp);
            ROR(&temp);
            break;            
        }
        case 0x6E: //ROR xxxx
        {
            NES.RAM.Read(ADDR_ABSOLUTE, &temp);
            ROR(&temp);
            break;            
        }
        case 0x76: //ROR xx+X
        {
            NES.RAM.Read(ADDR_ZERO_PAGE_X, &temp);
            if(!NES.RAM.AddressCarry)
            {
                NEXT_CYCLE;
            }
            ROL(&temp);
            break;   
        }
        case 0x7E: //ROR xxxx+X
        {
            NES.RAM.Read(ADDR_ABSOLUTE_X, &temp);
            if(!NES.RAM.AddressCarry)
            {
                NEXT_CYCLE;
            }
            ROL(&temp);
            break;  
        }


        
        case 0xE6: //INC xx
        {
            NES.RAM.Read(ADDR_ZERO_PAGE, &temp);
            INC(&temp);
            break;
        }
        case 0xF6: //INC xx+X
        {
            NES.RAM.Read(ADDR_ZERO_PAGE_X, &temp);
            if(!NES.RAM.AddressCarry)
            {
                NEXT_CYCLE;
            }
            INC(&temp);
            break;
        }
        case 0xEE: //INC xxxx
        {
            NES.RAM.Read(ADDR_ABSOLUTE, &temp);
            INC(&temp);
            break;
        }
        case 0xFE: //INC xxxx+X
        {
            NES.RAM.Read(ADDR_ABSOLUTE_X, &temp);
            if(!NES.RAM.AddressCarry)
            {
                NEXT_CYCLE;
            }
            INC(&temp);
            break;
        }


        
        case 0xC6: //DEC xx
        {
            NES.RAM.Read(ADDR_ZERO_PAGE, &temp);
            DEC(&temp);
            break;
        }
        case 0xD6: //DEC xx+X
        {
            NES.RAM.Read(ADDR_ZERO_PAGE_X, &temp);
            if(!NES.RAM.AddressCarry)
            {
                NEXT_CYCLE;
            }
            DEC(&temp);
            break;
        }
        case 0xCE: //DEC xxxx
        {
            NES.RAM.Read(ADDR_ABSOLUTE, &temp);
            DEC(&temp);
            break;
        }
        case 0xDE: //DEC xxxx+X
        {
            NES.RAM.Read(ADDR_ABSOLUTE_X, &temp);
            if(!NES.RAM.AddressCarry)
            {
                NEXT_CYCLE;
            }
            DEC(&temp);
            break;
        }


        
        case 0xCA: //DEX
        {
            X--;
            UpdateFlags(X);
            NEXT_CYCLE;
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
            NEXT_CYCLE;
            break;
        }


        
        case 0x07: //SLO xx
        {
            NES.RAM.Read(ADDR_ZERO_PAGE, &temp);
            SLO(&temp);
            break;
        }
        case 0x17: //SLO xx+X
        {
            NES.RAM.Read(ADDR_ZERO_PAGE_X, &temp);
            if(!NES.RAM.AddressCarry)
            {
                NEXT_CYCLE;
            }
            SLO(&temp);
            break;
        }
        case 0x0F: //SLO xxxx
        {
            NES.RAM.Read(ADDR_ABSOLUTE, &temp);
            SLO(&temp);
            break;
        }
        case 0x1F: //SLO xxxx+X
        {
            NES.RAM.Read(ADDR_ABSOLUTE_X, &temp);
            if(!NES.RAM.AddressCarry)
            {
                NEXT_CYCLE;
            }
            SLO(&temp);
            break;
        }
        case 0x1B: //SLO xxxx+Y
        {
            NES.RAM.Read(ADDR_ABSOLUTE_Y, &temp);
            if(!NES.RAM.AddressCarry)
            {
                NEXT_CYCLE;
            }
            SLO(&temp);
            break;
        }
        case 0x03: //SLO (xx)+X
        {
            NES.RAM.Read(ADDR_INDIRECT_X, &temp);
            SLO(&temp);
            break;
        }
        case 0x13: //SLO (xx+Y)
        {
            NES.RAM.Read(ADDR_INDIRECT_Y, &temp);
            if(!NES.RAM.AddressCarry)
            {
                NEXT_CYCLE;
            }
            SLO(&temp);
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
    NES.RAM.Read(addrMode, &temp);
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



void CPUClass::Branch(unsigned char flag, bool clear)
{
    int CyclesToTake = 0;
    temp = NES.RAM.Read(PC);
    PC++;
    NEXT_CYCLE;
    if(clear)
    {
        if(P & flag)
        {
            return;
        }
    }
    else
    {
        if(!(P & flag))
        {
            return;
        }
    }

    
    NES.RAM.Read(PC);
    if((PC >> 8) !=
       ((PC + (char)temp) >> 8))
    {
        CyclesToTake = 4;
    }
    PC += (char)temp;
    NEXT_CYCLE;
    if(!CyclesToTake) return;

    
    NES.RAM.Read(PC - 256);
    NEXT_CYCLE;
}



void CPUClass::ASL(unsigned char *Value)
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
    UpdateFlags(temp);
    NEXT_CYCLE;
    
    NES.RAM.Write(NES.RAM.AddressBus, *Value);
    NEXT_CYCLE;
}



void CPUClass::LSR(unsigned char *Value)
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
    UpdateFlags(temp);
    NEXT_CYCLE;

    NES.RAM.Write(NES.RAM.AddressBus, *Value);
    NEXT_CYCLE;
}



void CPUClass::ROL(unsigned char *Value)
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
    UpdateFlags(*Value);
    NEXT_CYCLE;
 
    NES.RAM.Write(NES.RAM.AddressBus, *Value);
    NEXT_CYCLE;
}



void CPUClass::ROR(unsigned char *Value)
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
    UpdateFlags(*Value);
    NEXT_CYCLE;

    NES.RAM.Write(NES.RAM.AddressBus, *Value);
    NEXT_CYCLE;
}



void CPUClass::INC(unsigned char* Value)
{
    (*Value)++;
    UpdateFlags(*Value);
    NEXT_CYCLE;
    
    NES.RAM.Write(NES.RAM.AddressBus, *Value);
    NEXT_CYCLE;
}



void CPUClass::DEC(unsigned char* Value)
{
    (*Value)--;
    UpdateFlags(*Value);
    NEXT_CYCLE;
    
    NES.RAM.Write(NES.RAM.AddressBus, *Value);
    NEXT_CYCLE;
}



void CPUClass::SLO(unsigned char* Value)
{
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
    UpdateFlags(A);
    NEXT_CYCLE;
    
    NES.RAM.Write(NES.RAM.AddressBus, *Value);
    NEXT_CYCLE;
}



void CPUClass::UpdateFlags(unsigned char Register)
{
    if(Register)
    {
        P &= 0b11111101;
    }
    else
    {
        P |= 2;
    }
    if(Register & 128)
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
    NES.RAM.Read(addrMode, &a);
}



void CPUClass::Push(unsigned char Value)
{
    NES.RAM.Read(PC);
    NEXT_CYCLE;
        
    NES.RAM.Write(((unsigned short)S)|256, Value);
    S--;
    NEXT_CYCLE;
}



void CPUClass::Pull(unsigned char *Value)
{
        NES.RAM.Read(PC);
        S++;
        NEXT_CYCLES(2);
        
        *Value = NES.RAM.Read(0x100+S);
        NEXT_CYCLE;
}



void CPUClass::Reset()
{
    NES.KIL = false;
    memcpy(NES.CartridgeMemory, NES.ROMFile + 16, NES.RAM.PRGROMSize);
    PC = NES.RAM.Read(0xFFFC);
    PC |= (((unsigned short)(NES.RAM.Read(0xFFFD))) << 8);
    NES.FrameCycle = 7;
    P = 0x34;
    A = 0;
    X = 0;
    Y = 0;
    S = 0xfd;
}

