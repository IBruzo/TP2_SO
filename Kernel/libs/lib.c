#include "lib.h"

static int fontsize = 1;
static int cursorX = 4;
static int cursorY = 4;

List *PCBTable;

/* --------------------------------------- PRINT FUNCTIONS -------------------------- */

void *memset(void *destination, int32_t c, uint64_t length)
{
	uint8_t chr = (uint8_t)c;
	char *dst = (char *)destination;

	while (length--)
		dst[length] = chr;

	return destination;
}

void *memcpy(void *destination, const void *source, uint64_t length)
{
	/*
	 * memcpy does not support overlapping buffers, so always do it
	 * forwards. (Don't change this without adjusting memmove.)
	 *
	 * For speedy copying, optimize the common case where both pointers
	 * and the length are word-aligned, and copy word-at-a-time instead
	 * of byte-at-a-time. Otherwise, copy by bytes.
	 *
	 * The alignment logic below should be portable. We rely on
	 * the compiler to be reasonably intelligent about optimizing
	 * the divides and modulos out. Fortunately, it is.
	 */
	uint64_t i;

	if ((uint64_t)destination % sizeof(uint32_t) == 0 &&
		(uint64_t)source % sizeof(uint32_t) == 0 &&
		length % sizeof(uint32_t) == 0)
	{
		uint32_t *d = (uint32_t *)destination;
		const uint32_t *s = (const uint32_t *)source;

		for (i = 0; i < length / sizeof(uint32_t); i++)
			d[i] = s[i];
	}
	else
	{
		uint8_t *d = (uint8_t *)destination;
		const uint8_t *s = (const uint8_t *)source;

		for (i = 0; i < length; i++)
			d[i] = s[i];
	}

	return destination;
}

uint32_t uintToBase(uint64_t value, char *buffer, uint32_t base)
{
	char *p = buffer;
	char *p1, *p2;
	uint32_t digits = 0;

	// Calculate characters for each digit
	do
	{
		uint32_t remainder = value % base;
		*p++ = (remainder < 10) ? remainder + '0' : remainder + 'A' - 10;
		digits++;
	} while (value /= base);

	// Terminate string in buffer.
	*p = 0;

	// Reverse string in buffer.
	p1 = buffer;
	p2 = p - 1;
	while (p1 < p2)
	{
		char tmp = *p1;
		*p1 = *p2;
		*p2 = tmp;
		p1++;
		p2--;
	}

	return digits;
}

static void getStringToPrint(char str[], int strSize, char *foundation, va_list vl)
{

	int i = 0, j = 0;
	// el buffer final y el buffer temporal para las conversiones numericas
	// buff deberia ser dinamico pero no tenemos malloc :/
	char buff[1024] = {0}, tmp[20];
	// buffer para el string
	char *str_arg;

	// mientras haya caracteres y argumentos
	while (foundation && foundation[i])
	{
		// caso especial de que encuentre un porcentaje que representa la insercion de una variable
		if (foundation[i] == '%')
		{
			i++;
			switch (foundation[i])
			{
			// caso char
			case 'c':
			{
				// se toma el argumento que se presume de tipo char y se lo castea para guardarlo
				buff[j] = (char)va_arg(vl, int);
				j++;
				break;
			}
			// caso integer
			case 'd':
			{
				// se toma un int, se lo pasa a decimal y luego a string guardandose en tmp
				itoa(va_arg(vl, int), tmp, 10);
				// se copia tmp al buffer
				strcpy(&buff[j], tmp);
				// se aumenta la posicion del string final
				j += strlen(tmp);
				break;
			}
			// caso hexa
			case 'x':
			{
				// identico al previo pero con base 16
				itoa(va_arg(vl, int), tmp, 16);
				strcpy(&buff[j], tmp);
				j += strlen(tmp);
				break;
			}
			// caso octal
			case 'o':
			{
				// identico al anterior pero con base 8
				itoa(va_arg(vl, int), tmp, 8);
				strcpy(&buff[j], tmp);
				j += strlen(tmp);
				break;
			}
			// caso string
			case 's':
			{
				// no se necesita conversion entonces se guarda directo
				str_arg = va_arg(vl, char *);
				strcpy(&buff[j], str_arg);
				j += strlen(str_arg);
				break;
			}
			}
			// caso donde no hay un porcentaje y se continua el string como si nada
		}
		else
		{
			buff[j] = foundation[i];
			j++;
		}
		i++;
	}

	strcpy(str, buff);
	str[j] = 0;
}

void print(char *foundation, ...)
{
	va_list args;
	va_start(args, foundation);
	char str[1024];
	int strSize = 1024;
	getStringToPrint(str, strSize, foundation, args);
	va_end(args);

	appendstringColor(str, FONTCOLOR);
}

char *snprintf(char *foundation, ...)
{
	va_list args;
	va_start(args, foundation);
	static char str[1024];
	int strSize = 1024;
	getStringToPrint(str, strSize, foundation, args);
	va_end(args);

	return str;
}

void strncpy(char *destination, const char *origin, int n)
{
	int i;
	for (i = 0; origin[i] != '\0' && i < n; i++)
		destination[i] = origin[i];
	destination[i] = '\0';
}

// copia el string de origian a destination
void strcpy(char *destination, const char *origin)
{
	strncpy(destination, origin, strlen(origin));
}

void strncat(char *destination, const char *origin, int n)
{
	int i, j;
	for (i = 0; destination[i] != '\0'; i++)
		;
	for (j = 0; origin[j] != '\0' && j < n; j++)
		destination[i + j] = origin[j];
	destination[i + j] = '\0';
}
void strcat(char *destination, const char *origin)
{
	strncat(destination, origin, strlen(origin));
}
/**
 * @brief pasa a ascii un numero en cualquier base
 * @param i numero a transformar
 * @param strout donde se guarda el string de asciis
 * @param base en que base esta el numero
 */
char *itoa(int i, char *strout, int base)
{
	char *str = strout;
	int digit, sign = 0;
	if (i == 0)
	{
		*str++ = '0';
		*str = '\0';
		return strout;
	}
	if (i < 0)
	{
		sign = 1;
		i *= -1;
	}
	while (i)
	{
		digit = i % base;
		*str = (digit > 9) ? ('A' + digit - 10) : '0' + digit;
		i = i / base;
		str++;
	}
	if (sign)
	{
		*str++ = '-';
	}
	*str = '\0';
	strrev(strout);
	return strout;
}
int strlen(const char *str)
{
	int s;
	for (s = 0; str[s] != 0; ++s)
		;
	return s;
}

// da vuelta el string ABC -> CBA
char *strrev(char *str)
{
	int i;
	int len = 0;
	char c;
	if (!str)
		return 0;
	while (str[len] != '\0')
	{
		len++;
	}
	for (i = 0; i < (len / 2); i++)
	{
		c = str[i];
		str[i] = str[len - i - 1];
		str[len - i - 1] = c;
	}
	return str;
}
void appendstringColor(char *string, int color)
{
	for (int i = 0; string[i] != 0; i++)
	{
		appendcharColor(string[i], color);
	}
}

void appendcharColor(char character, int color)
{
	if (character == '\n')
	{
		newline();
		return;
	}

	drawCursor(CURRENT_CURSOR_COLOR);
	putcharSpecifics(character, cursorX, cursorY, fontsize, color);
	updateCursor();
	drawCursor(FONTCOLOR);
}

void updateCursor()
{
	if (cursorX + fontsize * 8 >= 1000)
	{
		cursorY += fontsize * 16;
		cursorX = 4;
	}
	else
	{
		cursorX += fontsize * 8;
	}
}

void drawCursor(int color)
{
	sys_putSquare(cursorX, cursorY, fontsize * 8, color);
	sys_putSquare(cursorX, cursorY + fontsize * 8, fontsize * 8, color);
}

void putcharSpecifics(char character, int x, int y, int size, int color)
{
	sys_write(character, x, y, size, color);
}

void newline()
{
	drawCursor(CURRENT_CURSOR_COLOR);
	cursorX = 4;
	cursorY += 16 * fontsize;
}

/* --------------------------------------- NATIVE KERNEL FUNCTIONS -------------------------- */

int strcmp(const char *str1, const char *str2)
{
	while (*str1 && (*str1 == *str2))
	{
		str1++;
		str2++;
	}

	return *(unsigned char *)str1 - *(unsigned char *)str2;
}
