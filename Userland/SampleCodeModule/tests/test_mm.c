#include "tests.h"


uint64_t test_mm(uint64_t argc, char *argv[]);

typedef struct MM_rq
{
  void *address;
  uint32_t size;
} mm_rq;

void * memsett(void * destination, int32_t c, uint64_t length);

uint64_t test_mm(uint64_t argc, char *argv[])
{

  mm_rq mm_rqs[MAX_BLOCKS];
  uint8_t rq;
  uint32_t total;
  uint64_t max_memory;

  if (argc != 1)
    return -1;

  if ((max_memory = satoi(argv[0])) <= 0)
    return -1;

  while (1)
  {
    rq = 0;
    total = 0;
    // Request as many blocks as we can
    while (rq < MAX_BLOCKS && total < max_memory)
    {
      mm_rqs[rq].size = (GetUniform(max_memory - total - 1) + 1);
      mm_rqs[rq].address = allocMem(mm_rqs[rq].size);
      
    /*  print("Block Adress: %x\n", mm_rqs[rq].address);
      print("Block Size: %d\n", mm_rqs[rq].size); */
     // print("cant pags: %d\n",(mm_rqs[rq].size*1048576 + 4096-1 )/4096);
      if (mm_rqs[rq].address)
      {
        total += mm_rqs[rq].size;
        rq++;
      }
    }

    // Set
    uint32_t i;
    char *aux;
    for (i = 0; i < rq; i++)
    {
      if (mm_rqs[i].address)
      print("%d)\n", i);
       aux = memsett(mm_rqs[i].address, i, mm_rqs[i].size);
      print("%x--->%d--- size:%d\n", mm_rqs[i].address, *((char *)(mm_rqs[i].address)), mm_rqs[i].size);
    }

    // Check
    for (i = 0; i < rq; i++)
      if (mm_rqs[i].address)
        if (!memcheck(mm_rqs[i].address, i, mm_rqs[i].size))
        {
          print("test_mm ERROR\n");
          return -1;
        }

    // Free
    for (i = 0; i < rq; i++)
      if (mm_rqs[i].address)
        freeBits(mm_rqs[i].address, mm_rqs[i].size);
  }
}


void * memsett(void * destination, int32_t c, uint64_t length)
{
	uint8_t chr = (uint8_t)c;
	char * dst = (char*)destination;

	while(length--)
		dst[length] = chr;

	return destination;
}