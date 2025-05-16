#ifndef MAZE_H
#define MAZE_H


#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "raylib.h"

typedef enum
{
    NO_PATH = 0,
    INCOMING_PATH = 1,
    OUTGOING_PATH = 2
} PathType;

typedef enum {
    EAST = 0,
    NORTH = 1,
    WEST = 2,
    SOUTH = 3,
} Direction;

typedef struct
{
    unsigned char paths;
    bool visited;
    bool onPath;
} Cell;

typedef struct root
{
    int x, y;
} Root;

typedef struct
{
    Cell **path;
    unsigned int cellsVisited;
    Root root;
} Maze;

Maze* createMaze(int height, int width);
void getDir(Cell cell, unsigned int *east, unsigned int *north, unsigned int *west, unsigned int *south);
int setDir(Cell* cell, unsigned int east, unsigned int north, unsigned int west, unsigned int south);
void RandomizeMaze(Maze *maze, int height, int width, long long count);
void InitializeMaze(Maze *maze, int mazeHeight, int mazeWidth);
Image ConvertMazeToCubicMap(Maze *maze, int height, int width);
void VisitCell(Maze *maze, int playerCellX, int playerCellY, Image cubicmap, Image *minimap);

#endif
