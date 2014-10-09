#ifndef __SYSTEM_H__
#define __SYSTEM_H__


#include "CmdLine/CmdLine.h"
#include "system.h"

#ifdef __APPLE__
#include <SDL/SDL.h>
#include <SDL/SDL_gfxPrimitives.h>
#include <SDL/SDL_rotozoom.h>
#else
#include <SDL.h>
#include <SDL/SDL_gfxPrimitives.h>
#include <SDL/SDL_rotozoom.h>
#endif

#define MAX_COLORS  16
#define ZOOM_FACTOR 16

typedef struct {
    int r, g, b;
} RGB_Value;

extern RGB_Value color_palette[];

/*==========================================================================*/

class System
{

    private:
        char *machine_buffer_ptr;
        int   machine_width,
              machine_height;

        SDL_Surface *machine_screen;
        int screen_scale;

    public:
        SDL_Surface *screen, *display;
        CCmdLine *cmdline;
        /*----------------------------------*/
        System();
        void init( int argc, char **argv );
        int done();
        void setWindowTitle( string title );
        void showHelp();
        void showBanner();

        void setMachineBuffer( char *buffer, int width, int height );
        void freeMachineBuffer();

        int update();
        void draw();
};

/*==========================================================================*/

extern System sys;

#endif
