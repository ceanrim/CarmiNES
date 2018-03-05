/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Carmine Foggia $
   ======================================================================== */
#include <windows.h>
#include <SDL.h>
#include <SDL_SysWM.h>
#include "..\bin\resource.h"
#include "main.h"
#include "cpu.h"
#include "memory.h"
namespace globals
{
    int Region = PAL;
    float Speed = 1;
    bool Running;
    HANDLE LogFileHandle = INVALID_HANDLE_VALUE;
    unsigned char *InternalMemory = NULL;
    unsigned char *CartridgeMemory = NULL;
    WNDPROC SDLWindowCallback;
    const char ToHex[] = {'0', '1', '2', '3',
                         '4', '5', '6', '7',
                         '8', '9', 'a', 'b',
                         'c', 'd', 'e', 'f'};
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
void Render(SDL_Surface *Surface)
{
    unsigned *Row = (unsigned *)Surface->pixels;
    for(int y = 0;
        y < Surface->h;
        y++)
    {
        unsigned *Pixel = Row;
        for(int x = 0;
            x < Surface->w;
            x++)
        {
            *Pixel = 0xFF << Surface->format->Rshift;
            Pixel++;
        }
        Row += Surface->pitch / 4;
    }
}

HWND GetHwnd(SDL_Window *Window)
{
    SDL_SysWMinfo WindowInfo;
    SDL_VERSION(&WindowInfo.version);
    SDL_GetWindowWMInfo(Window, &WindowInfo);
    return WindowInfo.info.win.window;
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
                default:
                {
                    return 0;
                }
            }
        }
        default:
        {
            return CallWindowProc(globals::SDLWindowCallback, hWnd, uMsg, wParam, lParam);
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
    
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS))
    {
        Log("Aborting. Couldn't initialize SDL.");
        return 1;
    }

    Log("Initialized SDL.");
    
    SDL_Window  *Window = 0;
    SDL_Surface *Screen = 0;
    Window = SDL_CreateWindow("NESEmu17 by Carmine",
                              SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED,
                              640,
                              480,
                              SDL_WINDOW_RESIZABLE);
    if(!Window)
    {
        Log("Aborting. Couldn't create window.");
        return 1;
    }

    Log("Created window.");

    HMENU Menu = LoadMenu(hInstance, MAKEINTRESOURCE(IDR_MENU1));
    if(Menu)
    {
        Log("Loaded window menu.");
        if(SetMenu(GetHwnd(Window), Menu))
        {
            Log("Set window menu.");
        }
        else
        {
            Log("Aborting. Couldn't set menu.");
            return 1;
        }
    }
    else
    {
        Log("Aborting. Couldn't load menu.");
        return 1;
    }

    MainWindowCallback(GetHwnd(Window), WM_COMMAND, ID_SPEED_1X, 0);
    
    Screen = SDL_GetWindowSurface(Window);
    SDL_FillRect(Screen, 0, SDL_MapRGB(Screen->format, 0xFF, 0xFF, 0xFF));
    SDL_UpdateWindowSurface(Window);
    SDL_Event e;
    globals::SDLWindowCallback = (WNDPROC)(GetWindowLongPtr(GetHwnd(Window), GWLP_WNDPROC));
    SetWindowLongPtr(GetHwnd(Window), GWLP_WNDPROC,
                     (LONG_PTR)(&MainWindowCallback));
    CheckMenuItem(GetMenu(GetHwnd(Window)), ID_REGION_PAL, MF_CHECKED);

    //Initialise CPU
    CPU::CurrentCycle = 0; //CYCLES START AT 0!!!!
    CPU::P = 98;
    CPU::A = 0;
    CPU::X = 0;
    CPU::Y = 3;
    CPU::S = 0xfd;
    CPU::PC = 0; //It should be equal to the bytes at FFFC-FFFD,
                 //but we don't have that yet
    CPU::InstructionCycle = 0;
    CPU::CurrentCycle = 0;
    unsigned nOfFrames = 0;
    globals::InternalMemory[0] = 0xbe;
    globals::InternalMemory[1] = 0xfe;
    globals::InternalMemory[2] = 0x00;
    globals::InternalMemory[0x101] = 0xff;
    while(globals::Running) //Every iteration of this loop is a frame
    {
        while(SDL_PollEvent(&e) != 0)
        {
            if(e.type == SDL_QUIT)
            {
                globals::Running = false;
            }
        }
        //HERE LIETH THE CPU LOOP:
        while(true)
        {
            
            {
                char cycleInString[11];
                UnsignedtoString(CPU::CurrentCycle, cycleInString);
                WriteToLog("Cycle: ");
                WriteToLog(cycleInString);
                WriteToLog("\r\n");
            }
            {
                char PCInString[11];
                UnsignedtoString(CPU::PC, PCInString);
                WriteToLog("PC: ");
                WriteToLog(PCInString);
                WriteToLog("\r\n");
            }
            {
                char toLog1[] =
                    {globals::ToHex[(Memory::AddressBus & 0b1111000000000000)
                                    >> 12], 0};
                char toLog2[] =
                    {globals::ToHex[(Memory::AddressBus & 0b0000111100000000) >> 8],
                     0};
                char toLog3[] =
                    {globals::ToHex[(Memory::AddressBus & 0b11110000) >> 4], 0};
                char toLog4[] = {globals::ToHex[(Memory::AddressBus & 0b00001111)], 0};
                WriteToLog("AddressBus: ");
                WriteToLog(toLog1);
                WriteToLog(toLog2);
                WriteToLog(toLog3);
                WriteToLog(toLog4);
                WriteToLog("\r\n");
            }
            {
                char toLog1[] = {globals::ToHex[(CPU::A & 0b11110000) >> 4], 0};
                char toLog2[] = {globals::ToHex[(CPU::A & 0b00001111)], 0};
                WriteToLog("A: ");
                WriteToLog(toLog1);
                WriteToLog(toLog2);
                WriteToLog("\r\n");
            }
            {
                char toLog1[] = {globals::ToHex[(CPU::X & 0b11110000) >> 4], 0};
                char toLog2[] = {globals::ToHex[(CPU::X & 0b00001111)], 0};
                WriteToLog("X: ");
                WriteToLog(toLog1);
                WriteToLog(toLog2);
                WriteToLog("\r\n");
            }
            if(CPU::P & 128)
            {
                WriteToLog("Negative flag set.\r\n");
            }
            else
            {
                WriteToLog("Negative flag not set.\r\n");
            }
            if(CPU::P & 64)
            {
                WriteToLog("Overflow flag set.\r\n");
            }
            else
            {
                WriteToLog("Overflow flag not set.\r\n");
            }
            if(CPU::P & 8)
            {
                WriteToLog("Decimal flag set.\r\n");
            }
            else
            {
                WriteToLog("Decimal flag not set.\r\n");
            }
            if(CPU::P & 4)
            {
                WriteToLog("Interrupt flag set.\r\n");
            }
            else
            {
                WriteToLog("Interrupt flag not set.\r\n");
            }
            if(CPU::P & 2)
            {
                WriteToLog("Zero flag set.\r\n");
            }
            else
            {
                WriteToLog("Zero flag not set.\r\n");
            }
            if(CPU::P & 1)
            {
                WriteToLog("Carry flag set.\r\n");
            }
            else
            {
                WriteToLog("Carry flag not set.\r\n");
            }
            WriteToLog("\r\n");
            //TODO: Test
            if(CPU::CurrentCycle == 14)
            {
                goto quit;
            }
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
        Screen = SDL_GetWindowSurface(Window);
        Render(Screen);
        SDL_UpdateWindowSurface(Window);
        nOfFrames++;
    }
quit:
    char nOfFramesInString[11];
    UnsignedtoString(nOfFrames, nOfFramesInString);
    Log(nOfFramesInString);
    Log("Quitting.");
    SDL_Quit();
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
