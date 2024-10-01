#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>

int main(int argc, char **argv)
{
    if(argc != 2) {
        return 1;
    }
    int fd = atoi(argv[1]); // Get the FD to print to
    
    char buffer[64];
    int sz = snprintf(buffer, sizeof(buffer), "Child PID in child ns: %d\n", getpid());
    assert (sz > 0);

    write(fd, buffer, sizeof(buffer));

    // Idle until killed
    while (1)
    {
        sleep(10);
    }

    return 0;
}