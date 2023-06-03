#ifndef SYSCALLS_H
#define SYSCALLS_H

#include <stdint.h>
#include <irqDispatcher.h>
#include <keyboard_driver.h>
#include <sound_driver.h>
#include <interrupts.h>
#include <lib.h>
#include <list.h>

#define STDOUT 1
#define STDRED 2
#define MAXBUFFER 500

char sys_getchar();
char sys_write(uint8_t character, uint32_t x, uint32_t y, uint32_t size, uint32_t color);
int sys_tick();
void sys_halt();
void sys_putSquare(uint32_t x, uint32_t y, uint32_t tam, uint32_t color);
int sys_getTime(int op);
char sys_getLastKey();
void sys_getRegisters(void);
void sys_beep(int freq, int time);
void sys_sleep(int seconds);
void sys_clearkeybuffer();
void sys_changeLanguage(int lan);
void sys_storeRegisters(void);
void sys_memAccess(uint64_t memDir);
char *sys_mem();
void *sys_allocMem(int bytes);
void scroll_up_once(uint32_t tamY, uint32_t color);
void sys_free(void *dir);
int sys_createProcess(void *(*f)(int, char **), int argc, char **argv, int *fd);
int sys_getPid();
int sys_increasePriority(int PID);
int sys_decreasePriority(int PID);
void sys_yield();
int sys_kill(int pid);
void sys_exit();
int sys_semCreate(char *name, int initValue);
int sys_semOpen(char *name, int initValue);
int sys_semClose(char *name);
int sys_semWait(char *name);
int sys_semPost(char *name);
void sys_waitPid(int pid);
int sys_block(int pid);
int sys_unblock(int pid);
int sys_nice(int pid, int prio);
void sys_changeInputFD(int pid, int newFD);
void sys_changeOutputFD(int pid, int newFD);
int sys_getInputFD(int pid);
int sys_getOutputFD(int pid);
int sys_openPipe(char *name);
int sys_closePipe(int fd);

int getCurrentPid(); //>.>

#endif
