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

int b32sort(const int32_t *a, unsigned int **p, unsigned int N){
  static const unsigned int bw = 32;
  unsigned int *W,*H,*iB,*O,L[(bw*256)+(5*256)];
  int B[bw*256];

  unsigned int i,j;
  unsigned int d;
  static const unsigned int mask = 0x000000FF;
  static const unsigned int dmask1[4] = {0xFF,0xFF,0xFF,0xFF};
  static const unsigned int dmask2[4] = {0xFF00,0xFF00,0xFF00,0xFF00};
  static const unsigned int dmask3[4] = {0xFF0000,0xFF0000,0xFF0000,0xFF0000};
  static const unsigned int dmask4[4] = {0x80000000,0x80000000,0x80000000,0x80000000};
  unsigned int h1[4];
  unsigned int h2[4];
  unsigned int h3[4];
  unsigned int h4[4];
  unsigned int *iwriter,rank,*I;
  const unsigned int *ireader;
  const int32_t *reader;
  int32_t *writer;
  unsigned int S,temp;
  int32_t c=0,*buff;
  int offset;


  static const unsigned int BLOCKSIZE = 4;
  unsigned int remainder;
  __m128i *src, xmm0,xmask1,xmask2,xmask3,xmask4,r1,r2,r3,r4;

  remainder = N%BLOCKSIZE;
  xmask1 = _mm_loadu_si128((__m128i*)dmask1);
  xmask2 = _mm_loadu_si128((__m128i*)dmask2);
  xmask3 = _mm_loadu_si128((__m128i*)dmask3);
  xmask4 = _mm_loadu_si128((__m128i*)dmask4);

  memset(L,0,sizeof(unsigned int)*((bw*256)+(5*256)));
  iB = &L[0];
  H  = &L[bw*256];
  O  = &L[(bw*256)+(4*256)];

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

  /* removed ranks from here */
  src = (__m128i*)reader; 
  for(i=0;i<N-remainder;i+=BLOCKSIZE){
    xmm0 = _mm_loadu_si128(src++);
    r1 = _mm_and_si128(xmm0,xmask1);
    r2 = _mm_and_si128(xmm0,xmask2);
    r3 = _mm_and_si128(xmm0,xmask3);
    r4 = _mm_xor_si128(xmm0,xmask4);

    r2 = _mm_srli_epi32(r2,8);
    r3 = _mm_srli_epi32(r3,16);
    r4 = _mm_srli_epi32(r4,24);
    _mm_storeu_si128((__m128i *)h1,r1);
    _mm_storeu_si128((__m128i *)h2,r2);
    _mm_storeu_si128((__m128i *)h3,r3);
    _mm_storeu_si128((__m128i *)h4,r4);
    for(j=0;j<BLOCKSIZE;j++){
      H[h1[j]]+=1;
      H[(256*1)+h2[j]]+=1;
      H[(256*2)+h3[j]]+=1;
      H[(256*3)+h4[j]]+=1;
    }
  }

  for(rank=0;rank<=2;rank++){
    W=&H[256*rank];
    d = rank*8;
    for(i=N-remainder;i<N;i++){
      c = (reader[i] >> d) & mask;
      W[c]+=1;
    }
  }

  rank=3;
  W=&H[256*rank];
  d = rank*8;
  for(i=N-remainder;i<N;i++){
    c = (((unsigned)reader[i]) >> d) ^ 0x00000080;
    W[c]+=1;
  }


  for(rank=0;rank<=3;rank++){
    W=&H[256*rank];
    S = 0;
    temp =0;
    for(i=0;i<256;i++){
      temp=W[i];
      W[i]=S;
      S+=temp;
    }
    W[0]=0;
  }


  /* Start sort on the ranks */
 

  for(rank=0;rank<=2;rank++){
    W=&H[256*rank];
    memcpy(O,W,256*sizeof(unsigned int));
    d = rank*8;
    for(i=0;i<N;i++){
      c = (reader[i] >> d) & mask;

      offset = W[c]-O[c];

      iB[(c*bw)+(offset%bw)] = ireader[i];
      B [(c*bw)+(offset%bw)] = reader[i];
      if(((offset+1)%bw)==0){
        memcpy(&iwriter[W[c]-bw+1], &iB[c*bw] ,bw*sizeof(unsigned int));
        memcpy(&writer [W[c]-bw+1], &B[c*bw]  ,bw*sizeof(int32_t));
      }
      W[c]+=1;
    }
    for(i=0;i<256;i++){
      offset = W[i]-O[i];
      memcpy(&iwriter[W[i]-(offset%bw)],&iB[i*bw],(offset%bw)*sizeof(unsigned int));
      memcpy(&writer [W[i]-(offset%bw)] ,&B[i*bw],(offset%bw)*sizeof(int32_t));
    } 


    iwriter=&I[((rank)%2)*N];
    ireader=&I[((rank+1)%2)*N];

    writer=&buff[((rank)%2)*N];
    reader=&buff[((rank+1)%2)*N];

  }


  /* unroll the last rank because we have to do something a bit different */

  rank=3;
  W=&H[256*rank];
  memcpy(O,W,256*sizeof(unsigned int));
  d = rank*8;

  for(i=0;i<N;i++){
    c = (((unsigned)reader[i]) >> d) ^ 0x00000080;

    offset = W[c]-O[c];

    iB[(c*bw)+(offset%bw)] = ireader[i];
    B [(c*bw)+(offset%bw)] = reader[i];
    if(((offset+1)%bw)==0){
      memcpy(&iwriter[W[c]-bw+1], &iB[c*bw] ,bw*sizeof(unsigned int));
      memcpy(&writer [W[c]-bw+1], &B[c*bw]  ,bw*sizeof(int32_t));
    }
    W[c]+=1;
  }
  for(i=0;i<256;i++){
    offset = W[i]-O[i];
    memcpy(&iwriter[W[i]-(offset%bw)],&iB[i*bw],(offset%bw)*sizeof(unsigned int));
    memcpy(&writer [W[i]-(offset%bw)] ,&B[i*bw],(offset%bw)*sizeof(int32_t));
  } 

  /* End unrolling */


  if((*p=realloc(*p,sizeof(unsigned int)*N))==NULL){
    return(1);
  }

  free(buff);
  return(0);
}

int main(int argc, char **argv){
  int32_t *a;
  unsigned int N,M,i,*I,n,m;
  clock_t begin,end;
  int e;
  double diff;

  if(argc<3){
    N=1;
    for(n=0;n<8;n++){
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
          e=b32sort(a,&I,N);
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
  }
  else{
    N=atoi(argv[1]);
    M=atoi(argv[2]);


    if((a = malloc(sizeof(int32_t)*N))==NULL){
      printf("malloc failed...\n");
      exit(1);
    }
    //I = (unsigned int *)malloc(sizeof(unsigned int)*N);
    for(i=0;i<N;i++){
      a[i] = (rand()%(2*M))-M;
    }


    begin=clock();
    e=b32sort(a,&I,N);
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


  return(0);

}
