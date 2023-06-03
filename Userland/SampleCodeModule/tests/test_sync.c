#include <stdint.h>
#include "system_calls.h"
#include "tests.h"
#include "library.h"

#define SEM_ID "sem"

uint64_t global; // shared memory


void * processInc(int argc, char *argv[]) {
  uint64_t n;
  uint64_t process;

  if (argc != 2)
    return NULL;

  if ((n = satoi(argv[0])) <= 0)
    return NULL;
  if ((process = satoi(argv[1])) <= 0)
    return NULL;

  uint64_t i;
  for (i = 0; i < n; i++) {
      semWait(SEM_ID);
      printColor("wait ", 0xFF0000);
    
      global++;
      print("%d, %d ", global, process);
    
      printColor("post ", 0x00FF00);
      semPost(SEM_ID);
  }
  print("\n");

  print("Process %d finished\n", process);
  exit();
  return NULL;
}

uint64_t test_sync() { //{n, use_sem, 0}
  uint64_t pids[3];

  global = 0;
  semOpen(SEM_ID, 1);
  print("opening semaphore\n");
  
  uint64_t i;

  char *argv1[] = {"10", "1", NULL};
  char *argv2[] = {"10", "2", NULL};
  char *argv3[] = {"10", "3", NULL};
  
  pids[0] = createFGProcess("R1",processInc, 2, argv1);
  pids[1] = createFGProcess("R2",processInc, 2, argv2);
  pids[2] = createFGProcess("R3",processInc, 2, argv3);

  for (i = 0; i < 3; i++) {
    uint64_t primerpid = pids[i];
    waitPid(pids[i]);
    //kill(pids[i]);
    print("process %d brutally murdered\n", primerpid);
  }

  semClose(SEM_ID);
  print("closing semaphore\n");

  sleep(5);
  print("Final value: %d\n", global);
  return 0;
}