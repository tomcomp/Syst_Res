#include <stdio.h>
#include <unistd.h>

typedef struct HEADER_TAG {
    struct HEADER_TAG * ptr_next; /* pointe sur le prochain bloc libre */
    size_t bloc_size; /* taille du memory bloc en octets*/
    long magic_number; /* 0x0123456789ABCDEFL */
} HEADER;

HEADER* create_block(size_t size) {
    size_t total_size = sizeof(HEADER) + size + sizeof(long);
    HEADER* header = sbrk(total_size);
    if (header == (void*)-1) {
        return NULL;
    }

    header->ptr_next = NULL;
    header->bloc_size = size;
    header->magic_number = 0x0123456789ABCDEFL;
    long* footer = (long*) (((void*)header) + sizeof(HEADER) + size);
    *footer = 0x0123456789ABCDEFL;
    return header;
}

void insert_block(HEADER** head, HEADER* new_block) {
    new_block->ptr_next = *head;
    *head = new_block;
}

void print_list(HEADER* head) {
    HEADER* curr = head;
    while (curr) {
        printf("Block at %p, size: %zu, magic: %lx\n", (void*)curr, curr->bloc_size, curr->magic_number);
        curr = curr->ptr_next;
    }
}

void* malloc_3is(size_t size) {
    size_t total_size = sizeof(HEADER) + size + sizeof(long);
    HEADER* header = (HEADER*)sbrk(total_size);
    if (header == (void*)-1) {
        return NULL;
    }

    header->ptr_next = NULL;
    header->bloc_size = size;
    header->magic_number = 0x0123456789ABCDEFL;
    long* footer = (long*) (((void*)header) + sizeof(HEADER) + size);
    *footer = 0x0123456789ABCDEFL;
    return (void*)(header + 1);
}



int main() {

    HEADER* free_list = NULL;

    void* ptr1 = malloc_3is(1<<8);
    void* ptr2 = malloc_3is(1<<9);
    void* ptr3 = malloc_3is(1<<10);

    HEADER* block1 = (void*)ptr1 - sizeof(HEADER);
    HEADER* block2 = (void*)ptr2 - sizeof(HEADER);
    HEADER* block3 = (void*)ptr3 - sizeof(HEADER);

    printf("Allocated blocks:\n");
    printf("Block1 at %p, size: %zu, magic: %lx\n", (void*)block1, block1->bloc_size, block1->magic_number);
    printf("Block2 at %p, size: %zu, magic: %lx\n", (void*)block2, block2->bloc_size, block2->magic_number);
    printf("Block3 at %p, size: %zu, magic: %lx\n", (void*)block3, block3->bloc_size, block3->magic_number);

    insert_block(&free_list, block1);
    insert_block(&free_list, block2);

    printf("\nFree list after inserting block1 and block2:\n");
    print_list(free_list);

    return 0;
}