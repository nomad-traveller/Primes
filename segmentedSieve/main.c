// C program to print all primes smaller than
// n using segmented sieve
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <assert.h>

typedef unsigned long u64;
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;

#define SET(arr,bit) (*((arr)+(((bit)>>7)))|=((1llu<<(((bit)>>1)&63))))
#define GET(arr,bit) (*((arr)+(((bit)>>7)))& ((1llu<<(((bit)>>1)&63))))

// This functions finds all primes smaller than 'limit'
// using simple sieve of eratosthenes. It also stores
// found primes in vector prime[]
u64 simpleSieve(u64 limit, u32 *prime) {
    // Create a boolean array "mark[0..n-1]" and initialize
    // all entries of it as true. A value in mark[p] will
    // finally be false if 'p' is Not a prime, else true.
    bool *mark = (bool *) calloc(limit+1,sizeof(bool));

    for (u64 i=3; i <= sqrt(limit); i+=2) {
        if ( mark[i])
            continue;
        for (u64 j=i*i; j<=limit; j+=2*i) {
            mark[j]=true;
        }
    }


// Pru32 all prime numbers and store them in prime

    u64 i = 0;
    for (u64 p=1; p<=limit; p+=2) {
        (mark[p] == false)?prime[i++]=p:0;
    }

    prime[0] = 2;
    free(mark);
    return i;
}

// Print all prime numbers smaller than 'n'
u64 segmentedSieve(u64 n) {
    // Compute all primes smaller than or equal
    // to square root of n using simple sieve
    u64 jSize;
    jSize ^= jSize;
    u64 limit = floor(sqrt(n));
    u32 iPrime;
    u32 prime[limit];
    u32 jSeg = n/limit;
    u64 iSize = simpleSieve(limit, prime);
    printf("->%lu %lu %lu %lu\n",iSize,limit,n,jSeg);

    // Divide the range [0..n-1] in different segments
    // We have chosen segment size as sqrt(n).
    u64 low = limit;
    u64 high = limit<<1;
    bool *mark = (bool *) calloc(limit,sizeof(bool));
    assert(mark);
    // While all segments of range [0..n-1] are not processed,
    // process one segment at a time

    for ( u32 j = 0; j < jSeg; j++) {

        // To mark primes in current range. A value in mark[i]
        // will finally be false if 'i-low' is Not a prime,
        // else true.
        memset(mark,0,limit);
        // Use the found primes by simpleSieve() to find
        // primes in current range
        //printf(":%u \n",iSize);
        for (u32 i = 1; i < iSize; i++) {
            // Find the minimum number in [low..high] that is
            // a multiple of prime[i] (divisible by prime[i])
            // For example, if low is 31 and prime[i] is 3,
            // we start with 33.
            //printf("%u\n",prime[i]);
            iPrime = prime[i];
            u64 loLim = (low/iPrime) * iPrime;
            loLim += (loLim<low)?iPrime:0;
            /* Mark multiples of prime[i] in [low..high]:
            	We are marking j - low for j, i.e. each number
            	in range [low, high] is mapped to [0, high-low]
            	so if range is [50, 100] marking 50 corresponds
            	to marking 0, marking 51 corresponds to 1 and
            	so on. In this way we need to allocate space only
            	for range */
            for (u64 j=loLim; j<high; j+=iPrime) {
                mark[j-low] = true;
            }
        }

        // Numbers which are not marked as true are prime
        for (u64 i = low|1; i<high; i+=2) {
            (mark[i-low])?0:jSize++;
        }
        // Update low and high for next segment
        low = high;
        high += limit;
        high = (high<n)?high:n;
    }
    free(mark);
    return iSize+jSize;

}

// Driver program to test above function
int main() {
    u64 n = 1e9 ;
    printf("Primes smaller than %lu",n);
    u64 noPrimes = segmentedSieve(n);
    printf("%I64u\n",noPrimes);
    return 0;
}
