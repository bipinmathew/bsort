#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#define _0(x) (x &         0x000000FF)
#define _1(x) ((x >> 8) &  0x000000FF)
#define _2(x) ((x >> 16) & 0x000000FF)
#define _3(x) ((x >> 24) & 0x000000FF)

int validate_sort(const uint32_t *a, const uint32_t *s, const unsigned int *p, unsigned int N){
  unsigned int i;
  for(i=1;i<N;i++){
    if(s[i]<s[i-1]){
      printf("  invalid sort at index %u\n",i);
      return(1);
    }
  }

  for(i=0;i<N;i++){
    if(a[p[i]]!=s[i]){
      printf("  invalid permutation value at index %u\n",i);
      return(1);
    }
  }

  return(0);
} 

int bu32sort(const uint32_t *a, uint32_t **s, unsigned int **p, unsigned int N){
  unsigned int i;
  unsigned int d;
  unsigned char mask;
  unsigned int C[256];
  uint32_t c, *reader, *writer, *b;
  unsigned int *ireader,*iwriter,rank,*I;
  mask = 0xFF;


  *p = (unsigned int *) malloc(sizeof(unsigned int)*2*N);
  *s = (uint32_t *) malloc(sizeof(uint32_t)*2*N);
  I = *p;
  b = *s;

  rank = 1; 
  iwriter = &I[((rank)%2)*N];
  writer  = &b[((rank)%2)*N];

  for(i=0;i<N;i++){
    iwriter[i]=i;
  }
  memcpy(writer,a,sizeof(uint32_t)*N);


  for(d=0;d<=24;d+=8){
    rank++;

    writer=&b[((rank)%2)*N];
    reader=&b[((rank+1)%2)*N];

    iwriter=&I[((rank)%2)*N];
    ireader=&I[((rank+1)%2)*N];
  
    memset(C,0,256*sizeof(unsigned int));
    for(i=0;i<N;i++){
      c = (reader[i] >> d) & mask;
      C[c]+=1;
    }

    for(i=1;i<256;i++){
      C[i]=C[i]+C[i-1];
    }

    // turn this into i++ to sort decreasing...
    i=N;
    while(i--){
      c = (reader[i] >> d) & mask;
      iwriter[C[c]-1]=ireader[i];
      writer[C[c]-1]=reader[i];
      C[c]-=1;
    }

  }

  if(NULL==realloc(I,sizeof(unsigned int)*N)){
    return(1);
  }

  if(NULL==realloc(b,sizeof(uint32_t)*N)){
    return(1);
  }

  return(0);
}

int main(){
  uint32_t *a,*b;
  unsigned int N,M,i,*I;
  clock_t begin,end;
  double diff;

  N = 10000000;
  M = 10000;
  a = (uint32_t *)malloc(sizeof(uint32_t)*N);
  //I = (unsigned int *)malloc(sizeof(unsigned int)*N);

  for(i=0;i<N;i++){
    a[i] = rand()%M;
  }

  // for(i=0;i<N;i++){
  //    printf("%u ",a[i]);
  // }
  // printf("\n");

  begin=clock();
  bu32sort(a,&b,&I,N);
  end=clock();
  diff=(double)(end - begin) / CLOCKS_PER_SEC;
  if(validate_sort(a,b,I,N)==0){
    printf("N: %u, M: %u,time: %f, uint32_t/sec: %f\n",N,M,diff,N/diff);
  }

  // for(i=0;i<N;i++){
  //   printf("%u ",a[I[i]]);
  // }
  // printf("\n"); 

  free(a);
  free(b);
  free(I);
  return(0);

}
