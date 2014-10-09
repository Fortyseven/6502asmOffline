#include "system.h"
#include "machine.h"

#include "version.h"

#define APP_TITLE       "6502asmOffline"
#define APP_VERSION     AutoVersion::FULLVERSION_STRING



/*
    Philip "Pepto" Timmermann's VIC-II palette, <pepto@pepto.de>
    http://www.pepto.de/projects/colorvic/
*/

/*
RGB_Value color_palette[MAX_COLORS] = {
{
        {0x00, 0x00, 0x00},
        {0xFF, 0xFF, 0xFF},
        {0x68, 0x37, 0x2B},
        {0x70, 0xA4, 0xB2},
        {0x6F, 0x3D, 0x86},
        {0x58, 0x8D, 0x43},
        {0x35, 0x28, 0x79},
        {0xB8, 0xC7, 0x6F},
        {0x6F, 0x4F, 0x25},
        {0x43, 0x39, 0x00},
        {0x9A, 0x67, 0x59},
        {0x44, 0x44, 0x44},
        {0x6C, 0x6C, 0x6C},
        {0x9A, 0xD2, 0x84},
        {0x6C, 0x5E, 0xB5},
        {0x95, 0x95, 0x95}
};*/

/* 6502asm.com palette */
RGB_Value color_palette[MAX_COLORS] = {
    {0x00, 0x00, 0x00},
    {0xff, 0xff, 0xff},
    {0x88, 0x00, 0x00},
    {0xaa, 0xff, 0xee},
    {0xcc, 0x44, 0xcc},
    {0x00, 0xcc, 0x55},
    {0x00, 0x00, 0xaa},
    {0xee, 0xee, 0x77},
    {0xdd, 0x88, 0x55},
    {0x66, 0x44, 0x00},
    {0xff, 0x77, 0x77},
    {0x33, 0x33, 0x33},
    {0x77, 0x77, 0x77},
    {0xaa, 0xff, 0x66},
    {0x00, 0x88, 0xff},
    {0xbb, 0xbb, 0xbb}
};

/*----------------------------------*/
System::System()
{
    screen = NULL;

    this->machine_buffer_ptr = NULL;
    this->machine_width = 0;
    this->machine_height = 0;
    this->screen_scale = ZOOM_FACTOR;
}

/*----------------------------------*/
void System::init( int argc, char **argv )
{
    this->showBanner();
    this->cmdline = new CCmdLine;
    this->cmdline->splitLine( argc, argv );

    if (argc == 1 ||
            !this->cmdline->hasSwitch("-file") ||
            this->cmdline->hasSwitch("-h") ||
            this->cmdline->hasSwitch("-help")) {
        this->showHelp();
        exit(0);
    }

    // initialize SDL video

    if ( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
        printf( "Unable to init SDL: %s\n", SDL_GetError() );
        exit( 1 );
    }

    // make sure SDL cleans up before exit
    atexit( SDL_Quit );

    if (this->cmdline->hasSwitch("-scale")) {
        this->screen_scale = atoi( this->cmdline->getArgument("-scale", 0).c_str() );
    }

    // create a new window
    screen = SDL_SetVideoMode( MEM_SCREEN_WIDTH * this->screen_scale,
                               MEM_SCREEN_HEIGHT * this->screen_scale,
                               32, SDL_HWSURFACE | SDL_DOUBLEBUF );

    if ( !screen ) {
        printf( "Unable to set video mode: %s\n", SDL_GetError() );
        exit( 1 );
    }

    SDL_EnableUNICODE(1);

    this->setWindowTitle("6502asmOffline");
}

/*----------------------------------*/
void System::setWindowTitle(string title)
{
    SDL_WM_SetCaption(title.c_str(), title.c_str());
}

/*----------------------------------*/
int System::done()
{
    return 0;
}

/*----------------------------------*/
void System::setMachineBuffer( char *buffer, int width, int height )
{
    this->machine_buffer_ptr    = buffer;
    this->machine_width         = width;
    this->machine_height        = height;

    if ( this->machine_screen ) {
        freeMachineBuffer();
    }

    this->machine_screen = SDL_CreateRGBSurface( SDL_SWSURFACE, width, height, 32, 0, 0, 0, 0 );
}

/*----------------------------------*/
void System::freeMachineBuffer()
{
    if ( this->machine_screen ) {
        SDL_FreeSurface( this->machine_screen );
    }
}

/*----------------------------------*/
void System::draw()
{
    // render machine display buffer
    for ( int y = 0; y < machine_height; y++ ) {
        for ( int x = 0; x < machine_width; x++ ) {
            unsigned int color = machine_buffer_ptr[( y * machine_width ) + x] & 0xF;
            pixelRGBA(
                machine_screen,
                x, y,

                color_palette[color].r,
                color_palette[color].g,
                color_palette[color].b,
                255
            );
        }
    }

    SDL_FillRect( screen, 0, SDL_MapRGB( screen->format, 0, 0, 0 ) );

    SDL_Surface *zoom = zoomSurface( machine_screen, this->screen_scale , this->screen_scale, 0 );
    SDL_BlitSurface( zoom, NULL, screen, NULL );
    SDL_FreeSurface( zoom );

    SDL_Flip( screen );

}

/*----------------------------------*/
int System::update()
{
    // message processing loop
    SDL_Event event;

    while ( SDL_PollEvent( &event ) ) {
        // check for messages
        switch ( event.type ) {
            // exit if the window is closed
            case SDL_QUIT:
                return 0;
                break;

            // check for keypresses
            case SDL_KEYDOWN:
                machine.curkey = event.key.keysym.unicode;

                if ( event.key.keysym.sym == SDLK_ESCAPE ) {
                    return 0;
                }
                break;
            case SDL_KEYUP:
                machine.curkey = 0;
                break;
        } // end switch
    } // end of message processing
    return 1;
}

/*----------------------------------*/
void System::showHelp()
{
    puts("usage: 6502asmOffline.exe -file filename.bin [-scale 16] [-cycles 100]"); // [-pal 0]");
    puts("");
    puts("-file   -- Loads the binary data inside the file specified at $600.");
    puts("-scale  -- Window size  = 32 * this number.  Using 1 would create");
    puts("           an adorable 32x32 pixel window.  [default = 16 (512x512)]");
    puts("-cycles -- Number of processor cycles executed per update. [default = 100]");
    //puts("-pal    -- 0 = Original 6502asm.com palette. [default]");
    //puts("           1 = Enhanced 'Pepto' VIC-II palette.");
}

void System::showBanner()
{
    puts(APP_TITLE);
    printf("Version %s\n\n", APP_VERSION);

}
