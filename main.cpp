#include "6502asmOffline.h"
#include "system.h"
#include "machine.h"

System sys;
Machine machine;


/* This hack is here so SDL doesn't steal stdout/stderr from us. */

#ifdef WIN32
#ifdef main
#undef main
#endif
#endif

//#include "test.h"


int main( int argc, char **argv )
{
    sys.init(argc, argv);
    machine.init();

    //machine.load( bin_compo1, sizeof( bin_compo1 ) );
    //machine.load("test/rule30.bin");

    while ( sys.update() ) {
        machine.update();
        sys.draw();
    }

    machine.done();

    return sys.done();
}

