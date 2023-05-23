#ifndef TEST_H
#define TEST_H
#include <stdint.h>
#include "tests.h"
/****************************/

#define MAX_BLOCKS 128

uint64_t test_mm(uint64_t argc, char *argv[]);

/****************************/

uint32_t GetUint();
uint32_t GetUniform(uint32_t max);
uint8_t memcheck(void *start, uint8_t value, uint32_t size);
int64_t satoi(char *str);
void bussy_wait(uint64_t n);
void endless_loop();
//void endless_loop_print(uint64_t wait);
/****************************/







#endif