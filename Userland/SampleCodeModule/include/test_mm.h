#ifndef TEST_MM_H_
#define TEST_MM_H_

#include "syscall.h"
#include "system_calls.h"
#include "test_util.h"
#include <library.h>


#define MAX_BLOCKS 128

uint64_t test_mm(uint64_t argc, char *argv[]);

#endif