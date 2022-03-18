#include "game_318639408.h"
#include <stdbool.h>
#include <unistd.h>

#define EXAMPLE_LOAF {  \
    "......",           \
    "..OO..",           \
    ".O..O.",           \
    "..O.O.",           \
    "...O..",           \
    "......"}

#define EXAMPLE_BEACON {\
    "......",           \
    ".OO...",           \
    ".OO...",           \
    "...OO.",           \
    "...OO.",           \
    "......"}

#define EXAMPLE_GLIDER {\
    "......",           \
    "...O..",           \
    ".O.O..",           \
    "..OO..",           \
    "......",           \
    "......"}

// try all 3 examples! 
// e.g. change EXAMPLE_LOAF TO EXAMPLE_BEACON
// and rebulid
//char board[SIZE][SIZE + 1] = EXAMPLE_GLIDER;

int main(int argc, char *argv[])
{
    bool interactive = true;
    struct game_state *gs = malloc(sizeof(struct game_state));
    gs->generation = N_GENERATIONS_DEFAULT;
    gs->board = NULL;
    gs->n_neighbors =NULL;

    if (argc == 1){
        printf("you must enter the name of file\n");
        return 1;
    }
    if(argc > 2){
        if(argc > 3){
            interactive = false;
            if(strcmp(argv[2], "noninteractive") == 0)
                gs->generation = atoi(argv[3]);
            else
                gs->generation = atoi(argv[2]);  
        }
        if(strcmp(argv[2], "noninteractive") == 0)
            interactive = false;
        else
            gs->generation = atoi(argv[2]);
    }
        

    if(!load(gs, argv[1]))
    {
        fprintf(stderr, "the load didnt sucsses\n");
        return 2;
    }

    unsigned gen;

    for (gen = 1; gen <= gs->generation; ++gen) {
        update(gs);
        if (interactive) {
            // display intermediate boards
            clear();
            display(gen, gs);
            sleep(1); // one second
        }
    }
    if (!interactive) // display only final board
        display(gen, gs);

    return 0;
}
