#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#define _0(x) (x &         0x000000FF)
#define _1(x) ((x >> 8) &  0x000000FF)
#define _2(x) ((x >> 16) & 0x000000FF)
#define _3(x) ((x >> 24) & 0x000000FF)

int bu32sort(const uint32_t *a, unsigned int **p, unsigned int N){
  unsigned int i;
  unsigned int d;
  unsigned char mask;
  unsigned int C[256];
  uint32_t c;
  unsigned int *reader,*writer,rank,*I;
  mask = 0xFF;


  *p = (unsigned int *)malloc(sizeof(unsigned int)*2*N);
  I = *p;
  rank = 1; 
  writer=&I[((rank)%2)*N];

  for(i=0;i<N;i++){
    writer[i]=i;
  }


  for(d=0;d<=24;d+=8){
    rank++;
    writer=&I[((rank)%2)*N];
    reader=&I[((rank+1)%2)*N];
  
    memset(C,0,256*sizeof(unsigned int));
    for(i=0;i<N;i++){
      c = (a[reader[i]] >> d) & mask;
      C[c]+=1;
    }

    for(i=1;i<256;i++){
      C[i]=C[i]+C[i-1];
    }

    i = N;
    while(i--){
      c = (a[reader[i]] >> d) & mask;
      writer[C[c]-1]=reader[i];
      C[c]-=1;
    }

  }

  if(NULL==realloc(I,sizeof(unsigned int)*N)){
    return(1);
  }

  return(0);
}

int main(){
  uint32_t *a;
  unsigned int N,i,*I;
  clock_t begin,end;
  double diff;

  N = 100000000;
  a = (uint32_t *)malloc(sizeof(uint32_t)*N);
  //I = (unsigned int *)malloc(sizeof(unsigned int)*N);

  for(i=0;i<N;i++){
    a[i] = rand()%1000000;
  }

 // for(i=0;i<N;i++){
 //    printf("%u ",a[i]);
 //  }
 //  printf("\n");

  begin=clock();
  bu32sort(a,&I,N);
  end=clock();
  diff=(double)(end - begin) / CLOCKS_PER_SEC;
  printf("N: %u, time: %f, uint32_t/sec: %f\n",N,diff,N/diff);

  // for(i=0;i<N;i++){
  //   printf("%u ",a[I[i]]);
  // }
  // printf("\n");

  free(a);
  free(I);
  return(0);

}
