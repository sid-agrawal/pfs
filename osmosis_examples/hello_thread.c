#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

void* thread_function(void* arg) {
   printf("Hello from thread %ld\n", (long)arg);
   while(1);
   return NULL;
}

int main() {
    printf("PID: %d\n", getpid());
    pthread_t threads[5];
    for (long i = 0; i < 5; i++)
    {
        pthread_create(&threads[i], NULL, thread_function, (void*)i);
   }


   // Join threads
   for (int i = 0; i < 5; i++) {
      pthread_join(threads[i], NULL);
   }

   return 0;
}