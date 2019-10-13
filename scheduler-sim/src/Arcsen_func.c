#include <stdio.h>
#include <math.h>

int factorial(int number){
  int factorial = 1;
  for(int i=1; i <= number; ++i){
    factorial *= i;
  }
  return factorial;
}
float arcsen(int n){
  long double coeff=1.0;
  long double result=0;
  for(int i=1; i <= n; i++){
    if(i%2==1){
      result+=coeff/i;
      coeff*=i;
    }else
      coeff/=i;
  }
  return result;
}

int main(){
  int n = 100000;
  double temp4;
  temp4 = arcsen(n);
  temp4 = 2.0 *temp4;
  printf("the results is: %0.10lf\n", temp4);
  return 0;
}
// gcc Arcsen_func.c -o ArcSenProgram -lm
