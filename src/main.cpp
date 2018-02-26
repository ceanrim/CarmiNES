/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Carmine Foggia $
   ======================================================================== */
#include <windows.h>
#include <SDL.h>
#include <SDL_SysWM.h>
#include "..\res\resource.h"
#include "main.h"
#include "cpu.h"
#include "memory.h"
bool IsAROMLoaded = false;
namespace globals
{
    int Region = PAL;
    float Speed = 1;
    bool Running;
    HANDLE LogFileHandle = INVALID_HANDLE_VALUE;
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

HWND GetHwnd(SDL_Window *Window)
{
    SDL_SysWMinfo WindowInfo;
    SDL_VERSION(&WindowInfo.version);
    SDL_GetWindowWMInfo(Window, &WindowInfo);
    return WindowInfo.info.win.window;
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
    IsAROMLoaded = true;
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
                default:
                {
                    return 0;
                }
            }
        }
        case WM_CLOSE:
        case WM_DESTROY:
        case WM_QUIT:
        {
            globals::Running = false;
            return 0;
            break;
        }
        default:
        {
            return CallWindowProc(DefWindowProc, hWnd, uMsg, wParam, lParam);
        }
    }       
}

void TestRender()
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
            *Pixel = 0x00ff0000;
            Pixel++;
        }
        Row += globals::RenderBuffer.Width;
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
    HWND Window = CreateWindow("CarmiNESWindowClass",
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
    if(!Window)
    {
        int a = GetLastError();
        Log("Aborting. Couldn't create window.");
        return 1;
    }

    Log("Created window.");
        
    CheckMenuItem(GetMenu(Window), ID_SPEED_1X, MF_CHECKED);
    CheckMenuItem(GetMenu(Window), ID_REGION_PAL, MF_CHECKED);
    unsigned nOfFrames = 0;
    HDC DeviceContext = GetDC(Window);
    while(globals::Running) //Every iteration of this loop is a frame
    {
        MSG Message;
        while(PeekMessage(&Message, 0, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&Message);
            DispatchMessage(&Message);
        }
        if(!IsAROMLoaded)
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
                    CPU::CurrentInstruction = Memory::Read(CPU::PC);
                    CPU::InstructionCycle++;
                    CPU::CurrentCycle++;
                    CPU::PC++;
                    continue;
                }
                CPU::RunCycle(CPU::CurrentInstruction, CPU::InstructionCycle);
                CPU::CurrentCycle++; /*A new cycle starts*/
            }
        }
        TestRender();
        RECT ClientRect;
        GetClientRect(Window, &ClientRect);
        StretchDIBits(DeviceContext,
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
