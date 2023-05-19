#include "bitMapADT.h" // import 2

uint8_t bitMap[BIT_MAP_SIZE];

// declarar un super array donde cada bit representa un bloque/pagina
// tenemos que hacer un inicializador del Memory Manager, donde se declare dicho  array
// una funcion que retorne un puntero a la memoria pedida
// una funcion que libere la memoria asignada

void initBitMap()
{
    memset(bitMap, 0, BIT_MAP_SIZE);
}

// funcion que switchea el valor del bit que se encuentra en position dentro del byte apuntado por byteDir
void switchBits(int posArr, int bitPos, int size)
{
    // ejemplo switchBits(8, 5, 14); se quiere switchear los 15 bits comenzando del 8-5 al 10-4
    char mask = 128 >> bitPos;                   // la mascara se setea en 0000 0100
    for (int i = bitPos; i < bitPos + size; i++) // i = 5 -> 19
    {
        int arrStep = i / 8; //  0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2
        //  ~ esta escrito? lo seteo en 0 : lo seteo en 1
        bitMap[posArr + arrStep] = (bitMap[posArr + arrStep] & mask) ? (bitMap[posArr + arrStep] & ~mask) : (bitMap[posArr + arrStep] | mask);
        // bitMap[posArr + arrStep] = (bitMap[posArr + arrStep] & mask) ? (bitMap[posArr + arrStep] & ~mask) : (bitMap[posArr + arrStep] ^ mask);
        mask = mask >> 1;
    }
    return;
}

// funcion que retorna la direccion donde comienza el bloque con el tamanio requerido
int findSpace(int cantPag, int *posArr, int *bitPos)
{
    int freeSpace = 0; // variable que acumula la cantidad de memoria que se encontro hasta el momento
    int bitMapPosition;
    char bitPosition;
    char mask = 128; // 1000 0000
    for (bitMapPosition = 0; bitMapPosition < BIT_MAP_SIZE; bitMapPosition++)
    {
        for (bitPosition = 0; bitPosition < 8; bitPosition++)
        {
            // bitMap[bitMapPosition] = 1100 0001
            // bitPosition 1000 0000 -> 0100 0000 -> 0010 0000 -> 0001 0000 -> 0000 1000 -> 0000 0100 -> 0000 0010 -> 0000 0001
            (bitMap[bitMapPosition] & mask) ? freeSpace = 0 : freeSpace++;
            if (freeSpace == cantPag)
            {
                *posArr = bitMapPosition;
                *bitPos = bitPosition;
                return 1;
            }
            mask = mask >> 1;
        }
    }
    return 0;
}

/**
 * @brief
 *
 * @param dir direccion donde esta
 * @param size es en bits
 */
void free(void *dir, int size)
{
  
    int dirMap = (((int) dir) - MEM_START) / PAG_SIZE; // base + 4k*(8*posArr + bitPoss) bitPos[ 0-7 ]
    int posArr = dirMap / 8;                      // se trunca
    int bitPos = dirMap % 8;                      // me da cosas del 0 al 7

    switchBits(posArr, bitPos, size); // gomensa
    return;
}

void * alloc(int size){

    int posArr=0;
    int bitPos=0;
    if(findSpace(size,&posArr,&bitPos)){
        switchBits(posArr, bitPos, size); 
        return MEM_START + PAG_SIZE*(8*posArr + bitPos);
    }

    return 0;
    
}