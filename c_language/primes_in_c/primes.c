#include <stdio.h>
#include <math.h>
#include <time.h>
#include <Windows.h>

#define MAXNUM 100000000
#define NUMTHREADS 8
#define TOPMAX 10

HANDLE threads[NUMTHREADS];
CRITICAL_SECTION lock;

int primes[MAXNUM + 1];
int lastNPrimes[TOPMAX];
unsigned long long int numPrimes;
int nextMult;
int stopPoint;
int lastN;
unsigned long long int sumPrimes;

DWORD WINAPI findPrimes(LPVOID lpParam);

int main()
{
    clock_t start = clock();

    // Initialize critical section
    InitializeCriticalSection(&lock);

    // We know almost all evens aren't primes, so mark them off first.
    numPrimes = 1;
    sumPrimes = 2;
    primes[0] = -1;
    primes[1] = -1;
    primes[2] = 2;

    // cross out all evens
    for (int i = 3; i <= MAXNUM; i++)
    {
        if (i % 2 == 0)
            primes[i] = -1;
        else
            primes[i] = i;
    }

    // Set multiple for sieve and initialize threads.
    nextMult = 3;

    for (int i = 0; i < NUMTHREADS; i++)
        threads[i] = CreateThread(NULL, 0, findPrimes, NULL, 0, NULL);

    // Wait for each thread to terminate sequentially.
    WaitForMultipleObjects(NUMTHREADS, threads, TRUE, INFINITE);

    printf("numPrimes: %llu\n", numPrimes);
    printf("stopPoint: %d\n", stopPoint);

    // Everything after the stop point greater than 0
    // is confirmed prime at this point.
    for (int i = 3; i < MAXNUM; i += 2)
    {
        if (primes[i] > 0)
        {
            numPrimes++;
            sumPrimes += primes[i];
        }
    }

    printf("numPrimes: %llu\n", numPrimes);

    // Store the last ten primes.
    lastN = 0;
    for (int i = MAXNUM; i >= 0; i--)
    {
        if (primes[i] > 0)
        {
            lastNPrimes[lastN] = primes[i];
            lastN++;
            if (lastN == TOPMAX)
                break;
        }
    }

    clock_t end = clock();

    // Print to file.
    FILE* ofp = fopen("primes.txt", "w");

    fprintf(ofp, "%f %llu %llu\n", ((double)(end - start) / CLOCKS_PER_SEC), numPrimes, sumPrimes);
    for (int i = 0; i < TOPMAX; i++)
        fprintf(ofp, "%d ", lastNPrimes[TOPMAX - i - 1]);

    fclose(ofp);

    // Cleanup critical section
    DeleteCriticalSection(&lock);
}

DWORD WINAPI findPrimes(LPVOID lpParam)
{
    int curMult = 0;

    // Wikipedia page for Sieve of Eratosthenes said
    // to only sieve up to square root.
    while (curMult * curMult <= MAXNUM)
    {
        EnterCriticalSection(&lock); // Lock the critical section.
        curMult = nextMult; // Update next multiple.
        nextMult += 2; // Just skip over evens.
        LeaveCriticalSection(&lock); // Unlock the critical section.

        if (primes[curMult] > 0)
        {
            // Update stop point and other variables.
            EnterCriticalSection(&lock); // Lock the critical section.
            stopPoint = curMult;
            // numPrimes++;
            // sumPrimes += primes[curMult];
            LeaveCriticalSection(&lock); // Unlock the critical section.

            // Sieve, cross out each multiple of curMult.
            for (int i = curMult * curMult; i <= MAXNUM; i += curMult)
                primes[i] = -1;
        }
    }

    return 0;
}
