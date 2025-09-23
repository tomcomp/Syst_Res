#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>

int data_var1 = 42;
char data_var2 = 'A';
double data_var3 = 3.14159;

int bss_var1 = 0;
char bss_var2;
double bss_var3 = 0.0;

char *str_const = "Hello World";

int main(void);

void run_pmap(const pid_t pid) {
    printf("\n=== PMAP Execution ===\n");
    char command[50];
    snprintf(command, sizeof(command), "pmap -X %d", pid);
    system(command);
}

void print_addresses(int stack_variable, int *heap_data, void* mmap_data) {
    printf("=== Memory Segments ===\n\n");

    printf("\nPID : %d\n", getpid());

    printf("DATA (Initialized globals data ) :\n");
    printf("data_var1 address (%d) : %p\n", data_var1, &data_var1);
    printf("data_var2 address (%c) : %p\n", data_var2, &data_var2);
    printf("data_var3 address (%f) : %p\n", data_var3, &data_var3);

    printf("\nBSS (Globals data initialized to 0) :\n");
    printf("bss_var1 address (%d) : %p\n", bss_var1, &bss_var1);
    printf("bss_var2 address (%d) : %p\n", bss_var2, &bss_var2);
    printf("bss_var3 address (%f) : %p\n", bss_var3, &bss_var3);

    printf("\nCHAR* :\n");
    printf("char* address (%s) : %p\n", str_const, &str_const);

    printf("\nHEAP :\n");
    printf("HEAP/malloc address : %p\n", heap_data);

    printf("\nSTACK :\n");
    printf("stack address (%d) : %p\n", stack_variable, &stack_variable);

    printf("\nMAIN FUNCTION :\n");
    printf("main function address : %p\n", main);

    printf("\nLIBS FUNCTION :\n");
    printf("printf address : %p\n", printf);

    printf("\nMMAP allocation :\n");
    printf("mmap address: %p\n", mmap_data);
}

int main(void) {
    const int stack_variable = 123;
    int *heap_data = malloc(1<<10);
    if (heap_data == NULL)
    {
        perror("malloc ERROR");
        exit(EXIT_FAILURE);
    }
    
    void* mmap_data = mmap(NULL, 1<<10, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE,-1,0);
    if (mmap_data == MAP_FAILED)
    {
        perror("mmap ERROR");
        exit(EXIT_FAILURE);
    }

    print_addresses(stack_variable,heap_data,mmap_data);

    run_pmap(getpid());

    free(heap_data);
    if (munmap(mmap_data, 1<<10) == -1)
    {
        perror("munmap ERROR");
    }

    return 0;

}
