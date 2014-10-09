#ifndef __MACHINE_H__
#define __MACHINE_H__

#include "6502/M6502.h"

#define CPU_CYCLES              100

#define MEM_PROGRAM_START       0x600

#define MEM_RANDOM_GENERATOR    0xFE
#define MEM_KEYBOARD_ASCII      0xFF
#define MEM_SCREEN_START        0x200

#define MEM_SCREEN_WIDTH        32
#define MEM_SCREEN_HEIGHT       32

#define MEM_SCREEN_SIZE         ( MEM_SCREEN_WIDTH * MEM_SCREEN_HEIGHT )

#define MEM_SIZE                65536

extern M6502 cpu;

class Machine
{
    private:
        char *screen_buffer;
    public:
        FILE *log;
        M6502 cpu;
        uint8_t  *ram;

        bool running;

        int cycles;
        int curkey;

        Machine();
        int init();
        int done();
        void update();

        void start();
        void stop();

        int load(char *binary, int length);
        int load(char *filename);
};


extern Machine machine;

#endif
