#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define FILE_NAME "test.txt"


void invert_data(void* mapped, size_t size){
    char *start = (char *)mapped;
    char *end = start + size - 1;
    while (start < end) {
        char temp = *start;
        *start = *end;
        *end = temp;
        start++;
        end--;
    }
}

int main() {
    int fd;
    struct stat file_stat;
    void *mapped;

    fd = open(FILE_NAME, O_RDWR);
    if (fd == -1) {
        perror("open ERROR");
        return EXIT_FAILURE;
    }

    if (fstat(fd, &file_stat) == -1) {
        perror("fstat ERROR");
        close(fd);
        return EXIT_FAILURE;
    }

    printf("File size: %ld bytes\n", file_stat.st_size);

    mapped = mmap(NULL, file_stat.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (mapped == MAP_FAILED) {
        perror("mmap ERROR");
        close(fd);
        return EXIT_FAILURE;
    }

    invert_data(mapped, file_stat.st_size);

    if (munmap(mapped, file_stat.st_size) == -1) {
        perror("unmap ERROR");
    }

    close(fd);

    printf("Les octets du fichier ont été inversés.\n");

    return EXIT_SUCCESS;
}