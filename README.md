# False-sharing-cache-misses
This demonstrates the cache-misses introduced due to multi-core system

This implementation is based on multi-threading because the threads share same address space as compared to processes which inherently have different address space.

Before going to discuss the contents of the repository, I have to discuss some supplement code and concepts:

So consider this example where we want to change the global variable  ```msg```  by using function ```ModifyMessage```. 

## Supplement Code #1

The supplement code works as follows:

* There are two function , namely ```ShowMessage()``` and ```ModifyMessage()``` which were to behave as per their name suggests.
* The ```ShowMessage()``` has ```sleep(2)```  , so the function ```ShowMessage()``` starts with ```printf``` then sleeps for 2 seconds.
* During this time the ```ModifyMessage()``` can jump in and modify the variable ```msg``` and return back the control to ```ShowMessage()```.
* Finally the other ```printf``` with modified message get excecuted and we are done.

#### Code :
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
  strcpy(msg, "I'm a noob, live with it.");
}

int main() {
 ShowMessage();
 ModifyMessage();
}
```
### Output:
```
rohan@hackerspace-$ g++ Demo1.cpp -o Manipulator  
rohan@hackerspace-$ ./Manipulator  
Address of msg is 0000000000403020 and msg:'uninitialized'  
A few seconds later msg = 'uninitialized'  
```
So this doesn't work in reality, the variable ```msg``` doen't get modified.

## Why supplement code #1 doesn't work ? 

* The function ```ShowMessage()```  has ```sleep(2)```  which basicaly makes entire conrol of the code sleep for 2 seconds.

* The main function has ```ShowMessage()``` & ```ModifyMessage()``` arranged serially such that the function  ```ModifyMessage()``` only executes when function ```ShowMessage()``` has done executing.

* Because of the above reason, the ```msg``` won't get modified before it reaches the last ```printf``` in function ```ShowMessage()```.

## Supplement Code #2 ( This works )

* This time, we will make a thread for function ```ShowMessage()```, for this code we will call this function as ```void *Child_Thread(void *tid)``` , where ```tid``` is Thread ID.
* Because threads share a common address space, the threads ChildThread and ParentThread are oblivious to each others share of memory.
* Threads are used to share the CPU time of application by leveraging the use of modern hardware i.e the threads can execute concurrently on single/multiple CPUs.
* Because the threads can run in parallel, the function ```ParentThread``` executes concurrently with thread ```ChildThread```.

#### Code :

```
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

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

### Output:
```
rohan@hackerspace-$ g++ Demo2.cpp -lpthread
rohan@hackerspace-$  ./a.out
In child  thread at 0x601080: 'uninitialized':
In parent thread at 0x601080: 'I know threads, duhh aa':
In child  thread at 0x601080: 'I know threads, duhh aa':
```

## Insights 

Given that the threads share memory or they are oblivious to each others share of memory.
