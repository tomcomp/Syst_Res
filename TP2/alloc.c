#include <stdio.h>
#include <unistd.h>

typedef struct HEADER_TAG {
    struct HEADER_TAG * ptr_next; /* pointe sur le prochain bloc libre */
    size_t bloc_size; /* taille du memory bloc en octets*/
    long magic_number; /* 0x0123456789ABCDEFL */
} HEADER;

HEADER* init_list() {
    return NULL;
}

void create_block(HEADER* block, size_t size) {
    block->ptr_next = NULL;
    block->bloc_size = size;
    block->magic_number = 0x0123456789ABCDEFL;
}

void insert_block(HEADER** head, HEADER* new_block) {
    new_block->ptr_next = *head;
    *head = new_block;
}

HEADER* pop_block(HEADER** head, HEADER* block_to_pop) {
    HEADER* curr = *head;
    HEADER* prev = NULL;
    while (curr) {
        if (curr == block_to_pop) {
            if (prev) {
                prev->ptr_next = curr->ptr_next;
            } else {
                *head = curr->ptr_next;
            }
            return curr;
        }
        prev = curr;
        curr = curr->ptr_next;
    }
    return NULL;
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
    void* ptr1 = malloc_3is(1<<8);
    void* ptr2 = malloc_3is(1<<10);
    void* ptr3 = malloc_3is(1<<12);

    HEADER* block1 = (void*)ptr1 - sizeof(HEADER);
    HEADER* block2 = (void*)ptr2 - sizeof(HEADER);
    HEADER* block3 = (void*)ptr3 - sizeof(HEADER);

    printf("Block1 at %p, size: %zu, magic: %lx\n", (void*)block1, block1->bloc_size, block1->magic_number);
    printf("Block2 at %p, size: %zu, magic: %lx\n", (void*)block2, block2->bloc_size, block2->magic_number);
    printf("Block3 at %p, size: %zu, magic: %lx\n", (void*)block3, block3->bloc_size, block3->magic_number);

    return 0;
}