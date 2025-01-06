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

typedef struct LinkedList
{
    Node* head;
    int count;
}LinkedList;

void getLinkedListPart(Node* head, LinkedList* a, LinkedList* b);
Node* mergeTwoSortedList(Node* listA, Node* listB, int (*cmp)(void*, void*));


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
        // printf("%d\n", *((int*) data));
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
    // printf(head == null ? "head is null\n" : "head is not null\n");
    Node* tmp = head;
    while (tmp != null)
    {
        Node* toDelete = tmp;
        tmp = tmp->next;
        free(toDelete->data);
        free(toDelete);
    }
}

/// uses merge sort algorithm
/// recursively cut linked list in half and merge two sorted lists
void mergeSortLinkedList(Node** head, int (*cmp)(void*, void*))
{
    // base case (It is vacuously true that a null head or single node head is already sorted)
    if ((*head) == null || (*head)->next == null)
    {
        return;
    }

    LinkedList* a = (LinkedList*) malloc(sizeof(LinkedList));
    a->head = null;
    a->count = 0;
    LinkedList* b = (LinkedList*) malloc(sizeof(LinkedList));
    b->head = null;
    b->count = 0;
    getLinkedListPart(*head, a, b);
    // printList(a->head, printInt);
    // printList(b->head, printInt);
    mergeSortLinkedList(&a->head, cmp);
    mergeSortLinkedList(&b->head, cmp);

    *head = mergeTwoSortedList(a->head, b->head, cmp);
    // printList(*head, printInt);
    free(a);
    free(b);
}

/// divide the linked list into two roughly equal parts (roughly because it wont be equal if the linked list has an odd number of nodes)
void getLinkedListPart(Node* head, LinkedList* a, LinkedList* b)
{
    // printf("%d\n", (*(int*) head->data));
    Node* slow = head, *fast = head, *mid = head;
    while (fast != null && fast->next != null)
    {
        mid = slow;
        slow = slow->next;
        fast = fast->next->next;
    }

    // cut in half
    mid->next = null;

    // first half
    a->head = head;

    // printf("%d\n", (*(int*) a->head->data));


    // second half
    b->head = slow;

    // printf("%d\n", (*(int*) slow->data));


    // printList(a->head, printInt);
    // printList(b->head, printInt);

}

/// assumes the two linked lists are sorted
Node* mergeTwoSortedList(Node* listA, Node* listB, int (*cmp)(void*, void*))
{
    int dummyVal = -1;
    Node* newHead = createNode(&dummyVal, sizeof(int));
    Node* aux = newHead;
    while (listA != null && listB != null)
    {
        // printf("listA data: %d\n", *((int*) listA->data));
        // printf("listB data: %d\n", *((int*) listB->data));
        if (cmp(listA->data, listB->data) < 0)
        {
            aux->next = listA;
            listA = listA->next;
        }
        // we include the case where they're equal here since it doesnt really matter which pointer advances
        else if (cmp(listA->data, listB->data) >= 0)
        {
            aux->next = listB;
            listB = listB->next;
        }

        aux = aux->next;
    }

    // include the rest of the list if it exists
    if (listA != null)
    {
        aux->next = listA;
    }

    if (listB != null)
    {
        aux->next = listB;
    }

    Node* toDelete = newHead;
    newHead = newHead->next;
    free(toDelete->data);
    free(toDelete);

    return newHead;
}


int main(int argc, char const *argv[])
{
    // compile program
    // gcc -o linked_list linked_list.c
    // to make sure there are no memory leaks:
    // valgrind --leak-check=full ./linked_list
    unsigned i_size = sizeof(int);
    int intArray[10] = {10, 20, 30, 40, 50, 60, 70, 80, 90, 100};

    // Node* head = createNode(intArray, i_size);
    // head = tailInsert(head, intArray+1, i_size);
    // printList(head, printInt);

    // Node* head2 = null;
    // head2 = tailInsert(head2, intArray+2, i_size);
    // head2 = tailInsert(head2, intArray+3, i_size);
    // head2 = tailInsert(head2, intArray+4, i_size);
    // printList(head2, printInt);

    // Node* merged = mergeTwoSortedList(head, head2, compareInt);
    // printList(merged, printInt);

    // LinkedList first, second;
    // getLinkedListPart(merged, &first, &second);


    // printList(first.head, printInt);
    // printList(second.head, printInt);



    // Creating a longer unsorted linked list
    Node* testSortList = createNode(intArray+6, i_size);  // 70
    testSortList = tailInsert(testSortList, intArray+1, i_size);  // 20
    testSortList = tailInsert(testSortList, intArray+8, i_size);  // 90
    testSortList = tailInsert(testSortList, intArray+3, i_size);  // 40
    testSortList = tailInsert(testSortList, intArray+5, i_size);  // 60
    testSortList = tailInsert(testSortList, intArray, i_size);    // 10
    testSortList = tailInsert(testSortList, intArray+9, i_size);  // 100
    testSortList = tailInsert(testSortList, intArray+2, i_size);  // 30
    testSortList = tailInsert(testSortList, intArray+7, i_size);  // 80
    testSortList = tailInsert(testSortList, intArray+4, i_size);  // 50

     // Print the unsorted list
    printf("Unsorted List:\n");
    printList(testSortList, printInt);

    // Perform merge sort
    mergeSortLinkedList(&testSortList, compareInt);

    // Print the sorted list
    printf("Sorted List:\n");
    printList(testSortList, printInt);

    // Clean up the list to avoid memory leaks
    cleanUp(testSortList);


    // head = headRemove(head);
    // printList(head, printInt);

    // head = tailRemove(head);
    // printList(head, printInt);

    // head = removeValue(head, intArray+4, compareInt);
    // printList(head, printInt);

    // no need clean up head and head2 since they were merged
    // cleanUp(head);
    // cleanUp(head2);
    // cleanUp(merged);
    return 0;
}

