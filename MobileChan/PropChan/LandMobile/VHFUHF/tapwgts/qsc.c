/*---------------------------------------------------------------------------*/
/* qsc.c - thin screen phase model function                                  */
/* qsc(psi,c)                                                                */
/* psi is relative delay                                                     */
/* c is the intensity parameter for thin screen phase model                  */
/*                                                                           */
/*---------------------------------------------------------------------------*/
/*                                                                           */
/*
 
   copyright (c) 1994 The MITRE Corporation Bedford, MA

   $Id: qsc.c,v 3.0 1994/08/30 18:43:12 jjb Exp $
   $Log: qsc.c,v $
 * Revision 3.0  1994/08/30  18:43:12  jjb
 * updated rev number.
 *
 * Revision 2.2  1994/07/25  18:47:02  jjb
 * added copyright notice
 *

*/

#include <math.h>

static char rcsid[] = "$Id: qsc.c,v 3.0 1994/08/30 18:43:12 jjb Exp $";

float qsc(float psi, float c) {
  float w, cp, term1, term2, term3, term4;
  extern double q1(double x);

  cp = sqrt(1+c*c);
  term1 = 2*cp*exp(c*c/2);
  term2 = exp(-2*cp*psi);
  term3 = c - (2/c)*cp*psi;
  term4 = q1(term3);
  w = term1*term2*term4;
  return(w);
}
