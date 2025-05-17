#include "maze.h"

Maze* createMaze(int height, int width)
{
    Maze *maze = (Maze *)malloc(sizeof(Maze));
    maze->path = (Cell **)malloc(height * sizeof(Cell *));
    for (int i = 0; i < height; i++)
    {
        maze->path[i] = (Cell *)malloc(width * sizeof(Cell));
        for (int j = 0; j < width; j++)
        {
            maze->path[i][j].paths = 0;
            maze->path[i][j].visited = false;
            maze->path[i][j].onPath = false;
        }
    }
    return maze;
}

void getDir(Cell cell, unsigned int *east, unsigned int *north, unsigned int *west, unsigned int *south)
{
    *east = (cell.paths >> 6) & 0b00000011;
    *north = (cell.paths >> 4) & 0b00000011;
    *west = (cell.paths >> 2) & 0b00000011;
    *south = cell.paths & 0b00000011;
}

int setDir(Cell *cell, unsigned int east, unsigned int north, unsigned int west, unsigned int south)
{
    if ((east > 2 || north > 2) || (west > 2 || south > 2))
        return -1;
    
    cell->paths = (east << 6) | (north << 4) | (west << 2) | south;
    
    return 0;
}

static void RemoveOutgoingPath(Maze *maze, int x, int y, int direction)
{
    unsigned int east, north, west, south;
    getDir(maze->path[x][y], &east, &north, &west, &south);

    switch (direction)
    {
    case EAST: // East
        setDir(&maze->path[x][y], NO_PATH, north, west, south);
        getDir(maze->path[x][y + 1], &east, &north, &west, &south);
        setDir(&maze->path[x][y + 1], east, north, NO_PATH, south);
        break;
    case NORTH: // North
        setDir(&maze->path[x][y], east, NO_PATH, west, south);
        getDir(maze->path[x - 1][y], &east, &north, &west, &south);
        setDir(&maze->path[x - 1][y], east, north, west, NO_PATH);
        break;
    case WEST: // West
        setDir(&maze->path[x][y], east, north, NO_PATH, south);
        getDir(maze->path[x][y - 1], &east, &north, &west, &south);
        setDir(&maze->path[x][y - 1], NO_PATH, north, west, south);
        break;
    case SOUTH: // South
        setDir(&maze->path[x][y], east, north, west, NO_PATH);
        getDir(maze->path[x + 1][y], &east, &north, &west, &south);
        setDir(&maze->path[x + 1][y], east, NO_PATH, west, south);
        break;
    }
}

void InitializeMaze(Maze *maze, int mazeHeight, int mazeWidth)
{
    maze->cellsVisited = 0;
    for (int i = 0; i < mazeHeight; i++)
    {
        for (int j = 0; j < mazeWidth; j++)
        {
            maze->path[i][j].visited = false;
            maze->path[i][j].onPath = false;

            if (j == 0)
            {
                setDir(&maze->path[i][j], OUTGOING_PATH, NO_PATH, NO_PATH, NO_PATH);
            }
            else if (j < mazeWidth - 1)
            {
                setDir(&maze->path[i][j], OUTGOING_PATH, NO_PATH, INCOMING_PATH, NO_PATH);
            }
            else if (i == 0)
            {
                setDir(&maze->path[i][j], NO_PATH, NO_PATH, INCOMING_PATH, OUTGOING_PATH);
            }
            else if (i < mazeHeight - 1)
            {
                setDir(&maze->path[i][j], NO_PATH, INCOMING_PATH, INCOMING_PATH, OUTGOING_PATH);
            }
            else
            {
                setDir(&maze->path[i][j], NO_PATH, INCOMING_PATH, INCOMING_PATH, NO_PATH);
            }
        }
    }
}

// Randomize maze
void RandomizeMaze(Maze *maze, int height, int width, long long count)
{
    long long i = 0;
    while (i < count)
    {
        unsigned int east, north, west, south;
        getDir(maze->path[maze->root.x][maze->root.y], &east, &north, &west, &south);
        // stores the possible directions to go
        int choices[4] = {1, 1, 1, 1};
        if (maze->root.x == height - 1)
            choices[3] = 0;
        if (maze->root.x == 0)
            choices[1] = 0;
        if (maze->root.y == 0)
            choices[2] = 0;
        if (maze->root.y == width - 1)
            choices[0] = 0;
        // picks a valid direction
        int random = rand() % 4;
        while (choices[random] == 0)
        {
            random = rand() % 4;
        }

        switch (random)
        {
        case EAST: // picked east
            // make an outgoing path east
            setDir(&maze->path[maze->root.x][maze->root.y], OUTGOING_PATH, north, west, south);
            // change the new root to be east of the old one
            maze->root.y++;
            // replace west path from the new root with an incoming path
            getDir(maze->path[maze->root.x][maze->root.y], &east, &north, &west, &south);
            setDir(&maze->path[maze->root.x][maze->root.y], east, north, INCOMING_PATH, south);
            break;
        case NORTH: // picked north
            setDir(&maze->path[maze->root.x][maze->root.y], east, OUTGOING_PATH, west, south);
            // change the new root to be north of the old one
            maze->root.x--;
            // replace south path from the new root with an incoming path
            getDir(maze->path[maze->root.x][maze->root.y], &east, &north, &west, &south);
            setDir(&maze->path[maze->root.x][maze->root.y], east, north, west, INCOMING_PATH);
            break;
        case WEST: // picked west
            setDir(&maze->path[maze->root.x][maze->root.y], east, north, OUTGOING_PATH, south);
            // change the new root to be west of the old one
            maze->root.y--;
            // replace east path from the new root with an incoming path
            getDir(maze->path[maze->root.x][maze->root.y], &east, &north, &west, &south);
            setDir(&maze->path[maze->root.x][maze->root.y], INCOMING_PATH, north, west, south);
            break;
        case SOUTH: // picked south
            setDir(&maze->path[maze->root.x][maze->root.y], east, north, west, OUTGOING_PATH);
            // change the new root to be south of the old one
            maze->root.x++;
            // replace north path from the new root with an incoming path
            getDir(maze->path[maze->root.x][maze->root.y], &east, &north, &west, &south);
            setDir(&maze->path[maze->root.x][maze->root.y], east, INCOMING_PATH, west, south);
            break;
        default:
            break;
        }

        getDir(maze->path[maze->root.x][maze->root.y], &east, &north, &west, &south);
        if (east == OUTGOING_PATH)
        {
            RemoveOutgoingPath(maze, maze->root.x, maze->root.y, EAST);
        }
        else if (north == OUTGOING_PATH)
        {
            RemoveOutgoingPath(maze, maze->root.x, maze->root.y, NORTH);
        }
        else if (west == OUTGOING_PATH)
        {
            RemoveOutgoingPath(maze, maze->root.x, maze->root.y, WEST);
        }
        else if (south == OUTGOING_PATH)
        {
            RemoveOutgoingPath(maze, maze->root.x, maze->root.y, SOUTH);
        }
        i++;
    }
}

Image ConvertMazeToCubicMap(Maze *maze, int height, int width)
{
    // Create an image with dimensions 2*width+1 x 2*height+1
    // This gives us 1 pixel per cell and 1 pixel per wall
    const int imageWidth = width * 2 + 1;
    const int imageHeight = height * 2 + 1;
    
    // Create an image filled with white (walls everywhere)
    Image cubicmap = GenImageColor(imageWidth, imageHeight, WHITE);
    
    // Process each cell in the maze
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            unsigned int east, north, west, south;
            getDir(maze->path[i][j], &east, &north, &west, &south);
            
            // Calculate the cell's pixel position (cells are at even coordinates)
            int cellX = j * 2 + 1;
            int cellY = i * 2 + 1;
            
            // Make the cell itself black in the cubic map
            ImageDrawPixel(&cubicmap, cellX, cellY, BLACK);

            if (north == 1)
            {
                ImageDrawPixel(&cubicmap, cellX, cellY - 1, BLACK);
            }

            if (west == 1)
            {
                ImageDrawPixel(&cubicmap, cellX - 1, cellY, BLACK);
            }

            if(east == 1)
            {
                ImageDrawPixel(&cubicmap, cellX + 1, cellY, BLACK);
            }

            if(south == 1)
            {
                ImageDrawPixel(&cubicmap, cellX, cellY + 1, BLACK);
            }
        }
    }
    
    return cubicmap;
}

void RevealMinimap(Maze *maze, int playerCellX, int playerCellY, Image cubicmap, Image *minimap)
{
    // Use grid coordinates directly for image operations
    int cellX = playerCellX;
    int cellY = playerCellY;
    
    for(int i = -1; i <= 1; i++)
    for(int j = -1; j <= 1; j++)
    {
        int checkX = cellX + i;
        int checkY = cellY + j;
        
        // Check bounds before accessing
        if(checkX >= 0 && checkX < cubicmap.width && 
           checkY >= 0 && checkY < cubicmap.height)
        {
            if(GetImageColor(cubicmap, checkX, checkY).r == 0)
            {
                ImageDrawPixel(minimap, checkX, checkY, WHITE);
            }
            else
            {
                ImageDrawPixel(minimap, checkX, checkY, BLACK);
            }
        }
    }  
}
