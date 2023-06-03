
#ifndef _PIPE_H_
#define _PIPE_H_

#define MAX_PIPES 256
#define MAX_PIPE_BUFFER_SIZE 4096
#define MAX_PIPE_NAME_SIZE 256
#define BUFFER_SIZE 4096

void initPipes();
int createPipe(char *name);
int pipeOpen(char *name);
int pipeClose(int fd);
int pipeRead(int fd, char *buf, int count);
int pipeWrite(int fd, const char *buf);

#endif