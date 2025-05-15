#include <stdio.h>
#include<string.h>
#include<stdlib.h>

#define MAX 100

typedef struct 
{
    int volume;  //volum ce variaza intre 0 si 10
    int game_state;   //joc in desfasurare sau joc oprit
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

void push(Node **top, const char *command)
{
    Node *new = (Node*)malloc(sizeof(Node));

    strcpy(new->command, command);
    new->next = *top;
    *top = new;
}

void pop(Node **top)
{
    if(*top)
    {
        Node *auxiliar = *top;
        *top = (*top)->next;
    }
}

void print_history(Node *top)
{
    while(top)
    {
        printf("%s\n", top->command);
        top = top->next;
    }
}

void resume_game(Game *game_state)
{
    printf("GAME RESUMED");
    game_state->game_state = 1;  //game on
}

void restart_game(Game *game_state)
{
    printf("LEVEL RESTARTED");
}

void set_volume(Game *game_state)
{
    int i, new_volume;
    printf("Actual volume is: %d", game_state->volume);

    printf("[");
    for(i = 0; i < 10; i++)
    {
        if(i < game_state->volume)
            printf("▊");

        else 
            printf("-");
    }
    printf("]");

    printf("New volume is: ");
    scanf("%d", &new_volume);

    if (new_volume>= 0 && new_volume<=10)
    {
        game_state->volume = new_volume;
    }
}

void exit_game(Game *state)
{
    printf("Exit");
    state->game_state = 0;
}

int add_option(Option **head, const char *option_name, void(*command)(Game*))
{
    Option *new = (Option *)malloc(sizeof(Option));

    strcpy(new->option_name, option_name);
    new->command = command;
    new->next = NULL;

    if(!*head)
    {
        *head = new;
    }
    else
    {
        Option *auxiliar = *head;

        while(auxiliar->next)
             auxiliar = auxiliar->next;

        auxiliar->next = new;
        
    }

    return 1;
}

void print_menu(Option *head, Game *state, Node **history)
{
    int index = 1;
    int choose;
    int i;

    Option *current_option = head;

    printf("\n\tMENU\n");
    while(current_option)
    {
        printf("%d. %s\n", index, current_option->option_name);
        current_option = current_option->next;
        index = index + 1;
    }

    printf("Choose an option:");
    scanf("%d", &choose);

    current_option = head;

    for(i = 1; i < choose && current_option; i++)
        current_option = current_option->next;

    if(current_option)
    {
        push(history, current_option->option_name);
        current_option->command(state);
    }
    else
    {
        printf("Invalid option");
    }
}

int main()
{
    Option *menu = NULL;
    Node *history = NULL;

    int volume;

    Game state = { .volume = 5, .game_state = 1};

    add_option(&menu, "RESUME GAME", resume_game);
    add_option(&menu, "RESTART GAME", restart_game);
    add_option(&menu, "VOLUME", set_volume);
    add_option(&menu, "EXIT GAME", exit_game);

    while(state.game_state)
        print_menu(menu, &state, &history);

    print_history(history);

    return 0;
}

 