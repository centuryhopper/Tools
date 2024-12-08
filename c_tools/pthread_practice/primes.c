#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>

#define NUM_THREADS 8
#define N 100000000



// to run the program
// time gcc primes.c -lm -pthread && ./a.out

// sanity check:
// valgrind --leak-check=full ./a.out

// find all primes between 1 and sqrt(N) using sieve of eratosthenes
// spawn 8 threads each covering 12.5 million numbers
// use each prime number p of the prime numbers found <= sqrt(N) to mark the multiples smallest multiple of p in the threads subrange



typedef struct sieveArray
{
    int size;
    int *array;
} sieveArray;

typedef struct ThreadArgs {
    bool* mainArray;
    int start;
    int end;
    int* smallPrimes;
    int smallPrimesSize;
} ThreadArgs;


int getSmallestMultipleOfPInSubrange(int p, int);
sieveArray *sieveOfEratosthenes(int n);
void *markComposites(void*);

int getSmallestMultipleOfPInSubrange(int p, int rangeStart)
{
    return (int)fmax(pow(p, 2), ceil(rangeStart / p) * p);
}

sieveArray *sieveOfEratosthenes(int n)
{
    if (n > sqrt(N))
    {
        printf("n must be less than the sqrt of N\n");
        return NULL;
    }
    // Create a boolean array "prime[0..n]" and initialize
    // all entries it as true. A value in prime[i] will
    // finally be false if i is Not a prime, else true.
    bool prime[n + 1];
    memset(prime, true, sizeof(prime));
    prime[0] = prime[1] = false;

    for (int p = 2; p * p <= n; p++)
    {
        // If prime[p] is not changed, then it is a prime
        if (prime[p])
        {
            // Update all multiples of p greater than or
            // equal to the square of it numbers which are
            // multiple of p and are less than p^2 are
            // already been marked.
            for (int i = p * p; i <= n; i += p)
                prime[i] = false;
        }
    }

    int *primes = (int *)malloc(sizeof(int) * 10000);
    memset(primes, 0, sizeof(primes));
    int cur = 0;

    // get all prime numbers
    for (int p = 2; p <= n; p++)
    {
        if (prime[p])
        {
            primes[cur++] = p;
        }
    }

    if (cur > 0)
    {
        primes = realloc(primes, cur * sizeof(int));
    }

    sieveArray *sa = (sieveArray *)malloc(sizeof(sieveArray));
    sa->size = cur;
    sa->array = primes;

    return sa;
}

void *markComposites(void* arg)
{
    ThreadArgs *args = (ThreadArgs *)arg;
    bool *mainArray = args->mainArray;
    int start = args->start;
    int end = args->end;
    int *smallPrimes = args->smallPrimes;
    int smallPrimesSize = args->smallPrimesSize;

    // go thru each small prime and use formula to find the smallest multiple of p
    for (int i = 0; i < smallPrimesSize; i++)
    {
        int startingPoint = getSmallestMultipleOfPInSubrange(smallPrimes[i], start);
        for (int j = startingPoint; j <= end; j+=smallPrimes[i])
        {
            if (j != smallPrimes[i])
            {
                mainArray[j] = false;
            }
        }
    }

    return NULL;
}

int main(int argc, char const *argv[])
{
    // calculate small primes
    int n = sqrt(N);
    sieveArray *sa = sieveOfEratosthenes(n);
    // printf("%d\n", sa->size);
    // for (int i = 0;i < sa->size; i++)
    // {
    //     printf("%d%s", sa->array[i], (i == sa->size - 1) ? "\n" : ", ");
    // }

    bool* mainArray = (bool*) malloc(sizeof(bool) * N);
    memset(mainArray, true, sizeof(bool) * N);

    // spawn 8 threads
    pthread_t threads[NUM_THREADS];
    ThreadArgs threadArgs[NUM_THREADS];  // Array of arguments for each thread

    int rangeSize = N / NUM_THREADS;  // Divide the range among threads

    for (int i = 0; i < NUM_THREADS; i++)
    {
        // Calculate the start and end for each thread
        int start = i * rangeSize;
        int end = (i == NUM_THREADS - 1) ? N - 1 : (i + 1) * rangeSize - 1;

        // Prepare the arguments for the thread
        threadArgs[i].mainArray = mainArray;
        threadArgs[i].start = start;
        threadArgs[i].end = end;
        threadArgs[i].smallPrimes = sa->array;
        threadArgs[i].smallPrimesSize = sa->size;
                // Create the thread
        if (pthread_create(&threads[i], NULL, markComposites, (void*)&threadArgs[i]) != 0) {
            perror("Failed to create thread");
            exit(EXIT_FAILURE);
        }
    }
    // from sympy import primerange


    // Wait for all threads to finish
    for (int i = 0; i < NUM_THREADS; i++)
    {
        pthread_join(threads[i], NULL);
    }

    int cnt = 0;
    long sum = 0;

    for (long i = 2; i < N;i++)
    {
        // if (mainArray[i])
        // {
        //     printf("%d\n", i);
        // }

        // printf("%d%s", mainArray[i], i == 999 ? "\n" : ", ");
        if (mainArray[i])
        {
            cnt += 1;
            sum += i;
        }
    }

    // get largest 10 primes less than N
    int cur = 0;
    int largestTen[10] = {0};

    for (int i = N; i >= 2; i--)
    {
        if (mainArray[i])
        {
            largestTen[cur++] = i;
            if (cur == 10) break;
        }
    }

    printf("total number of primes: %d\n", cnt);
    printf("total sum of primes: %ld\n", sum);
    printf("largest 10 primes less than N:\n");
    for (int i = 0; i < 10; i++)
    {
        printf("%s%d%s", (i == 0 ? "[" : ""), largestTen[i], (i == 9 ? "]\n" : ", "));
    }

    free(sa->array);
    free(sa);
    free(mainArray);
    return 0;
}
