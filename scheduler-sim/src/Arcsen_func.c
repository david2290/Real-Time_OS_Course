#include <stdio.h>
#include <math.h>

int factorial(int number){
  int factorial = 1;
  if (number < 0){
    if (number == 0){
      return 1;
    }
    else{
      return 1;
    }
  }
  else{
    for(int i=1; i <= number; ++i){
      factorial *= i;
    }
    return factorial;
  }
}
float arcsen(int n){
  double temp1;
  double temp2;
  double temp3;
  double temp4;
  double var;
  int i=0;
  for(i; i <= n; ++i){
    temp1 = pow(4,i) * pow(factorial(i),2) * (2*i + 1);
    temp2 = factorial(2*i);
    temp3 = temp2 / temp1;
    temp4 = temp3 * pow (1,(2*i+1)) + var;
    var = temp4;
    printf("num ");
  }
  //var = i;
  return var;
}

int main(){
  int n = 20;
  double temp4;
  temp4 = arcsen(n);
  temp4 = 2 * (double)temp4;
  printf("the results is: %0.10lf", temp4);
  return 0;
}
// gcc Arcsen_func.c -o ArcSenProgram -lm
