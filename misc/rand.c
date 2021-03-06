/*

   copyright (c) 1994 The MITRE Corporation Bedford, MA

   $Id: rand.c,v 2.2 1994/07/25 18:30:41 jjb Exp $
   $Log: rand.c,v $
 * Revision 2.2  1994/07/25  18:30:41  jjb
 * added Copyright notice.
 *

 * rdm(idum)	uniform random number generator
 * reference: Park, S. K. and K. W. Miller, `Random Number Generators: Good
 * Ones Are Hard to Find', Comm. ACM, vol 31,2 (1988), pages 1192--1201
 * Integer Version 2, page 1195, based on Schrage's method
 * normally begins with seed k=1
 *
 * this is a c version of the fortran version
 *
 * called with y  = rdm(&idum) ;
 *
 * SYNOPSIS:
 * double rdm( );
 * long int  *idum ;
 *
 * routine returns a double over (0.0,1.0)
 * setting idum < 0 resets the generator to given seed 
 * setting idum = -1 sets to value obtained with  no initialization 
 * seed automatically set to k=1 on the first call, if no initialization
 * is specified.
 */

  
#define	m  2147483647L
#define	a  16807
#define	q  127773L
#define	r  2836
#define YES 1
#define NO 0

static char rcsid[] = "$Id: rand.c,v 2.2 1994/07/25 18:30:41 jjb Exp $";

double	 rdm(k)
long int *k;

{

   long int h,l,t;
   static short int initialized = NO;

   /* reset generator if input is negative, or if first time */
  
   if (*k < 0 ) *k = -(*k);
   else if (initialized == NO) *k=1;

   initialized=YES;

   h = *k / q;
   l = *k %q;
   t = a * l - r * h;

   if (t > 0) *k = t;
   else *k = t + m;

   return((double)(*k)/m);
}

#undef	m   
#undef  a  
#undef	q 
#undef	r
#undef YES 
#undef NO 
