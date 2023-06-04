// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <video_driver.h>
#include <fonts.h>

#define ANCHO_PIX 1024
#define ALTURA_PIX 768
#define ANCHO_LETRA_PIX 8
#define BACKROUND 0x002B36
#define isMinusc(x) ((x) <= 'a' ? (((x) >= 'z') ? 1 : 0) : 0)
// esta estructura es de omar-> https://wiki.osdev.org/User:Omarrx024/VESA_Tutorial
typedef struct vbe_mode_info_structure
{
	uint16_t attributes;  // deprecated, only bit 7 should be of interest to you, and it indicates the mode supports a linear frame buffer.
	uint8_t window_a;	  // deprecated
	uint8_t window_b;	  // deprecated
	uint16_t granularity; // deprecated; used while calculating bank numbers
	uint16_t window_size;
	uint16_t segment_a;
	uint16_t segment_b;
	uint32_t win_func_ptr; // deprecated; used to switch banks from protected mode without returning to real mode
	uint16_t pitch;		   // number of bytes per horizontal line
	uint16_t width;		   // width in pixels resolution X
	uint16_t height;	   // height in pixels resolution Y
	uint8_t w_char;		   // unused...
	uint8_t y_char;		   // ...
	uint8_t planes;
	uint8_t bpp;   // bits per pixel in this mode
	uint8_t banks; // deprecated; total number of banks in this mode
	uint8_t memory_model;
	uint8_t bank_size; // deprecated; size of a bank, almost always 64 KB but may be 16 KB...
	uint8_t image_pages;
	uint8_t reserved0;

	uint8_t red_mask;
	uint8_t red_position;
	uint8_t green_mask;
	uint8_t green_position;
	uint8_t blue_mask;
	uint8_t blue_position;
	uint8_t reserved_mask;
	uint8_t reserved_position;
	uint8_t direct_color_attributes;

	uint32_t framebuffer; // physical address of the linear frame buffer; write here to draw to the screen
	uint32_t off_screen_mem_off;
	uint16_t off_screen_mem_size; // size of memory in the framebuffer but not being displayed on the screen
	uint8_t reserved1[206];
} __attribute__((packed)) VbeModeInfo;

VbeModeInfo *info = (VbeModeInfo *)0x0000000000005C00; // en sysvar.asm linea 21 se carga el struct en el bootloader

void put_pixel(uint32_t x, uint32_t y, uint32_t color)
{
	uint32_t pixel_offset = (y) * (info->pitch) + ((x) * ((info->bpp) / 8));
	uint8_t *screen = (uint8_t *)(uint64_t)info->framebuffer;

	screen[pixel_offset] = (uint8_t)color & 0xFF;				// B
	screen[pixel_offset + 1] = (uint8_t)((color >> 8) & 0xFF);	// G
	screen[pixel_offset + 2] = (uint8_t)((color >> 16) & 0xFF); // R
}

/**
 * @brief los cuadrados se arman de arriba a la izq hacia abajo a la derecha
 * @param x pos en x
 * @param y pos en y
 * @param tam tamaño de 1 lado
 * @param color
 */
void put_square(uint32_t x, uint32_t y, uint32_t tam, uint32_t color)
{
	put_rectangle(x, y, tam, tam, color);
}

// esta optimizado por recomendacion de osdev(un poco cambiado)->https://wiki.osdev.org/Drawing_In_Protected_Mode#Drawing_Text
void put_rectangle(uint32_t x, uint32_t y, uint32_t tamX, uint32_t tamY, uint32_t color)
{
	uint8_t *screen = (uint8_t *)(uint64_t)info->framebuffer;
	uint32_t pixel_offset = (y) * (info->pitch);
	for (int j = 0; j < tamY; j++)
	{
		for (int i = 0; i < tamX; i++)
		{
			uint32_t aux = pixel_offset + (x + i) * 3;
			screen[aux] = (uint8_t)color & 0xFF;			   // B
			screen[aux + 1] = (uint8_t)((color >> 8) & 0xFF);  // G
			screen[aux + 2] = (uint8_t)((color >> 16) & 0xFF); // R
		}
		pixel_offset += info->pitch;
	}
}

// entre "A" 65 - 33

/** parametros:
	@param letra
	@param x -> posicion horizontal inicial
	@param y -> posicion vertical inicial
	@param tamaño de la letra
	@param color
*/
int put_letter(char letter, uint32_t x, uint32_t y, uint32_t tam, uint32_t color)
{

	int a = x;
	int start = letter - FIRST_CHAR;

	if (letter == ' ')
	{
		return a + tam * ANCHO_LETRA_PIX;
	}
	// extended ascii table ñ y Ñ en el vector de fonts

	for (int i = 0; i < 32; i++)
	{
		if (i % 2 == 0 && i != 0)
		{
			y += tam;
			a = x;
		}
		char mask = 0x01;
		for (int j = 0; j < 8; j++)
		{
			((uint8_t)font[i + (start * 32)] & (uint8_t)mask) >> j ? put_square(a, y, tam, color) : 0;
			a += tam;
			mask = mask << 1;
		}
	}
	return a + tam; // posicion horizontal final
}
/**
 * @brief encadena letras con put letter moviendose en x por lo q ocupa un caracter
 *
 */
void put_word(char *string, uint32_t x, uint32_t y, uint32_t tam, uint32_t color)
{
	int accum;
	for (int i = 0; string[i] != 0; i++)
	{
		accum = i * tam * ANCHO_LETRA_PIX;
		put_letter(string[i], x + accum, y, tam, color);
	}
}

void scroll_up_once(uint32_t tamY, uint32_t color)
{
	int amount = tamY;
	uint32_t *screen = (uint32_t *)(uint64_t)info->framebuffer;

	// Calculate the number of pixels to shift up
	// uint32_t numPixels = info->pitch / sizeof(uint32_t) * amount;

	// Calculate the number of words per line
	uint32_t wordsPerLine = info->pitch / sizeof(uint32_t);

	// Move each pixel up by `amount` pixels
	for (uint32_t y = 0; y < info->height - amount; y++)
	{
		uint32_t *src = &screen[(y + amount) * wordsPerLine];
		uint32_t *dest = &screen[y * wordsPerLine];
		for (uint32_t x = 0; x < info->width; x++)
		{
			dest[x] = src[x];
		}
	}

	// Copy the background color to the last `amount` lines
	for (uint32_t y = info->height - amount; y < info->height; y++)
	{
		uint32_t *line = &screen[y * wordsPerLine];
		for (uint32_t x = 0; x < info->width; x++)
		{
			line[x] = color;
		}
	}
}
