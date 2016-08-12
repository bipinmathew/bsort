#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "b32sort.h"

#define _0(x) (x &         0x0000000F)
#define _1(x) ((x >> 4)  & 0x0000000F)
#define _2(x) ((x >> 8)  & 0x0000000F)
#define _3(x) ((x >> 12) & 0x0000000F)
#define _4(x) ((x >> 16) & 0x0000000F)
#define _5(x) ((x >> 20) & 0x0000000F)
#define _6(x) ((x >> 24) & 0x0000000F)
#define _7(x) ((((unsigned) x ) >> 28) ^ 0x00000008)



int validate_sort(const int32_t *a, const uint32_t *p, uint32_t N){
  uint32_t i;

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

int b32sort(const int32_t *a, uint32_t *I, uint32_t N){
  static const uint32_t RANKWIDTH = 4;
  static const uint32_t MAXRANKS = 32/RANKWIDTH;
  static const uint32_t BINWIDTH = 1<<RANKWIDTH;
  static const uint32_t BW = 64;
  uint32_t *W,*H,*iB,*O,*L,nopermute;
  int B[BW*BINWIDTH];

  uint32_t i;
  uint32_t d;
  uint32_t *iwriter,rank,numranks;
  uint32_t *ireader;
  const int32_t *reader;
  int32_t *writer;
  uint32_t S,temp;
  int32_t c,*buff;
  int offset,start,sz;
  uint32_t *ibuff,*iswap;

  L = calloc(((BW*BINWIDTH)+((MAXRANKS+1)*BINWIDTH)),sizeof(uint32_t));


  memset(B,0,sizeof(int)*BINWIDTH*BW);
  iB = &L[0];
  O  = &L[BW*BINWIDTH];
  H  = &L[BW*BINWIDTH+BINWIDTH];

  if((buff=malloc(2*N*sizeof(int32_t)))==NULL){
    return(1);
  }

  if((ibuff = malloc(N*sizeof(uint32_t)))==NULL){
    return(1);
  }


  for(i=0;i<N;i++){
    H[(BINWIDTH*0)+_0(a[i])]+=1;
    H[(BINWIDTH*1)+_1(a[i])]+=1;
    H[(BINWIDTH*2)+_2(a[i])]+=1;
    H[(BINWIDTH*3)+_3(a[i])]+=1;
    H[(BINWIDTH*4)+_4(a[i])]+=1;
    H[(BINWIDTH*5)+_5(a[i])]+=1;
    H[(BINWIDTH*6)+_6(a[i])]+=1;
    H[(BINWIDTH*7)+_7(a[i])]+=1;
    I[i]=i;
  }


  numranks=0;
  for(rank=0;rank<MAXRANKS;rank++){
    W=&H[BINWIDTH*rank];
    S = 0;
    temp =0;
    nopermute=0;
    for(i=0;i<BINWIDTH;i++){
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
    W=&H[BINWIDTH*rank];
    memcpy(O,W,BINWIDTH*sizeof(uint32_t));
    d = rank*RANKWIDTH;
    for(i=0;i<N;i++){
      c = (rank<(MAXRANKS-1)) ? ((((unsigned)reader[i]) >> d) & 0x0000000F) : ((((unsigned)reader[i]) >> d) ^ 0x00000008);

      offset = W[c]-O[c];

      iB[(c*BW)+(offset%BW)] = ireader[i];
      B [(c*BW)+(offset%BW)] = reader[i];
      W[c]+=1;
      if(((offset+1)%BW)==0){
        memcpy(&iwriter[W[c]-BW], &iB[c*BW] ,BW*sizeof(uint32_t));
        memcpy(&writer [W[c]-BW], &B[c*BW]  ,BW*sizeof(int32_t));
      }
    }
    for(i=0;i<BINWIDTH;i++){
      offset = W[i]-O[i];
      memcpy(&iwriter[W[i]-(offset%BW)],&iB[i*BW],(offset%BW)*sizeof(uint32_t));
      memcpy(&writer [W[i]-(offset%BW)] ,&B[i*BW],(offset%BW)*sizeof(int32_t));
    } 

    iswap = ireader;
    ireader = iwriter;
    iwriter = iswap;


    writer=&buff[((rank+start)%2)*N];
    reader=&buff[((rank+1+start)%2)*N];

    O=W;

  }
  if(numranks%2){
    memcpy(I,ibuff,N*sizeof(uint32_t));
  }

  free(L);
  free(ibuff);
  free(buff);

  return(0);
}

