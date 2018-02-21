/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Carmine Foggia $
   ======================================================================== */
#include <gtk/gtk.h>
#include "main.h"
#include "cpu.h"
#include "memory.h"
namespace globals
{
    int Region = PAL;
    float Speed = 1;
    bool Running;
    // HANDLE LogFileHandle = INVALID_HANDLE_VALUE;
    unsigned char *InternalMemory = NULL;
    unsigned char *CartridgeMemory = NULL;
    // WNDPROC SDLWindowCallback;
    const char ToHex[] = {'0', '1', '2', '3',
                         '4', '5', '6', '7',
                         '8', '9', 'a', 'b',
                         'c', 'd', 'e', 'f'};
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
/*void Render(SDL_Surface *Surface)
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
    }*/

void activate(GtkApplication *App, gpointer UserData)
{
    GtkBuilder *Builder;
    GObject *Window;
    Builder = gtk_builder_new();
    gtk_builder_add_from_file(Builder, "./res/builder.ui", NULL);
    Window = gtk_builder_get_object(Builder, "window1");
    gtk_application_add_window(App, GTK_WINDOW(Window));
    g_signal_connect_swapped
        (Window, "destroy", G_CALLBACK(g_application_quit), G_APPLICATION(App));
    gtk_widget_show_all(GTK_WIDGET(Window));
}

int main
(int argc,
 char **argv)
{
    //Initialise globals
    globals::Region = PAL;
    globals::Speed = 1;
    globals::Running = false;
    globals::InternalMemory = NULL;
    globals::CartridgeMemory = NULL;

    
    /* if(!CreateLogFile())
    {
        //Nothing to write the log to, so we just close
        return 1;
        }*/
    
    globals::InternalMemory = new unsigned char [2048];
    globals::CartridgeMemory = new unsigned char[32768];
    if(globals::InternalMemory == NULL || globals::CartridgeMemory == NULL)
    {
        // Log("Aborting. Couldn't allocate space for internal memory.");
        return 1;
    }
    for(int i = 0; i < 2048; i++)
    {
        globals::InternalMemory[i] = (unsigned char)0;
    }

    // Log("Allocated internal memory.");
    GtkApplication *App;
    App = gtk_application_new("car.min.efoggia", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(App, "activate", G_CALLBACK(activate), NULL);
    g_application_run(G_APPLICATION(App), argc, argv);
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
    while(globals::Running) //Every iteration of this loop is a frame
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
        nOfFrames++;
    }
quit:
    char nOfFramesInString[11];
    UnsignedtoString(nOfFrames, nOfFramesInString);
    return 0;
}

