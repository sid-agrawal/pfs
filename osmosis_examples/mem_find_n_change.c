#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>

 #pragma GCC diagnostic ignored "-Wunused-result"
 #pragma GCC diagnostic ignored "-Wmaybe-uninitialized"

// ORIG_BUFFER will be placed in memory and will then be changed to NEW_BUFFER
// They must be the same length
#define ORIG_BUFFER "Hello, World!"
#define NEW_BUFFER "Hello, Linux!"

// The page frame shifted left by PAGE_SHIFT will give us the physcial address of the frame
// Note that this number is architecture dependent. For me on x86_64 with 4096 page sizes,
// it is defined as 12. If you're running something different, check the kernel source
// for what it is defined as.
#define PAGE_SHIFT 12
#define PAGEMAP_LENGTH 8

void* create_buffer(void);
unsigned long get_page_frame_number_of_address(void *addr);
int open_memory(void);
void seek_memory(int fd, unsigned long offset);


unsigned long get_pa(void * va) {

   unsigned long page_frame_number = get_page_frame_number_of_address(va);
   printf("Page frame: 0x%lx\n", page_frame_number);

   // Find the difference from the buffer to the page boundary
   unsigned long distance_from_page_boundary = (unsigned long)va % getpagesize();

   // Determine how far to seek into memory to find the buffer
   unsigned long offset = (page_frame_number << PAGE_SHIFT) + distance_from_page_boundary;
   return offset;
}
int main(void) {
   // Create a buffer with some data in it
   void *buffer = create_buffer();

   // Get pa
   unsigned long offset = get_pa(buffer);

   // Open /dev/mem, seek the calculated offset, and
   // map it into memory so we can manipulate it
   // CONFIG_STRICT_DEVMEM must be disabled for this
   int mem_fd = open_memory();
   seek_memory(mem_fd, offset);

   printf("Buffer: %s\n", (char *) buffer);
   printf("Changing buffer through /dev/mem... at offset/PA: 0x%lx\n",offset);

   // Change the contents of the buffer by writing into /dev/mem
   // Note that since the strings are the same length, there's no purpose in
   // copying the NUL terminator again
   if(write(mem_fd, NEW_BUFFER, strlen(NEW_BUFFER)) == -1) {
      fprintf(stderr, "Write failed: %s\n", strerror(errno));
   }

   fsync(mem_fd);
   while (1) {
           printf("Buffer[0x%p  ::  0x%lx] %s\n",buffer, get_pa(buffer), (char *)  buffer);
           sleep (4);
   }

   // Clean up
   free(buffer);
   close(mem_fd);

   return 0;
}

void* create_buffer(void) {
   size_t buf_size = strlen(ORIG_BUFFER) + 1;

   // Allocate some memory to manipulate
   void *buffer = malloc(buf_size);
   if(buffer == NULL) {
      fprintf(stderr, "Failed to allocate memory for buffer\n");
      exit(1);
   }

   // Lock the page in memory
   // Do this before writing data to the buffer so that any copy-on-write
   // mechanisms will give us our own page locked in memory
   if(mlock(buffer, buf_size) == -1) {
      fprintf(stderr, "Failed to lock page in memory: %s\n", strerror(errno));
      exit(1);
   }

   // Add some data to the memory
   strncpy(buffer, ORIG_BUFFER, strlen(ORIG_BUFFER) +1);

   return buffer;
}

unsigned long get_page_frame_number_of_address(void *addr) {
   // Open the pagemap file for the current process
   FILE *pagemap = fopen("/proc/self/pagemap", "rb");
   if (pagemap == NULL) {
      fprintf(stderr, "fopen to pagemap failed: %s\n", strerror(errno));
   }

   // Seek to the page that the buffer is on
   unsigned long offset = (unsigned long)addr / getpagesize() * PAGEMAP_LENGTH;
   if(fseek(pagemap, (unsigned long)offset, SEEK_SET) != 0) {
      fprintf(stderr, "Failed to seek pagemap to proper location\n");
      exit(1);
   }

   // The page frame number is in bits 0-54 so read the first 7 bytes and clear the 55th bit
   unsigned long page_frame_number = 0;
   int ret = fread(&page_frame_number, 1, PAGEMAP_LENGTH-1, pagemap);
   if (ret == -1) {
      fprintf(stderr, "fread to pagemap failed: %s\n", strerror(errno));
   }

   page_frame_number &= 0x7FFFFFFFFFFFFF;

   fclose(pagemap);

   return page_frame_number;
}

int open_memory(void) {
   // Open the memory (must be root for this)
   int fd = open("/dev/mem", O_RDWR);
   if(fd == -1) {
      fprintf(stderr, "Error opening /dev/mem: %s\n", strerror(errno));
      exit(1);
   }

   return fd;
}

void seek_memory(int fd, unsigned long offset) {
   off_t pos = lseek(fd, offset, SEEK_SET);

   if(pos == -1) {
      fprintf(stderr, "Failed to seek /dev/mem: %s\n", strerror(errno));
      exit(1);
   }
}
