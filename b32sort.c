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

int validate_sort(const int32_t *a, const unsigned int *p, unsigned int N){
  unsigned int i;

  for(i=1;i<N;i++){
    if(a[p[i]]<a[p[i-1]]){
      printf("Invalid permutation value at index %u\n",i);
      printf("  p[i] = %u, p[i-1]=%u\n",p[i],p[i-1]);
      printf("  a[p[i]] = %d, a[p[i-1]]=%d\n",a[p[i]],a[p[i-1]]);

      printf("I: ");
      for(i=0;i<N;i++){
        printf("%u ",p[i]);
      }
      printf("\n\n");

      printf("a[i]: ");
      for(i=0;i<N;i++){
        printf("%d ",a[i]);
      }
      printf("\n\n");

      printf("a[p[i]]: ");
      for(i=0;i<N;i++){
        printf("%d ",a[p[i]]);
      }
      printf("\n\n");
      
      return(1);
    }
  }
  //printf("sort is valid.\n");

  return(0);
} 

int bu32sort(const int32_t *a, unsigned int **p, unsigned int N){
  unsigned int i,j;
  unsigned int d;
  static const unsigned int mask = 0x000000FF;
  unsigned int B[4*256],*C;
  int32_t c,*buff;
  unsigned int *iwriter,rank,*I;
  const unsigned int *ireader;
  const int32_t *reader;
  int32_t *writer;


  if((buff=malloc(2*N*sizeof(int32_t)))==NULL){
    return(1);
  }

  *p = NULL;
  if((*p = malloc(2*N*sizeof(unsigned int)))==NULL){
    return(1);
  }

  I = *p;

  iwriter=&I[0];
  for(i=0;i<N;i++){
    iwriter[i]=i;
  }
  ireader=iwriter;
  iwriter=&I[N];

  reader=&a[0];
  writer=&buff[N];

  memset(B,0,4*256*sizeof(unsigned int));
  for(rank=0;rank<=2;rank++){
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



  rank=3;

  C=&B[256*rank];
  d = rank*8;


  for(i=0;i<N;i++){
    c = (((unsigned)reader[i]) >> d) ^ 0x00000080;
    C[c]+=1;
  }

  for(i=1;i<256;i++){
    C[i]=C[i]+C[i-1];
  }

  for(j=1;j<=N;j++){
    i=N-j;
    c = (((unsigned)reader[i]) >> d) ^ 0x00000080;
    iwriter[C[c]-1]=ireader[i];
    writer[C[c]-1] = reader[i];
    C[c]-=1;
  }


  iwriter=&I[((rank)%2)*N];
  ireader=&I[((rank+1)%2)*N];

  writer=&buff[((rank)%2)*N];
  reader=&buff[((rank+1)%2)*N];







  if((*p=realloc(*p,sizeof(unsigned int)*N))==NULL){
    return(1);
  }

  free(buff);
  return(0);
}

int main(){
  int32_t *a;
  unsigned int N,M,i,*I,n,m;
  clock_t begin,end;
  int e;
  double diff;


  N=1;
  for(n=0;n<7;n++){
    N*=10;
    M=1;
    for(m=0;m<9;m++){
        M*=10;
        if((a = malloc(sizeof(int32_t)*N))==NULL){
          printf("malloc failed...\n");
          exit(1);
        }
        //I = (unsigned int *)malloc(sizeof(unsigned int)*N);
        for(i=0;i<N;i++){
          a[i] = (rand()%(2*M))-M;
        }


        begin=clock();
        e=bu32sort(a,&I,N);
        end=clock();
        diff=(double)(end - begin) / CLOCKS_PER_SEC;

        if(e>0){
          printf("  there appears to have been a memory allocation error.\n");
          exit(1);
        }

        if(validate_sort(a,I,N)!=0){
          printf("** Invalid sort ** N: %u, M: %u,time: %f, int32_t/sec: %f\n",N,M,diff,N/diff);
          exit(1);
        }
        printf("N: %u, M: %u,time: %f, int32_t/sec: %f\n",N,M,diff,N/diff);
        free(a);
        free(I);
    }
  }
  return(0);

}
