// C++ program to pruint32_t pruint32_t all primes smaller than
// n using segmented sieve
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

// This functions finds all primes smaller than 'limit'
// using simple sieve of eratosthenes. It also stores
// found primes in vector prime[]
uint64_t simpleSieve(uint64_t limit, uint32_t *prime) {
    // Create a boolean array "mark[0..n-1]" and initialize
    // all entries of it as true. A value in mark[p] will
    // finally be false if 'p' is Not a prime, else true.
    bool *mark = (bool *) calloc(limit+1,sizeof(bool));

    for (uint64_t i=3; i <= sqrt(limit); i+=2) {
        for (uint64_t j=i*i; j<=limit; j+=2*i) {
            if ( !mark[j] )
                mark[j] = true;
        }
    }


// Pruint32_t all prime numbers and store them in prime

    uint64_t i = 0;
    for (uint64_t p=1; p<=limit; p+=2) {
        if (mark[p] == false) {
            prime[i++] = p;
            //printf("%u\n",p);
        }
    }
    prime[0] = 2;
    printf(":%u\n",i);
    free(mark);
    return i;
}

// Pruint32_ts all prime numbers smaller than 'n'
uint64_t segmentedSieve(uint64_t n) {
    // Compute all primes smaller than or equal
    // to square root of n using simple sieve
    uint64_t jSize = 0ull;
    uint64_t limit = floor(sqrt(n))+1ull;

    uint32_t prime[limit];
    uint64_t iSize = simpleSieve(limit, prime);
    printf("->%llu %llu %llu\n",iSize,limit,n);

    // Divide the range [0..n-1] in different segments
    // We have chosen segment size as sqrt(n).
    uint64_t low = limit;
    uint64_t high = limit<<1;

    // While all segments of range [0..n-1] are not processed,
    // process one segment at a time
    while (low < n) {
        if (high >= n)
            high = n;

        // To mark primes in current range. A value in mark[i]
        // will finally be false if 'i-low' is Not a prime,
        // else true.
        bool *mark = (bool *) calloc(limit+1,sizeof(bool));

        // Use the found primes by simpleSieve() to find
        // primes in current range
        //pruint32_tf(":%u \n",iSize);
        for (uint64_t i = 0; i < iSize; i++) {
            // Find the minimum number in [low..high] that is
            // a multiple of prime[i] (divisible by prime[i])
            // For example, if low is 31 and prime[i] is 3,
            // we start with 33.
            //printf("%u\n",prime[i]);
            uint64_t loLim = floor(low/prime[i]) * prime[i];
            if (loLim < low)
            loLim += prime[i];

            /* Mark multiples of prime[i] in [low..high]:
            	We are marking j - low for j, i.e. each number
            	in range [low, high] is mapped to [0, high-low]
            	so if range is [50, 100] marking 50 corresponds
            	to marking 0, marking 51 corresponds to 1 and
            	so on. In this way we need to allocate space only
            	for range */
            for (uint64_t j=loLim; j<high; j+=prime[i]) {
                mark[j-low] = true;
                printf(":%u %u %u %u\n",j,low,high,prime[i]);
            }
        }

        // Numbers which are not marked as true are prime
        for (uint64_t i = low; i<high; i+=1)
            if (mark[i - low] == false) {
                jSize++;
                printf("%u\n",i);
            }

        // Update low and high for next segment
        low = low + limit;
        low = high;
        high += limit;
        free(mark);
    }
    return iSize+jSize;
}

// Driver program to test above function
int main() {
    uint64_t n = 1e2;
    printf("Primes smaller than %llu",n);
    uint64_t noPrimes = segmentedSieve(n);
    printf("%llu\n",noPrimes);
    return 0;
}
