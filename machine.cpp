#include "6502\M6502.h"
#include "system.h"
#include "machine.h"

#include <sstream>

extern System sys;

/*-----------------------*/
Machine::Machine()
{
    this->screen_buffer = NULL;
    this->ram = NULL;
    this->cycles = CPU_CYCLES;
    this->running = false;
}

/*-----------------------*/
int Machine::init()
{
    this->ram = ( uint8_t *) malloc( MEM_SIZE );
    this->screen_buffer = (char *) (ram + MEM_SCREEN_START);
    sys.setMachineBuffer( this->screen_buffer, MEM_SCREEN_WIDTH, MEM_SCREEN_HEIGHT );

    char *fname = strdup( sys.cmdline->getArgument("-file", 0).c_str() );

    printf("* Loading \"%s\"...", fname);

    if (machine.load( fname )) {
        printf("ERROR.\n");
        exit(-1);
    } else {
        printf("OK!\n");
    }
    free(fname);

    this->cpu.PC.W = MEM_PROGRAM_START;

    //this->log = fopen("log.txt", "w");

    if ( sys.cmdline->hasSwitch("-cycles")) {
        this->cycles = atoi( sys.cmdline->getArgument("-cycles", 0).c_str() );
    }

    printf("* Cycles = %d\n", this->cycles);

    return 0;
}

/*-----------------------*/
int Machine::done()
{
    if (this->log) {
        fclose( this->log );
    }

    return 0;
}

/*-----------------------*/
int Machine::load( char *binary, int length )
{
    this->cpu.PC.W = MEM_PROGRAM_START;
    memcpy( this->ram + MEM_PROGRAM_START, binary, length );
    this->start();
    return 0;
}

/*-----------------------*/
int Machine::load( char *filename )
{
    FILE *F = fopen(filename, "rb");
    if (F) {
        fseek(F, 0, SEEK_END);
        int length = ftell(F);
        //printf("Opening %s (%d bytes, using %x, *$%x)...\n", filename, length, F, this->ram);
        fseek(F, 0, SEEK_SET);
        fread(this->ram + MEM_PROGRAM_START, length, 1, F);
        fclose(F);
        this->cpu.PC.W = MEM_PROGRAM_START;
        this->start();
        return 0;
    } else {
        return 1;
    }
}
/*-----------------------*/
void Machine::start()
{
    this->running = true;
}
/*-----------------------*/
void Machine::stop()
{
    this->running = false;
}
/*-----------------------*/
void Machine::update()
{
    if (this->running) {
        Exec6502( &( this->cpu ), this->cycles );
    }
}

/*----------------------------------------------------------------*/
void Wr6502( register word Addr, register byte Value )
{
    machine.ram[ Addr ] = Value;
}

/*----------------------------------------------------------------*/
byte Rd6502( word Addr )
{
    switch ( Addr ) {
        case MEM_RANDOM_GENERATOR:
            return rand() % 256;
            break;
        case MEM_KEYBOARD_ASCII:
            return machine.curkey;
            break;
    }

    // printf("$%0.4x = $%0.2x\n", Addr, (uint8_t) machine.ram[ Addr ]);


    return (uint8_t) machine.ram[ Addr ];
}

/*----------------------------------------------------------------*/
byte Op6502( word Addr )
{
    return machine.ram[ Addr ];
}

/*----------------------------------------------------------------*/
byte Loop6502( register M6502 *R )
{
    return INT_NONE;
}

/*----------------------------------------------------------------*/
byte Patch6502( register byte Op, register M6502 *R )
{
    /*
    printf("*** Program stopped at $%x\n", R->PC.W);
    machine.stop();
    */
    return 0;
}


