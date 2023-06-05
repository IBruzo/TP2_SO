#ifndef VIDEO_LOADER_H
#define VIDEO_LOADER_H

#include <stdint.h>

#define ANCHO_PIX 1024
#define ALTURA_PIX 768
#define ANCHO_LETRA_PIX 8
#define BACKROUND 0x002B36
#define isMinusc(x) ((x) <= 'a' ? (((x) >= 'z') ? 1 : 0) : 0)

void put_pixel(uint32_t x, uint32_t y, uint32_t color);
int put_letter(char letter, uint32_t x, uint32_t y, uint32_t tam, uint32_t color);
void put_square(uint32_t x, uint32_t y, uint32_t tam, uint32_t color);
void put_word(char *string, uint32_t x, uint32_t y, uint32_t tam, uint32_t color);
void put_rectangle(uint32_t x, uint32_t y, uint32_t tamX, uint32_t tamY, uint32_t color);
void scroll_up_once(uint32_t tamY, uint32_t color);

#endif
