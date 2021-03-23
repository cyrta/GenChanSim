/*

   copyright (c) 1994 The MITRE Corporation Bedford, MA

   $Id: complex.c,v 2.2 1994/07/25 18:30:41 jjb Exp $
   $Log: complex.c,v $
 * Revision 2.2  1994/07/25  18:30:41  jjb
 * added Copyright notice.
 *

*/
/*
 * December 1995 added complex subtract, complex divide and complex
 * square root functions
 */
#include <math.h>
#include "complex.h"

static char rcsid[] = "$Id: complex.c,v 2.2 1994/07/25 18:30:41 jjb Exp $";

fcomplex Cadd(fcomplex a, fcomplex b) {
	fcomplex	c;
	c.r = a.r+b.r;
	c.i = a.i+b.i;
	return(c);
}

fcomplex Cinit(fcomplex a) {
	fcomplex c;
	c.r = a.r;
	c.i = a.i;
	return(c);
}

fcomplex Cmul(fcomplex a, fcomplex b) {
	fcomplex c;
	c.r = a.r*b.r-a.i*b.i;
	c.i = a.i*b.r+a.r*b.i;
	return(c);
}
fcomplex Csub(fcomplex a, fcomplex b) {
	fcomplex	c;
	c.r = a.r - b.r;
	c.i = a.i - b.i;
	return(c);
}
fcomplex Csqrt(fcomplex z)
  {
    fcomplex it;
    float sabs = (float)sqrt( sqrt((double)(z.r*z.r) + (double)(z.i*z.i)));
    float phi  = (float)atan2((double)z.i,(double)z.r)/2.;
    it.r = sabs*cos(phi);
    it.i = sabs*sin(phi);
    return(it);
  }
/*
 * complex divide:
 *
 *    if e=a+jb and v=c+jd then
 *
 *    e/v = (ac+bd)/(c**2 + d**2) + j((bc-ad)/(c**2+d**2))
 *
 */

fcomplex Cdiv(fcomplex c1,fcomplex c2){
  fcomplex ans;
  float denominator;

  denominator = (c2.r*c2.r) + (c2.i*c2.i);

  ans.r = ((c1.r*c2.r) + (c1.i*c2.i))/denominator;

  ans.i = ((c1.i*c2.r) - (c1.r*c2.i))/denominator;

  return(ans);

}
  
  
  
