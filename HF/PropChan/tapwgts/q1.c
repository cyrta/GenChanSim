/*---------------------------------------------------------------------------*/
/* q1.c - Evaluate Gaussian error function Q(x)                              */
/* this function subroutine evaluates the real complimentary error function  */
/* of the argument x.  the algorithm was taken from abromawitz and steigun   */
/* and consists of a power series expansion which is valid over the interval */
/* 0 <= x < 2 with an error magnitude smaller than 1.5e-7.  for values of    */
/* x => 2, a continued fraction expansion form is used to evaluate erfc(x).  */
/*---------------------------------------------------------------------------*/
/*                                                                           */
/*

   copyright (c) 1994 The MITRE Corporation Bedford, MA

   $Id: q1.c,v 2.2 1994/07/25 19:20:47 jjb Exp $
   $Log: q1.c,v $
 * Revision 2.2  1994/07/25  19:20:47  jjb
 * added copyright notice
 *

*/

#include <math.h>

#define C  0.5641895835
#define P  .3275911
#define A1 0.254829592
#define A2 -0.284496736
#define A3 1.421413741
#define A4 -1.453152027
#define A5 1.061405429

static char rcsid[] = "$Id: q1.c,v 2.2 1994/07/25 19:20:47 jjb Exp $";

double q1(double x)
  {
  double y, t, term, val, p;
  int i;
  y=fabs(x/sqrt(2.));
  if(y <= 2) 
    {
    t = 1.0/(1.0+P*y);
    p = t*(A1+t*(A2+t*(A3+t*(A4+t*A5))));
    }
  else
    {
    /* continued fraction expansion.*/
    term = 5.0;
    val = y;
    for (i=0; i<10; i++)
      {
      val = y+term/val;
      term = term-0.5;
      }
    p = C/val;
    }
  p = p * exp(-y*y)/2.;
  return  x >= 0.0 ? p : 1.0-p;
  }
