#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>

#define MAGIC_NUMBER 0x0123456789ABCDEFL

int CORRUPTED_FREE = 0;

typedef struct HEADER_TAG {
    struct HEADER_TAG * ptr_next; /* pointe sur le prochain bloc libre */
    size_t bloc_size; /* taille du memory bloc en octets*/
    long magic_number; /* 0x0123456789ABCDEFL */
} HEADER;

HEADER* free_list = NULL;

void print_list(HEADER* head) {
    HEADER* curr = head;
    while (curr) {
        printf("Block at %p, size: %zu, magic: %lx\n", (void*)curr, curr->bloc_size, curr->magic_number);
        curr = curr->ptr_next;
    }
}

bool check_block(HEADER* block) {

    long* footer = (long*) (((void*)block) + sizeof(HEADER) + block->bloc_size);
    if (*footer != block->magic_number) {
        return false;
    }
    return true;
}

HEADER* find_fit(size_t size) {
    HEADER* curr = free_list;
    HEADER* prev = NULL;

    while (curr) {
        if (curr->bloc_size >= size && check_block(curr)) {
            if (prev) {
                prev->ptr_next = curr->ptr_next;
            } else {
                free_list = curr->ptr_next;
            }
            return curr;
        }
        prev = curr;
        curr = curr->ptr_next;
    }
    return NULL;
}

void* malloc_3is(size_t size) {
    HEADER* header = find_fit(size);
    if (header) {
        return (void*)(header + 1);
    }

    size_t total_size = sizeof(HEADER) + size + sizeof(long);
    header = (HEADER*)sbrk(total_size);
    if (header == (void*)-1) {
        return NULL;
    }
    header->ptr_next = NULL;
    header->bloc_size = size;
    header->magic_number = MAGIC_NUMBER;
    long* footer = (long*) (((void*)header) + sizeof(HEADER) + size);
    *footer = MAGIC_NUMBER;
    return (void*)(header + 1);
}

void free_3is(void* ptr) {
    if (!ptr) {
        return;
    }
    HEADER* block = ((HEADER*)ptr) - 1;
    if (check_block(block) == false) {
        CORRUPTED_FREE++;
        return;
    }

    block->ptr_next = free_list;
    free_list = block;
}


int main() {

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

    printf("===================================\n");
    free_3is(ptr1);
    memset(ptr2, 0xFF, 1<<10); // Simulate corruption
    free_3is(ptr2);

    printf("Free list after inserting block1 and block2:\n");
    print_list(free_list);

    printf("===================================\n");

    printf("CORRUPTED_FREE: %d\n", CORRUPTED_FREE);

    printf("===================================\n");

    void* ptr4 = malloc_3is(250);
    HEADER* block4 = (void*)ptr4 - sizeof(HEADER);
    printf("Allocated block4 at %p, size: %zu, magic: %lx\n", (void*)block4, block4->bloc_size, block4->magic_number);
    printf("Free list after allocating block4:\n");
    print_list(free_list);

    printf("===================================\n");

    return 0;
}