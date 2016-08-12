#include <stdlib.h>
#include <stdio.h>
#include "e.h"
#include "b32sort.h"

K kb32sort(K v){  
  K ref = gtn(-1,v->n);
  if(ref==NULL){
    return kerr("memory");
  }
  if(b32sort(KI(v),KI(ref),v->n)){
    return kerr("domain");
  }
  return ref;
}
