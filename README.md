# False-sharing-cache-misses
This demonstrates the cache-misses introduced due to multi-core system

This implementation is based on multi-threading because the threads share same address space as compared to processes which inherently have different address space.

Before going to discuss the contents of the repository, I have to discuss some supplement code and concepts:

So consider this example where we want to change the global variable  ```msg```  by using function ```ModifyMessage```. 

## Supplement Code

The supplement code works as follows:

* There are two function , namely ```ShowMessage()``` and ```ModifyMessage()``` which were to behave as per their name suggests.
* The ```ShowMessage()``` has ```sleep(2)```  , so the function ```ShowMessage()``` starts with ```printf``` then sleeps for 2 seconds.
* During this time the ```ModifyMessage()``` can jump in and modify the variable ```msg``` and return back the control to ```ShowMessage()```.
* Finally the other ```printf``` with modified message get excecuted and we are done.  

```
#include <stdio.h>
#include <unistd.h>

char msg[50] = "uninitialized";

void *ShowMessage() {
  printf("Address of msg is %p and msg:'%s'\n", &msg, msg);
  sleep(2);
  printf("A few seconds later msg = '%s'\n", msg);
}

void *ModifyMessage() {
  //This does work in theory, in reality let see further.
  strcpy(msg, "I'm a noob, duhh aa");
}

int main() {
 ShowMessage();
 ModifyMessage();
}
```
Output:
```
rohan@hackerspace-$ g++ Demo.cpp -o Manipulator  
rohan@hackerspace-$ ./Manipulator  
Address of msg is 0000000000403020 and msg:'uninitialized'  
A few seconds later msg = 'uninitialized'  
```
So this doesn't work in reality, the variable ```msg``` doen't get modified.

## Why supplement code doesn't work ? 

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

