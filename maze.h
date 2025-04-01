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

#define NO_PATH  (unsigned int)0
#define OUTGOING_PATH  (unsigned int)2
#define INCOMING_PATH  (unsigned int)1
    
typedef struct root
{
    int x,y;
}ROOT;

int getDir(unsigned char directions, unsigned int *east,unsigned int *north,unsigned int *west,unsigned int *south);
int setDir(unsigned char *directions,unsigned int east,unsigned int north,unsigned int west,unsigned int south);
ROOT RandomizeMaze(unsigned char **path,int height,int width,int root_x,int root_y,long long count);


#endif              //End