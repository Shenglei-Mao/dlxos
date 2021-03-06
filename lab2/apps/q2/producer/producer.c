#include "lab2-api.h"
#include "usertraps.h"
#include "misc.h"

#include "spawn.h"

void main (int argc, char *argv[])
{
  cbuffer *cb;        
  uint32 h_mem;            
  sem_t s_procs_completed;
  lock_t lock;
  int i;
  char randomletter;
  


  char content[] = {'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd'};

  if (argc != 4) { 
    Printf("Usage: "); Printf(argv[0]); Printf(" <handle_to_shared_memory_page> <handle_to_page_mapped_semaphore> <handle to lock>\n"); 
    Exit();
  } 

  // Convert the command-line strings into integers for use as handles
  h_mem = dstrtol(argv[1], NULL, 10); // The "10" means base 10
  s_procs_completed = dstrtol(argv[2], NULL, 10);
  lock = dstrtol(argv[3], NULL, 10);

  // Map shared memory page into this process's memory space
  if ((cb = (cbuffer *)shmat(h_mem)) == NULL) {
    Printf("Could not map the virtual address to the memory in "); Printf(argv[0]); Printf(", exiting...\n");
    Exit();
  }

  // lock acquire
  // if (lock_acquire(lock) != SYNC_SUCCESS) {
  //   Printf("ERROR: lock acquire fail"); Printf(argv[0]); Printf(", exiting...\n");
  //   Exit();
  // }

  // add to buffer
  i = 0;
  while (i < 11) {
    // if buffer is full, wait without increament i
    // if (cb->start = cb->end - 1 || (cb->start == sizeof(cb->store) / sizeof(char) && cb->end == 0)) {
    if (lock_acquire(lock) != SYNC_SUCCESS) {
      Printf("ERROR: lock acquire fail"); Printf(argv[0]); Printf(", exiting...\n");
      Exit();
    }
    if (cb->start == cb->end - 1 || (cb->start == 7 && cb->end == 0)) {
      // Printf("cb-start %d\n", cb->start);
      // Printf("cb-end %d\n", cb->end);
        // lock release
      if (lock_release(lock) != SYNC_SUCCESS) {
        Printf("ERROR: lock release fail"); Printf(argv[0]); Printf(", exiting...\n");
        Exit();
      }
      continue;
    }
    // else produce an char and increment i
    else {
      if (lock_acquire(lock) != SYNC_SUCCESS) {
        Printf("ERROR: lock acquire fail"); Printf(argv[0]); Printf(", exiting...\n");
        Exit();
      }
      // char randomletter = 'A' + (random() % 26);
      randomletter = content[i];
      cb->store[cb->start++] = randomletter;
      cb->start = cb->start % 8;
      Printf("Producer %d inserted: %c\n", Getpid(), randomletter);
      if (lock_release(lock) != SYNC_SUCCESS) {
        Printf("ERROR: lock release fail"); Printf(argv[0]); Printf(", exiting...\n");
        Exit();
      }
      i = i + 1;
    }    
  }



 
  // Now print a message to show that everything worked
  // Printf("spawn_me: This is one of the %d instances you created.  ", mc->numprocs);
  // Printf("spawn_me: Missile code is: %c\n", mc->really_important_char);
  // Printf("spawn_me: My PID is %d\n", Getpid());

  // Signal the semaphore to tell the original process that we're done
  // Printf("spawn_me: PID %d is complete.\n", Getpid());
  if(sem_signal(s_procs_completed) != SYNC_SUCCESS) {
    Printf("Bad semaphore s_procs_completed (%d) in ", s_procs_completed); Printf(argv[0]); Printf(", exiting...\n");
    Exit();
  }
}