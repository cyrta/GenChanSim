/* IIR filter fading header file
 */
/*
   copyright (c) 1994 The MITRE Corporation Bedford, MA

   $Id: iirfade.h,v 2.2 1994/07/25 19:06:52 jjb Exp $
   $Log: iirfade.h,v $
 * Revision 2.2  1994/07/25  19:06:52  jjb
 * added copyright notice
 *

*/

#ifndef _IIRFADE_H
#define _IIRFADE_H

#include <filter.h>

/* functions */
extern char *getenv(char *);
void readfilterfile(char [], float [], int);
extern void interp(fcomplex [], int, int, int);
extern void iir(fcomplex *, FILTER *, DELAY_4 *[]);
extern void getfiltbank(char [], FILTER *);
extern void zerofilter(fcomplex [], int, int, float [], int, fcomplex []);
extern void linearinterp(fcomplex [], int, int, int);
extern void interp64(fcomplex [], int, int, int, fcomplex *[]);
extern void iirfilter(fcomplex [], int, DELAY_4 *[]);

#endif /* _IIRFADE_H */
