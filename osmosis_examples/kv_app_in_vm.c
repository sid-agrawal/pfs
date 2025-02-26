#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "kv_inmemory.h"
#include <sys/mman.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <ctype.h>
#include <termios.h>
#include <sys/types.h>

#define FATAL do { fprintf(stderr, "Error at line %d, file %s (%d) [%s]\n", \
                __LINE__, __FILE__, errno, strerror(errno)); exit(1); } while(0)

#define MAP_SIZE 4096UL
#define MAP_MASK (MAP_SIZE - 1)

void atomic_store_int(int *obj, int val) {
    __atomic_store_n(obj, val, __ATOMIC_SEQ_CST);
}

int atomic_load_int(int *obj) {
    return __atomic_load_n(obj, __ATOMIC_SEQ_CST);
}


// App
void *generator(void *arg) {
    shared_buffer_t *shared_buffer = (shared_buffer_t *)arg;
    atomic_store_int(&shared_buffer->message_ready, 0);
    atomic_store_int(&shared_buffer->result_ready, 0);

    while (1) {
        shared_buffer->cmd = rand() % 2 == 0 ? GET : SET;
        if (shared_buffer->cmd == SET) {
            shared_buffer->key = rand() %10;
            shared_buffer->value = rand() %100;
        } else {
            shared_buffer->key = rand() %10;
        }
        atomic_store_int(&shared_buffer->message_ready, 1);

        while (!atomic_load_int(&shared_buffer->result_ready)) {
            // Busy-wait
        }
        if (shared_buffer->cmd == SET) {
            printf("SET: Success with key %d \n", shared_buffer->key);
        } else {
            if (shared_buffer->result != -1 ) {
                printf("GET: Success. Value : %d\n", shared_buffer->result);
            } else {
                printf("GET: Error : %d\n", shared_buffer->result);
            }
        }
        atomic_store_int(&shared_buffer->result_ready, 0);

        // Sleep for a random time between 1 and 2 seconds
        // sleep(1 + rand() % 2);
    }

    return NULL;
}

// Consumer
// #define KVS_VM_SHARED_PAGE_HOST_PA 0x5f600000
#define KVS_VM_SHARED_PAGE_HOST_PA 0x100000000

int main(int argc, char **argv) {
    void *map_base, *virt_addr;
    int fd;
    off_t target;

    if (argc < 2)
    {
        fprintf(
            stderr,
            "\nUsage:\t%s { Buffer PA }\n",
            argv[0]);
        exit(1);
    }
    target = strtoul(argv[1], 0, 0);


    
    /* Map Dev Mem at  0x5f600000 */
    if((fd = open("/dev/mem", O_RDWR | O_SYNC)) == -1) {
        FATAL;
    }
        
    printf("/dev/mem opened.\n"); 
    fflush(stdout);

    /* Map one page */
    size_t map_size = getpagesize();
    size_t map_mask = map_size - 1;
    map_base = mmap(0, map_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, target & ~map_mask);
    if(map_base == (void *) -1) {
            FATAL; 
    } else {
            printf("Memory mapped at address %p.\n", map_base); 
    }
    fflush(stdout);

    virt_addr = (char * )map_base + (target & MAP_MASK);

    generator(virt_addr);

    return 0;
}
