/* ========================================================================
    File: 
    Date: 
    Revision:
    Creator: Carmine Foggia 
   ======================================================================== */
#ifndef PPUDIALOG_CPP
#define PPUDIALOG_CPP
#include "ppudialog.h"
#include "nesclass.h"
#include "main.h"

/*PPUDialog::PPUDialog(unsigned short resourceID, size_t Width, size_t Height)
{
    ResourceID = resourceID;
    RenderingMemorySize = Width * Height;
    isOpen = false;
    RenderingMemoryInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    RenderingMemoryInfo.bmiHeader.biWidth = Width;
    RenderingMemoryInfo.bmiHeader.biHeight = -Height;
    RenderingMemoryInfo.bmiHeader.biPlanes = 1;
    RenderingMemoryInfo.bmiHeader.biBitCount = 32;
    RenderingMemoryInfo.bmiHeader.biCompression = BI_RGB;
}

CHRROMViewerClass::CHRROMViewerClass(unsigned short resourceID,
                          size_t Width, size_t Height):
        PPUDialog(resourceID, Width, Height)
{
}

PaletteViewerClass::PaletteViewerClass(unsigned short resourceID,
                            size_t Width, size_t Height):
        PPUDialog(resourceID, Width, Height)
{
}

NametableViewerClass::NametableViewerClass(unsigned short resourceID,
                                size_t Width, size_t Height):
        PPUDialog(resourceID, Width, Height)
{
}

LRESULT CALLBACK PPUDialogProc(HWND hWnd,
                               UINT uMsg,
                               WPARAM wParam,
                               LPARAM lParam)
{
    if(hWnd == NES.Debugger.CHRROMViewer.WindowHandle)
    {
        return NES.Debugger.CHRROMViewer.WindowProc(hWnd, uMsg, wParam, lParam);
    }
    else if(hWnd == NES.Debugger.PaletteViewer.WindowHandle)
    {
        return NES.Debugger.PaletteViewer.WindowProc(hWnd, uMsg, wParam, lParam);
    }
    else if(hWnd == NES.Debugger.NametableViewer.WindowHandle)
    {
        return NES.Debugger.NametableViewer.WindowProc(hWnd, uMsg, wParam, lParam);
    }
}

void PPUDialog::Open(HWND hWnd)
{
    if(!isOpen)
    {
        WindowHandle = CreateDialog(GetModuleHandle(NULL),
                                    MAKEINTRESOURCE(IDD_PALETTES),
                                    hWnd, PPUDialogProc);
        if(WindowHandle)
        {
            ShowWindow(WindowHandle, SW_SHOW);
            isOpen = true;
        }
    }
}

LRESULT PPUDialog::WindowProc(HWND hWnd,
                             UINT uMsg,
                             WPARAM wParam,
                             LPARAM lParam)
{
    switch(uMsg)
    {
        case WM_INITDIALOG:
        {
            RenderingMemory = (unsigned *)
                VirtualAlloc(0, 4 * RenderingMemorySize,
                             MEM_COMMIT, PAGE_READWRITE);
            Refresh();
            return TRUE;
        }
        case WM_PAINT:
        {
            Refresh();
            HDC DeviceContext = GetDC(WindowHandle);
            RECT ClientRect;
            GetClientRect(WindowHandle, &ClientRect);
            StretchDIBits(DeviceContext,
                          0, 0,
                          ClientRect.right - ClientRect.left,
                          ClientRect.bottom - ClientRect.top,
                          0, 0, RenderingMemoryInfo.bmiHeader.biWidth,
                          -RenderingMemoryInfo.bmiHeader.biHeight,
                          RenderingMemory,
                          &RenderingMemoryInfo,
                          DIB_RGB_COLORS, SRCCOPY);
            ReleaseDC(WindowHandle, DeviceContext);
            return TRUE;
        }
        case WM_CLOSE:
        case WM_DESTROY:
        {
            if(RenderingMemory)
            {
                VirtualFree(RenderingMemory, 4 * RenderingMemorySize, MEM_RELEASE);
                RenderingMemory = 0;
            }
            DestroyWindow(WindowHandle);
            WindowHandle = 0;
            isOpen = false;
            return TRUE;
        }
        default:
        {
            return FALSE;
        }
    }
}

void PPUDialog::Refresh()
{
}

void CHRROMViewerClass::Refresh()
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
                            RenderingMemory
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

void PaletteViewerClass::Refresh()
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
                            RenderingMemory
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
                            RenderingMemory
                                [i * 16 * 64 + j * 64 + k * 16 + l] = Color;
                        }
                    }
                }
            }
        }
    }
}

void NametableViewerClass::Refresh()
{
}*/
#endif
