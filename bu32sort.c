#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#define _0(x) (x &         0x000000FF)
#define _1(x) ((x >> 8) &  0x000000FF)
#define _2(x) ((x >> 16) & 0x000000FF)
#define _3(x) ((x >> 24) & 0x000000FF)

int validate_sort(const uint32_t *a, const unsigned int *p, unsigned int N){
  unsigned int i;

  for(i=1;i<N;i++){
    if(a[p[i]]<a[p[i-1]]){
      printf("  invalid permutation value at index %u\n",i);
      return(1);
    }
  }
  printf("sort is valid.\n");

  return(0);
} 

int bu32sort(uint32_t **A, unsigned int **p, unsigned int N){
  unsigned int i;
  unsigned int d;
  static const unsigned int mask = 0x000000FF;
  unsigned int B[4*256],*C,*a;
  uint32_t c;
  unsigned int *ireader,*iwriter,rank,*I;
  uint32_t *reader,*writer;

  a = *A;
  if(NULL==(a=realloc(a,2*N*sizeof(uint32_t)))){
    return(1);
  }

  if((*p = (unsigned int *)malloc(sizeof(unsigned int)*2*N))==NULL){
    return(1);
  }
  I = *p;
  iwriter=&I[N];

  for(i=0;i<N;i++){
    iwriter[i]=i;
  }



  memset(B,0,4*256*sizeof(unsigned int));
  for(rank=0;rank<=3;rank++){
    C=&B[256*rank];
    d = rank*8;

    reader=&a[((rank+1)%2)*N];
    writer=&a[((rank)%2)*N];

    iwriter=&I[((rank)%2)*N];
    ireader=&I[((rank+1)%2)*N];
  
    for(i=0;i<N;i++){
      c = (reader[i] >> d) & mask;
      C[c]+=1;
    }

    for(i=1;i<256;i++){
      C[i]=C[i]+C[i-1];
    }

    i = N;
    while(i--){
      c = (reader[i] >> d) & mask;
      iwriter[C[c]-1]=ireader[i];
      writer[C[c]-1] = reader[i];
      C[c]-=1;
    }


  }

  if(NULL==(a=realloc(a,sizeof(uint32_t)*N))){
    return(1);
  }
  if(NULL==realloc(I,sizeof(unsigned int)*N)){
    return(1);
  }

  *A = a;

  return(0);
}

int main(){
  uint32_t *a;
  unsigned int N,M,i,*I;
  clock_t begin,end;
  int e;
  double diff;

  N = 100000000;
  M = 1000000;
  a = (uint32_t *)malloc(sizeof(uint32_t)*N);
  //I = (unsigned int *)malloc(sizeof(unsigned int)*N);

  for(i=0;i<N;i++){
    a[i] = rand()%M;
  }

 // for(i=0;i<N;i++){
 //    printf("%u ",a[i]);
 //  }
 //  printf("\n");

  begin=clock();
  e=bu32sort(&a,&I,N);
  end=clock();
  diff=(double)(end - begin) / CLOCKS_PER_SEC;

  if(e>0){
    printf("  there appears to have been a memory allocation error.\n");
    exit(1);
  }

  if(validate_sort(a,I,N)==0){
    printf("N: %u, M: %u,time: %f, uint32_t/sec: %f\n",N,M,diff,N/diff);
  }
  // for(i=0;i<N;i++){
  //   printf("%u ",a[I[i]]);
  // }
  // printf("\n");

  free(a);
  free(I);
  return(0);

}
