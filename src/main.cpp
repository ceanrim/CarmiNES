/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Carmine Foggia $
   ======================================================================== */
#include <windows.h>
#include "..\res\resource.h"
#include "main.h"
#include "cpu.h"
#include "memory.h"
namespace globals
{
    int Region = PAL;
    float Speed = 1;
    bool Running;
    HANDLE LogFileHandle = INVALID_HANDLE_VALUE;
    HWND Window;
    HDC MainWindowDC;
    unsigned char *InternalMemory = NULL;
    unsigned char *CartridgeMemory = NULL;
    struct aaa
    {
        BITMAPINFO Info;
        void *Memory;
        int Width;
        int Height;
    };
    aaa RenderBuffer {};
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

        ADDR_IMPLIED, ADDR_INDIRECT_X, ADDR_IMMEDIATE, ADDR_INDIRECT_X,
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
    bool Pause = false;
    bool Debugger = false;
    HWND DebuggerHandle = 0;
    bool IsAROMLoaded = false;
}
namespace Debugger
{
    unsigned short CurrentMemoryAddress = 0;
}
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
    WriteFile(globals::LogFileHandle,
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
    WriteFile(globals::LogFileHandle,
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
    WriteFile(globals::LogFileHandle,
              &colon,
              2,
              &ColonNumberOfBytesWritten,
              NULL);
    if(ColonNumberOfBytesWritten != 2)
    {
        return false;
    }
    DWORD MessageNumberOfBytesWritten = 0;
    WriteFile(globals::LogFileHandle,
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
    WriteFile(globals::LogFileHandle,
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
    globals::LogFileHandle = CreateFile(LogName,
                                        GENERIC_READ | GENERIC_WRITE,
                                        FILE_SHARE_READ | FILE_SHARE_WRITE,
                                        NULL,
                                        CREATE_NEW,
                                        FILE_ATTRIBUTE_NORMAL,
                                        NULL);
    if(globals::LogFileHandle == INVALID_HANDLE_VALUE)
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
    string[0] = globals::ToHex[(number & 0b11110000) >> 4];
    string[1] = globals::ToHex[number & 0b1111];
    if(NullTerminated)
    {
        string[2] = 0;
    }
}

void UshorttoHex(unsigned short number, char* string, bool NullTerminated)
{
    string[0] = globals::ToHex[(number & 0b1111000000000000) >> 12];
    string[1] = globals::ToHex[(number & 0b111100000000) >> 8];
    string[2] = globals::ToHex[(number & 0b11110000) >> 4];
    string[3] = globals::ToHex[number & 0b1111];
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
    HANDLE FileHandle = CreateFile(path,
                                   GENERIC_READ,
                                   FILE_SHARE_READ,
                                   NULL,
                                   OPEN_EXISTING,
                                   FILE_ATTRIBUTE_NORMAL |
                                   FILE_FLAG_OVERLAPPED,
                                   NULL);
    DWORD FileSizeHigh, FileSizeLow;
    FileSizeLow = GetFileSize(FileHandle, &FileSizeHigh);
    if(FileSizeLow < 16)
    {
        MessageBox(0, TEXT("File too small"), TEXT("Error"), MB_OK);
        return false;
    }
    void *FileMemory = VirtualAlloc(0, FileSizeLow, MEM_COMMIT, PAGE_READWRITE);
    OVERLAPPED ol {};
    DWORD BytesRead;
    ReadFile(FileHandle,
             FileMemory,
             FileSizeLow,
             &BytesRead,
             &ol);
    unsigned char *FileMemoryChar     = (unsigned char *)(FileMemory);
    unsigned      *FileMemoryUnsigned = (unsigned *)(FileMemory);
    if(FileMemoryUnsigned[0] != 0x1A53454E) //NES\x14
    {
        MessageBox(0, TEXT("Invalid NES file."), TEXT("Error"), MB_OK);
        return false;
    }
    Memory::Mapper = (FileMemoryChar[6] >> 4) | (FileMemoryChar[7] & 0xF0);
    if(Memory::Mapper != 0)
    {
        MessageBox(0, TEXT("Unsupported mapper."), TEXT("Error"), MB_OK);
        return false;
    }
    Memory::PRGROMSize = FileMemoryChar[4] << 14;
    if((Memory::PRGROMSize != 16384) && (Memory::PRGROMSize != 32768))
    {
        MessageBox(0, TEXT("Mapper 0 file of size different from 16k or 32k."),
                   TEXT("Error"), MB_OK);
        return false;
    }
    unsigned char *PRGROM = FileMemoryChar + 16;
    memcpy(globals::CartridgeMemory, PRGROM, Memory::PRGROMSize);
    CloseHandle(FileHandle);
    globals::IsAROMLoaded = true;
    CPU::Reset();
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
    unsigned *Row = (unsigned *)globals::RenderBuffer.Memory;
    for(int y = 0;
        y < globals::RenderBuffer.Height;
        y++)
    {
        unsigned *Pixel = (unsigned *)Row;
        for(int x = 0;
            x < globals::RenderBuffer.Width;
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
        Row += globals::RenderBuffer.Width;
    }
}

void ShowDisassembly(HWND hWnd, int Control, unsigned short Address)
{
    unsigned a = 0;
    unsigned short b = CPU::PC;
    char* string =
        (char *)VirtualAlloc(0, 2048, MEM_COMMIT, PAGE_READWRITE);
    memset((void *)string, 0, 2048);
    for(int i = 0; i < 25; i++)
    {
        UshorttoHex(b, string+a, false);
        string[a += 4] = ' ';
        a++;
        unsigned char Instruction = Memory::ReadWithNoSideEffects(b);
        void const *InstructionString =
            (void const *)globals::InstructionTable[Instruction];
        memcpy((void *)&(string[a]), InstructionString, 4);
        a += 3;
        switch(globals::InstrAddrModeTable[Instruction])
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
                UchartoHex(Memory::ReadWithNoSideEffects(b+1), string+a, false);
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
                UchartoHex(Memory::ReadWithNoSideEffects(b+1), string+a, false);
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
                UchartoHex(Memory::ReadWithNoSideEffects(b+1), string+a, false);
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
                UchartoHex(Memory::ReadWithNoSideEffects(b+1), string+a, false);
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
                unsigned short c = Memory::ReadWithNoSideEffects(b+1);
                c |= (Memory::ReadWithNoSideEffects(b+2) << 8);
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
                unsigned short c = Memory::ReadWithNoSideEffects(b+1);
                c |= (Memory::ReadWithNoSideEffects(b+2) << 8);
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
                unsigned short c = Memory::ReadWithNoSideEffects(b+1);
                c |= (Memory::ReadWithNoSideEffects(b+2) << 8);
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
                unsigned short c = Memory::ReadWithNoSideEffects(b+1);
                c |= (Memory::ReadWithNoSideEffects(b+2) << 8);
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
                UchartoHex(Memory::ReadWithNoSideEffects(b+1), string+a, false);
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
                UchartoHex(Memory::ReadWithNoSideEffects(b+1), string+a, false);
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
                break;
            }
            case ADDR_RELATIVE: //TODO
            {
                string[a] = '\n';
                a++;
                b++;
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
            UchartoHex(Memory::ReadWithNoSideEffects(Address+i), string+k, false);
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
    char string[6];
    memset((void*)string, 0, 6);
    string[0] = 'A';
    string[1] = ':';
    string[2] = ' ';
    UchartoHex(CPU::A, string+3, false);
    SetDlgItemText(hWnd, AControl, string);
    string[0] = 'X';
    UchartoHex(CPU::X, string+3, false);
    SetDlgItemText(hWnd, XControl, string);
    string[0] = 'Y';
    UchartoHex(CPU::Y, string+3, false);
    SetDlgItemText(hWnd, YControl, string);
    string[0] = 'S';
    UchartoHex(CPU::S, string+3, false);
    SetDlgItemText(hWnd, SControl, string);
    string[0] = 'P';
    UchartoHex(CPU::P, string+3, false);
    SetDlgItemText(hWnd, PControl, string);
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
            ShowDisassembly(hWnd, ID_STATIC_INSTRUCTION, CPU::PC);
            return TRUE;
        }
        case WM_CLOSE:
        case WM_DESTROY:
        {
            DestroyWindow(hWnd);
            globals::DebuggerHandle = 0;
            globals::Debugger = false;
            return TRUE;
        }
        case WM_COMMAND:
        {
            switch(wParam & 0xFFFF)
            {
                case IDC_ARROWUP:
                {
                    Debugger::CurrentMemoryAddress -= 16;
                    ShowMemory(hWnd, ID_STATIC_MEMORY,
                               Debugger::CurrentMemoryAddress);
                    ShowRegisters(hWnd, ID_STATIC_A, ID_STATIC_X, ID_STATIC_Y,
                                  ID_STATIC_S, ID_STATIC_P);
                    break;
                }
                case IDC_ARROWDOWN:
                {
                    Debugger::CurrentMemoryAddress += 16;
                    ShowMemory(hWnd, ID_STATIC_MEMORY,
                               Debugger::CurrentMemoryAddress);
                    ShowRegisters(hWnd, ID_STATIC_A, ID_STATIC_X, ID_STATIC_Y,
                                  ID_STATIC_S, ID_STATIC_P);
                    break;
                }
                case IDC_REFRESH:
                {
                    ShowDisassembly(hWnd, ID_STATIC_INSTRUCTION, CPU::PC);
                    char EditText[5];
                    GetDlgItemText(hWnd, IDC_EDIT1, EditText, 5);
                    toUpper(EditText, 4);
                    if(isValidHex(EditText, 4))
                    {
                        Debugger::CurrentMemoryAddress = HextoUshort(EditText);
                    }
                    ShowMemory(hWnd, ID_STATIC_MEMORY,
                               Debugger::CurrentMemoryAddress);
                    ShowRegisters(hWnd, ID_STATIC_A, ID_STATIC_X, ID_STATIC_Y,
                                  ID_STATIC_S, ID_STATIC_P);
                    break;
                }
                case IDC_CYCLE:
                {
                    if(CPU::InstructionCycle == 0)
                    {
                        CPU::PC = CPU::PCTemp;
                        CPU::CurrentInstruction = Memory::Read(CPU::PCTemp);
                        CPU::InstructionCycle++;
                        CPU::CurrentCycle++;
                        ShowDisassembly(hWnd, ID_STATIC_INSTRUCTION, CPU::PC);
                        CPU::PCTemp++;
                    }
                    else
                    {
                        CPU::RunCycle(CPU::CurrentInstruction, CPU::InstructionCycle);
                        CPU::CurrentCycle++;
                    }
                    ShowMemory(hWnd, ID_STATIC_MEMORY,
                               Debugger::CurrentMemoryAddress);
                    ShowRegisters(hWnd, ID_STATIC_A, ID_STATIC_X, ID_STATIC_Y,
                                  ID_STATIC_S, ID_STATIC_P);
                    break;
                }
                case IDC_INSTRUCTION:
                {
                    do
                    {
                        if(CPU::InstructionCycle == 0)
                        {
                            CPU::PC = CPU::PCTemp;
                            CPU::CurrentInstruction = Memory::Read(CPU::PCTemp);
                            CPU::InstructionCycle++;
                            CPU::CurrentCycle++;
                            ShowDisassembly(hWnd, ID_STATIC_INSTRUCTION, CPU::PC);
                            CPU::PCTemp++;
                        }
                        else
                        {
                            CPU::RunCycle(CPU::CurrentInstruction, CPU::InstructionCycle);
                            CPU::CurrentCycle++;
                        }
                    } while(CPU::InstructionCycle != 0);
                    ShowMemory(hWnd, ID_STATIC_MEMORY,
                               Debugger::CurrentMemoryAddress);
                    ShowRegisters(hWnd, ID_STATIC_A, ID_STATIC_X, ID_STATIC_Y,
                                  ID_STATIC_S, ID_STATIC_P);
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
                    globals::Running = false;
                    return 0;
                }
                case ID_REGION_PAL:
                {
                    if(globals::Region == PAL)
                    {
                        return 0;
                    }
                    globals::Region = PAL;
                    CheckMenuItem(GetMenu(hWnd), ID_REGION_NTSC, MF_UNCHECKED);
                    CheckMenuItem(GetMenu(hWnd), ID_REGION_PAL,  MF_CHECKED);
                    return 0;
                }
                case ID_REGION_NTSC:
                {
                    if(globals::Region == NTSC)
                    {
                        return 0;
                    }
                    globals::Region = NTSC;
                    CheckMenuItem(GetMenu(hWnd), ID_REGION_PAL,  MF_UNCHECKED);
                    CheckMenuItem(GetMenu(hWnd), ID_REGION_NTSC, MF_CHECKED);
                    return 0;
                }
                case ID_SPEED_0:
                {
                    globals::Speed = 0.125f;
                    goto IDSpeedALL;
                }
                case ID_SPEED_1:
                {
                    globals::Speed = 0.25f;
                    goto IDSpeedALL;
                }
                case ID_SPEED_2:
                {
                    globals::Speed = 0.5f;
                    goto IDSpeedALL;
                }
                case ID_SPEED_3:
                {
                    globals::Speed = 0.75f;
                    goto IDSpeedALL;
                }
                case ID_SPEED_5:
                {
                    globals::Speed = 1.5f;
                    goto IDSpeedALL;
                }
                case ID_SPEED_1X:
                {
                    globals::Speed = 1.f;
                    goto IDSpeedALL;
                }
                case ID_SPEED_2X:
                {
                    globals::Speed = 2.f;
                    goto IDSpeedALL;
                }
                case ID_SPEED_4X:
                {
                    globals::Speed = 4.f;
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
                    globals::Pause = !globals::Pause;
                    CheckMenuItem(GetMenu(hWnd), ID_EMULATOR_PAUSE,
                                  globals::Pause?MF_CHECKED:MF_UNCHECKED);
                    return 0;
                }
                case ID_EMULATOR_RESET:
                {
                    CPU::Reset();
                    break;
                }
                case ID_TOOLS_DEBUGGER:
                {
                    if(!globals::Debugger)
                    {
                        globals::DebuggerHandle =
                            CreateDialog(GetModuleHandle(NULL),
                                         MAKEINTRESOURCE(IDD_DEBUGGER),
                                         hWnd, DebuggerProc);
                        if(globals::DebuggerHandle)
                        {
                            ShowWindow(globals::DebuggerHandle, SW_SHOW);
                            globals::Debugger = true;
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
        case WM_CLOSE:
        case WM_DESTROY:
        case WM_QUIT:
        {
            globals::Running = false;
            return 0;
            break;
        }
        case WM_PAINT:
        {
            TestRender(globals::Debugger);
            RECT ClientRect;
            GetClientRect(globals::Window, &ClientRect);
            StretchDIBits(globals::MainWindowDC,
                          0, 0,
                          ClientRect.right  - ClientRect.left,
                          ClientRect.bottom - ClientRect.top,
                          0, 0,
                          globals::RenderBuffer.Width, globals::RenderBuffer.Height,
                          globals::RenderBuffer.Memory, &globals::RenderBuffer.Info,
                          DIB_RGB_COLORS, SRCCOPY);
            return DefWindowProc(hWnd, uMsg, wParam, lParam);
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
    //Initialise globals
    globals::Region = PAL;
    globals::Speed = 1;
    globals::Running = true;
    globals::LogFileHandle = INVALID_HANDLE_VALUE;
    globals::InternalMemory = NULL;
    globals::CartridgeMemory = NULL;
    globals::RenderBuffer.Width                        = 1280;
    globals::RenderBuffer.Height                       = 720;
    globals::RenderBuffer.Info.bmiHeader.biSize        =
        sizeof(globals::RenderBuffer.Info.bmiHeader);
    globals::RenderBuffer.Info.bmiHeader.biWidth       = 1280;
    globals::RenderBuffer.Info.bmiHeader.biHeight      = -720;
    globals::RenderBuffer.Info.bmiHeader.biPlanes      = 1;
    globals::RenderBuffer.Info.bmiHeader.biBitCount    = 32;
    globals::RenderBuffer.Info.bmiHeader.biCompression = BI_RGB;
    globals::RenderBuffer.Memory =
        VirtualAlloc(0, 4 * 720 * 1280, MEM_COMMIT, PAGE_READWRITE);
    
    if(!CreateLogFile())
    {
        //Nothing to write the log to, so we just close
        return 1;
    }
    
    globals::InternalMemory =
        (unsigned char *)(VirtualAlloc(0, 2048, MEM_COMMIT, PAGE_READWRITE));
    globals::CartridgeMemory =
        (unsigned char *)(VirtualAlloc(0, 32768, MEM_COMMIT, PAGE_READWRITE));
    if(globals::InternalMemory == NULL || globals::CartridgeMemory == NULL)
    {
        Log("Aborting. Couldn't allocate space for internal memory.");
        return 1;
    }
    for(int i = 0; i < 2048; i++)
    {
        globals::InternalMemory[i] = (unsigned char)0;
    }

    Log("Allocated internal memory.");

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
    globals::Window = CreateWindow("CarmiNESWindowClass",
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
    if(!globals::Window)
    {
        int a = GetLastError();
        Log("Aborting. Couldn't create window.");
        return 1;
    }

    Log("Created window.");
        
    CheckMenuItem(GetMenu(globals::Window), ID_SPEED_1X, MF_CHECKED);
    CheckMenuItem(GetMenu(globals::Window), ID_REGION_PAL, MF_CHECKED);
    unsigned nOfFrames = 0;
    globals::MainWindowDC = GetDC(globals::Window);
    while(globals::Running) //Every iteration of this loop is a frame
    {
        MSG Message;
        while(PeekMessage(&Message, 0, 0, 0, PM_REMOVE))
        {
            if(globals::DebuggerHandle != 0)
            {
                if(!IsDialogMessage(globals::DebuggerHandle, &Message))
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
        if(globals::Pause || !globals::IsAROMLoaded || globals::Debugger)
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
                if(CPU::CurrentCycle ==                                                        /*The frame is over,    */
                   NTSC_CYCLE_COUNT+((PAL_CYCLE_COUNT-NTSC_CYCLE_COUNT)*globals::Region))    /*we pass on to the next*/
                {
                    CPU::CurrentCycle -=
                        (NTSC_CYCLE_COUNT+
                         ((PAL_CYCLE_COUNT-NTSC_CYCLE_COUNT)*globals::Region));
                    break;
                }
                if(CPU::InstructionCycle == 0) //We fetch a new instruction
                {
                    CPU::PC = CPU::PCTemp;
                    CPU::CurrentInstruction = Memory::Read(CPU::PC);
                    CPU::InstructionCycle++;
                    CPU::CurrentCycle++;
                    CPU::PCTemp++;
                    continue;
                }
                CPU::RunCycle(CPU::CurrentInstruction, CPU::InstructionCycle);
                CPU::CurrentCycle++; /*A new cycle starts*/
            }
        }
        TestRender(globals::Debugger?1:0);
        RECT ClientRect;
        GetClientRect(globals::Window, &ClientRect);
        StretchDIBits(globals::MainWindowDC,
                      0, 0,
                      ClientRect.right  - ClientRect.left,
                      ClientRect.bottom - ClientRect.top,
                      0, 0,
                      globals::RenderBuffer.Width, globals::RenderBuffer.Height,
                      globals::RenderBuffer.Memory, &globals::RenderBuffer.Info,
                      DIB_RGB_COLORS, SRCCOPY);
        nOfFrames++;
    }
quit:
    char nOfFramesInString[11];
    UnsignedtoString(nOfFrames, nOfFramesInString);
    Log(nOfFramesInString);
    Log("Quitting.");
    CloseHandle(globals::LogFileHandle);
    return 0;
}
void WinMainCRTStartup()
{
    ExitProcess(WinMain(GetModuleHandle(NULL),
                        NULL,
                        GetCommandLine(),
                        SW_SHOWNORMAL));
}
