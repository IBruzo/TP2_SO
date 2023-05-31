#ifndef IRQDISPATCHER_H
#define IRQDISPATCHER_H

#include <stdarg.h>
#include <syscalls.h>
#include <stdint.h>
#include <keyboard_driver.h>
#include <sound_driver.h>
#include <video_driver.h>
#include <interrupts.h>
#include <time.h>
#include "scheduler.h"

#define SYS_GETCHAR_ID 0
#define SYS_WRITE_ID 1
#define SYS_TICK_ID 2
#define SYS_HALT_ID 3
#define SYS_PUTSQUARE_ID 4
#define SYS_GETTIME_ID 5
#define SYS_GETLASTKEY_ID 6
#define SYS_GETREGISTERS_ID 7
#define SYS_BEEP_ID 8
#define SYS_SLEEP_ID 9
#define SYS_CLEARKEYBUFFER_ID 10
#define SYS_CHANGELANGUAGE_ID 11
#define SYS_STOREREGISTERS_ID 12
#define SYS_MEMACCESS_ID 13
#define SYS_ALLOCMEM_ID 14
#define SYS_FREE_ID 15
#define SYS_CREATEPROCESS_ID 16
#define SYS_SCROLL_UP_ONCE_ID 17
#define SYS_GETPID_ID 18
#define SYS_INCREASEPRIORITY_ID 19
#define SYS_DECREASEPRIORITY_ID 20
#define SYS_YIELD_ID 21
#define SYS_KILL_ID 22
#define SYS_EXIT_ID 23
#define SYS_MEM_ID 24
void initialize();
char *getBuffer(); // esta quien es? (joaco)

#endif
