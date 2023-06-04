#include <lib.h>
#include <pipe.h>
#include <semaphores.h>

void printBuffer(int fd);

typedef struct pipe
{
    int pipePID;
    int sem[2];
    char buffer[MAX_PIPE_BUFFER_SIZE];
    size_t bufferSize;
    size_t readIndex;
    size_t writeIndex;
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
            char readEndName[MAX_PIPE_NAME_SIZE];
            strcpy(readEndName, name);
            strcat(readEndName, "R");
            uint64_t semRead = semOpen(readEndName, 0);
            if (semRead == -1)
            {
                return -1;
            }

            char writeEndName[MAX_PIPE_NAME_SIZE];
            strcpy(writeEndName, name);
            strcat(writeEndName, "W");
            uint64_t semWrite = semOpen(writeEndName, MAX_PIPE_BUFFER_SIZE);
            if (semWrite == -1)
            {
                semClose(semRead);
                return -1;
            }

            pipeTable[i].pipe.pipePID = i;
            pipeTable[i].pipe.sem[0] = semRead;
            pipeTable[i].pipe.sem[1] = semWrite;
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
            pipeTable[i].mutex = semOpen(name, 1);

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

    semWait(pipeTable[fd].mutex);

    if (fd == -1)
    {
        fd = createPipe(name);
        if (fd == -1)
        {
            semPost(pipeTable[fd].mutex);
            return -1;
        }
    }

    semPost(pipeTable[fd].mutex);
    return fd;
}

char *strdup(const char *str)
{
    if (str == NULL)
    {
        return NULL;
    }

    size_t len = strlen(str) + 1;
    char *copy = (char *)sys_mAlloc(len);

    if (copy != NULL)
    {
        memcpy(copy, str, len);
    }

    return copy;
}

int pipeClose(int fd)
{

    // printBuffer(fd);

    if (!isValidFd(fd))
    {
        return -1;
    }
    // debugging
    print("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
    semWait(pipeTable[fd].mutex);
    printList(PCBTable);

    char *lalala = strdup(getSemName(pipeTable[fd].pipe.sem[0]));
    print("\n\n --> ReadSem [%d] FD [%d] \n", pipeTable[fd].pipe.sem[0], fd);
    print(" --> ReadSemName : [%s]\n", lalala);

    semClose(lalala);

    char *lala = strdup(getSemName(pipeTable[fd].pipe.sem[1]));
    print("\n --> WriteSem [%d] FD [%d] \n", pipeTable[fd].pipe.sem[1], fd);
    print(" --> WriteSemName [%s]\n", lala);

    semClose(lala);
    print("\n--------->AAAAAAAAAAA<-------------\n");

    pipeTable[fd].used = 0;

    semPost(pipeTable[fd].mutex);

    // debugging
    printList(PCBTable);
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
    semWait(pipeTable[fd].pipe.sem[0]);

    while (i < count && rIndex != wIndex)
    {
        buf[i] = pipeTable[fd].pipe.buffer[rIndex];
        rIndex = (rIndex + 1) % MAX_PIPE_BUFFER_SIZE;
        i++;
        pipeTable[fd].pipe.bufferSize--;
    }

    pipeTable[fd].pipe.readIndex = rIndex;
    semPost(pipeTable[fd].pipe.sem[1]);

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
    semWait(pipeTable[fd].pipe.sem[1]);

    while (i < count && ((wIndex + 1) % MAX_PIPE_BUFFER_SIZE) != rIndex)
    {
        pipeTable[fd].pipe.buffer[wIndex] = buf[i];
        wIndex = (wIndex + 1) % MAX_PIPE_BUFFER_SIZE;
        i++;
        pipeTable[fd].pipe.bufferSize++;
    }

    pipeTable[fd].pipe.writeIndex = wIndex;
    semPost(pipeTable[fd].pipe.sem[0]);

    return i;
}

void printBuffer(int fd)
{
    if (!isValidFd(fd))
    {
        print("Invalid file descriptor.\n");
        return;
    }

    char buffer[MAX_PIPE_BUFFER_SIZE];
    int bytesRead = pipeRead(fd, buffer, MAX_PIPE_BUFFER_SIZE - 1);
    if (bytesRead == -1)
    {
        print("Error reading from the pipe.\n");
        return;
    }

    buffer[bytesRead] = '\0';
    print("Buffer contents: ");
    print(buffer);
    print("\n");
}
