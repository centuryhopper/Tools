#include <stdio.h>
#include <stdlib.h>

/// @brief dynamic array data structure implementation
typedef struct
{
  int *array;
  size_t used;
  size_t size;
} Array;

Array* createArray(size_t initialSize)
{
    Array* a = malloc(sizeof(Array));
    if (a == NULL)
    {
        printf("Array malloc failed :(\n");
        return NULL;
    }
    a->array = malloc(initialSize * sizeof(int));
    if (a->array == NULL)
    {
        printf("a->array malloc failed :(\n");
        return NULL;
    }
    a->used = 0;
    a->size = initialSize;
    return a;
}

void appendToArray(Array *a, int element)
{
    // a->used is the number of used entries, because a->array[a->used++] updates a->used only *after* the array has been accessed.
    // Therefore a->used can go up to a->size
    if (a->used == a->size)
    {
        a->size *= 2;
        a->array = realloc(a->array, a->size * sizeof(int));
    }
    a->array[a->used++] = element;
}

Array* freeArray(Array *a)
{
    free(a->array);
    a->array = NULL;
    a->used = a->size = 0;
    free(a);
    return NULL;
}

void insert(Array *a, int pos)
{

}

void remove(Array* a, int pos)
{

}

Array* clone(Array* a)
{
    return NULL;
}

void sort(Array* a)
{
    // TODO: use merge sort
}

int main(int argc, char const *argv[])
{
    // initially 16 elements
    Array* a = createArray(16);
    int i;

    for (i = 0; i < 100; i++)
        appendToArray(a, i);  // automatically resizes as necessary
    printf("%d\n", a->array[9]);  // print 10th element
    printf("%d\n", a->used);  // print number of elements
    a = freeArray(a);
    return 0;
}
