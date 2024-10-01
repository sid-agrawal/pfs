#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wunused-result"

int main(int argc, char **argv)
{
    if(argc != 2) {
        return 1;
    }
    int fd = atoi(argv[1]); // Get the FD to print to
    
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "Child PID in child ns: %d\n", getpid());
    write(fd, buffer, sizeof(buffer));

    // Idle until killed
    while (1)
    {
        sleep(10);
    }

    return 0;
}
#pragma GCC diagnostic pop