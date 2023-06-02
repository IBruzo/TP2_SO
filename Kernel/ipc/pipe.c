
#include <lib.h>
#include <pipe.h>
#include <semaphores.h>
// https://tldp.org/LDP/lpg/node11.html

typedef struct pipe
{
    int pipePID;                       // Process ID of the pipe
    int sem[2];                        // id of the semaphores
    char buffer[MAX_PIPE_BUFFER_SIZE]; // Data buffer
    size_t bufferSize;                 // Size of the buffer
    size_t readIndex;                  // Read index into the buffer
    size_t writeIndex;                 // Write index into the buffer
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
int createPipe(const char *name);
int pipeOpen(const char *name);
int pipeClose(int fd);
int pipeRead(int fd, char *buf, int count);
int pipeWrite(int fd, const char *buf);

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
{ // valido y en uso
    return fd >= 0 && fd < MAX_PIPES && pipeTable[fd].used;
}

uint64_t hashName(const char *str)
{
    uint64_t hash = 5381;
    int c;

    while ((c = *str++))
    {
        hash = ((hash << 5) + hash) + c; // djb2 algorithm
    }

    return hash;
}

int getPipeFd(const char *pipeName)
{
    for (int i = 0; i < MAX_PIPES; i++)
    {
        if (pipeTable[i].used && pipeTable[i].hashName == hashName(pipeName))
        {

            return i; // si encontro el pipe
        }
    }
    return -1; // si no encontro el pipe
}
int createPipe(const char *name)
{
    int len = pipeCheckName(name);
    if ((len == -1))
    {
        return -1;
    }
    if (getPipeFd(name) != -1)
    {
        return -1;
    }
    // Busco el primer lugar libre en la tabla de pipes
    for (int i = 0; i < MAX_PIPES; i++)
    {
        if (pipeTable[i].used == 0)
        {

            // Creo los semaforos de lectura y escritura
            char readEndName[MAX_PIPE_NAME_SIZE];
            strcpy(readEndName, name);
            strcat(readEndName, "R");
            uint64_t semRead = semOpen(readEndName, 0); // inicialmente nadie puede leer
            if (semRead == -1)
            {
                return -1;
            }
            char writeEndName[MAX_PIPE_NAME_SIZE];
            strcpy(writeEndName, name);
            strcat(readEndName, "W");
            uint64_t semWrite = semOpen(writeEndName, MAX_PIPE_BUFFER_SIZE);
            if (semWrite == -1)
            {
                return -1;
            }

            // pipe_t pipe;
            pipeTable[i].pipe.pipePID = i;
            pipeTable[i].pipe.sem[0] = semRead;
            pipeTable[i].pipe.sem[1] = semWrite;
            pipeTable[i].pipe.readIndex = 0;
            pipeTable[i].pipe.writeIndex = 0;
            for (size_t i = 0; i < MAX_BUFFER; i++)
            {
                pipeTable[i].pipe.buffer[i] = 0;
            }
            pipeTable[i].pipe.bufferSize = 0;

            // pipeEntry_t pipeEntry;
            strcpy(pipeTable[i].name, name);
            pipeTable[i].nameSize = len;
            pipeTable[i].hashName = hashName(name);
            pipeTable[i].used = 1;
            pipeTable[i].mutex = semOpen(name, 1);

            return i;
        }
    }
    return -1; // No available pipe slot
}

int pipeOpen(const char *name)
{
    int fd = getPipeFd(name);
    if (fd == -1 || pipeCheckName(name) == -1)
    {
        return -1;
    }
    semWait(pipeTable[fd].mutex);

    if (fd == -1)
    {                          // no existe tal pipe, lo creo
        fd = createPipe(name); // creo el pipe

        if (fd == -1)
        { // si no se pudo crear
            semPost(pipeTable[fd].mutex);
            return -1;
        }
    }
    semPost(pipeTable[fd].mutex);

    return fd;
}

int pipeClose(int fd)
{
    if (!isValidFd(fd))
    {
        return -1;
    }
    semWait(pipeTable[fd].mutex);

    // Cierro los semaforos de lectura y escritura
    semClose(getSemName(pipeTable[fd].pipe.sem[0]));
    semClose(getSemName(pipeTable[fd].pipe.sem[1]));

    // Libero el lugar en la tabla de pipes
    pipeTable[fd].used = 0;

    semPost(pipeTable[fd].mutex);
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
    semWait(pipeTable[fd].pipe.sem[0]);

    return i;
}
