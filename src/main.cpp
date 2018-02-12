/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Carmine Foggia $
   ======================================================================== */

#include <windows.h>
#include <sdl.h>
#include "..\lib\include.h"
void TestRender() //We just make the screen red for test purposes.
{
    for(int i = 0; i < 1366 * 768; i++)
    {
        RenderBuffer[i] = 0x00FF0000;
    }
}
bool Running = true; // if this is false the program closes
HANDLE LogFileHandle = INVALID_HANDLE_VALUE; //The handle to the log file
unsigned char *InternalMemory = NULL; //Memory addresses between 0x0000 and 0x07FF
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
bool Log(char *Message)
//string has to be null-terminated, prints "HH:MM:SS: Message\n"
{
    char Time[9];
    TimeString(Time);
    DWORD TimeNumberOfBytesWritten = 0;
    WriteFile(LogFileHandle,
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
    WriteFile(LogFileHandle,
              &colon,
              2,
              &ColonNumberOfBytesWritten,
              NULL);
    if(ColonNumberOfBytesWritten != 2)
    {
        return false;
    }
    DWORD MessageNumberOfBytesWritten = 0;
    WriteFile(LogFileHandle,
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
    WriteFile(LogFileHandle,
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
    LogFileHandle = CreateFile(LogName,
                               GENERIC_READ | GENERIC_WRITE,
                               FILE_SHARE_READ | FILE_SHARE_WRITE,
                               NULL,
                               CREATE_NEW,
                               FILE_ATTRIBUTE_NORMAL,
                               NULL);
    if(LogFileHandle == INVALID_HANDLE_VALUE)
    {
        return false;
    }
    else
    {
        return true;
    }
}

int CALLBACK WinMain
(HINSTANCE hInstance,
 HINSTANCE hPrevInstance,
 LPSTR     lpCmdLine,
 int       nCmdShow)
{
    if(!CreateLogFile())
    {
        //Nothing to write the log to, so we just close
        return 1;
    }
    
    InternalMemory =
        (unsigned char *)(VirtualAlloc(0, 2048, MEM_COMMIT, PAGE_READWRITE));
    if(InternalMemory == NULL)
    {
        Log("Aborting. Couldn't allocate space for internal memory.");
        return 1;
    }
    else
    {
        Log("Internal Memory allocated.");
    }
    CloseHandle(LogFileHandle);
    return 0;
}
void WinMainCRTStartup()
{
    WinMain(GetModuleHandle(NULL),
            NULL,
            GetCommandLine(),
            SW_SHOWNORMAL);
}
