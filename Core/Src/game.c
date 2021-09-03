#include "ili9341.h"
#include "evan_be.c"
#include "tetris.h"

#define TETRIS_BLOCK_SIZE   16

#define TETRIS_NEXT_OFFSET_X    200
#define TETRIS_NEXT_OFFSET_Y    30 
#define TETRIS_NEXT_BLOCK_SIZE  10

extern struct tetris_tetrimino tetriminos[];

static uint8_t tetris_area_buf[TETRIS_HEIGHT][TETRIS_WIDTH];
static uint16_t pallete[] = 
{
/* TETRIS_EMPTY, */
COLOR(0, 0, 0),

/* TETRIS_CYAN, */
COLOR(0, 0x2F, 0x1F),

/* TETRIS_BLUE, */
COLOR(0, 0, 0x1F),

/* TETRIS_AMBER, */
COLOR(0x1F, 0x18, 0),

/* TETRIS_YELLOW, */
COLOR(0x1F, 0x2F, 0),

/* TETRIS_GREEN, */
COLOR(0x00, 0x2F, 0),

/* TETRIS_PURPLE, */
COLOR(0x1F, 0, 0x1F),

/* TETRIS_RED */
COLOR(0x1F, 0, 0)
};


void draw_scene()
{
    int x, y;
    ili9341_fill_rect(0,0,240,320, 0);
    ili9341_fill_rect(160,0, 5, 320, COLOR(10, 10<<1, 10));
    
    ili9341_draw_image(170, 240, evan_image.width, evan_image.height, evan_image.pixel_data);
    
    for(y=0;y<TETRIS_HEIGHT;y++) {
        for(x=0;x<TETRIS_WIDTH;x++) {
            tetris_area_buf[y][x] = TETRIS_EMPTY; 
        }
    }
}

void draw_next_tetrimino(uint8_t t)
{
    int i;
    /* All tetriminos have minimal block offset -1 from its center. */

    ili9341_fill_rect(TETRIS_NEXT_OFFSET_X - TETRIS_NEXT_BLOCK_SIZE,
                      TETRIS_NEXT_OFFSET_Y - TETRIS_NEXT_BLOCK_SIZE,
                      4 * TETRIS_NEXT_BLOCK_SIZE,
                      4 * TETRIS_NEXT_BLOCK_SIZE, 0);

    for(i=0;i<TETRIS_BLOCKS_IN_TETRIMINO;i++) {
        ili9341_fill_rect(TETRIS_NEXT_OFFSET_X + tetriminos[t].blocks[i].x * TETRIS_NEXT_BLOCK_SIZE,
                          TETRIS_NEXT_OFFSET_Y + tetriminos[t].blocks[i].y * TETRIS_NEXT_BLOCK_SIZE,
                          TETRIS_NEXT_BLOCK_SIZE-1,
                          TETRIS_NEXT_BLOCK_SIZE-1, 
                          pallete[tetriminos[t].color]);
    }
}

void draw_block(uint8_t x, uint8_t y, uint8_t color)
{
    if(tetris_area_buf[y][x] == color) {
        return;
    }

    tetris_area_buf[y][x] = color;
    ili9341_fill_rect(x*TETRIS_BLOCK_SIZE, y*TETRIS_BLOCK_SIZE, TETRIS_BLOCK_SIZE-1, TETRIS_BLOCK_SIZE-1, pallete[color]);
}


