#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#define _0(x) (x &         0x000000FF)
#define _1(x) ((x >> 8) &  0x000000FF)
#define _2(x) ((x >> 16) & 0x000000FF)
#define _3(x) ((x >> 24) & 0x000000FF)

int bu32sort(uint32_t *a, unsigned int N){
  unsigned int i;
  unsigned int d;
  unsigned int x,j,k;
  unsigned char mask;
  unsigned int C[256];
  uint32_t c, *b;
  mask = 0xFF;

  b = (uint32_t *)malloc(sizeof(unsigned int)*N);


  for(d=0;d<=24;d+=8){


    /* for(i=0;i<N;i++){
      printf("%08x %02x %02x %02x %02x\n",a[i],_0(a[i]),_1(a[i]),_2(a[i]),_3(a[i]));
    }
    printf("\n\n"); */


    memset(C,0,256*sizeof(unsigned int));
    memset(b,0,N*sizeof(uint32_t));
    for(i=0;i<N;i++){
      c = (a[i] >> d) & mask;
      C[c]+=1;
    }

    for(i=1;i<256;i++){
      C[i]=C[i]+C[i-1];
    }

    i = N;
    while(i--){
      c = (a[i] >> d) & mask;
      b[C[c]-1]=a[i];
      C[c]-=1;
    }

    for(i=0;i<N;i++){
      a[i]=b[i];
    }

  }

  free(b);

  return(0);
}

int main(){
  uint32_t *a, *b;
  unsigned int N,i;
  clock_t begin,end;
  double diff;

  N = 100000000;
  a = (uint32_t *)malloc(sizeof(uint32_t)*N);

  for(i=0;i<N;i++){
    a[i] = rand()%100000000;
  }

  /*
 for(i=0;i<N;i++){
    printf("%u ",a[i]);
  }
  printf("\n");
  */

  begin=clock();
  bu32sort(a,N);
  end=clock();
  diff=(double)(end - begin) / CLOCKS_PER_SEC;
  printf("N: %u, time: %f, uint32_t/sec: %f",N,diff,N/diff);
  /*
  for(i=0;i<N;i++){
    printf("%u ",a[i]);
  }
  printf("\n");
  */
  free(a);

}
