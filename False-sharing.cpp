
#include <stdio.h>
#include <pthread.h>
#include <vector>
#include <stdarg.h>
#include <string.h>
//#include <sys/mman.h>
//#include <assert.h>
#include <stdint.h>
int result[10];
static inline uint64_t rdtsc()
{
    unsigned long a, d;
    asm volatile("cpuid;rdtsc"
                 : "=a"(a), "=d"(d)
                 :
                 : "ebx", "ecx");
    return a | ((uint64_t)d << 32);
}


static inline uint64_t rdtsc2() {
  uint64_t a, d;
  asm volatile("mfence");
#if defined(USE_RDTSCP)
  asm volatile("rdtscp" : "=a"(a), "=d"(d) :: "rcx");
#else
  asm volatile("rdtsc" : "=a"(a), "=d"(d));
#endif
  a = (d << 32) | a;
  asm volatile("mfence");
  return a;
}


inline void clflush(void *p)
{
    asm volatile("clflush (%0)" ::"r"(p));
}
 
void *worker(void *arg)
{
    (void)arg;
    /// for(;;){
    uint64_t start, end;
    start = rdtsc2();
    result[2]++;
    int b = result[3];
    end = rdtsc2();
    printf("%ld ticks\n", end - start);

    //}

    // usleep(100000);

    return NULL;
}

void *worker33(void * arg){
    uint64_t a;
    

}

class pool
{
    int p, q;
    std::vector<pthread_t> thread;
    std::vector<cpu_set_t> cpuset;

  public:
    pool(int num_threads...)
    {
        thread.reserve(num_threads);
        cpuset.reserve(num_threads);
        q = num_threads;
        va_list thread_pool_core;
        va_start(thread_pool_core, num_threads);
        //assert(CPU_SETSIZE >= num_threads);
        for (int i = 0; i < num_threads; ++i)
        {
            pthread_create(&thread[i], NULL, worker, NULL);
            //Set cpu affinity mark to all-zero
            CPU_ZERO(&cpuset[i]);
            //affine thread to cpu#q
            int core = va_arg(thread_pool_core, int);
            CPU_SET(core, &cpuset[i]);
        }
    }

    ~pool()
    {
        this->p = 0;
        this->q = 0;
    }

    void run()
    {
        //Get thread's cpu#
        //Apply the settings
        for (int i = 0; i < q; ++i)
        {
            p = pthread_setaffinity_np(thread[i], sizeof(cpu_set_t), &cpuset[i]);
            // p = pthread_getaffinity_np(thread[i], sizeof(cpu_set_t), &cpuset[i]);
            //for (int j = 0; j < CPU_SETSIZE; j++)
            // if (CPU_ISSET(j, &cpuset[i]))
            //   printf("    CPU %d\n", j);
            // clflush((char *)worker);
            // clflush((char *)worker + 64);
            // clflush((char *)worker + 128);
            //  clflush((char *)worker + 192);
            // clflush((char *)worker + 256);
            pthread_join(thread[i], NULL);
        }
    }
};

int main()
{

    memset(result, 1, sizeof(result));

    pool p(2, 1, 3,4);
    p.run();
}
