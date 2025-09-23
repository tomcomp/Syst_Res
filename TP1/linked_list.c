#include <stdio.h>
#include <stdlib.h>

#define N 10

typedef struct Node {
    int data;
    struct Node* next;
} Node;

Node* create_node(int data) {
    Node* new_node = (Node*)malloc(sizeof(Node));
    if (!new_node) {
        perror("Failed to allocate memory");
        exit(EXIT_FAILURE);
    }
    new_node->data = data;
    new_node->next = NULL;
    return new_node;
}

Node* create_linked_list(int n) {
    if (n <= 0) return NULL;

    Node* head = create_node(1);
    Node* current = head;

    for (int i = 2; i <= n; i++) {
        current->next = create_node(i);
        current = current->next;
    }

    return head;
}
int linked_list_length(Node* head) {
    int length = 0;
    Node* current = head;

    while (current != NULL) {
        length++;
        current = current->next;
    }

    return length;
}

void print_linked_list(Node* head) {
    Node* current = head;
    while (current != NULL) {
        printf("<%p> %d\n",current, current->data);
        current = current->next;
    }
    printf("NULL\n");
}

Node pop_first(Node** head) {
    if (*head == NULL) {
        fprintf(stderr, "List is empty, cannot pop.\n");
        exit(EXIT_FAILURE);
    }
    Node popped_node = **head;
    Node* new_head = (*head)->next;
    free(*head);
    *head = new_head;
    return popped_node;
}

Node pop_last(Node** head) {
    if (*head == NULL) {
        fprintf(stderr, "List is empty, cannot pop.\n");
        exit(EXIT_FAILURE);
    }
    Node* current = *head;
    if (current->next == NULL) {
        Node popped_node = *current;
        free(current);
        *head = NULL;
        return popped_node;
    }
    while (current->next->next != NULL) {
        current = current->next;
    }
    Node popped_node = *(current->next);
    free(current->next);
    current->next = NULL;
    return popped_node;
}

void free_linked_list(Node* head) {
    Node* current = head;
    while (current != NULL) {
        Node* temp = current;
        current = current->next;
        free(temp);
    }
}

void push_to_end(Node** head, Node* new_node) {
    if (*head == NULL) {
        *head = new_node;
        return;
    }
    Node* current = *head;
    while (current->next != NULL) {
        current = current->next;
    }
    current->next = new_node;
}

Node* concat_lists(Node* list1, Node* list2) {
    if (list1 == NULL) return list2;
    Node* current = list1;
    while (current->next != NULL) {
        current = current->next;
    }
    current->next = list2;
    return list1;
} 



void push_to_beginning(Node** head, Node* new_node) {
    new_node->next = *head;
    *head = new_node;
}

int main() { 
    Node* list = create_linked_list(N);

    int length = linked_list_length(list);
    printf("Length of the linked list: %d\n", length);

    print_linked_list(list);

    Node first = pop_first(&list);
    printf("Popped first node: %d\n", first.data);
    print_linked_list(list);
    Node last = pop_last(&list);
    printf("Popped last node: %d\n", last.data);
    print_linked_list(list);

    Node* new_node_end = create_node(99);
    push_to_end(&list, new_node_end);
    printf("After pushing 99 to the end:\n");
    print_linked_list(list);

    Node* new_node_begin = create_node(0);
    push_to_beginning(&list, new_node_begin);
    printf("After pushing 0 to the beginning:\n");
    print_linked_list(list);

    Node* list2 = create_linked_list(5);
    printf("Second list:\n");
    print_linked_list(list2);
    Node* concatenated = concat_lists(list, list2);
    printf("After concatenation:\n");
    print_linked_list(concatenated);

    free_linked_list(list);
    free_linked_list(list2);
    free_linked_list(concatenated);
    return 0;
}