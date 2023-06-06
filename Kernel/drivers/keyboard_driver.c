// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <keyboard_driver.h>

static int language = SPANISH;

// Se cargarom los teclados en ingles y español
static char keyboards[2][MAX_SCANCODE][2] = {
    {{0, 0},
     {ESC, ESC},
     {'1', '!'},
     {'2', '@'},
     {'3', '#'},
     {'4', '$'},
     {'5', '%'},
     {'6', '^'},
     {'7', '&'},
     {'8', '*'},
     {'9', '('},
     {'0', ')'},
     {'-', '_'},
     {'=', '+'},
     {'\b', '\b'},
     {'\t', '\t'},
     {'q', 'Q'},
     {'w', 'W'},
     {'e', 'E'},
     {'r', 'R'},
     {'t', 'T'},
     {'y', 'Y'},
     {'u', 'U'},
     {'i', 'I'},
     {'o', 'O'},
     {'p', 'P'},
     {'[', '{'},
     {']', '}'},
     {'\n', '\n'},
     {0, 0},
     {'a', 'A'},
     {'s', 'S'},
     {'d', 'D'},
     {'f', 'F'},
     {'g', 'G'},
     {'h', 'H'},
     {'j', 'J'},
     {'k', 'K'},
     {'l', 'L'},
     {';', ':'},
     {'\'', '\"'},
     {'`', '~'},
     {0, 0},
     {'\\', '|'},
     {'z', 'Z'},
     {'x', 'X'},
     {'c', 'C'},
     {'v', 'V'},
     {'b', 'B'},
     {'n', 'N'},
     {'m', 'M'},
     {',', '<'},
     {'.', '>'},
     {'/', '?'},
     {0, 0},
     {0, 0},
     {0, 0},
     {' ', ' '}},
    {{0, 0},
     {ESC, ESC},
     {'1', '!'},
     {'2', '"'},
     {'3', '#'},
     {'4', '$'},
     {'5', '%'},
     {'6', '&'},
     {'7', '/'},
     {'8', '('},
     {'9', ')'},
     {'0', '='},
     {'\'', '?'},
     {'|', '}'},
     {'\b', '\b'},
     {'\t', '\t'},
     {'q', 'Q'},
     {'w', 'W'},
     {'e', 'E'},
     {'r', 'R'},
     {'t', 'T'},
     {'y', 'Y'},
     {'u', 'U'},
     {'i', 'I'},
     {'o', 'O'},
     {'p', 'P'},
     {'`', '\"'},
     {'+', '*'},
     {'\n', '\n'},
     {0, 0},
     {'a', 'A'},
     {'s', 'S'},
     {'d', 'D'},
     {'f', 'F'},
     {'g', 'G'},
     {'h', 'H'},
     {'j', 'J'},
     {'k', 'K'},
     {'l', 'L'},
     {'~' + 1, '~'},
     {'{', '['},
     {'`', '~'},
     {0, 0},
     {'}', ']'},
     {'z', 'Z'},
     {'x', 'X'},
     {'c', 'C'},
     {'v', 'V'},
     {'b', 'B'},
     {'n', 'N'},
     {'m', 'M'},
     {',', ';'},
     {'.', ':'},
     {'-', '_'},
     {0, 0},
     {0, 0},
     {0, 0},
     {' ', ' '}}};

static int shiftActivated = 0;
static int capsActivated = 0;
static int controlActivated = 0;

// Buffer de escritura guardado en kernel
static char keyBuffer[MAX_BUFFER];
static int bufferCount = 0;

void handleCtrlC();
void changeLanguage(int lan)
{
    language = lan;
}

void clearKeyBuffer()
{
    for (int i = 0; i < MAX_BUFFER; i++)
        keyBuffer[i] = (char)0;
    bufferCount = 0;
}

static char popBuffer()
{
    if (bufferCount <= 0)
    {
        return 0;
    }
    char key = keyBuffer[0];
    for (int i = 0; i < bufferCount; i++)
    {
        keyBuffer[i] = keyBuffer[i + 1];
    }
    bufferCount--;
    return key;
}

// Devuelve el la primer tecla presionada
char getKey()
{
    if (bufferCount <= 0)
    {
        block(getCurrentPid());
        push(&inputStack, getCurrentPid());
        return 0;
    }
    return popBuffer();
}

// Devuelve el ultimo caracter ingresado
char getLastChar()
{
    if (bufferCount <= 0)
    {
        return 0;
    }
    return keyBuffer[bufferCount - 1];
}

static int isValidScancode(int scancode)
{
    return scancode > 0 && scancode < MAX_SCANCODE;
}

static int isLetter(int scancode)
{
    if (scancode == 0x27)
        return 1;
    char ascii = keyboards[language][scancode][0];
    return (ascii >= 'a' && ascii <= 'z');
}

// Almacena el caracter presionado en el Buffer
void storeKey()
{
    int scancode = inb(0x60);
    switch (scancode)
    {
    case CTRL_PRESSED:
        controlActivated = 1;
        break;
    case CTRL_RELEASED:
        controlActivated = 0;
        break;
    case LSHIFT_PRESSED:
    case RSHIFT_PRESSED:
        shiftActivated = 1;
        break;
    case LSHIFT_RELEASED:
    case RSHIFT_RELEASED:
        shiftActivated = 0;
        break;
    case CAPSLOCK:
        capsActivated = !capsActivated;
        break;
    case SCANCODE_UP_ARROW:
        keyBuffer[bufferCount++] = ASC_UP;
        break;
    case SCANCODE_DOWN_ARROW:
        keyBuffer[bufferCount++] = ASC_DOWN;
        break;
    case SCANCODE_LEFT_ARROW:
        keyBuffer[bufferCount++] = ASC_LEFT;
        break;
    case SCANCODE_RIGHT_ARROW:
        keyBuffer[bufferCount++] = ASC_RIGHT;
        break;
    default:
        if (controlActivated)
        {
            int index = shiftActivated || (isLetter(scancode) && capsActivated) ? 1 : 0;
            char combinedChar = keyboards[language][scancode][index];
            if (combinedChar == 'c' || combinedChar == 'C')
            {
                handleCtrlC();
            }
            else if (combinedChar == 'd' || combinedChar == 'D')
            {
                keyBuffer[bufferCount++] = EOF;
            }
            break;
        }
        if (isValidScancode(scancode) && bufferCount < MAX_BUFFER)
        {
            int index = shiftActivated || (isLetter(scancode) && capsActivated) ? 1 : 0; // mayusucla o minuscula?
            keyBuffer[bufferCount] = keyboards[language][scancode][index];
            bufferCount++;
        }
        break;
    }
    if (bufferCount > 0)
    {
        unblock(peek(&inputStack));
        pop(&inputStack);
    }
    return;
}

void handleCtrlC()
{
    keyBuffer[bufferCount++] = KILL_PROCESS;
    PCB *curr = get(PCBTable, peek(&inputStack));
    // Se evita el asesinato de la shell
    if (curr->PID == 4)
    {
        // print("Beneath an unsinking black sun... through the boundless gloom... our journey continues.\n");
        return;
    }
    // Se asesina el proceso y se lo remueve del Input Stack
    if (peek(&inputStack) != -1)
    {
        sys_kill(peek(&inputStack));
        pop(&inputStack);
        forceTick();
        return;
    }
    // Se asesina el proceso y se lo remueve del Wait Stack, son casos excluyentes
    else if (peekWaitStack(&waitStack).cpid != -1)
    {
        PCB *curr = get(PCBTable, peekWaitStack(&waitStack).cpid);
        // El proceso puede estar en el Wait Stack debido a un sleep
        if (strcmp(curr->name, "sleep") == 0)
        {
            Process pro = peekWaitStack(&waitStack);
            sys_kill(pro.cpid); // mata hijo
            if (pro.pid != 4)
                sys_kill(pro.pid); // mata padre
            forceTick();
        }
        // Caso normal
        else
        {
            sys_kill(curr->PID);
        }
        return;
    }
    else
    {
        Iterator *it = dlcCreateIterator(&route);
        int iter = 0;
        list_t *aux;
        while (iter < dlcSize + 1)
        {
            aux = dlcNext(it);
            PCB *curr = get(PCBTable, aux->data);
            if (curr->FD[0] == 1)
            {
                unblock(curr->PPID);
                sys_kill(curr->PID);
                forceTick();
                return;
            }

            iter++;
        }
    }
}