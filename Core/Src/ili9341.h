#ifndef _ILI9341_H_
#define _ILI9341_H_

#include <stdint.h>

#define COLOR(r, g, b) ((uint16_t)r<<11 | (uint16_t)g<<5 | (uint16_t)b)

void ili9341_reset();
void ili9341_init();
void ili9341_read_id();
void ili9341_fill_rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
void ili9341_draw_image(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t*data);

#endif /* _ILI9341_H_ */
