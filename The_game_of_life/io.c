#include "game_318639408.h"
#include <assert.h>
#include <stdio.h>

// clear terminal screen
void clear()
{
    printf("\e[2J\e[H"); // magic!
}

// display board and current generation
void display(unsigned gen, struct game_state *gs)
{
    printf("Game generation %u\n", gen);
    for (unsigned i = 0; i < gs->n_rows; ++i) {
        puts(gs->board[i]);
    }
    fflush(stdout); // make sure it gets displayed
}

void free_game_state(struct game_state *gs)
{
    for(int i = 0; i < gs->n_rows; i++)
    {
        free(gs->board[i]);
        free(gs->n_neighbors[i]);
    }
    free(gs->board);
    free(gs->n_neighbors);
}


// returns true if load was successful
bool load(struct game_state *gs, char const *filename)
{
    assert(gs && filename);
    FILE *fp = fopen(filename, "r");
    if(!fp){
        printf("the file not open\n");
        return false;
    }

    gs->n_rows = fgetc(fp) - '0';

    //printf("rows = %d\n", gs->n_rows);
    fseek(fp, 1, SEEK_CUR);
    gs->n_cols = fgetc(fp) - '0';

    //printf("cols = %d\n", gs->n_cols);

    if(gs->n_cols > SIZEMAX || gs->n_rows > SIZEMAX){
        printf("over sizemax\n");
        return false;
    }
    gs->board = malloc(sizeof(char*) * gs->n_rows);
    if(!gs->board){
        fclose(fp);
        return false;
    }
    gs->n_neighbors = malloc(sizeof(unsigned*) * (gs->n_rows + 1));
    if(!gs->n_neighbors){
        free(gs->board);
        fclose(fp);
        return false;
    }
    for(int c = 0; c < gs->n_rows +  1; c++){
        gs->board[c] = malloc(gs->n_cols + 1);
        if(!gs->board[c]){
            free_game_state(gs);
            fclose(fp);
            return false;
        }
        gs->n_neighbors[c] = malloc(sizeof(unsigned) * (gs->n_cols));
        if(!gs->n_neighbors[c]){
            free_game_state(gs);
            fclose(fp);
            return false;
        }
    }

    fseek(fp, 1, SEEK_CUR);
    int i = 0;
    int j;
    char ch = '0';
    while(i < gs->n_rows)
    {
        j = 0;
        while(j < gs->n_cols)
        {
            ch = fgetc(fp);

            //printf("%c\n", ch);

            if(ch == EOF) return false;
            if(ch == '\n') break;
            if (ch != DEAD && ch != ALIVE){
                printf("wrong val\n");
                return false;
            }
            gs->board[i][j] = ch;
            j++;
        }
        ch = fgetc(fp);

        //printf("%c\n", ch);

        if(ch == EOF) return false;
        if(j != gs->n_cols || ch != '\n'){
            printf("problem with cols\n");
            return false;
        }
        gs->board[i][j + 1] = '\0';
        i++;

    }
    ch = fgetc(fp);
    //printf("%c\n", ch);
    if(i != gs->n_rows || ch != EOF){
        printf("problem with rows\n");
        return false;
    }
    fclose(fp);
    return true;
}
