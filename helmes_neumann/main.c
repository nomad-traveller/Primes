//***************************************************************************
/*
Algorithm      : Bernhard Helmes
Implementation : Jasper Neumann

Date           : 23.09.2009

Sieve of Eratosthenes

0. Task of this program:
------------------------
The algorithm finds all primes p up to n=10^12.

1. Heap-Structure:
------------------
Every prime up to the square root of the maximum of n Element N is insert in the heap.
The first element of the heap is the smallest.
There is no need for the complete list of 3, 5, 7 - n_max, which is one advantage.
Instead of this, a cycle help array, placed into the first or second level CPU-cache,
speeds up the calculation.

2. Help Array in the second Level Cache:
---------------------------------------
The byte size of the help array must be a power of 2.
We are using a ring array where the start index
moves cyclic through the array.

3. Libs
-------
iostream
math.h
string.h

4. Compiled with
----------------
g++ -O3 ./sieve_of_era.cpp -lm
gcc version 4.8.4 (Ubuntu 4.8.4-2ubuntu1~14.04.3)


5. Results / Run time performance
---------------------------------
a) Test results for Computer RedPelican:

Hardware       : CPU AMD 64 bit 2*4000+, 8 gbyte RAM, 2*64 kbyte first level cpu cache
Compiler       : GNU g++ V 3.41
OS-system      : PelicanHPC

Runtime time   : .------------------------------------------------------------------.
                 ! Run number: !  max_a ! max_a1= max_a / 2^4   ! Runtime Real      !
                 ....................................................................
                 !     1       !  2^19  ! 2^15                  !         504m      !
                 ....................................................................
                 !     2       !  2^20  ! 2^16                  !         370m      !
                 ....................................................................
                 !     2       !  2^21  ! 2^17                  !         273m      !
                 ................................................................
                 !     3       !  2^22  ! 2^18                  !         228m      !
                 ....................................................................

b) Cpu Athlon 64bit 5000+ Dualcore 2*64 kbyte first level cpu Cache

                 !     3       !  2^22  ! 2^18                  !         202m      !
                 ....................................................................

c) Test results for Intel Q9950, 2*6MByte second level cache:

g++-4.1.2         a_max = 2^23  real     95m57.184s  user    93m16.190s  sys     0m0.240s
g++-4.1.2         a_max = 2^24  real     87m14.361s  user    87m15.423s  sys     0m0.000s
g++-4.3.3         a_max = 2^23  real    102m8.316s   user   101m59.090s  sys     0m0.024s
g++-4.3.3         a_max = 2^24  real     97m8.038s   user    95m9.573s   sys     0m0.264s
icc 10.1 20080801 a_max = 2^23  real     88m54.500s  user    88m55.385s  sys     0m0.008s
icc 10.1 20080801 a_max = 2^24  real     83m53.319s  user    83m54.091s  sys     0m0.028s

d) Processor : 6x AMD FX(tm)-6300 Six-Core Processor,

L1 6* 16 KB 68939 MB/s,
L2 3*2048 KB 32858 MB/s,
L3 8 MB, 9554 MB/s,
4*8 GB Ram DDR3 1600 7813 MB/s,

	              a_max = 2^21   real   115m30.667s
                  a_max = 2^22   real	 91m6.129s
                  a_max = 2^23   real	 77m51.582s
                  a_max = 2^24   real	 82m22.845s
*/
//***************************************************************************

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>


// Define size of Array !!!
//===========================
// That is all you have to change to make a test !!!

#define max_a (1<<23)

// max_a1=max_a>>4
// max_a1 is the help array
//===========================

#define max_heap   100000
#define max_limit  1000000

#define sqr(x) (pow(x,2))
#define max_x sqr((unsigned long long)(max_limit))

// array size is defined !!!
//==========================
#define buf_size (max_a>>4)
#define max_a1 (buf_size-1)

struct to_bits {
    char buf[buf_size];
    void init() {
        memset(&buf,0, buf_size);
    }
    bool get(int i) {
        return ( buf[(i>>4) & max_a1] & (1<<((i>>1)&7)) ) != 0;
    }
    void set0(int i) {
        buf[(i>>4) & max_a1] &= ~(1<<((i>>1)&7));
    }
    void set1(int i) {
        buf[(i>>4) & max_a1] |= (1<<((i>>1)&7));
    }
};

struct tle_node {
  public:
    unsigned long long x,t;
};

typedef tle_node T;

void exch(T& a, T& b) {
    T tmp;

    tmp=a;
    a=b;
    b=tmp;
}

void fixdown(T a[], int idx, int count) {
    while (true) {
        int j=idx*2+1;
        if (j>=count)
            break;
        if (j<count-1 && a[j+1].x<a[j].x)
            j++;
        if (!(a[j].x<a[idx].x))
            break;
        exch(a[idx],a[j]);
        idx=j;
    }
}


void fixup(T a[], int idx) {
    int n;
    while (idx>0) {
        n=(idx-1) >> 1;
        if (a[n].x<a[idx].x)
            break;
        exch(a[n],a[idx]);
        idx=n;
    }
}

class PQ {
  public:
    T* pq;
    int count;
    PQ(int max) {
        pq=new T[max_heap];
        count=0;
    }
    int empty() const {
        return count==0;
    }
    void include(T& q) {
        pq[count]=q;
        count++;
        fixup(pq,count-1);
    }
    T& first() {
        return pq[0];
    }
    void delete_first() {
        exch(pq[0],pq[count-1]);
        count--;
        fixdown(pq,0,count);
    }
};

// Anzahl!
unsigned long long max_count;
PQ* l_node;

void insert(unsigned long long _x, unsigned long long _t) {
    tle_node le_node;

    le_node.x=_x;
    le_node.t=_t;
    l_node->include(le_node);
}

#define min(a,b) ((a<b)?a:b)

int main(int argc, char* argv[]) {
    struct to_bits bits;
    unsigned long long p,x, anz_aus=10, anz_exp=1, pot=0;;
    unsigned long long anz=1, i;
    unsigned long long next;

    l_node=new PQ(max_heap);

    tle_node le_node_found;

    // generate the primes below this number
    unsigned long limit = 100000000;
    if (argc >= 2)
        limit = atol(argv[1]);


    bits.init();
    insert(0xffffffffffffffffull,0);  // dummy
    max_count=0;
    x=3;
    le_node_found=l_node->first();
    do {
        if (le_node_found.x==x) {
            // Treffer
            next=min(max_x,x+max_a);
            do {
                l_node->delete_first();
                while (true) {
                    le_node_found.x+=le_node_found.t;
                    if (le_node_found.x>=next)
                        break;
                    bits.set1(le_node_found.x);
                };
                if (le_node_found.x<max_x)
                    l_node->include(le_node_found);
                le_node_found=l_node->first();
            } while (le_node_found.x==x);
            bits.set0(x);
        } else {
            if (!bits.get(x)) {
                // not sieved out, therefore prime
                anz++;
                if (x<max_limit)
                    insert(x*x,x*2);
                le_node_found=l_node->first();
            } else
                bits.set0(x);
        }

        x+=2;

#if 0
        if (x>anz_aus) {
            printf("10^ %i count of primes = %i\n",anz_exp,nz);
            anz_aus*=10;
            anz_exp++;
        }
    } while (x<=limit);
#else
    }
    while (x<=limit);
    printf("count of primes = %i\n",anz);
#endif
}
