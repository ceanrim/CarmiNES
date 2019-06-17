/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Carmine Foggia $
   ======================================================================== */
//What you were doing yesterday: implementing the PPU rendering pipeline
//TODO: Power up state
//      PPU read/write buffer
#include <windows.h>
#include "..\res\resource.h"
#include "main.h"
#include "cpu.h"
#include "memory.h"
#include "nesclass.h"
#include "debuggerclass.h"

NESClass NES = NESClass();

const char ToHex[] = {'0', '1', '2', '3',
                      '4', '5', '6', '7',
                      '8', '9', 'a', 'b',
                      'c', 'd', 'e', 'f'};
const char *InstructionTable[] =
{"BRK", "ORA", "KIL", "SLO", "NOP", "ORA", "ASL", "SLO",
 "PHP", "ORA", "ASL", "ANC", "NOP", "ORA", "ASL", "SLO",
 "BPL", "ORA", "KIL", "SLO", "NOP", "ORA", "ASL", "SLO",
 "CLC", "ORA", "NOP", "SLO", "NOP", "ORA", "ASL", "SLO",
 "JSR", "AND", "KIL", "RLA", "BIT", "AND", "ROL", "RLA",
 "PLP", "AND", "ROL", "ANC", "BIT", "AND", "ROL", "RLA",
 "BMI", "AND", "KIL", "RLA", "NOP", "AND", "ROL", "RLA",
 "SEC", "AND", "NOP", "RLA", "NOP", "AND", "ROL", "RLA",
 "RTI", "EOR", "KIL", "SRE", "NOP", "EOR", "LSR", "SRE",
 "PHA", "EOR", "LSR", "ASR", "JMP", "EOR", "LSR", "SRE",
 "BVC", "EOR", "KIL", "SRE", "NOP", "EOR", "LSR", "SRE",
 "CLI", "EOR", "NOP", "SRE", "NOP", "EOR", "LSR", "SRE",
 "RTS", "ADC", "KIL", "RRA", "NOP", "ADC", "ROR", "RRA",
 "PLA", "ADC", "ROR", "ARR", "JMP", "ADC", "ROR", "RRA",
 "BVS", "ADC", "KIL", "RRA", "NOP", "ADC", "ROR", "RRA",
 "SEI", "ADC", "NOP", "RRA", "NOP", "ADC", "ROR", "RRA",
 "NOP", "STA", "NOP", "SAX", "STY", "STA", "STX", "SAX",
 "DEY", "NOP", "TXA", "ANE", "STY", "STA", "STX", "SAX",
 "BCC", "STA", "KIL", "SHA", "STY", "STA", "STX", "SAX",
 "TYA", "STA", "TXS", "SHS", "SHY", "STA", "SHX", "SHA",
 "LDY", "LDA", "LDX", "LAX", "LDY", "LDA", "LDX", "LAX",
 "TAY", "LDA", "TAX", "LXA", "LDY", "LDA", "LDX", "LAX",
 "BCS", "LDA", "KIL", "LAX", "LDY", "LDA", "LDX", "LAX",
 "CLV", "LDA", "TSX", "LAS", "LDY", "LDA", "LDX", "LAX",
 "CPY", "CMP", "NOP", "DCP", "CPY", "CMP", "DEC", "DCP",
 "INY", "CMP", "DEX", "SBX", "CPY", "CMP", "DEC", "DCP",
 "BNE", "CMP", "KIL", "DCP", "NOP", "CMP", "DEC", "DCP",
 "CLD", "CMP", "NOP", "DCP", "NOP", "CMP", "DEC", "DCP",
 "CPX", "SBC", "NOP", "ISB", "CPX", "SBC", "INC", "ISB",
 "INX", "SBC", "NOP", "SBC", "CPX", "SBC", "INC", "ISB",
 "BEQ", "SBC", "KIL", "ISB", "NOP", "SBC", "INC", "ISB",
 "SED", "SBC", "NOP", "ISB", "NOP", "SBC", "INC", "ISB"};
const char InstrAddrModeTable[] =
{
    ADDR_IMPLIED, ADDR_INDIRECT_X, ADDR_IMMEDIATE, ADDR_INDIRECT_X,
    ADDR_ZERO_PAGE, ADDR_ZERO_PAGE, ADDR_ZERO_PAGE, ADDR_ZERO_PAGE,
    ADDR_IMPLIED, ADDR_IMMEDIATE, ADDR_ACCUMULATOR, ADDR_IMMEDIATE,
    ADDR_ABSOLUTE, ADDR_ABSOLUTE, ADDR_ABSOLUTE, ADDR_ABSOLUTE,
    ADDR_RELATIVE, ADDR_INDIRECT_Y, ADDR_INDIRECT_Y, ADDR_INDIRECT_Y,
    ADDR_ZERO_PAGE_X, ADDR_ZERO_PAGE_X, ADDR_ZERO_PAGE_X, ADDR_ZERO_PAGE_X,
    ADDR_IMPLIED, ADDR_ABSOLUTE_Y, ADDR_IMPLIED, ADDR_ABSOLUTE_Y,
    ADDR_ABSOLUTE_X, ADDR_ABSOLUTE_X, ADDR_ABSOLUTE_X, ADDR_ABSOLUTE_X,

    ADDR_ABSOLUTE, ADDR_INDIRECT_X, ADDR_IMMEDIATE, ADDR_INDIRECT_X,
    ADDR_ZERO_PAGE, ADDR_ZERO_PAGE, ADDR_ZERO_PAGE, ADDR_ZERO_PAGE,
    ADDR_IMPLIED, ADDR_IMMEDIATE, ADDR_ACCUMULATOR, ADDR_IMMEDIATE,
    ADDR_ABSOLUTE, ADDR_ABSOLUTE, ADDR_ABSOLUTE, ADDR_ABSOLUTE,
    ADDR_RELATIVE, ADDR_INDIRECT_Y, ADDR_INDIRECT_Y, ADDR_INDIRECT_Y,
    ADDR_ZERO_PAGE_X, ADDR_ZERO_PAGE_X, ADDR_ZERO_PAGE_X, ADDR_ZERO_PAGE_X,
    ADDR_IMPLIED, ADDR_ABSOLUTE_Y, ADDR_IMPLIED, ADDR_ABSOLUTE_Y,
    ADDR_ABSOLUTE_X, ADDR_ABSOLUTE_X, ADDR_ABSOLUTE_X, ADDR_ABSOLUTE_X,

    ADDR_IMPLIED, ADDR_INDIRECT_X, ADDR_IMMEDIATE, ADDR_INDIRECT_X,
    ADDR_ZERO_PAGE, ADDR_ZERO_PAGE, ADDR_ZERO_PAGE, ADDR_ZERO_PAGE,
    ADDR_IMPLIED, ADDR_IMMEDIATE, ADDR_ACCUMULATOR, ADDR_IMMEDIATE,
    ADDR_ABSOLUTE, ADDR_ABSOLUTE, ADDR_ABSOLUTE, ADDR_ABSOLUTE,
    ADDR_RELATIVE, ADDR_INDIRECT_Y, ADDR_INDIRECT_Y, ADDR_INDIRECT_Y,
    ADDR_ZERO_PAGE_X, ADDR_ZERO_PAGE_X, ADDR_ZERO_PAGE_X, ADDR_ZERO_PAGE_X,
    ADDR_IMPLIED, ADDR_ABSOLUTE_Y, ADDR_IMPLIED, ADDR_ABSOLUTE_Y,
    ADDR_ABSOLUTE_X, ADDR_ABSOLUTE_X, ADDR_ABSOLUTE_X, ADDR_ABSOLUTE_X,

    ADDR_IMPLIED, ADDR_INDIRECT_X, ADDR_IMMEDIATE, ADDR_INDIRECT_X,
    ADDR_ZERO_PAGE, ADDR_ZERO_PAGE, ADDR_ZERO_PAGE, ADDR_ZERO_PAGE,
    ADDR_IMPLIED, ADDR_IMMEDIATE, ADDR_ACCUMULATOR, ADDR_IMMEDIATE,
    ADDR_ABSOLUTE_IND, ADDR_ABSOLUTE, ADDR_ABSOLUTE, ADDR_ABSOLUTE,
    ADDR_RELATIVE, ADDR_INDIRECT_Y, ADDR_INDIRECT_Y, ADDR_INDIRECT_Y,
    ADDR_ZERO_PAGE_X, ADDR_ZERO_PAGE_X, ADDR_ZERO_PAGE_X, ADDR_ZERO_PAGE_X,
    ADDR_IMPLIED, ADDR_ABSOLUTE_Y, ADDR_IMPLIED, ADDR_ABSOLUTE_Y,
    ADDR_ABSOLUTE_X, ADDR_ABSOLUTE_X, ADDR_ABSOLUTE_X, ADDR_ABSOLUTE_X,

    ADDR_IMMEDIATE, ADDR_INDIRECT_X, ADDR_IMMEDIATE, ADDR_INDIRECT_X,
    ADDR_ZERO_PAGE, ADDR_ZERO_PAGE, ADDR_ZERO_PAGE, ADDR_ZERO_PAGE,
    ADDR_IMPLIED, ADDR_IMMEDIATE, ADDR_IMPLIED, ADDR_IMMEDIATE,
    ADDR_ABSOLUTE, ADDR_ABSOLUTE, ADDR_ABSOLUTE, ADDR_ABSOLUTE,
    ADDR_RELATIVE, ADDR_INDIRECT_Y, ADDR_INDIRECT_Y, ADDR_INDIRECT_Y,
    ADDR_ZERO_PAGE_X, ADDR_ZERO_PAGE_X, ADDR_ZERO_PAGE_Y, ADDR_ZERO_PAGE_X,
    ADDR_IMPLIED, ADDR_ABSOLUTE_Y, ADDR_IMPLIED, ADDR_ABSOLUTE_Y,
    ADDR_ABSOLUTE_X, ADDR_ABSOLUTE_X, ADDR_ABSOLUTE_X, ADDR_ABSOLUTE_X,

    ADDR_IMMEDIATE, ADDR_INDIRECT_X, ADDR_IMMEDIATE, ADDR_INDIRECT_X,
    ADDR_ZERO_PAGE, ADDR_ZERO_PAGE, ADDR_ZERO_PAGE, ADDR_ZERO_PAGE,
    ADDR_IMPLIED, ADDR_IMMEDIATE, ADDR_IMPLIED, ADDR_IMMEDIATE,
    ADDR_ABSOLUTE, ADDR_ABSOLUTE, ADDR_ABSOLUTE, ADDR_ABSOLUTE,
    ADDR_RELATIVE, ADDR_INDIRECT_Y, ADDR_INDIRECT_Y, ADDR_INDIRECT_Y,
    ADDR_ZERO_PAGE_X, ADDR_ZERO_PAGE_X, ADDR_ZERO_PAGE_Y, ADDR_ZERO_PAGE_X,
    ADDR_IMPLIED, ADDR_ABSOLUTE_Y, ADDR_IMPLIED, ADDR_ABSOLUTE_Y,
    ADDR_ABSOLUTE_X, ADDR_ABSOLUTE_X, ADDR_ABSOLUTE_Y, ADDR_ABSOLUTE_X,

    ADDR_IMMEDIATE, ADDR_INDIRECT_X, ADDR_IMMEDIATE, ADDR_INDIRECT_X,
    ADDR_ZERO_PAGE, ADDR_ZERO_PAGE, ADDR_ZERO_PAGE, ADDR_ZERO_PAGE,
    ADDR_IMPLIED, ADDR_IMMEDIATE, ADDR_IMPLIED, ADDR_IMMEDIATE,
    ADDR_ABSOLUTE, ADDR_ABSOLUTE, ADDR_ABSOLUTE, ADDR_ABSOLUTE,
    ADDR_RELATIVE, ADDR_INDIRECT_Y, ADDR_INDIRECT_Y, ADDR_INDIRECT_Y,
    ADDR_ZERO_PAGE_X, ADDR_ZERO_PAGE_X, ADDR_ZERO_PAGE_X, ADDR_ZERO_PAGE_X,
    ADDR_IMPLIED, ADDR_ABSOLUTE_Y, ADDR_IMPLIED, ADDR_ABSOLUTE_Y,
    ADDR_ABSOLUTE_X, ADDR_ABSOLUTE_X, ADDR_ABSOLUTE_X, ADDR_ABSOLUTE_X,

    ADDR_IMMEDIATE, ADDR_INDIRECT_X, ADDR_IMMEDIATE, ADDR_INDIRECT_X,
    ADDR_ZERO_PAGE, ADDR_ZERO_PAGE, ADDR_ZERO_PAGE, ADDR_ZERO_PAGE,
    ADDR_IMPLIED, ADDR_IMMEDIATE, ADDR_IMPLIED, ADDR_IMMEDIATE,
    ADDR_ABSOLUTE, ADDR_ABSOLUTE, ADDR_ABSOLUTE, ADDR_ABSOLUTE,
    ADDR_RELATIVE, ADDR_INDIRECT_Y, ADDR_INDIRECT_Y, ADDR_INDIRECT_Y,
    ADDR_ZERO_PAGE_X, ADDR_ZERO_PAGE_X, ADDR_ZERO_PAGE_X, ADDR_ZERO_PAGE_X,
    ADDR_IMPLIED, ADDR_ABSOLUTE_Y, ADDR_IMPLIED, ADDR_ABSOLUTE_Y,
    ADDR_ABSOLUTE_X, ADDR_ABSOLUTE_X, ADDR_ABSOLUTE_X, ADDR_ABSOLUTE_X
};
bool IsAROMLoaded = false;

bool TimeString(char *Out) /*Outputs the system time as a string.
                            *Argument needs to be 9 bytes*/
{
    SYSTEMTIME Time;
    LPSYSTEMTIME TimePointer = &Time;
    GetSystemTime(TimePointer);
    Out[0] = (TimePointer->wHour   / 10) + 48;
    Out[1] = (TimePointer->wHour   % 10) + 48;
    Out[2] = ':';
    Out[3] = (TimePointer->wMinute / 10) + 48;
    Out[4] = (TimePointer->wMinute % 10) + 48;
    Out[5] = ':';
    Out[6] = (TimePointer->wSecond / 10) + 48;
    Out[7] = (TimePointer->wSecond % 10) + 48;
    Out[8] = 0;
    return true;
}
bool WriteToLog(char* Message) //Writes the message to the log file
{
    DWORD MessageNumberOfBytesWritten = 0;
    WriteFile(NES.LogFileHandle,
              Message,
              strlen(Message),
              &MessageNumberOfBytesWritten,
              NULL);
    if(MessageNumberOfBytesWritten != strlen(Message))
    {
        return false;
    }
    return true;
}
bool Log(char *Message)
//writes to the log file the time in the HH:MM:SS format followed by a colon,
//then the message, then a newline
{
    char Time[9];
    TimeString(Time);
    DWORD TimeNumberOfBytesWritten = 0;
    WriteFile(NES.LogFileHandle,
              Time,
              8,
              &TimeNumberOfBytesWritten,
              NULL);
    if(TimeNumberOfBytesWritten != 8)
    {
        return false;
    }
    char colon [2] = {':', ' '};
    DWORD ColonNumberOfBytesWritten = 0;
    WriteFile(NES.LogFileHandle,
              &colon,
              2,
              &ColonNumberOfBytesWritten,
              NULL);
    if(ColonNumberOfBytesWritten != 2)
    {
        return false;
    }
    DWORD MessageNumberOfBytesWritten = 0;
    WriteFile(NES.LogFileHandle,
              Message,
              strlen(Message),
              &MessageNumberOfBytesWritten,
              NULL);
    if(MessageNumberOfBytesWritten != strlen(Message))
    {
        return false;
    }
    char *newLine = "\r\n";
    DWORD newLineNumberOfBytesWritten = 0;
    WriteFile(NES.LogFileHandle,
              newLine,
              2,
              &newLineNumberOfBytesWritten,
              NULL);
    if(newLineNumberOfBytesWritten != 2)
    {
        return false;
    }
    return true;
}
bool CreateLogFile()
{
    SYSTEMTIME Time; //The log's name has format logSSMMHHDDMMYY.txt, so we need
                     //the system time
    LPSYSTEMTIME TimePointer = &Time;
    GetSystemTime(TimePointer);
    char LogName [20] {};
    LogName[0]  = 'l';
    LogName[1]  = 'o';
    LogName[2]  = 'g';
    LogName[3]  = (TimePointer->wDay    / 10)          + 48;
    LogName[4]  = (TimePointer->wDay    % 10)          + 48;
    LogName[5]  = (TimePointer->wMonth  / 10)          + 48;
    LogName[6]  = (TimePointer->wMonth  % 10)          + 48;
    LogName[7]  = ((TimePointer->wYear  % 100) / 10)   + 48;
    LogName[8]  = (TimePointer->wYear   % 10)          + 48;
    LogName[9]  = (TimePointer->wHour   / 10)          + 48;
    LogName[10] = (TimePointer->wHour   % 10)          + 48;
    LogName[11] = (TimePointer->wMinute / 10)          + 48;
    LogName[12] = (TimePointer->wMinute % 10)          + 48;
    LogName[13] = (TimePointer->wSecond / 10)          + 48;
    LogName[14] = (TimePointer->wSecond % 10)          + 48;
    LogName[15] = '.';
    LogName[16] = 't';
    LogName[17] = 'x';
    LogName[18] = 't';
    LogName[19] =  0;
    NES.LogFileHandle = CreateFile(LogName,
                                        GENERIC_READ | GENERIC_WRITE,
                                        FILE_SHARE_READ | FILE_SHARE_WRITE,
                                        NULL,
                                        CREATE_NEW,
                                        FILE_ATTRIBUTE_NORMAL,
                                        NULL);
    if(NES.LogFileHandle == INVALID_HANDLE_VALUE)
    {
        return false;
    }
    else
    {
        return true;
    }
}
void UnsignedtoString(unsigned number, char* string)
{
    unsigned numberOfDigits = 0;
    if(number)
    {
        for(int i = 1, tempNumber = number; i <= 10; i++)
        {
            tempNumber /= 10;
            if(!tempNumber)
            {
                numberOfDigits = i;
                break;
            }
        }
        for(int j = 0; j < (10 - numberOfDigits); j++)
        {
            string[j] = '0';
        }
        for(int k = numberOfDigits - 1; k >= 0; k--)
        {
            string[10 - numberOfDigits + k] = (char)((number % 10) + 48);
            number /= 10;
        }
        string[10] = '\0';
    }
    else
    {
        string[0] = '0';
        string[1] = '\0';
    }
}

void toUpper(char *string, int length)
{
    char Letters[] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i',
                      'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r',
                      's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};
    for(int i = 0; i < length; i++)
    {
        for(int j = 0; j < 26; j++)
        {
            if(string[i] == Letters[j])
            {
                string[i] -= 32;
            }
        }
    }
}

bool isValidHex(char *string, int length)
{
    bool isValid = false;
    char ValidHexChars[] = {'A', 'B', 'C', 'D', 'E', 'F', '0', '1', '2', '3', '4',
                            '5', '6', '7', '8', '9'};
    for(int i = 0; i < length; i++)
    {
        isValid = false;
        for(int j = 0; j < 16; j++)
        {
            if(string[i] == ValidHexChars[j])
            {
                isValid = true;
            }
        }
        if(!isValid)
        {
            return false;
        }
    }
    return true;
}

void UchartoHex(unsigned char number, char* string, bool NullTerminated)
{
    string[0] = ToHex[(number & 0b11110000) >> 4];
    string[1] = ToHex[number & 0b1111];
    if(NullTerminated)
    {
        string[2] = 0;
    }
}

void UshorttoHex(unsigned short number, char* string, bool NullTerminated)
{
    string[0] = ToHex[(number & 0b1111000000000000) >> 12];
    string[1] = ToHex[(number & 0b111100000000) >> 8];
    string[2] = ToHex[(number & 0b11110000) >> 4];
    string[3] = ToHex[number & 0b1111];
    if(NullTerminated)
    {
        string[4] = 0;
    }
}

unsigned short HextoUshort(char *string)
{
    unsigned short retVal = 0;
    retVal |= ((string[0] >= 'A')?string[0]-55:string[0]-48) << 12;    
    retVal |= ((string[1] >= 'A')?string[1]-55:string[1]-48) << 8;    
    retVal |= ((string[2] >= 'A')?string[2]-55:string[2]-48) << 4;    
    retVal |= ((string[3] >= 'A')?string[3]-55:string[3]-48);
    return retVal;
}

bool LoadROM(char *path)
{
    if(NES.ROMFile)
    {
        VirtualFree(NES.ROMFile, NES.ROMFileSize, MEM_RELEASE);
        NES.RAM.Destroy();
    }
    HANDLE FileHandle = CreateFile(path,
                                   GENERIC_READ,
                                   FILE_SHARE_READ,
                                   NULL,
                                   OPEN_EXISTING,
                                   FILE_ATTRIBUTE_NORMAL |
                                   FILE_FLAG_OVERLAPPED,
                                   NULL);
    DWORD FileSizeHigh, FileSizeLow;
    NES.ROMFileSize = GetFileSize(FileHandle, &FileSizeHigh);
    if(NES.ROMFileSize < 16)
    {
        MessageBox(0, TEXT("File too small"), TEXT("Error"), MB_OK);
        return false;
    }
    NES.ROMFile = (unsigned char*)VirtualAlloc(0, NES.ROMFileSize, MEM_COMMIT, PAGE_READWRITE);
    OVERLAPPED ol {};
    DWORD BytesRead;
    ReadFile(FileHandle,
             NES.ROMFile,
             NES.ROMFileSize,
             &BytesRead,
             &ol);
    unsigned char *FileMemoryChar     = (unsigned char *)(NES.ROMFile);
    unsigned      *FileMemoryUnsigned = (unsigned *)(NES.ROMFile);
    if(FileMemoryUnsigned[0] != 0x1A53454E) //NES\x14
    {
        MessageBox(0, TEXT("Invalid NES file."), TEXT("Error"), MB_OK);
        return false;
    }
    NES.RAM.Mapper = (FileMemoryChar[6] >> 4) | (FileMemoryChar[7] & 0xF0);
    if(NES.RAM.Mapper != 0)
    {
        MessageBox(0, TEXT("Unsupported mapper."), TEXT("Error"), MB_OK);
        return false;
    }
    NES.RAM.PRGROMSize = FileMemoryChar[4] << 14;
    NES.PPU.CHRROMSize = FileMemoryChar[5] << 13;
    NES.PPU.Mirroring  = (FileMemoryChar[6] & 9);
    NES.RAM.Init(NES.RAM.Mapper);
    NES.PPU.Init(NES.RAM.Mapper);
    CloseHandle(FileHandle);
    IsAROMLoaded = true;
    NES.CPU.Reset();
    return true;
}

LRESULT CALLBACK AboutDlgProc
(HWND   hWnd,
 UINT   uMsg,
 WPARAM wParam,
 LPARAM lParam)
{
    switch(uMsg)
    {
        case WM_INITDIALOG:
        {
            return TRUE;
        }
        case WM_COMMAND:
        {
            if((wParam & 65535) == IDOK)
            {
                EndDialog(hWnd, IDOK);
                return TRUE;
            }
        }
        default:
        {
            return FALSE;
        }
    }
    return TRUE;
}

void TestRender(int a)
{
    unsigned *Row = (unsigned *)NES.RenderBuffer.Memory;
    for(int y = 0;
        y < -NES.RenderBuffer.Height;
        y++)
    {
        unsigned *Pixel = (unsigned *)Row;
        for(int x = 0;
            x < NES.RenderBuffer.Width;
            x++)
        {
            if(y < 100 & x < 100)
            {
                *Pixel = 0x0000ff00;
            }
            else
            {
                *Pixel = 0x00ff0000;
            }
            if(a == 1)
            {
                if(y > 620 & x > 980)
                {
                    *Pixel = 0x0000ff00;
                }
            }
            Pixel++;
        }
        Row += NES.RenderBuffer.Width;
    }
}

void ShowDisassembly(HWND hWnd, int Control, unsigned short Address)
{
    unsigned a = 0;
    unsigned short b = NES.CPU.PC;
    char* string =
        (char *)VirtualAlloc(0, 2048, MEM_COMMIT, PAGE_READWRITE);
    memset((void *)string, 0, 2048);
    for(int i = 0; i < 25; i++)
    {
        UshorttoHex(b, string+a, false);
        string[a += 4] = ' ';
        a++;
        unsigned char Instruction = NES.RAM.ReadWithNoSideEffects(b);
        void const *InstructionString =
            (void const *)InstructionTable[Instruction];
        memcpy((void *)&(string[a]), InstructionString, 4);
        a += 3;
        switch(InstrAddrModeTable[Instruction])
        {
            case ADDR_IMPLIED:
            {
                string[a] = '\n';
                a++;
                b++;
                break;
            }
            case ADDR_IMMEDIATE:
            {
                string[a] = ' ';
                a++;
                string[a] = '#';
                a++;
                UchartoHex(NES.RAM.ReadWithNoSideEffects(b+1), string+a, false);
                a+=2;
                string[a] = '\n';
                a++;
                b+=2;
                break;
            }
            case ADDR_ZERO_PAGE:
            {
                string[a] = ' ';
                a++;
                UchartoHex(NES.RAM.ReadWithNoSideEffects(b+1), string+a, false);
                a+=2;
                string[a] = '\n';
                a++;
                b+=2;
                break;
            }
            case ADDR_ZERO_PAGE_X:
            {
                string[a] = ' ';
                a++;
                UchartoHex(NES.RAM.ReadWithNoSideEffects(b+1), string+a, false);
                a+=2;
                string[a] = '+';
                a++;
                string[a] = 'X';
                a++;
                string[a] = '\n';
                a++;
                b+=2;
                break;
            }
            case ADDR_ZERO_PAGE_Y:
            {
                string[a] = ' ';
                a++;
                UchartoHex(NES.RAM.ReadWithNoSideEffects(b+1), string+a, false);
                a+=2;
                string[a] = '+';
                a++;
                string[a] = 'Y';
                a++;
                string[a] = '\n';
                a++;
                b+=2;
                break;
            }
            case ADDR_ABSOLUTE:
            {
                string[a] = ' ';
                a++;
                unsigned short c = NES.RAM.ReadWithNoSideEffects(b+1);
                c |= (NES.RAM.ReadWithNoSideEffects(b+2) << 8);
                UshorttoHex(c, string+a, false);
                a += 4;
                string[a] = '\n';
                a++;
                b+=3;
                break;
            }
            case ADDR_ABSOLUTE_IND:
            {
                string[a] = ' ';
                a++;
                string[a] = '(';
                a++;
                unsigned short c = NES.RAM.ReadWithNoSideEffects(b+1);
                c |= (NES.RAM.ReadWithNoSideEffects(b+2) << 8);
                UshorttoHex(c, string+a, false);
                a+=4;
                string[a] = ')';
                a++;
                string[a] = '\n';
                a++;
                b+=3;
                break;
            }
            case ADDR_ABSOLUTE_X:
            {
                string[a] = ' ';
                a++;
                unsigned short c = NES.RAM.ReadWithNoSideEffects(b+1);
                c |= (NES.RAM.ReadWithNoSideEffects(b+2) << 8);
                UshorttoHex(c, string+a, false);
                a += 4;
                string[a] = '+';
                a++;
                string[a] = 'X';
                a++;
                string[a] = '\n';
                a++;
                b+=3;
                break;
            }
            case ADDR_ABSOLUTE_Y:
            {
                string[a] = ' ';
                a++;
                unsigned short c = NES.RAM.ReadWithNoSideEffects(b+1);
                c |= (NES.RAM.ReadWithNoSideEffects(b+2) << 8);
                UshorttoHex(c, string+a, false);
                a += 4;
                string[a] = '+';
                a++;
                string[a] = 'Y';
                a++;
                string[a] = '\n';
                a++;
                b+=3;
                break;
            }
            case ADDR_INDIRECT_X:
            {
                string[a] = ' ';
                a++;
                string[a] = '(';
                a++;
                UchartoHex(NES.RAM.ReadWithNoSideEffects(b+1), string+a, false);
                a += 2;
                string[a] = '+';
                a++;
                string[a] = 'X';
                a++;
                string[a] = ')';
                a++;
                string[a] = '\n';
                a++;
                b+=2;
                break;
            }
            case ADDR_INDIRECT_Y:
            {
                string[a] = ' ';
                a++;
                string[a] = '(';
                a++;
                UchartoHex(NES.RAM.ReadWithNoSideEffects(b+1), string+a, false);
                a += 2;
                string[a] = ')';
                a++;
                string[a] = '+';
                a++;
                string[a] = 'Y';
                a++;
                string[a] = '\n';
                a++;
                b+=2;
                break;
            }
            case ADDR_ACCUMULATOR:
            {
                string[a] = ' ';
                a++;
                string[a] = 'A';
                a++;
                string[a] = '\n';
                a++;
                b++;
                break;
            }
            case ADDR_RELATIVE: //TODO
            {
                string[a] = ' ';
                a++;
                unsigned short b_ = b + (char)NES.RAM.ReadWithNoSideEffects(b+1) + 2;
                UshorttoHex(b_, string+a, false);
                a += 4;
                string[a] = '\n';
                a++;
                b+=2;
                break;
            }
            default:
            {
                break;
            }
        }
    }
    SetDlgItemText(hWnd, Control, string);
    VirtualFree(string, 2048, MEM_RELEASE);
}

void ShowMemory(HWND hWnd, int Control, unsigned short Address)
{
    char* string;
    string = (char *)VirtualAlloc(0, 2048, MEM_COMMIT, PAGE_READWRITE);
    memset((void *)string, 0, 2048);
    int k = 0;
    for(int j = 0; j < 25; j++)
    {
        UshorttoHex(Address, string+k, false);
        k += 4;
        for(int i = 0; i < 16; i++)
        {
            string[k] = ' ';
            k++;
            UchartoHex(NES.RAM.ReadWithNoSideEffects(Address+i), string+k, false);
            k += 2;
        }
        string[k] = '\r';
        string[k+1] = '\n';
        k += 2;
        Address += 16;
    }
    SetDlgItemText(hWnd, Control, string);
    VirtualFree(string, 2048, MEM_RELEASE);
}

void ShowRegisters(HWND hWnd, int AControl, int XControl, int YControl,
                   int SControl, int PControl)
{
    char string[12];
    memset((void*)string, 0, 12);
    string[0] = 'A';
    string[1] = ':';
    string[2] = ' ';
    UchartoHex(NES.CPU.A, string+3, false);
    SetDlgItemText(hWnd, AControl, string);
    string[0] = 'X';
    UchartoHex(NES.CPU.X, string+3, false);
    SetDlgItemText(hWnd, XControl, string);
    string[0] = 'Y';
    UchartoHex(NES.CPU.Y, string+3, false);
    SetDlgItemText(hWnd, YControl, string);
    string[0] = 'S';
    UchartoHex(NES.CPU.S, string+3, false);
    SetDlgItemText(hWnd, SControl, string);
    string[0]  = 'P';
    string[3]  = (NES.CPU.N)?'N':'n';
    string[4]  = (NES.CPU.V)?'V':'v';
    string[5]  = string[6] = '-';
    string[7]  = (NES.CPU.D)?'D':'d';
    string[8]  = (NES.CPU.I)?'I':'i';
    string[9]  = (NES.CPU.Z)?'Z':'z';
    string[10] = (NES.CPU.C)?'C':'c';
    SetDlgItemText(hWnd, PControl, string);
}

void RefreshCHRROMViewer()
{
    if(NES.ROMFile)
    {
        for(int i = 0; i < 2; i++) //Pattern tables
        {
            for(int j = 0; j < 16; j++) //Tile rows
            {
                for(int k = 0; k < 16; k++) //Tile columns
                {
                    for(int m = 0; m < 8; m++) //Rows
                    {
                        unsigned Address = (j << 8) | (k << 4) | m;
                        for(int n = 0; n < 8; n++) //Pixels
                        {
                            unsigned ColorLowBit =
                                NES.PPU.PatternTables[i][Address];
                            ColorLowBit &= (1 << (7 - n));
                            ColorLowBit >>= (7 - n); 
                            unsigned ColorHighBit =
                                NES.PPU.PatternTables[i][Address+8];
                            ColorHighBit &= (1 << (7 - n));
                            ColorHighBit >>= (7 - n);
                            ColorHighBit <<= 1;
                            unsigned Color = (ColorLowBit | ColorHighBit);
                            NES.Debugger.PatternTablesRendered
                                [((i * 128 + j * 8 + m) * 128) +
                                 (k * 8 + n)] =
                                NES.Debugger.Greys[Color];
                        }
                    }
                }
            }
        }
    }
}

LRESULT CALLBACK CHRROMViewerProc
(HWND hWnd,
 UINT uMsg,
 WPARAM wParam,
 LPARAM lParam)
{
    switch(uMsg)
    {
        case WM_COMMAND:
        {
            switch(wParam & 0xFFF)
            {
                default:
                {
                    return FALSE;
                }
            }
        }
        case WM_INITDIALOG:
        {
            NES.Debugger.PatternTablesRendered = (unsigned *)
                VirtualAlloc(0, 4 * 256 * 128,
                             MEM_COMMIT,
                             PAGE_READWRITE);
            RefreshCHRROMViewer();
            return TRUE;
        }
        case WM_PAINT:
        {
            RefreshCHRROMViewer();
            HDC DeviceContext = GetDC(hWnd);
            RECT ClientRect;
            GetClientRect(hWnd, &ClientRect);
            StretchDIBits(DeviceContext,
                          0, 0,
                          ClientRect.right - ClientRect.left,
                          ClientRect.bottom - ClientRect.top,
                          0, 0, 128, 256,
                          NES.Debugger.PatternTablesRendered,
                          &NES.Debugger.PTRInfo,
                          DIB_RGB_COLORS, SRCCOPY);
            ReleaseDC(hWnd, DeviceContext);
            return TRUE;
        }
        case WM_CLOSE:
        case WM_DESTROY:
        {
            if(NES.Debugger.PatternTablesRendered)
            {
                VirtualFree(NES.Debugger.PatternTablesRendered,
                            4 * 128 * 256, MEM_RELEASE);
                NES.Debugger.PatternTablesRendered = 0;
            }
            DestroyWindow(hWnd);
            NES.Debugger.CHRROMViewerHandle = 0;
            NES.Debugger.isCHRROMViewerOpen = false;
            return TRUE;
        }
        default:
        {
            return FALSE;
        }
    }
}

void RefreshPaletteViewer()
{
    if(NES.ROMFile)
    {
        {
            for(int i = 0; i < 4; i++) //Background palettes
            {
                for(int j = 0; j < 16; j++) //Rows
                {
                    for(int k = 0; k < 4; k++) //Palette colors
                    {
                        for(int l = 0; l < 16; l++) //Columns
                        {
                            unsigned Color =
                                NES.Debugger.ColorRGBTable
                                [NES.PPU.BackgroundPalettes[i][k]];
                            NES.Debugger.PalettesRendered
                                [i * 16 * 64 + j * 64 + k * 16 + l] = Color;
                        }
                    }
                }
            }
            for(int i = 4; i < 8; i++) //Sprite palettes
            {
                for(int j = 0; j < 16; j++) //Rows
                {
                    for(int k = 0; k < 4; k++) //Palette colors
                    {
                        for(int l = 0; l < 16; l++) //Columns
                        {
                            unsigned Color =
                                NES.Debugger.ColorRGBTable
                                [NES.PPU.SpritePalettes[i - 4][k]];
                            NES.Debugger.PalettesRendered
                                [i * 16 * 64 + j * 64 + k * 16 + l] = Color;
                        }
                    }
                }
            }
        }
    }
}

LRESULT CALLBACK PaletteViewerProc
(HWND   hWnd,
 UINT   uMsg,
 WPARAM wParam,
 LPARAM lParam)
{
    switch(uMsg)
    {
        case WM_INITDIALOG:
        {
            NES.Debugger.PalettesRendered = (unsigned *)
                VirtualAlloc(0, 4 * 64 * 128, MEM_COMMIT, PAGE_READWRITE);
            RefreshPaletteViewer();
            return TRUE;
        }
        case WM_PAINT:
        {
            RefreshPaletteViewer();
            HDC DeviceContext = GetDC(hWnd);
            RECT ClientRect;
            GetClientRect(hWnd, &ClientRect);
            StretchDIBits(DeviceContext,
                          0, 0,
                          ClientRect.right - ClientRect.left,
                          ClientRect.bottom - ClientRect.top,
                          0, 0, 64, 128,
                          NES.Debugger.PalettesRendered,
                          &NES.Debugger.PTInfo,
                          DIB_RGB_COLORS, SRCCOPY);
            ReleaseDC(hWnd, DeviceContext);
            return TRUE;
        }
        case WM_CLOSE:
        case WM_DESTROY:
        {
            if(NES.Debugger.PalettesRendered)
            {
                VirtualFree(NES.Debugger.PalettesRendered,
                            4 * 128 * 64, MEM_RELEASE);
                NES.Debugger.PalettesRendered = 0;
            }
            DestroyWindow(hWnd);
            NES.Debugger.PaletteViewerHandle = 0;
            NES.Debugger.isPaletteViewerOpen = false;
            return TRUE;
        }
        default:
        {
            return FALSE;
        }
    }
}

void RefreshNametableViewer()
{
    if(NES.ROMFile)
    {
        for(int i = 0; i < 2; i++) //Nametable rows
        {
            unsigned basePosition0 = 2 * i * 256 * 240;
            for(int j = 0; j < 2; j++) //Nametable columns
            {
                unsigned basePosition1 = basePosition0 + j * 256;
                for(int k = 0; k < 240; k++) //Rows
                {
                    unsigned basePosition2 = basePosition1 + k * 2 * 256;
                    for(int l = 0; l < 256; l++) //'Pixels'
                    {
                        unsigned Position = basePosition2 + l;
                        unsigned NametableRow = k / 8;
                        unsigned NametableColumn = l / 8;
                        unsigned NametableEntry = NametableColumn + NametableRow * 32;
                        unsigned CurrentNameTable = i * 2 + j;
                        unsigned char TileToFetch = NES.PPU.Nametables[CurrentNameTable]
                            [NametableEntry];
                        unsigned AttributeRow = k / 32;
                        unsigned AttributeColumn = l / 32;
                        unsigned AttributeEntry = AttributeRow * 8 + AttributeColumn;
                        unsigned LocationInsideEntry =
                            (NametableRow & 2) +
                            ((NametableColumn & 2) >> 1);
                        unsigned char PaletteToUse =
                            ((NES.PPU.Nametables
                              [CurrentNameTable]
                              [960 + AttributeEntry] & (3 << (2*LocationInsideEntry)))
                             >> (2 * LocationInsideEntry));
                        unsigned char CHRROMColor =
                            ((NES.PPU.PatternTables[NES.PPU.PatternTableBase]
                              [(TileToFetch << 4) + (k & 7)]) & (1 << (7 - (l & 7)))) >> (7 - (l & 7));
                        CHRROMColor |=
                            (((NES.PPU.PatternTables[NES.PPU.PatternTableBase]
                               [(TileToFetch << 4) + 8 + (k & 7)]) & (1 << (7 - (l & 7))))
                             >> (7 - (l & 7)))
                            << 1;
                        unsigned char Color = NES.PPU.BackgroundPalettes[PaletteToUse][CHRROMColor];
                        NES.Debugger.NametableRendered[Position] = NES.Debugger.ColorRGBTable[Color];
                    }
                }
            }
        }
    }
}

LRESULT CALLBACK NametableViewerProc
(HWND   hWnd,
 UINT   uMsg,
 WPARAM wParam,
 LPARAM lParam)
{
    switch(uMsg)
    {
        case WM_INITDIALOG:
        {
            NES.Debugger.NametableRendered = (unsigned *)
                VirtualAlloc(0, 4 * 512 * 480, MEM_COMMIT, PAGE_READWRITE);
            RefreshNametableViewer();
            return TRUE;
        }
        case WM_PAINT:
        {
            RefreshNametableViewer();
            HDC DeviceContext = GetDC(hWnd);
            RECT ClientRect;
            GetClientRect(hWnd, &ClientRect);
            int a = StretchDIBits(DeviceContext,
                          0, 0,
                          ClientRect.right - ClientRect.left,
                          ClientRect.bottom - ClientRect.top,
                          0, 0, 512, 480,
                          NES.Debugger.NametableRendered,
                          &NES.Debugger.NTInfo,
                          DIB_RGB_COLORS, SRCCOPY);
            ReleaseDC(hWnd, DeviceContext);
            return TRUE;
        }
        case WM_CLOSE:
        case WM_DESTROY:
        {
            if(NES.Debugger.NametableRendered)
            {
                VirtualFree(NES.Debugger.NametableRendered,
                            4 * 512 * 480, MEM_RELEASE);
                NES.Debugger.NametableRendered = 0;
            }
            DestroyWindow(hWnd);
            NES.Debugger.NametableViewerHandle = 0;
            NES.Debugger.isNametableViewerOpen = false;
            return TRUE;
        }
        default:
        {
            return FALSE;
        }
    }
}

LRESULT CALLBACK DebuggerProc
(HWND   hWnd,
 UINT   uMsg,
 WPARAM wParam,
 LPARAM lParam)
{
    switch(uMsg)
    {
        case WM_INITDIALOG:
        {
            HICON ArrowUpIcon = (HICON)LoadImage(GetModuleHandle(0),
                                                 MAKEINTRESOURCE(ARROWUP_ICON),
                                                 IMAGE_ICON, 15, 14,
                                                 LR_DEFAULTCOLOR);
            HICON ArrowDownIcon = (HICON)LoadImage(GetModuleHandle(0),
                                                   MAKEINTRESOURCE(ARROWDN_ICON),
                                                   IMAGE_ICON, 15, 14,
                                                   LR_DEFAULTCOLOR);
            SendDlgItemMessage(hWnd, IDC_ARROWUP,
                               BM_SETIMAGE, IMAGE_ICON, (LPARAM)ArrowUpIcon);
            SendDlgItemMessage(hWnd, IDC_ARROWDOWN,
                               BM_SETIMAGE, IMAGE_ICON, (LPARAM)ArrowDownIcon);
            ShowDisassembly(hWnd, ID_STATIC_INSTRUCTION, NES.CPU.PC);
            return TRUE;
        }
        case WM_CLOSE:
        case WM_DESTROY:
        {
            DestroyWindow(hWnd);
            NES.Debugger.DebuggerHandle = 0;
            NES.Debugger.isDebuggerActive = false;
            NES.Pause = false;
            return TRUE;
        }
        case WM_COMMAND:
        {
            switch(wParam & 0xFFFF)
            {
                case IDC_ARROWUP:
                {
                    NES.Debugger.CurrentMemoryAddress -= 16;
                    ShowMemory(hWnd, ID_STATIC_MEMORY,
                               NES.Debugger.CurrentMemoryAddress);
                    ShowRegisters(hWnd, ID_STATIC_A, ID_STATIC_X, ID_STATIC_Y,
                                  ID_STATIC_S, ID_STATIC_P);
                    break;
                }
                case IDC_ARROWDOWN:
                {
                    NES.Debugger.CurrentMemoryAddress += 16;
                    ShowMemory(hWnd, ID_STATIC_MEMORY,
                               NES.Debugger.CurrentMemoryAddress);
                    ShowRegisters(hWnd, ID_STATIC_A, ID_STATIC_X, ID_STATIC_Y,
                                  ID_STATIC_S, ID_STATIC_P);
                    break;
                }
                case IDC_REFRESH:
                {
                    ShowDisassembly(hWnd, ID_STATIC_INSTRUCTION, NES.CPU.PC);
                    char EditText[5];
                    GetDlgItemText(hWnd, IDC_EDIT1, EditText, 5);
                    toUpper(EditText, 4);
                    if(isValidHex(EditText, 4))
                    {
                        NES.Debugger.CurrentMemoryAddress = HextoUshort(EditText);
                    }
                    ShowMemory(hWnd, ID_STATIC_MEMORY,
                               NES.Debugger.CurrentMemoryAddress);
                    ShowRegisters(hWnd, ID_STATIC_A, ID_STATIC_X, ID_STATIC_Y,
                                  ID_STATIC_S, ID_STATIC_P);
                    break;
                }
                case ID_CPU_STEP:
                {
                    if(IsAROMLoaded)
                    {
                        if(!NES.KIL)
                        {
                            NES.CPU.Run();
                        }
                        ShowDisassembly(hWnd, ID_STATIC_INSTRUCTION, NES.CPU.PC);
                        ShowMemory(hWnd, ID_STATIC_MEMORY,
                                   NES.Debugger.CurrentMemoryAddress);
                        ShowRegisters(hWnd, ID_STATIC_A, ID_STATIC_X, ID_STATIC_Y,
                                      ID_STATIC_S, ID_STATIC_P);
                    }
                    break;
                }
                case ID_CPU_WAITUNTILNMI:
                {
                    if(IsAROMLoaded)
                    {
                        while(NES.NMI == false && !NES.KIL)
                        {
                            NES.CPU.Run();
                        }
                        ShowDisassembly(hWnd, ID_STATIC_INSTRUCTION, NES.CPU.PC);
                        ShowMemory(hWnd, ID_STATIC_MEMORY,
                                   NES.Debugger.CurrentMemoryAddress);
                        ShowRegisters(hWnd, ID_STATIC_A, ID_STATIC_X, ID_STATIC_Y,
                                      ID_STATIC_S, ID_STATIC_P);
                    }
                    break;
                }
                case ID_PPU_CHRROM:
                {
                    if(!NES.Debugger.isCHRROMViewerOpen)
                    {
                        NES.Debugger.CHRROMViewerHandle =
                            CreateDialog(GetModuleHandle(NULL),
                                         MAKEINTRESOURCE(IDD_CHRROM),
                                         hWnd, CHRROMViewerProc);
                        if(NES.Debugger.CHRROMViewerHandle)
                        {
                            ShowWindow(NES.Debugger.CHRROMViewerHandle, SW_SHOW);
                            NES.Debugger.isCHRROMViewerOpen = true;
                        }
                    }
                    //NES.Debugger.CHRROMViewer.Open(hWnd);
                    break;
                }
                case ID_PPU_PALETTES:
                {
                    if(!NES.Debugger.isPaletteViewerOpen)
                    {
                        NES.Debugger.PaletteViewerHandle =
                            CreateDialog(GetModuleHandle(NULL),
                                         MAKEINTRESOURCE(IDD_PALETTES),
                                         hWnd, PaletteViewerProc);
                        if(NES.Debugger.PaletteViewerHandle)
                        {
                            ShowWindow(NES.Debugger.PaletteViewerHandle, SW_SHOW);
                            NES.Debugger.isPaletteViewerOpen = true;
                        }
                    }
                    //NES.Debugger.PaletteViewer.Open(hWnd);
                    break;
                }
                case ID_PPU_NAMETABLE:
                {
                    
                    if(!NES.Debugger.isNametableViewerOpen)
                    {
                        NES.Debugger.NametableViewerHandle =
                            CreateDialog(GetModuleHandle(NULL),
                                         MAKEINTRESOURCE(IDD_NAMETABLES),
                                         hWnd, NametableViewerProc);
                        if(NES.Debugger.NametableViewerHandle)
                        {
                            ShowWindow(NES.Debugger.NametableViewerHandle, SW_SHOW);
                            NES.Debugger.isNametableViewerOpen = true;
                        }
                    }
                    //NES.Debugger.NametableViewer.Open(hWnd);
                    break;
                }
                default:
                {
                    return FALSE;
                }
            }
        }
        default:
        {
            return FALSE;
        }
    }
}

LRESULT CALLBACK MainWindowCallback
(HWND   hWnd,
 UINT   uMsg,
 WPARAM wParam,
 LPARAM lParam)
{
    switch(uMsg)
    {
        case WM_COMMAND:
        {
            switch(wParam & 0xFFFF)
            {
                case ID_FILE_OPEN:
                {
                    char FileToOpen[256] {};
                    OPENFILENAME OpenFile {};
                    OpenFile.lStructSize = sizeof(OPENFILENAME);
                    OpenFile.hwndOwner = hWnd;
                    OpenFile.hInstance = GetModuleHandle(0);
                    OpenFile.lpstrFilter = TEXT("NES ROMs\0*.nes\0\0");
                    OpenFile.lpstrFile = FileToOpen;
                    OpenFile.nMaxFile = 256;
                    OpenFile.Flags = OFN_FILEMUSTEXIST;
                    OpenFile.lpstrDefExt = TEXT("nes");
                    if(GetOpenFileName(&OpenFile))
                    {
                        if(LoadROM(OpenFile.lpstrFile))
                        {
                            Log("");
                            WriteToLog("\tOpened ROM ");
                            WriteToLog(OpenFile.lpstrFile);
                            WriteToLog("\r\n");
                        }
                    }
                    break;
                }
                case ID_FILE_EXIT:
                {
                    NES.Running = false;
                    return 0;
                }
                case ID_REGION_PAL:
                {
                    if(NES.Region == PAL)
                    {
                        return 0;
                    }
                    NES.Region = PAL;
                    CheckMenuItem(GetMenu(hWnd), ID_REGION_NTSC, MF_UNCHECKED);
                    CheckMenuItem(GetMenu(hWnd), ID_REGION_PAL,  MF_CHECKED);
                    return 0;
                }
                case ID_REGION_NTSC:
                {
                    if(NES.Region == NTSC)
                    {
                        return 0;
                    }
                    NES.Region = NTSC;
                    CheckMenuItem(GetMenu(hWnd), ID_REGION_PAL,  MF_UNCHECKED);
                    CheckMenuItem(GetMenu(hWnd), ID_REGION_NTSC, MF_CHECKED);
                    return 0;
                }
                case ID_SPEED_0:
                {
                    NES.Speed = 0.125f;
                    goto IDSpeedALL;
                }
                case ID_SPEED_1:
                {
                    NES.Speed = 0.25f;
                    goto IDSpeedALL;
                }
                case ID_SPEED_2:
                {
                    NES.Speed = 0.5f;
                    goto IDSpeedALL;
                }
                case ID_SPEED_3:
                {
                    NES.Speed = 0.75f;
                    goto IDSpeedALL;
                }
                case ID_SPEED_5:
                {
                    NES.Speed = 1.5f;
                    goto IDSpeedALL;
                }
                case ID_SPEED_1X:
                {
                    NES.Speed = 1.f;
                    goto IDSpeedALL;
                }
                case ID_SPEED_2X:
                {
                    NES.Speed = 2.f;
                    goto IDSpeedALL;
                }
                case ID_SPEED_4X:
                {
                    NES.Speed = 4.f;
                    goto IDSpeedALL;
                }
                IDSpeedALL:
                {
                    for(int i = ID_SPEED_0;
                        i <= ID_SPEED_4X;
                        i++)
                    {
                        CheckMenuItem(GetMenu(hWnd), i, MF_UNCHECKED);
                    }
                    CheckMenuItem(GetMenu(hWnd), (wParam & 0xFFFF), MF_CHECKED);
                    return 0;
                }
                case ID_ABOUT:
                {
                    DialogBox(GetModuleHandle(NULL),
                              MAKEINTRESOURCE(IDD_ABOUT_DLG),
                              hWnd, AboutDlgProc);
                    return 0;
                }
                case ID_EMULATOR_PAUSE:
                {
                    NES.Pause = !NES.Pause;
                    CheckMenuItem(GetMenu(hWnd), ID_EMULATOR_PAUSE,
                                  NES.Pause?MF_CHECKED:MF_UNCHECKED);
                    return 0;
                }
                case ID_EMULATOR_RESET:
                {
                    NES.CPU.Reset();
                    return 0;
                }
                case ID_TOOLS_DEBUGGER:
                {
                    if(!NES.Debugger.isDebuggerActive)
                    {
                        NES.Debugger.DebuggerHandle =
                            CreateDialog(GetModuleHandle(NULL),
                                         MAKEINTRESOURCE(IDD_DEBUGGER),
                                         hWnd, DebuggerProc);
                        if(NES.Debugger.DebuggerHandle)
                        {
                            ShowWindow(NES.Debugger.DebuggerHandle, SW_SHOW);
                            NES.Debugger.isDebuggerActive = true;
                            NES.Pause = true;
                        }
                    }
                    return 0;
                }
                default:
                {
                    return 0;
                }
            }
            break;
        }
        case WM_SYSCOMMAND:
        {
            switch(wParam & 0xfff0)
            {
                case SC_MONITORPOWER:
                case SC_SCREENSAVE:
                {
                    return 0;
                    break;
                }
                default:
                {
                    return DefWindowProc(hWnd, uMsg, wParam, lParam);
                }
            }
        }
        case WM_CLOSE:
        case WM_DESTROY:
        case WM_QUIT:
        {
            NES.Running = false;
            return 0;
            break;
        }
        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            BeginPaint(hWnd, &ps);
            RECT ClientRect;
            GetClientRect(NES.Window, &ClientRect);
            TestRender(0);
            int a = StretchDIBits(NES.MainWindowDC,
                          0, 0,
                          ClientRect.right  - ClientRect.left,
                          ClientRect.bottom - ClientRect.top,
                          0, 0,
                          NES.RenderBuffer.Width, -NES.RenderBuffer.Height,
                          NES.RenderBuffer.Memory, &NES.RenderBuffer.Info,
                          DIB_RGB_COLORS, SRCCOPY);
            EndPaint(hWnd, &ps);
            return 0;
            break;
        }
        default:
        {
            return CallWindowProc(DefWindowProc, hWnd, uMsg, wParam, lParam);
        }
    }       
}

int CALLBACK WinMain
(HINSTANCE hInstance,
 HINSTANCE hPrevInstance,
 LPSTR     lpCmdLine,
 int       nCmdShow)
{
    NES = NESClass();
    NES.LogFileHandle = INVALID_HANDLE_VALUE;
    NES.RenderBuffer.Width                        = 256;
    NES.RenderBuffer.Height                       = -262;
    NES.RenderBuffer.Info.bmiHeader.biSize        =
        sizeof(NES.RenderBuffer.Info.bmiHeader);
    NES.RenderBuffer.Info.bmiHeader.biWidth       = 256;
    NES.RenderBuffer.Info.bmiHeader.biHeight      = -262;
    NES.RenderBuffer.Info.bmiHeader.biPlanes      = 1;
    NES.RenderBuffer.Info.bmiHeader.biBitCount    = 32;
    NES.RenderBuffer.Info.bmiHeader.biCompression = BI_RGB;
    NES.RenderBuffer.Memory =
        VirtualAlloc(0, 4 * 256 * 262, MEM_COMMIT, PAGE_READWRITE);
    memset(NES.RenderBuffer.Memory, 0, 4 * 256 * 262);
    
    if(!CreateLogFile())
    {
        //Nothing to write the log to, so we just close
        return 1;
    }

    NES.InternalMemory =
        (unsigned char*)(VirtualAlloc(0, 2048, MEM_COMMIT, PAGE_READWRITE));
    memset(NES.InternalMemory, 0, 256);
    NES.CartridgeMemory =
        (unsigned char*)(VirtualAlloc(0, 32768, MEM_COMMIT, PAGE_READWRITE));
    NES.WRAM = (unsigned char*)(VirtualAlloc(0, 8192, MEM_COMMIT, PAGE_READWRITE));
    memset(NES.WRAM, 0, 8192);
    WNDCLASS WindowClass {};
    WindowClass.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    WindowClass.lpfnWndProc   = MainWindowCallback;
    WindowClass.cbClsExtra    = 0;
    WindowClass.cbWndExtra    = 0;
    WindowClass.hInstance     = hInstance;
    WindowClass.hIcon         = LoadIcon(hInstance, MAKEINTRESOURCE(TOOLBAR_ICON));
    WindowClass.hCursor       = LoadCursor(0, MAKEINTRESOURCE(IDC_ARROW));
    WindowClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    WindowClass.lpszMenuName  = MAKEINTRESOURCE(IDR_MENU1);
    WindowClass.lpszClassName = "CarmiNESWindowClass";
    RegisterClass(&WindowClass);
    NES.Window = CreateWindow("CarmiNESWindowClass",
                              "CarmiNES",
                              WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                              CW_USEDEFAULT,
                              CW_USEDEFAULT,
                              CW_USEDEFAULT,
                              CW_USEDEFAULT,
                              NULL,
                              NULL,
                              hInstance,
                              NULL);
    if(!NES.Window)
    {
        int a = GetLastError();
        Log("Aborting. Couldn't create window.");
        return 1;
    }

    Log("Created window.");
        
    CheckMenuItem(GetMenu(NES.Window), ID_SPEED_1X, MF_CHECKED);
    CheckMenuItem(GetMenu(NES.Window), ID_REGION_PAL, MF_CHECKED);
    unsigned nOfFrames = 0;
    NES.MainWindowDC = GetDC(NES.Window);
    QueryPerformanceCounter(&NES.LastFrameTime);
    while(NES.Running) //Every iteration of this loop is a frame
    {
        MSG Message;
        while(PeekMessage(&Message, 0, 0, 0, PM_REMOVE))
        {
            if(NES.Debugger.DebuggerHandle != 0)
            {
                if(!IsDialogMessage(NES.Debugger.DebuggerHandle, &Message))
                {
                    TranslateMessage(&Message);
                    DispatchMessage(&Message);
                }
            }
            else
            {
                TranslateMessage(&Message);
                DispatchMessage(&Message);
            }
        }
        if(NES.Pause || !IsAROMLoaded || NES.Debugger.isDebuggerActive || NES.KIL)
        {
            timeBeginPeriod(1);
            Sleep(33);
            timeEndPeriod(1);
        }
        else
        {
            //HERE LIETH THE CPU LOOP:
            while(true)
            {
                if(NES.FrameCycle >=                                                    /*The frame is over,    */
                   NTSC_CYCLE_COUNT+((PAL_CYCLE_COUNT-NTSC_CYCLE_COUNT)*NES.Region))    /*we pass on to the next*/
                {
                    NES.FrameCycle -=
                        (NTSC_CYCLE_COUNT+
                         ((PAL_CYCLE_COUNT-NTSC_CYCLE_COUNT)*NES.Region));
                    NES.PPU.Run(NES.FrameCycle);
                    break;
                }
                NES.CPU.Run();
            }
        }
        TestRender(0);
        RECT ClientRect;
        GetClientRect(NES.Window, &ClientRect);
        StretchDIBits(NES.MainWindowDC,
                      0, 0,
                      ClientRect.right  - ClientRect.left,
                      ClientRect.bottom - ClientRect.top,
                      0, 0,
                      NES.RenderBuffer.Width, -NES.RenderBuffer.Height,
                      NES.RenderBuffer.Memory, &NES.RenderBuffer.Info,
                      DIB_RGB_COLORS, SRCCOPY);
        nOfFrames++;
        QueryPerformanceCounter(&NES.CurrentFrameTime);
        NES.FrameTimes[NES.FrameCount&63] =
            (NES.CurrentFrameTime.QuadPart - NES.LastFrameTime.QuadPart)
            / NES.PerformanceCounterFrequency.QuadPart;
        long long TimeElapsedMilliseconds = NES.FrameTimes[NES.FrameCount&63];
        if(NES.FrameCount && !(NES.FrameCount & 63))
        {
            char MessageToLog[] = "nnnnnnnnnn FPS.";
            float FrameAverage = 0;
            for(int i = 0; i < 64; i++)
            {
                FrameAverage += NES.FrameTimes[i];
            }
            FrameAverage /= 64;
            UnsignedtoString(1000.f / FrameAverage, MessageToLog);
            MessageToLog[10] = ' ';
            Log(MessageToLog);
        }
        timeBeginPeriod(1);
        if(TimeElapsedMilliseconds > 0 && TimeElapsedMilliseconds < 17)
        {
            Sleep(17-TimeElapsedMilliseconds);
        }
        timeEndPeriod(1);
        NES.LastFrameTime = NES.CurrentFrameTime;
        NES.FrameCount++;
    }
quit:
    char nOfFramesInString[11];
    UnsignedtoString(nOfFrames, nOfFramesInString);
    Log(nOfFramesInString);
    Log("Quitting.");
    CloseHandle(NES.LogFileHandle);
    return 0;
}
void WinMainCRTStartup()
{
    ExitProcess(WinMain(GetModuleHandle(NULL),
                        NULL,
                        GetCommandLine(),
                        SW_SHOWNORMAL));
}
