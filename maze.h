#ifndef MAZE_H
#define MAZE_H

#include <stdbool.h>

typedef enum {
    NO_PATH = 0,
    INCOMING_PATH = 1,
    OUTGOING_PATH = 2
} PathType;

typedef struct {
    unsigned char paths;
    bool visited;
    bool onPath;
} Cell;

typedef struct root {
    int x, y;
} ROOT;

// Update function declarations to match implementation
Cell** createMaze(int height, int width);
void setCell(Cell* cell, unsigned char paths, bool visited, bool onPath);
void getDir(Cell cell, unsigned int *east, unsigned int *north, unsigned int *west, unsigned int *south);
int setDir(Cell* cell, unsigned int east, unsigned int north, unsigned int west, unsigned int south);
ROOT RandomizeMaze(Cell** path, int height, int width, int root_x, int root_y, long long count);

#endif