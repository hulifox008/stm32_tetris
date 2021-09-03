#ifndef _TETRIS_H_
#define _TETRIS_H_

#include <stdint.h>

#define TETRIS_WIDTH    10
#define TETRIS_HEIGHT   20

#define TETRIS_BLOCKS_IN_TETRIMINO      4

enum {
TETRIS_EMPTY,
TETRIS_CYAN,
TETRIS_BLUE,
TETRIS_AMBER,
TETRIS_YELLOW,
TETRIS_GREEN,
TETRIS_PURPLE,
TETRIS_RED
};

struct block_offsets
{
    int8_t x;
    int8_t y;
};

struct tetris_tetrimino
{
    struct block_offsets blocks[TETRIS_BLOCKS_IN_TETRIMINO]; 
    uint8_t color;
    uint8_t next;
};

#define TETRIS_INPUT_UP     (1<<0)
#define TETRIS_INPUT_LEFT   (1<<1)
#define TETRIS_INPUT_RIGHT  (1<<2)
#define TETRIS_INPUT_DOWN   (1<<3)

int tetris_init();

int tetris_iterate(uint8_t input, uint32_t tick);

#endif /* _TETRIS_H_ */
