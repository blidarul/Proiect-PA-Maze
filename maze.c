#include "maze.h"

Cell **createMaze(int height, int width)
{
    Cell **maze = (Cell **)malloc(height * sizeof(Cell *));
    for (int i = 0; i < height; i++)
    {
        maze[i] = (Cell *)malloc(width * sizeof(Cell));
        for (int j = 0; j < width; j++)
        {
            maze[i][j].paths = 0;
            maze[i][j].visited = false;
            maze[i][j].onPath = false;
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

static void RemoveOutgoingPath(Cell **path, int x, int y, int direction)
{
    unsigned int east, north, west, south;
    getDir(path[x][y], &east, &north, &west, &south);

    switch (direction)
    {
    case 0: // East
        setDir(&path[x][y], NO_PATH, north, west, south);
        getDir(path[x][y + 1], &east, &north, &west, &south);
        setDir(&path[x][y + 1], east, north, NO_PATH, south);
        break;
    case 1: // North
        setDir(&path[x][y], east, NO_PATH, west, south);
        getDir(path[x - 1][y], &east, &north, &west, &south);
        setDir(&path[x - 1][y], east, north, west, NO_PATH);
        break;
    case 2: // West
        setDir(&path[x][y], east, north, NO_PATH, south);
        getDir(path[x][y - 1], &east, &north, &west, &south);
        setDir(&path[x][y - 1], NO_PATH, north, west, south);
        break;
    case 3: // South
        setDir(&path[x][y], east, north, west, NO_PATH);
        getDir(path[x + 1][y], &east, &north, &west, &south);
        setDir(&path[x + 1][y], east, NO_PATH, west, south);
        break;
    }
}

void InitializeMaze(Cell **path, int mazeHeight, int mazeWidth)
{
    for (int i = 0; i < mazeHeight; i++)
    {
        for (int j = 0; j < mazeWidth; j++)
        {
            path[i][j].visited = false;
            path[i][j].onPath = false;

            if (j == 0)
            {
                setDir(&path[i][j], OUTGOING_PATH, NO_PATH, NO_PATH, NO_PATH);
            }
            else if (j < mazeWidth - 1)
            {
                setDir(&path[i][j], OUTGOING_PATH, NO_PATH, INCOMING_PATH, NO_PATH);
            }
            else if (i == 0)
            {
                setDir(&path[i][j], NO_PATH, NO_PATH, INCOMING_PATH, OUTGOING_PATH);
            }
            else if (i < mazeHeight - 1)
            {
                setDir(&path[i][j], NO_PATH, INCOMING_PATH, INCOMING_PATH, OUTGOING_PATH);
            }
            else
            {
                setDir(&path[i][j], NO_PATH, INCOMING_PATH, INCOMING_PATH, NO_PATH);
            }
        }
    }
}

// Randomize maze
void RandomizeMaze(Cell **path, int height, int width, Root *root, long long count)
{
    long long i = 0;
    while (i < count)
    {
        unsigned int east, north, west, south;
        getDir(path[root->x][root->y], &east, &north, &west, &south);
        // stores the possible directions to go
        int choices[4] = {1, 1, 1, 1};
        if (root->x == height - 1)
            choices[3] = 0;
        if (root->x == 0)
            choices[1] = 0;
        if (root->y == 0)
            choices[2] = 0;
        if (root->y == width - 1)
            choices[0] = 0;
        // picks a valid direction
        int random = rand() % 4;
        while (choices[random] == 0)
        {
            random = rand() % 4;
        }

        switch (random)
        {
        case 0: // picked east
            // make an outgoing path east
            setDir(&path[root->x][root->y], OUTGOING_PATH, north, west, south);
            // change the new root to be east of the old one
            root->y++;
            // replace west path from the new root with an incoming path
            getDir(path[root->x][root->y], &east, &north, &west, &south);
            setDir(&path[root->x][root->y], east, north, INCOMING_PATH, south);
            break;
        case 1: // picked north
            setDir(&path[root->x][root->y], east, OUTGOING_PATH, west, south);
            // change the new root to be north of the old one
            root->x--;
            // replace south path from the new root with an incoming path
            getDir(path[root->x][root->y], &east, &north, &west, &south);
            setDir(&path[root->x][root->y], east, north, west, INCOMING_PATH);
            break;
        case 2: // picked west
            setDir(&path[root->x][root->y], east, north, OUTGOING_PATH, south);
            // change the new root to be west of the old one
            root->y--;
            // replace east path from the new root with an incoming path
            getDir(path[root->x][root->y], &east, &north, &west, &south);
            setDir(&path[root->x][root->y], INCOMING_PATH, north, west, south);
            break;
        case 3: // picked south
            setDir(&path[root->x][root->y], east, north, west, OUTGOING_PATH);
            // change the new root to be south of the old one
            root->x++;
            // replace north path from the new root with an incoming path
            getDir(path[root->x][root->y], &east, &north, &west, &south);
            setDir(&path[root->x][root->y], east, INCOMING_PATH, west, south);
            break;
        default:
            break;
        }

        getDir(path[root->x][root->y], &east, &north, &west, &south);
        if (east == OUTGOING_PATH)
        {
            RemoveOutgoingPath(path, root->x, root->y, 0);
        }
        else if (north == OUTGOING_PATH)
        {
            RemoveOutgoingPath(path, root->x, root->y, 1);
        }
        else if (west == OUTGOING_PATH)
        {
            RemoveOutgoingPath(path, root->x, root->y, 2);
        }
        else if (south == OUTGOING_PATH)
        {
            RemoveOutgoingPath(path, root->x, root->y, 3);
        }
        i++;
    }
}

Image ConvertMazeToImage(Cell **path, int height, int width, Root root)
{
    // Create an image with dimensions 2*width+1 x 2*height+1
    // This gives us 1 pixel per cell and 1 pixel per wall
    const int imageWidth = width * 2 + 1;
    const int imageHeight = height * 2 + 1;
    
    // Create an image filled with white (walls everywhere)
    Image mazeImage = GenImageColor(imageWidth, imageHeight, WHITE);
    
    // Process each cell in the maze
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            unsigned int east, north, west, south;
            getDir(path[i][j], &east, &north, &west, &south);
            
            // Calculate the cell's pixel position (cells are at even coordinates)
            int cellX = j * 2 + 1;
            int cellY = i * 2 + 1;
            
            // Make the cell itself white
            if(i == root.x && j == root.y)
            {
                ImageDrawPixel(&mazeImage, cellX, cellY, RED); // Mark the root cell in red
            }
            else
            {
                ImageDrawPixel(&mazeImage, cellX, cellY, BLACK); // Mark the cell in black
            } 

            if (north == 1)
            {
                ImageDrawPixel(&mazeImage, cellX, cellY - 1, BLACK);
            }

            if (west == 1)
            {
                ImageDrawPixel(&mazeImage, cellX - 1, cellY, BLACK);
            }

            if(east == 1)
            {
                ImageDrawPixel(&mazeImage, cellX + 1, cellY, BLACK);
            }

            if(south == 1)
            {
                ImageDrawPixel(&mazeImage, cellX, cellY + 1, BLACK);
            }
        }
    }
    
    return mazeImage;
}