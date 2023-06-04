// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <lib.h>
#include <pipe.h>
#include <semaphores.h>

typedef struct pipe
{
    int pipePID;
    char buffer[MAX_PIPE_BUFFER_SIZE];
    int sem[2];
    size_t bufferSize;
    size_t readIndex;
    size_t writeIndex;
    int readSem;  // Semaphore for read synchronization
    int writeSem; // Semaphore for write synchronization
} pipe_t;

typedef struct
{
    pipe_t pipe;
    char name[MAX_PIPE_NAME_SIZE];
    size_t nameSize;
    uint64_t hashName;
    int used;
    int mutex;
} pipeEntry_t;

pipeEntry_t pipeTable[MAX_PIPES];

void initPipes();
int pipeCheckName(const char *name);
int isValidFd(int fd);
uint64_t hashName(const char *str);
int getPipeFd(const char *pipeName);
int createPipe(char *name);
int pipeOpen(char *name);
int pipeClose(int fd);
int pipeRead(int fd, char *buf, int count);
int pipeWrite(int fd, const char *buf);

int createPipe(char *name)
{
    int len = pipeCheckName(name);
    if (len == -1)
    {
        return -1;
    }
    if (getPipeFd(name) != -1)
    {
        return -1;
    }

    for (int i = 2; i < MAX_PIPES; i++)
    {
        if (pipeTable[i].used == 0)
        {

            pipeTable[i].pipe.pipePID = i;
            pipeTable[i].pipe.readIndex = 0;
            pipeTable[i].pipe.writeIndex = 0;
            for (size_t j = 0; j < MAX_PIPE_BUFFER_SIZE; j++)
            {
                pipeTable[i].pipe.buffer[j] = 0;
            }
            pipeTable[i].pipe.bufferSize = 0;

            strcpy(pipeTable[i].name, name);
            pipeTable[i].nameSize = len;
            pipeTable[i].hashName = hashName(name);
            pipeTable[i].used = 1;

            return i;
        }
    }

    return -1;
}

int pipeOpen(char *name)
{
    int fd = getPipeFd(name);
    if (pipeCheckName(name) == -1)
    {
        return -1;
    }

    if (fd == -1)
    {                          // no existe tal pipe, lo creo
        fd = createPipe(name); // creo el pipe

        if (fd == -1)
        { // si no se pudo crear
            // semPost(pipeTable[fd].mutex);
            return -1;
        }
    }

    return fd;
}

int pipeClose(int fd)
{

    if (!isValidFd(fd))
    {
        return -1;
    }

    pipeTable[fd].used = 0;

    return 0;
}

int pipeRead(int fd, char *buf, int count)
{
    if (!isValidFd(fd))
    {
        return -1;
    }
    if (count <= 0)
    {
        return -1;
    }
    if (buf == NULL)
    {
        return -1;
    }

    int rIndex = pipeTable[fd].pipe.readIndex;
    int wIndex = pipeTable[fd].pipe.writeIndex;
    int i = 0;

    while (i < count && rIndex != wIndex)
    {
        buf[i] = pipeTable[fd].pipe.buffer[rIndex];
        rIndex = (rIndex + 1) % MAX_PIPE_BUFFER_SIZE;
        i++;
        pipeTable[fd].pipe.bufferSize--;
    }

    pipeTable[fd].pipe.readIndex = rIndex;

    return i;
}

int pipeWrite(int fd, const char *buf)
{
    if (!isValidFd(fd))
    {
        return -1;
    }
    if (buf == NULL)
    {
        return -1;
    }
    int count = strlen(buf);
    int rIndex = pipeTable[fd].pipe.readIndex;
    int wIndex = pipeTable[fd].pipe.writeIndex;
    int i = 0;

    while (i < count && ((wIndex + 1) % MAX_PIPE_BUFFER_SIZE) != rIndex)
    {
        pipeTable[fd].pipe.buffer[wIndex] = buf[i];
        wIndex = (wIndex + 1) % MAX_PIPE_BUFFER_SIZE;
        i++;
        pipeTable[fd].pipe.bufferSize++;
    }

    pipeTable[fd].pipe.writeIndex = wIndex;

    return i;
}

void initPipes()
{
    for (int i = 0; i < MAX_PIPES; i++)
    {
        pipeTable[i].used = 0;
    }
}

int pipeCheckName(const char *name)
{
    int len = strlen(name);
    if (len >= MAX_PIPE_NAME_SIZE || len <= 0)
    {
        return -1;
    }
    return len;
}

int isValidFd(int fd)
{
    return fd >= 2 && fd < MAX_PIPES && pipeTable[fd].used;
}

uint64_t hashName(const char *str)
{
    uint64_t hash = 5381;
    int c;

    while ((c = *str++))
    {
        hash = ((hash << 5) + hash) + c;
    }

    return hash;
}

int getPipeFd(const char *pipeName)
{
    for (int i = 2; i < MAX_PIPES; i++)
    {
        if (pipeTable[i].used && pipeTable[i].hashName == hashName(pipeName))
        {
            return i;
        }
    }
    return -1;
}