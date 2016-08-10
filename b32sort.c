#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
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

int b32sort(const int32_t *a, unsigned int **p, unsigned int N){
  static const unsigned int bw = 32;
  unsigned int *W,*H,*iB,*O,L[(bw*256)+(5*256)],nopermute;
  int B[bw*256];

  unsigned int i;
  unsigned int d;
  unsigned int *iwriter,rank,numranks,*I;
  const unsigned int *ireader;
  const int32_t *reader;
  int32_t *writer;
  unsigned int S,temp;
  int32_t c,*buff;
  int offset,start,sz;


  memset(L,0,sizeof(unsigned int)*((bw*256)+(5*256)));
  memset(B,0,sizeof(int)*256*bw);
  iB = &L[0];
  O  = &L[bw*256];
  H  = &L[bw*256+256];

  if((buff=malloc(2*N*sizeof(int32_t)))==NULL){
    return(1);
  }

  *p = NULL;
  if((*p = malloc(2*N*sizeof(unsigned int)))==NULL){
    return(1);
  }

  I = *p;


  for(i=0;i<N;i++){
    H[(256*0)+_0(a[i])]+=1;
    H[(256*1)+_1(a[i])]+=1;
    H[(256*2)+_2(a[i])]+=1;
    H[(256*3)+_3(a[i])]+=1;
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
  iwriter=&I[N*start];
  for(i=0;i<N;i++){
    iwriter[i]=i;
  }
  ireader=iwriter;
  iwriter=&I[((start+1)%2)*N];

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


    iwriter=&I[((rank+start)%2)*N];
    ireader=&I[((rank+1+start)%2)*N];

    writer=&buff[((rank+start)%2)*N];
    reader=&buff[((rank+1+start)%2)*N];

    O=W;

  }


  free(buff);

  if((*p=realloc(*p,sizeof(unsigned int)*N))==NULL){
    return(1);
  }

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
            a[i] = (rand()%2*M)-M;
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
      a[i] = (rand()%2*M)-M;
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
