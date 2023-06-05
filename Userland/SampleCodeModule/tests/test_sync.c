#include <stdint.h>
#include "system_calls.h"
#include "tests.h"
#include "library.h"

#define SEM_ID "sem"
#define TOTAL_PAIR_PROCESSES 2

int64_t global; // shared memory
char namBuf[][3] = {"R1", "R2", "R3", "R4"};

void *slowInc(int64_t *p, int64_t inc)
{
  uint64_t aux = *p;
  print("| %d + (%d) = ", aux, inc);
  yield(); // This makes the race condition highly probable
  aux += inc;
  *p = aux;
  print("%d |\n", *p);
  return NULL;
}

void *my_process_inc(int argc, char *argv[])
{
  uint64_t n;
  int8_t inc;
  int8_t use_sem;

  if (argc != 3)
    return NULL;

  if ((n = satoi(argv[0])) <= 0)
    return NULL;
  if ((inc = satoi(argv[1])) == 0)
    return NULL;
  if ((use_sem = satoi(argv[2])) < 0)
    return NULL;

  if (use_sem)
    if (semOpen(SEM_ID, 1) == -1)
    {
      print("test_sync: ERROR opening semaphore\n");
      return NULL;
    }

  uint64_t i;
  for (i = 0; i < n; i++)
  {
    if (use_sem)
      semWait(SEM_ID);
    slowInc(&global, inc);
    if (use_sem)
      semPost(SEM_ID);
  }

  if (use_sem)
    semClose(SEM_ID);
  exit();

  return NULL;
}

void *my_process_inc_no_sem(int argc, char *argv[])
{
  uint64_t n;
  int8_t inc;
  int8_t use_sem;

  if (argc != 3)
    return NULL;

  if ((n = satoi(argv[0])) <= 0)
    return NULL;
  if ((inc = satoi(argv[1])) == 0)
    return NULL;
  if ((use_sem = satoi(argv[2])) < 0)
    return NULL;

  uint64_t i;
  for (i = 0; i < n; i++)
  {
    slowInc(&global, inc);
  }

  exit();

  return NULL;
}

void *test_sync(int argc, char *argv[])
{ //{n, use_sem, 0}

  uint64_t pids[2 * TOTAL_PAIR_PROCESSES];

  if (argc != 3)
    return NULL;

  char *argvDec[] = {argv[0], "-1", argv[1], NULL};
  char *argvInc[] = {argv[0], "1", argv[1], NULL};

  global = 0;
  uint64_t i;

  if (strcmp(argv[2], "0") == 0)
  {
    for (i = 0; i < TOTAL_PAIR_PROCESSES; i++)
    {
      pids[i] = createFGProcess(namBuf[i], my_process_inc, 3, argvDec);
      pids[i + TOTAL_PAIR_PROCESSES] = createFGProcess(namBuf[i], my_process_inc, 3, argvInc);
    }

    for (i = 0; i < TOTAL_PAIR_PROCESSES; i++)
    {
      waitPid(pids[i]);
      waitPid(pids[i + TOTAL_PAIR_PROCESSES]);
    }
  }
  else
  {
    for (i = 0; i < TOTAL_PAIR_PROCESSES; i++)
    {
      pids[i] = createFGProcess(namBuf[i], my_process_inc_no_sem, 3, argvDec);
      pids[i + TOTAL_PAIR_PROCESSES] = createFGProcess(namBuf[i], my_process_inc_no_sem, 3, argvInc);
    }

    for (i = 0; i < TOTAL_PAIR_PROCESSES; i++)
    {
      waitPid(pids[i]);
      waitPid(pids[i + TOTAL_PAIR_PROCESSES]);
    }
  }

  print("Final value: %d\n", global);
  exit();
  return NULL;
}
