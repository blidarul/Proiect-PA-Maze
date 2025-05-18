#include <stdio.h>
#include<string.h>
#include<stdlib.h>

#define MAX 100

typedef struct 
{
    int volume;
    int game_state;
}Game;


typedef struct Option
{
    char option_name[MAX];
    void (*command)(Game*);
    struct Option *next;
}Option;

typedef struct Node
{
    char command[50];
    struct Node *next;
}Node;



 