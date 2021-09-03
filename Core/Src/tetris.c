#include <stdint.h>
#include <stdlib.h>
#include "main.h"
#include "tetris.h"
#include "game.h"


#define TETRIS_DROP_INTERVAL_MS     500
#define TETRIS_INPUT_INTERVAL_1_MS    300 
#define TETRIS_INPUT_INTERVAL_2_MS    100 

#define INVALID_TETRIMINO           0xFF

struct tetris_tetrimino tetriminos[] = {

/* 0
 *
 * #
 * #
 * #
 * #
 *
 */
{
    .blocks = {{0,-1}, {0,0}, {0,1}, {0,2}},
    .color = TETRIS_CYAN,
    .next = 1
},

/* 1
 *
 * ####
 *
 */
{ 
    .blocks = {{-1,1}, {0,1}, {1,1}, {2,1}},
    .color = TETRIS_CYAN,
    .next = 0
},

/* 2
 *
 * ##
 * ##
 *
 */
{
    .blocks = {{0,0}, {1,0}, {0,1}, {1,1}},
    .color = TETRIS_YELLOW,
    .next = 2
},

/* 3
 *
 *  #
 * ###
 *
 */
{
    .blocks = {{-1,1}, {0,1}, {1,1}, {0,0}},
    .color = TETRIS_PURPLE,
    .next = 4
},

/* 4 
 *
 * #
 * ##
 * #
 *
 */
{
    .blocks = {{-1,-1}, {-1,0}, {0, 0}, {-1,1}},
    .color = TETRIS_PURPLE,
    .next = 5
},

/* 5
 *
 * ###
 *  #
 *
 */
{
    .blocks = {{-1,-1}, {0,-1}, {1,-1}, {0,0}},
    .color = TETRIS_PURPLE,
    .next = 6
},

/* 6 
 *
 *  #
 * ##
 *  #
 *
 */
{
    .blocks = {{1,-1}, {0,0}, {1, 0}, {1,1}},
    .color = TETRIS_PURPLE,
    .next = 3
},

/* 7 
 * 
 * ##
 *  #
 *  #
 *
 */
{
    .blocks = {{0,-1}, {1, -1}, {1, 0}, {1, 1}},
    .color = TETRIS_AMBER,
    .next = 8
},

/* 8
 *
 *   #
 * ###
 *
 */
{
    .blocks = {{1,0}, {-1,1}, {0,1}, {1,1}},
    .color = TETRIS_AMBER,
    .next = 9
},

/* 9
 *
 * #
 * #
 * ##
 *
 */
{
    .blocks = {{-1,-1}, {-1,0}, {-1,1}, {0,1}},
    .color = TETRIS_AMBER,
    .next = 10
},

/* 10
 *
 * ###
 * #
 *
 */
{
    .blocks = {{-1,-1}, {0,-1}, {1,-1}, {-1,0}},
    .color = TETRIS_AMBER,
    .next = 7
},

/* 11
 *
 * ##
 * #
 * #
 *
 */
{
    .blocks = {{-1,-1}, {0,-1}, {-1,0}, {-1,1}},
    .color = TETRIS_BLUE,
    .next = 12
},

/* 12
 *
 * ###
 *   #
 *
 */
{
    .blocks = {{-1,-1}, {0,-1}, {1,-1}, {1,0}},
    .color = TETRIS_BLUE,
    .next = 13
},

/* 13
 *
 *  #
 *  #
 * ##
 *
 */
{
    .blocks = {{1,-1}, {1,0}, {1,1}, {0,1}},
    .color = TETRIS_BLUE,
    .next = 14
},

/* 14
 *
 * #
 * ###
 *
 */
{
    .blocks = {{-1,0}, {-1,1}, {0,1}, {1,1}},
    .color = TETRIS_BLUE,
    .next = 11
},

/* 15
 *
 * #
 * ##
 *  #
 *
 */
{
    .blocks = {{-1,-1}, {-1,0}, {0,0}, {0,1}},
    .color = TETRIS_GREEN,
    .next = 16
},

/* 16
 *
 *  ##
 * ##
 *
 */
{
    .blocks = {{0,-1}, {1,-1}, {-1,0}, {0,0}},
    .color = TETRIS_GREEN,
    .next = 15
},

/* 17
 *
 *  #
 * ##
 * #
 *
 */
{
    .blocks = {{0,-1}, {-1,0}, {0,0}, {-1,1}},
    .color = TETRIS_RED,
    .next = 18
},

/* 18
 *
 * ##
 *  ##
 *
 */
{
    .blocks = {{-1,-1}, {0,-1}, {0,0}, {1,0}},
    .color = TETRIS_RED,
    .next = 17
}
};

static uint8_t tetris_area[TETRIS_HEIGHT][TETRIS_WIDTH];
static uint8_t last_input_state = 0;
static uint8_t current_tetrimino, next_tetrimino;
static uint32_t last_iterated;
static uint32_t input_expire;
static int8_t tetrimino_x, tetrimino_y;

static uint8_t tetris_generate_tetrimino()
{
    return rand() % (sizeof(tetriminos)/sizeof(tetriminos[0]));
}

static int is_conflict(uint8_t tetrimino, uint8_t x, uint8_t y)
{
    int i;
    struct tetris_tetrimino *t = &tetriminos[tetrimino];
    for(i=0;i<TETRIS_BLOCKS_IN_TETRIMINO;i++) {
        uint8_t b_x = t->blocks[i].x+x;
        uint8_t b_y = t->blocks[i].y+y;

        if(b_x >= TETRIS_WIDTH || b_y >= TETRIS_HEIGHT) {
            return 1;
        }
        if(tetris_area[b_y][b_x] != TETRIS_EMPTY) {
            return 1;
        }
    }
    return 0;
}

void tetris_land()
{
    int i;
    int x, y;
    struct tetris_tetrimino *t = &tetriminos[current_tetrimino];
    for(i=0;i<TETRIS_BLOCKS_IN_TETRIMINO;i++) {
        tetris_area[t->blocks[i].y + tetrimino_y][t->blocks[i].x + tetrimino_x] = t->color;
    }

    
    /* Look for completed lines */
    /* TODO: no need to start looking from bottom line, but the line we last landed on. */
    y=TETRIS_HEIGHT-1;
    for(;;) {
        int blocks_in_line = 0;
        for(x=0;x<TETRIS_WIDTH;x++) {
            if(tetris_area[y][x] != TETRIS_EMPTY) {
                blocks_in_line++;
            }
        }

        if(blocks_in_line == 0) {
            break;
        } else if (blocks_in_line == TETRIS_WIDTH) {
            /* This is a full line, move all blocks above down. */
            int line = y;
            for(;line>0;line--) {
                for(x=0;x<TETRIS_WIDTH;x++) {
                    tetris_area[line][x] = tetris_area[line-1][x];
                }
            }
            continue;
        }

        if(--y == 0) {
            break;
        }
    }
}

int8_t tetrimino_min_block_offset_y(uint8_t t)
{
    int8_t min = 0;
    int i;
    for(i=0;i<TETRIS_BLOCKS_IN_TETRIMINO;i++) {
        if (tetriminos[t].blocks[i].y < min ) {
            min = tetriminos[t].blocks[i].y; 
        }
    }
    return min; 
}

int tetris_init()
{
    int x, y;
    for(y=0;y<TETRIS_HEIGHT;y++) {
        for(x=0;x<TETRIS_WIDTH;x++) {
            tetris_area[y][x] = TETRIS_EMPTY;
        }
    }

    srand(1);

    current_tetrimino = INVALID_TETRIMINO;
    next_tetrimino = tetris_generate_tetrimino();

    last_iterated = HAL_GetTick();
    input_expire = 0;

    return 0;
}

static void tetris_redraw()
{
    int x, y;
    for(y=0;y<TETRIS_HEIGHT;y++) {
        for(x=0;x<TETRIS_WIDTH;x++) {
            draw_block(x, y, tetris_area[y][x]);
        }
    }

    if(current_tetrimino != INVALID_TETRIMINO) {
        struct tetris_tetrimino *t = &tetriminos[current_tetrimino];
        for(x=0;x<TETRIS_BLOCKS_IN_TETRIMINO;x++) {
            draw_block( t->blocks[x].x + tetrimino_x, t->blocks[x].y + tetrimino_y, t->color);
        }
    }
}

static void tetris_move_down()
{
    /* Test if we can drop down */
    if(is_conflict(current_tetrimino, tetrimino_x, tetrimino_y+1)) {
        tetris_land();
        current_tetrimino = INVALID_TETRIMINO;
    } else {
        tetrimino_y++;
    }
}

int tetris_iterate(uint8_t input, uint32_t tick)
{
    int redraw = 0;
    uint8_t changed_input = last_input_state ^ input;
    last_input_state = input;


    if ( (changed_input || (tick > input_expire)) && current_tetrimino != INVALID_TETRIMINO) {

        input_expire = tick + (changed_input ? TETRIS_INPUT_INTERVAL_1_MS : TETRIS_INPUT_INTERVAL_2_MS);

        if((input & TETRIS_INPUT_LEFT) && !is_conflict(current_tetrimino, tetrimino_x-1, tetrimino_y)) {
            tetrimino_x--;
            redraw = 1;
        }

        if((input & TETRIS_INPUT_RIGHT) && !is_conflict(current_tetrimino, tetrimino_x+1, tetrimino_y)) {
            tetrimino_x++;
            redraw = 1;
        }

        if((input & TETRIS_INPUT_UP) && !is_conflict(tetriminos[current_tetrimino].next, tetrimino_x, tetrimino_y)) {
            current_tetrimino = tetriminos[current_tetrimino].next;
            redraw = 1;
        }

        if(redraw) {
            tetris_redraw();
        }

        if((input & TETRIS_INPUT_DOWN)) {
            tetris_move_down();
            last_iterated = tick;
            tetris_redraw();
            return 0;
        }
    }

    if ( tick - last_iterated < TETRIS_DROP_INTERVAL_MS )  {
        return 0;
    }
    last_iterated = tick;

    if(current_tetrimino == INVALID_TETRIMINO) {
        current_tetrimino = next_tetrimino;
        next_tetrimino =  tetris_generate_tetrimino();
        draw_next_tetrimino(next_tetrimino);

        tetrimino_x = TETRIS_WIDTH/2 - 1;
        tetrimino_y = 0 - tetrimino_min_block_offset_y(current_tetrimino);

        tetris_redraw();
        if(is_conflict(current_tetrimino, tetrimino_x, tetrimino_y)) {
            /* Game over */
            return -1;
        }
    } else {
        tetris_move_down();
        tetris_redraw();
    }
    return 0;
}

