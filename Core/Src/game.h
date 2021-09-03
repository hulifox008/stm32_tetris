#ifndef _GAME_H_
#define _GAME_H_

#include <stdint.h>

void draw_scene();

void draw_block(uint8_t x, uint8_t y, uint8_t color);

void draw_next_tetrimino(uint8_t t);

#endif
