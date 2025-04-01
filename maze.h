#ifndef MAZE_H      // Check if not defined
#define MAZE_H      // Define it

//#include <raylib.h>

//Using bit manipulation to save memory
    //unsigned char directions = 0b00000000;
    //most significant 2 bits is east
    //then north
    //then west
    //then south
    //EE NN WW SS

const unsigned int NO_PATH = 0;
const unsigned int OUTGOING_PATH = 2;
const unsigned int INCOMING_PATH = 1;

int getDir(unsigned char directions, unsigned int *east,unsigned int *north,unsigned int *west,unsigned int *south);
int setDir(unsigned char *directions,unsigned int east,unsigned int north,unsigned int west,unsigned int south);



#endif              //End