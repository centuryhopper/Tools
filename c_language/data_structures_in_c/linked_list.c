#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#define null NULL

typedef struct Node
{
    void* data;
    struct Node* next;
} Node;


Node* createNode(void* data, size_t dataSize)
{
    Node* newNode = (Node*) malloc(sizeof(Node));
    if (newNode == null)
    {
        printf("malloc failed...\n");
        return null;
    }
    newNode->data = malloc(dataSize);
    newNode->next = null;
    // Copy contents of data to newly allocated memory.
    memcpy(newNode->data, data, dataSize);
    return newNode;
}

Node* headInsert(Node* head, void* data, size_t dataSize)
{
    Node* newNode = createNode(data, dataSize);
    if (newNode == null)
    {
        return null;
    }
    newNode->next = head;
    return newNode;
}

Node* tailInsert(Node* head, void* data, size_t dataSize)
{
    Node* newNode = createNode(data, dataSize);
    Node* originalHead = head;
    if (newNode == null)
    {
        return null;
    }
    if (head == null)
    {
        return newNode;
    }
    while (head->next != null)
    {
        head = head->next;
    }
    head->next = newNode;
    return originalHead;
}

Node* headRemove(Node* head)
{
    if (head == null)
    {
        return null;
    }

    Node* toDelete = head;
    Node* retVal = head->next;
    free(toDelete->data);
    free(toDelete);
    return retVal;
}

Node* tailRemove(Node* head)
{
    if (head == null)
    {
        return null;
    }

    Node* toDelete = head;
    Node* prev = head;
    while (toDelete->next != null)
    {
        prev = toDelete;
        toDelete = toDelete->next;
    }

    prev->next = null;
    free(toDelete->data);
    free(toDelete);
    return head;
}

/// remove the first instance of value in the linked list
Node* removeValue(Node* head, void* value, int (*cmp)(void*, void*))
{
    Node* tmp = head;
    Node* prev = null;
    while (tmp != null)
    {
        if (cmp(tmp->data, value) == 0)
        {
            // head contains value
            if (prev == null)
            {
                Node* retVal = tmp->next;
                free(tmp->data);
                free(tmp);
                return retVal;
            }
            prev->next = tmp->next;
            free(tmp->data);
            free(tmp);
            return head;
        }
        
        prev = tmp;
        tmp = tmp->next;
    }

    printf("value was not found.\n");
    return head;
}

void printList(Node* head, void (*print)(void*))
{
    while (head != null)
    {
        print(head->data);
        head = head->next;
    }
    printf("\n");
}

void printInt(void* value)
{
    printf("%d ", *((int*)value));
}

int compareInt(void* a, void* b) {
    return (*(int*)a - *(int*)b);
}

void cleanUp(Node* head)
{
    printf(head == null ? "head is null\n" : "head is not null\n");
    Node* tmp = head;
    while (tmp != null)
    {
        Node* toDelete = tmp;
        tmp = tmp->next;
        free(toDelete->data);
        free(toDelete);
    }
}

Node* sort()
{
    // use merge sort
    // recursively cut linked list in half and merge two sorted lists
}


int main(int argc, char const *argv[])
{
    // compile program
    // gcc -o linked_list linked_list.c
    // to make sure there are no memory leaks:
    // valgrind --leak-check=full ./linked_list
    unsigned i_size = sizeof(int);
    int intArray[10] = {40, 30, 20, 10, 12};
    Node* head = createNode(intArray+4, i_size);
    printList(head, printInt);

    head = headInsert(head, intArray, i_size);
    head = tailInsert(head, intArray+3, i_size);
    printList(head, printInt);

    head = headRemove(head);
    printList(head, printInt);

    head = tailRemove(head);
    printList(head, printInt);

    head = removeValue(head, intArray+4, compareInt);
    printList(head, printInt);

    cleanUp(head);
    return 0;
}

