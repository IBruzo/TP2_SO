#ifndef SYSTEM_CALLS_H
#define SYSTEM_CALLS_H

extern void write(char character, int x, int y, int fd, int color);
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
extern int createProcess(char *pname, void *(*f)(int, char **), int argc, char **argv, int *fd);
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
extern void waitPid(int pid);
extern int block(int pid);
extern int unblock(int pid);
extern int nice(int pid, int prio);
extern void changeInputFD(int pid, int newFD);
extern void changeOutputFD(int pid, int newFD);
extern int getInputFD(int pid);
extern int getOutputFD(int pid);
extern void ps(char *buffer);
extern void mem(char *buffer, int unit);
#endif
