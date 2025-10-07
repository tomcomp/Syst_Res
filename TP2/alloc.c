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

HEADER* split_block(HEADER* block, size_t size) {
    size_t remaining_size = block->bloc_size - size - sizeof(HEADER) - sizeof(long);
    if (remaining_size < sizeof(HEADER) + sizeof(long)) {
        return NULL;
    }

    HEADER* new_block = (HEADER*) (((void*)block) + sizeof(HEADER) + size + sizeof(long));
    new_block->ptr_next = block->ptr_next;
    new_block->bloc_size = remaining_size;
    new_block->magic_number = MAGIC_NUMBER;
    long* new_footer = (long*) (((void*)new_block) + sizeof(HEADER) + remaining_size);
    *new_footer = MAGIC_NUMBER;

    block->bloc_size = size;
    long* footer = (long*) (((void*)block) + sizeof(HEADER) + size);
    *footer = MAGIC_NUMBER;

    return new_block;
}

HEADER* find_fit(size_t size) {
    HEADER* curr = free_list;
    HEADER* prev = NULL;

    while (curr) {
        if (curr->bloc_size == size && check_block(curr)) {
            if (prev) {
                prev->ptr_next = curr->ptr_next;
            } else {
                free_list = curr->ptr_next;
            }
            return curr;
        }

        if (curr->bloc_size > size && check_block(curr)) {
            HEADER* new_block = split_block(curr, size);
            if (new_block) {
                if (prev) {
                    prev->ptr_next = new_block;
                } else {
                    free_list = new_block;
                }
            } else {
                if (prev) {
                    prev->ptr_next = curr->ptr_next;
                } else {
                    free_list = curr->ptr_next;
                }
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
        header->ptr_next = NULL;
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

    HEADER* curr = free_list;
    HEADER* prev = NULL;
    while (curr && curr->bloc_size < block->bloc_size) {
        prev = curr;
        curr = curr->ptr_next;
    }
    block->ptr_next = curr;
    if (prev) {
        prev->ptr_next = block;
    } else {
        free_list = block;
    }
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
    void* ptr5 = malloc_3is(256);
    HEADER* block4 = (void*)ptr4 - sizeof(HEADER);
    HEADER* block5 = (void*)ptr5 - sizeof(HEADER);
    printf("Allocated block4 at %p, size: %zu, magic: %lx\n", (void*)block4, block4->bloc_size, block4->magic_number);
    printf("Allocated block5 at %p, size: %zu, magic: %lx\n", (void*)block5, block5->bloc_size, block5->magic_number);

    free_3is(ptr4);
    printf("Free list after allocating block4 and block5 and then block4 freed:\n");
    print_list(free_list);

    printf("===================================\n");

    return 0;
}