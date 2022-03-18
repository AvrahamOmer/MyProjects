#include "game_318639408.h"

/* this function is not declared in game.h since it is
   internal to game.c; it is even defined as static -- 
   so the linker won't allow using it outside of game.c */
static unsigned count_neighbors(unsigned i, unsigned j, struct game_state *gs)
{
    unsigned num_of_neighbors = 0;
    for(int row = i -1; row <= i + 1; row++)
    {
        if(row == gs->n_rows + 1)
            row = 0;
        else if (row == -1)
            row = gs->n_rows;
        for(int col = j -1; col <= j + 1; col++)
        {   
            if(col == gs->n_cols + 1)
                col = 0;
            else if (col == -1)
                col = gs->n_cols;

            if(row == i && col == j)
                continue;
            if(gs->board[row][col] == ALIVE)
                num_of_neighbors++;
        }
    }
    return num_of_neighbors;
}

void update(struct game_state *gs)
{
    for (unsigned i = 0; i < gs->n_rows; ++i) {
        for (unsigned j = 0; j < gs->n_cols; ++j) {
            gs->n_neighbors[i][j] = count_neighbors(i, j, gs);
        }
    }

    for (unsigned i = 0; i < gs->n_rows; ++i) {
        for (unsigned j = 0; j < gs->n_cols; ++j) {
            unsigned nn = gs->n_neighbors[i][j];
            if (nn < 2 || nn > 3)
                gs->board[i][j] = DEAD;
            else if (nn == 3)
                gs->board[i][j] = ALIVE;
            // when nn == 2, the previous value is retained
        }
    }
}
