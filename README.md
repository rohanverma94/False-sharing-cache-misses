# False-sharing-cache-misses
This demonstrates the cache-misses introduced due to multi-core system

This implementation is based on multi-threading because the threads share same address space as compared to processes which have different address space.

Before going to discuss the contents of the repository, I have to discuss some supplement code and concepts:

```

```
This is a message manipulator based upon threads

```
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

char msg[100] = "uninitialized";

void *Child_Thread(void *tid) {
  printf("In child  thread at %p: '%s':\n", &msg, msg);
  sleep(2);
  printf("In child  thread at %p: '%s':\n", &msg, msg);
}

void Parent_Thread() {
  sleep(1);
  strcpy(msg, "I know threads, duhh aa");
  printf("In parent thread at %p: '%s':\n", &msg, msg);
}

int main() {
  pthread_t thr;

  pthread_create(&thr, NULL, Child_Thread, NULL); // Spawn thread

  Parent_Thread();

  pthread_exit(NULL);
}

```
