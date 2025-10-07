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

void* malloc_3is(size_t size, HEADER** list) {
    size_t total_size = sizeof(HEADER) + size + sizeof(long);
    HEADER* block = (HEADER*)sbrk(total_size);
    if (block == (void*)-1) return NULL;
    block->ptr_next = NULL;
    block->bloc_size = size;
    block->magic_number = 0x0123456789ABCDEFL;
    insert_block(list, block);
    return (void*)((char*)block + sizeof(HEADER));
}

int main() {
    HEADER* my_list = init_list();
    HEADER* block1 = malloc_3is(100, &my_list);
    HEADER* block2 = malloc_3is(200, &my_list);
    HEADER* block3 = malloc_3is(100, &my_list);
    HEADER* block4 = malloc_3is(300, &my_list);
    HEADER* block5 = malloc_3is(400, &my_list);
    print_list(my_list);


}