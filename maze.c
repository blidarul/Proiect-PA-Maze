#include <time.h>
#include <stdlib.h>

void getDir(unsigned char directions, unsigned int *east,unsigned int *north,unsigned int *west,unsigned int *south)
{
    // EE NN WW SS >> 6 = 00 00 00 EE(shifts the whole number 6 bits to the right)
    // 00 00 00 EE & 00 00 00 11 = EE(uses a mask to only look at the last 2 bits)
    *east = (directions >> 6) & 0b00000011;
    *north = (directions >> 4) & 0b00000011;
    *west = (directions >> 2) & 0b00000011;
    *south = directions & 0b00000011;
}

int setDir(unsigned char *directions,unsigned int east,unsigned int north,unsigned int west,unsigned int south)
{
    if((east > 2 || north > 2) || (west > 2 || south > 2))
        return -1;
    else
    {
        //east = EE     north = NN      west = WW       south = SS
        //east << 6     = EE 00 00 00
        //north << 4    = 00 NN 00 00
        //west << 2     = 00 00 WW 00
        //south         = 00 00 00 SS

        //(east << 6) | (north << 4) = EE NN 00 00
        // etc...
        *directions = (east << 6) | (north << 4) | (west << 2) | south;
    }
    return 0;
}

void RandomizeMaze(unsigned char **path,int height,int width,int root_x,int root_y,clock_t run_time)
{
    clock_t time = clock();
    while(1)
    {
        clock_t aux = clock();
        if(aux - time > run_time)
            break;
        unsigned int east,north,west,south;
        getDir(path[root_x][root_y],&east,&north,&west,&south);
        int choices[4] = {1,1,1,1};
        if(root_x == height - 1)
            choices[3] = 0;
        if(root_x == 0)
            choices[1] = 0;
        if(root_y == 0)
            choices[2] = 0;
        if(root_y == width - 1)
            choices[0] = 0;
        int random = rand() % 3;
        while(choices[random] == 0)
        {
            random = rand() % 3;
        }
        switch (random)
        {
        case 0:
            setDir(&path[root_x][root_y],2,north,west,south);
            root_y ++;
            break;
        case 1:
            setDir(&path[root_x][root_y],east,2,west,south);
            root_x --;
            break;
        case 2:
            setDir(&path[root_x][root_y],east,north,2,south);
            root_y --;
            break;
        case 3:
            setDir(&path[root_x][root_y],east,north,west,2);
            root_x ++;
            break;
        default:
            break;
        }
        getDir(path[root_x][root_y],&east,&north,&west,&south);
        if(east == 2)
            setDir(&path[root_x][root_y],0,north,west,south);
        else
        if(north == 2)
            setDir(&path[root_x][root_y],east,0,west,south);
        else
        if(west == 2)
            setDir(&path[root_x][root_y],east,north,0,south);
        else
        if(south == 2)
            setDir(&path[root_x][root_y],east,north,west,0);
    }

    return;
}