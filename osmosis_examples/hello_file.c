#include <stdio.h>
#include <stdlib.h>

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#define FILE_PATH "fusey-passthrough/README.md"

int main()
{

    int fd = open(FILE_PATH, O_RDONLY);
    if (fd == -1)
    {
        perror("Error opening file");
        fprintf(stderr, "Failed to open file: %s\n", FILE_PATH);
        return 1;
    }

    struct stat sb;
    if (fstat(fd, &sb) == -1)
    {
        perror("Error getting file size");
        close(fd);
        return 1;
    }

    char* file_content = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (file_content == MAP_FAILED)
    {
        perror("Error mapping file");
        close(fd);
        return 1;
    }

    for (off_t i = 0; i < sb.st_size; i++)
    {
        putchar(file_content[i]);
    }

    while (1)
        ;

    if (munmap(file_content, sb.st_size) == -1)
    {
        perror("Error unmapping file");
    }

    close(fd);
    return 0;
}