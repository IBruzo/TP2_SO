#ifndef SYSTEM_CALLS_H
#define SYSTEM_CALLS_H

extern void write(char character, int x, int y, int size, int color);
extern char getchar();
extern int gettick();
extern void halt();
extern void putSquare(int x, int y, int tam, int color);
extern int getTime(int code);
extern char getLastKey();
extern void getRegisters();
extern void beep(int freq, int time);
extern void sleep(int seconds);
extern void clearkeybuffer();
extern void changelanguage(int lan);
extern void storeRegisters();
extern void memAccess(unsigned long int memDir);
extern void *mAlloc(unsigned long int bytes);
extern void mFree(void *dir);
extern int createProcess(void *(*f)(int, char **), int argc, char **argv);
extern void scrollUp(int tamY, int color);
extern int getPid();
extern int increasePriority(int pid);
extern int decreasePriority(int pid);
extern void yield();
extern int kill(int pid);
extern void exit();
extern int semCreate(char *name, int initValue);
extern int semOpen(char *name, int initValue);
extern int semClose(char *name);
extern int semWait(char *name);
extern int semPost(char *name);
extern char * mem(unit);

#endif
