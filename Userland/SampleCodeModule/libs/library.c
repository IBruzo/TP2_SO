// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <library.h>
#define CURSOR_TICKS 9

static int fontsize = 1;
static int cursorX = 4; // por que estaba la hora
static int cursorY = 4;

#define IN_BOUNDS ((cursorX + fontsize * 8) / 1024) * 16 * fontsize < 736 // no termino de entender porque con 768 se pasa, REVISAR

// Retorna si es el ultimo enter
int lastEnter()
{
	return cursorY + 16 * fontsize >= 780;
}

// Retorna si puede escribir
int canWrite()
{
	return cursorX + fontsize * 8 + 16 < 1000;
}

// Retorna si es alpha numerico
int isalnum(int c)
{
	return (c >= 'A' && c <= 'Z') ||
		   (c >= 'a' && c <= 'z') ||
		   (c >= '0' && c <= '9');
}

// inthex to ascii
unsigned char inthextoa(unsigned char a)
{
	if (a <= 9)
		return a + '0';
	else if (a <= 15)
		return a - 10 + 'A';
	return a;
}

// ascii to inthex
unsigned char atointhex(unsigned char a)
{
	unsigned char returnChar = 0; // 0000 0000
	if (a >= 'A' && a <= 'F')
		returnChar = (a % 'A') + 10; // caso B seria 0000 1011
	else if (a >= '0' && a <= '9')
		returnChar = a % '0';
	return returnChar;
}

// Convierte un string a int
int strToInt(char *str)
{
	int i = 0;
	int rta = 0;
	while (str[i] != 0)
	{
		rta = rta * 10 + str[i] - '0';
		i++;
	}
	return rta;
}

// Convierte un int a string
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

// Convierte un hex a un char
unsigned char hexToChar(unsigned char a, unsigned char b)
{
	unsigned char returnChar = atointhex(a);
	returnChar = returnChar << 4;
	return returnChar + atointhex(b);
}

// Memcopy
void memCopy(char *pointer1, char *pointer2, int chars)
{
	for (int i = 0; i < chars; i++)
		pointer1[i] = pointer2[i];
}

// Convierte un hex string a int
uint64_t hexstringToInt(char *s)
{
	if (s[0] == '0' && s[1] == 'x')
		s += 2;

	unsigned int len = strlen(s);
	unsigned long int result = 0;
	for (int i = 0; i < len; i++)
		result = result * 16 + atointhex(s[i]);
	return result;
}

// Calcula base elevado a exp
int pow(int base, unsigned int exp)
{
	int rta = 1;
	for (int i = 0; i < exp; i++)
		rta *= base;
	return rta;
}

// Retorna si es un char hex valido
char isHexChar(char c)
{
	if ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F'))
	{
		return 1;
	}
	return 0;
}

// Retorna si el string es solo hex
char onlyHexChars(char *string)
{
	for (int i = 0; string[i] != 0; i++)
		if (!(isHexChar(string[i])))
			return 0;
	return 1;
}

// Divide un string en dos en base al splitter
void splitString(char *original, char *firstSplit, char splitter)
{
	int i = 0, j = 0, k = 0;
	int flag = 0;
	for (; original[i] != 0; i++)
	{
		if (original[i] == splitter && flag == 0)
		{
			flag = 1;
			i++;
		}
		if (flag)
		{
			firstSplit[j++] = original[i];
		}
		else
		{
			original[k++] = original[i];
		}
	}
	firstSplit[j] = 0;
	original[k] = 0;
	return;
}

// Cuenta la cantidad de digitos de un numero
int countDigits(int number)
{
	int digits, limit;
	if (number < 0)
		number *= -1;
	for (digits = 1, limit = 10;; digits++, limit *= 10)
	{
		if (number < limit)
			break;
	}
	return digits;
}

// Carga en el buffer el numero
void numLoader(int number, char *buffer, int digits)
{
	for (digits--; number != 0; number /= 10, digits--)
	{
		buffer[digits] = number % 10 + 48; // voy construyendo el numero de atras para adelante
	}
}

// Convierte un int a string
void intToString(int number, char *buffer, int digits)
{
	if (number < 0)
	{
		buffer[0] = '-';
		buffer[digits + 1] = 0;
		numLoader(-number, buffer + 1, digits);
	}
	else
	{
		buffer[digits] = 0;
		numLoader(number, buffer, digits);
	}

	return;
}

// Convierte un float a string
void floatToString(float number, char *buffer, int digits)
{
	intToString(number, buffer, digits);
	buffer[digits] = '.';
	if (number < 0)
		number *= -1;

	number -= (int)number;
	int i;
	for (i = digits + 1; i < digits + 1 + 8; i++)
	{
		int aux = ((int)(number * 10)) % 10;
		number *= 10;
		buffer[i] = (char)(aux + 48);
	}
	buffer[i] = 0;
	return;
}

// Actualiza la posicion del cursor
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

// Concatena a la posicion actual un caracter con color dado
void appendcharColor(char character, int color)
{
	if (character == '\n' && getOutputFD(getPid()) != -1)
	{
		newline();
	}
	// si es output a foreground se renderiza
	if (getOutputFD(getPid()) != -1)
	{
		drawCursor(CURRENT_CURSOR_COLOR);
	}

	putcharSpecifics(character, cursorX, cursorY, fontsize, color);

	// si es output a foreground se actualiza
	if (getOutputFD(getPid()) != -1)
	{
		updateCursor();
		drawCursor(FONTCOLOR);
	}
}

// Concatena a la posicion actual un caracter
void appendchar(char character)
{
	appendcharColor(character, FONTCOLOR);
}

// Concatena a la posicion actual un string con color dado
void appendstringColor(char *string, int color)
{
	for (int i = 0; string[i] != 0; i++)
	{
		appendcharColor(string[i], color);
	}
}

// Concatena a la posicion actual un string
void appendstring(char *string)
{
	appendstringColor(string, FONTCOLOR);
}

// mueve el cursor como si fuera un \n
void newline()
{
	drawCursor(CURRENT_CURSOR_COLOR);
	cursorX = 4;
	cursorY += 16 * fontsize;
}

// Backspace del cursor
void backspace()
{
	if (cursorX != 0)
	{
		drawCursor(CURRENT_CURSOR_COLOR);
		if (cursorX - fontsize * 8 < 4)
		{
			cursorY -= fontsize * 16;
			cursorX = 1024 - 8 * fontsize;
		}
		else
			cursorX -= fontsize * 8;
		putSquare(cursorX, cursorY, fontsize * 8, BACKGROUND_COLOR);
		putSquare(cursorX, cursorY + fontsize * 8, fontsize * 8, BACKGROUND_COLOR);
		drawCursor(FONTCOLOR);
	}
}

// putchar
void putchar(char c)
{
	appendchar(c);
}

// Imprime char con parametros especificos
void putcharSpecifics(char character, int x, int y, int size, int color)
{
	write(character, x, y, size, color);
}

// Imprime string con parametros especificos
void putstringSpecifics(char *string, int x, int y, int size, int color)
{
	int accum;
	for (int i = 0; string[i] != 0; i++)
	{
		accum = i * size * 8;
		putcharSpecifics(string[i], x + accum, y, size, color);
	}
}

// Compara dos strings y retorna la diferencia
int strcmp(const char *s1, const char *s2)
{
	while (*s1 && (*s1 == *s2))
	{
		s1++, s2++;
	}
	return *(const unsigned char *)s1 - *(const unsigned char *)s2;
}

// Retorna el largo de un string
int strlen(const char *str)
{
	int s;
	for (s = 0; str[s] != 0; ++s)
		;
	return s;
}

// Imprime un string
static void wrapperprint(char *foundation, int color, va_list vl)
{

	int i = 0, j = 0;
	// el buffer final y el buffer temporal para las conversiones numericas
	// buff deberia ser dinamico pero no tenemos malloc :/
	char buff[4096] = {0}, tmp[20];
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
	appendstringColor(buff, color);
}

// Imprime un string con color
void printColor(char *foundation, int color, ...)
{
	va_list args;
	va_start(args, color);
	wrapperprint(foundation, color, args);
	va_end(args);
}

// Imprime un string con color por defecto (vendria a ser un printf para nosotros)
void print(char *foundation, ...)
{
	va_list args;
	va_start(args, foundation);
	wrapperprint(foundation, FONTCOLOR, args);
	va_end(args);
}

// Copia el string origin en destination
void strcpy(char *destination, const char *origin)
{
	strncpy(destination, origin, strlen(origin));
}

// Copia el string origin en destination hasta n caracteres
void strncpy(char *destination, const char *origin, int n)
{
	int i;
	for (i = 0; origin[i] != '\0' && i < n; i++)
		destination[i] = origin[i];
	destination[i] = '\0';
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

// Convierte a ascii un numero en cualquier base
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

// Convierte BCD en decimal
int bcdToDec(int bcd)
{
	return ((bcd / 16) * 10 + (bcd % 16));
}

// printf que no se recomienta usar
void printf(char *foundation, void *parameters[])
{
	int j = 0; // posicion en los parametros
	for (int i = 0; foundation[i] != 0; i++)
	{
		if (foundation[i] == '%')
		{
			i++;
			switch (foundation[i])
			{
			case 's':
			{
				appendstring((char *)parameters[j++]);
				break;
			}
			case 'd':
			{
				int number = *((int *)parameters[j++]);
				int digits = countDigits(number);
				char buffer[number < 0 ? digits + 2 : digits + 1];
				intToString(number, buffer, digits);
				appendstring(buffer);
				break;
			}
			case 'f':
			{
				float floatNumber = *((float *)parameters[j++]);
				int digits = countDigits((int)floatNumber);
				char buffer2[floatNumber < 0 ? digits + 3 + 8 : digits + 2 + 8]; // 8 es la precision de decimales que tiene nuestro float 2 es en casode que haya que agregar un punto y un '\0' al final, y tambien esta el caso del menos
				floatToString(floatNumber, buffer2, digits);
				appendstring(buffer2);
				break;
			}
			default:
				break;
			}
		}
		else
		{
			appendchar(foundation[i]);
		}
	}
}

// Print para numeros en base 10
void printInt(uint64_t integer)
{
	char buffer[20] = {0};
	uintToBase(integer, buffer, 10);
	appendstring(buffer);
}

// Print para hexadecimales
void printHex(uint64_t integer)
{
	char buffer[20] = {0};
	uintToBase(integer, buffer, 16);
	appendstring(buffer);
}

// Print que termina con un newline
void println(char *string)
{
	appendstring(string);
	newline();
}

// cambia de lugar el cursor
void setCursor(int x, int y)
{
	cursorX = x;
	cursorY = y;
}

// Cursor de la termial
void drawCursor(int color)
{
	putSquare(cursorX, cursorY, fontsize * 8, color);
	putSquare(cursorX, cursorY + fontsize * 8, fontsize * 8, color);
}

// Resetea el cursor a su posicion inicial
void restartCursor()
{
	cursorX = 4;
	cursorY = 4;
}

// cambia el tamaño de la fuente
void changeFontSize(int increment)
{
	if (fontsize + increment > 5)
		return;
	if ((fontsize + increment) > 0)
		fontsize += increment;
}

// Comparacion de strings, retorna 1 si son iguales 0 si no
char streql(const char *stringA, const char *stringB)
{
	int i = 0;
	while (stringB[i] != '\0')
	{
		if (stringA[i] == '\0' || stringA[i] != stringB[i])
			return 0;
		i++;
	}
	if (stringA[i] != 0)
		return 0;
	return 1;
}

// Convierte string en mayusculas
char *toUpper(char *string)
{
	int i = 0;
	while (string[i] != 0)
	{
		if (string[i] >= 'a' && string[i] <= 'z')
			string[i] = string[i] - 32;

		i++;
	}
	return string;
}

// scanf que no se recomienda usar
int scan(char *str, ...)
{
	va_list vl;
	int i, ret = 0;
	va_start(vl, str);
	i = 0;
	while (str && str[i])
	{
		if (str[i] == '%')
		{
			i++;
			switch (str[i])
			{
			case 's':
			{
				char *string;
				string = va_arg(vl, char *);
				char buff[100];
				strcpy(buff, string);
				int k = 0;
				char c;
				do
				{
					c = getchar(); // clear buffer de los espacios
				} while (c == ' ');
				appendchar('A');
				while (c != '\n' && c != ' ')
				{
					buff[k++] = c;
					c = getchar();
				}
				buff[k] = '\0';
				strcpy(string, buff);
				ret++;
				break;
			}
			case 'c':
			{
				char c;
				do
				{
					c = getchar(); // clear buffer de los espacios
				} while (c == ' ');
				*(char *)va_arg(vl, char *) = c;
				ret++;
				break;
			}
			case 'd':
			{
				char c;
				do
				{
					c = getchar(); // clear buffer de los espacios
				} while (c == ' ');

				int sign = 1;
				if (c == '-')
				{
					sign = -1;
					c = getchar();
				}
				int result = 0;
				while (c != '\n' && c != ' ')
				{
					result = result * 10 + (c - '0');
					c = getchar();
				}
				*(int *)va_arg(vl, int *) = result * sign;
				ret++;
				break;
			}
			case 'x':
			{
				char c;
				do
				{
					c = getchar(); // clear buffer de los espacios
				} while (c == ' ');
				int result = 0;
				while (c != '\n' && c != ' ')
				{
					result = result * 16 + atointhex(c);
					c = getchar();
				}
				*(int *)va_arg(vl, int *) = result;

				ret++;
				break;
			}
			case 'f':
			{
				char c;
				do
				{
					c = getchar(); // clear buffer de los espacios
				} while (c == ' ');
				int sign = 1;
				if (c == '-')
				{
					sign = -1;
					c = getchar();
				}
				int result = 0;
				while (c != '.')
				{
					result = result * 10 + (c - '0');
					c = getchar();
				}
				c = getchar();
				int decimal = 0;
				int decimalCount = 0;
				while (c != '\n' && c != ' ')
				{
					decimal = decimal * 10 + (c - '0');
					c = getchar();
					decimalCount++;
				}
				*(float *)va_arg(vl, float *) = ((float)result + (float)decimal / (float)pow(10, decimalCount)) * sign;
				ret++;
				break;
			}
			}
		}
		i++;
	}
	va_end(vl);
	return ret;
}

// crea un proceso en foreground
int createFGProcess(char *name, void *(*f)(int, char **), int argc, char **argv)
{
	int FGFD[] = {0, 1};
	return createProcess(name, f, argc, argv, FGFD);
}

// crea un proceso en background
int createBGProcess(char *name, void *(*f)(int, char **), int argc, char **argv)
{
	int BGFD[] = {-1, -1};
	return createProcess(name, f, argc, argv, BGFD);
}

// Retorna si contiene un ampersand
int hasAmpersand(char *str)
{
	while (*str)
	{
		if (*str == '&')
		{
			return 1;
		}
		str++;
	}
	return 0;
}

// Retorna si contiene un pipe
int hasPipe(char *str)
{
	while (*str != '\0')
	{
		if (*str == '|')
		{
			return 1;
		}
		str++;
	}
	return 0;
}

// funcion WC pero de libreria
int wc(char *str)
{
	return (strlen(str) * fontsize * 8) / 1024 + 1;
}

// funcion que convierte un char a minuscula
char toLower(char c)
{
	if (c >= 'A' && c <= 'Z')
		c = c + 32;
	return c;
}

// funcion que filtra las vocales de un string
void filter(char *str, char *buffer)
{
	int len = strlen(str);
	int j = 0;

	for (int i = 0; i < len; i++)
	{
		if (toLower(str[i]) != 'a' && toLower(str[i]) != 'e' &&
			toLower(str[i]) != 'i' && toLower(str[i]) != 'o' &&
			toLower(str[i]) != 'u')
		{
			buffer[j++] = str[i];
		}
	}

	buffer[j] = '\0';
}

// sleep de la libreria
void makeshiftSleep(int duration)
{
	int startTick = gettick();
	int currentTick;

	do
	{
		currentTick = gettick();
	} while (currentTick - startTick < duration);
}

// funcion que filtra los espacios de un string
void filterSpaces(char *str)
{
	if (str == NULL)
	{
		return;
	}

	char *p = str;
	char *q = str;

	while (*q != '\0')
	{
		if (*q == ' ')
		{
			*p = *q;
			p++;
		}
		q++;
	}

	*p = '\0';
}
