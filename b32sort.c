#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "b32sort.h"
#define _0(x) (x &         0x000000FF)
#define _1(x) ((x >> 8) &  0x000000FF)
#define _2(x) ((x >> 16) & 0x000000FF)
#define _3(x) ((((unsigned) x ) >> 24) ^ 0x00000080)

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

  return(0);
} 

int b32sort(const int32_t *a, unsigned int *I, unsigned int N){
  static const unsigned int bw = 32;
  unsigned int *W,*H,*iB,*O,L[(bw*256)+(5*256)],nopermute;
  int B[bw*256];

  unsigned int i;
  unsigned int d;
  unsigned int *iwriter,rank,numranks;
  unsigned int *ireader;
  const int32_t *reader;
  int32_t *writer;
  unsigned int S,temp;
  int32_t c,*buff;
  int offset,start,sz;
  unsigned int *ibuff,*iswap;


  memset(L,0,sizeof(unsigned int)*((bw*256)+(5*256)));
  memset(B,0,sizeof(int)*256*bw);
  iB = &L[0];
  O  = &L[bw*256];
  H  = &L[bw*256+256];

  if((buff=malloc(2*N*sizeof(int32_t)))==NULL){
    return(1);
  }

  if((ibuff = malloc(N*sizeof(unsigned int)))==NULL){
    return(1);
  }


  for(i=0;i<N;i++){
    H[(256*0)+_0(a[i])]+=1;
    H[(256*1)+_1(a[i])]+=1;
    H[(256*2)+_2(a[i])]+=1;
    H[(256*3)+_3(a[i])]+=1;
    I[i]=i;
  }


  numranks=0;
  for(rank=0;rank<=3;rank++){
    W=&H[256*rank];
    S = 0;
    temp =0;
    nopermute=0;
    for(i=0;i<256;i++){
      nopermute = (W[i]==N) ? 1 : nopermute;
      temp=W[i];
      W[i]=S;
      S+=temp;
    } 
    if(!nopermute){
      numranks=rank+1;
    }
  }

  /* 0 if numranks is even, N otherwise. */
  start = (numranks%2);

  ireader=I;
  iwriter=ibuff;

  reader=&a[0];
  writer=&buff[((start+1)%2)*N];

  /* Start sort on the ranks */
 

  for(rank=0;rank<numranks;rank++){
    W=&H[256*rank];
    memcpy(O,W,256*sizeof(unsigned int));
    d = rank*8;
    for(i=0;i<N;i++){
      c = (rank<=2) ? ((((unsigned)reader[i]) >> d) & 0x000000FF) : ((((unsigned)reader[i]) >> d) ^ 0x00000080);

      offset = W[c]-O[c];

      iB[(c*bw)+(offset%bw)] = ireader[i];
      B [(c*bw)+(offset%bw)] = reader[i];
      W[c]+=1;
      if(((offset+1)%bw)==0){
        memcpy(&iwriter[W[c]-bw], &iB[c*bw] ,bw*sizeof(unsigned int));
        memcpy(&writer [W[c]-bw], &B[c*bw]  ,bw*sizeof(int32_t));
      }
    }
    for(i=0;i<256;i++){
      offset = W[i]-O[i];
      memcpy(&iwriter[W[i]-(offset%bw)],&iB[i*bw],(offset%bw)*sizeof(unsigned int));
      memcpy(&writer [W[i]-(offset%bw)] ,&B[i*bw],(offset%bw)*sizeof(int32_t));
    } 

    iswap = ireader;
    ireader = iwriter;
    iwriter = iswap;


    writer=&buff[((rank+start)%2)*N];
    reader=&buff[((rank+1+start)%2)*N];

    O=W;

  }
  if(numranks%2){
    memcpy(I,ibuff,N*sizeof(unsigned int));
  }

  free(ibuff);
  free(buff);

  return(0);
}

