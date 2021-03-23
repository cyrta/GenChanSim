/* The function exp( -a * x ) * I0 ( b * x ) using the polynomial
   approximation of Abramowitz and Stegun for I0 (pp. 378) 
   --cmk, MIT Lincoln Laboratory */

#include <stdio.h>
#include <math.h>
#include <complex.h>
#include <values.h>

float expbess(
             float a,
             float b,
             float x)

{
  float t, t2, t4, t6, t8, t10, t12;
  float tn1, tn2, tn3, tn4, tn5, tn6, tn7, tn8;
  float u, expu, expux;
  float expx, invsqrtx;
  float bx;
  float val;  /* Returned value */
  
  bx = b * x;
  t = bx / 3.75;
  u = - a * x;
  
  if( bx < -3.75 ){
    printf("EXPBESS: argument to I0 is out of bounds, setting val to 0");
    val = 0;
  }
  else if( bx < 3.75 ) {
    t2 = t * t;
    t4 = t2 * t2;
    t6 = t2 * t4;
    t8 = t4 * t4;
    t10 = t4 * t6;
    t12 = t6 * t6;
    expu = exp(u);
    val = expu + 3.5156229 * t2 * expu
                 + 3.0899424 * t4 * expu
                 + 1.2067492 * t6 * expu
                 + 0.2659732 * t8 * expu
                 + 0.0360768 * t10 * expu
                 + 0.0045813 * t12 * expu;
  }
  else {
    expux = exp(bx+u);
    invsqrtx = 1.0 / sqrt(bx);
    tn1=1.0 / t;
    tn2= tn1 * tn1;
    tn3= tn1 * tn2;
    tn4 = tn2 * tn2;
    tn5 = tn1 * tn4;
    tn6 = tn3 * tn3;
    tn7 = tn1 * tn6;
    tn8 = tn4 * tn4;
    val = expux * invsqrtx * 0.39894228
          + expux * invsqrtx * 0.01328592 * tn1
          + expux * invsqrtx * 0.00225319 * tn2
          - expux * invsqrtx * 0.00157565 * tn3
          + expux * invsqrtx * 0.00916281 * tn4
          - expux * invsqrtx * 0.02057706 * tn5
          + expux * invsqrtx * 0.02635537 * tn6
          - expux * invsqrtx * 0.01647633 * tn7
          + expux * invsqrtx * 0.00392377 * tn8;
  }
  
  /*  printf("\n expbess value is: %g\n",val);*/
  return(val);
}
