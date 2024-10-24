#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>

#define PAGE_SIZE 4096
#define MMAP_SIZE PAGE_SIZE * 3
#define MMAP_PATH "shm_path"

int main() {
   // create some shared memory
   int shm_fd = shm_open(MMAP_PATH, O_RDWR | O_CREAT, 0644);
   assert(shm_fd > 0);

   // map it
   __attribute__((unused)) int fd = ftruncate(shm_fd, MMAP_SIZE); 

   size_t mmap_sz = PAGE_SIZE;
   off_t file_off = PAGE_SIZE;

   void *mem = mmap(NULL, mmap_sz, PROT_READ|PROT_WRITE, MAP_SHARED, shm_fd, file_off);
   assert(mem != MAP_FAILED);

   printf("Hello! mmap'ed [%p,%p] : SX : %lx\n", mem, (char*)mem + mmap_sz, mmap_sz);

   mmap_sz = MMAP_SIZE;
   file_off = 0;

   void *mem2 = mmap(NULL, mmap_sz, PROT_READ|PROT_WRITE, MAP_SHARED, shm_fd, file_off);
   assert(mem != MAP_FAILED);

   printf("Hello! mmap'ed [%p,%p] : SX : %lx\n", mem2, (char*)mem2 + mmap_sz, mmap_sz);

   int counter = 0;
   while (1) {
      counter = counter % 0x10000000;
      for (volatile int *i = mem; (char *) i < (char *) mem + PAGE_SIZE; i++) {
         *i |= counter;
         counter++;
      }
      for (volatile int *i = mem2; (char *) i < (char *) mem2 + MMAP_SIZE; i++) {
         *i |= counter;
         counter++;
      }
   }

   return 0;
}