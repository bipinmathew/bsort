#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <emmintrin.h>
#define _0(x) (x &         0x000000FF)
#define _1(x) ((x >> 8) &  0x000000FF)
#define _2(x) ((x >> 16) & 0x000000FF)
#define _3(x) ((x >> 24) & 0x000000FF)

int validate_sort(const uint32_t *a, const unsigned int *p, unsigned int N){
  unsigned int i;

  for(i=1;i<N;i++){
    if(a[p[i]]<a[p[i-1]]){
      printf("Invalid permutation value at index %u\n",i);
      printf("  p[i] = %u, p[i-1]=%u\n",p[i],p[i-1]);
      printf("  a[p[i]] = %u, a[p[i-1]]=%u\n",a[p[i]],a[p[i-1]]);

      printf("I: ");
      for(i=0;i<N;i++){
        printf("%u ",p[i]);
      }
      printf("\n\n");

      printf("a[i]: ");
      for(i=0;i<N;i++){
        printf("%u ",a[i]);
      }
      printf("\n\n");

      printf("a[p[i]]: ");
      for(i=0;i<N;i++){
        printf("%u ",a[p[i]]);
      }
      printf("\n\n");
      
      return(1);
    }
  }
  //printf("sort is valid.\n");

  return(0);
} 

int bu32sort(const uint32_t *a, unsigned int **p, unsigned int N){
  unsigned int i,j;
  unsigned int d;
  static const unsigned int mask = 0x000000FF;
  unsigned int B[4*256],*C;
  uint32_t c,*buff;
  unsigned int *ireader,*iwriter,rank,*I;
  const uint32_t *reader;
  uint32_t *writer;


  if((buff=malloc(2*N*sizeof(uint32_t)))==NULL){
    return(1);
  }

  *p = NULL;
  if((*p = malloc(2*N*sizeof(unsigned int)))==NULL){
    return(1);
  }

  I = *p;
  ireader=&I[0];
  iwriter=&I[N];

  for(i=0;i<N;i++){
    ireader[i]=i;
  }

  reader=&a[0];
  writer=&buff[N];

  memset(B,0,4*256*sizeof(unsigned int));
  for(rank=0;rank<=3;rank++){
    C=&B[256*rank];
    d = rank*8;

  
    for(i=0;i<N;i++){
      c = (reader[i] >> d) & mask;
      C[c]+=1;
    }

    for(i=1;i<256;i++){
      C[i]=C[i]+C[i-1];
    }

    for(j=1;j<=N;j++){
      i=N-j;
      c = (reader[i] >> d) & mask;
      iwriter[C[c]-1]=ireader[i];
      writer[C[c]-1] = reader[i];
      C[c]-=1;
    }


    iwriter=&I[((rank)%2)*N];
    ireader=&I[((rank+1)%2)*N];

    writer=&buff[((rank)%2)*N];
    reader=&buff[((rank+1)%2)*N];


  }


  if((*p=realloc(*p,sizeof(unsigned int)*N))==NULL){
    return(1);
  }

  free(buff);
  return(0);
}

int main(){
  uint32_t *a;
  unsigned int N,M,i,*I,n,m;
  clock_t begin,end;
  int e;
  double diff;


  N=1;
  for(n=0;n<8;n++){
    N*=10;
    M=1;
    for(m=0;m<9;m++){
        M*=10;
        if((a = malloc(sizeof(uint32_t)*N))==NULL){
          printf("malloc failed...\n");
          exit(1);
        }
        //I = (unsigned int *)malloc(sizeof(unsigned int)*N);
        for(i=0;i<N;i++){
          a[i] = rand()%M;
        }

        // for(i=0;i<N;i++){
        //   printf("%u ",a[i]);
        // }
        // printf("\n");

        begin=clock();
        e=bu32sort(a,&I,N);
        end=clock();
        diff=(double)(end - begin) / CLOCKS_PER_SEC;

        if(e>0){
          printf("  there appears to have been a memory allocation error.\n");
          exit(1);
        }

        if(validate_sort(a,I,N)!=0){
          printf("** Invalid sort ** N: %u, M: %u,time: %f, uint32_t/sec: %f\n",N,M,diff,N/diff);
          exit(1);
        }
        printf("N: %u, M: %u,time: %f, uint32_t/sec: %f\n",N,M,diff,N/diff);
        free(a);
        free(I);
    }
  }
  return(0);

}
