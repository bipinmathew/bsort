#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **argv){
  int32_t *a;
  unsigned int N,M,i,*I,n,m,testnum;
  static const unsigned int numtests=10;
  clock_t begin,end;
  int e;
  double diff;

  if(argc<3){

    printf("N,M,time,int32_t/sec\n");
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


          if((I = malloc(sizeof(unsigned int)*N))==NULL){
            printf("malloc failed...\n");
            exit(1);
          }

          diff = 0;
          for(testnum=0;testnum<numtests;testnum++){
              for(i=0;i<N;i++){
                a[i] = (rand()%(2*M))-M;
              }
              begin=clock();
              e=b32sort(a,I,N);
              end=clock();
              diff+=(double)(end - begin) / CLOCKS_PER_SEC;
              if(e>0){
                printf("  there appears to have been a memory allocation error.\n");
                exit(1);
              }
              if(validate_sort(a,I,N)!=0){
                printf("** Invalid sort ** N: %u, M: %u,time: %f, int32_t/sec: %f\n",N,M,diff,N/diff);
                exit(1);
              }
          }
          diff /= numtests;

          printf("%u,%u,%f,%f\n",N,M,diff,N/diff);
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


    if((I = malloc(sizeof(unsigned int)*N))==NULL){
      printf("malloc failed...\n");
      exit(1);
    }
    for(i=0;i<N;i++){
      if(argc>3){
        if(0==strcmp(argv[3],"UNSIGNED")){
          a[i] = rand()%M;
        }
        else{
          a[i] = (rand()%(2*M))-M;
        }
      }
      else{
          a[i] = (rand()%(2*M))-M;
      }
    }


    begin=clock();
    e=b32sort(a,I,N);
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
