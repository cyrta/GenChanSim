/* IIR filter fading header file

   copyright (c) 1994 The MITRE Corporation Bedford, MA

   $Id: ifde.h,v 3.0 1994/08/30 18:51:48 jjb Exp $
   $Log: ifde.h,v $
 * Revision 3.0  1994/08/30  18:51:48  jjb
 * updated prototype for interp64 to include extra arrays for
 * storage of previous states.
 *
 * Revision 2.2  1994/07/25  18:57:02  jjb
 * added copyright notice
 *

 */

#ifndef _IIRFADE_H
#define _IIRFADE_H


/* functions */
extern char *getenv(char *);
void readfilterfile(char [], float [], int);
extern void interp(fcomplex [], int, int, int);
extern void getfiltbank(char [], FILTER *);
extern void zerofilter(fcomplex [], int, int, float [], int, fcomplex []);
extern void linearinterp(fcomplex [], int, int, int);
extern void interp64(fcomplex [], int, int, int, fcomplex [], fcomplex [], fcomplex [], fcomplex []);

#endif /* _IIRFADE_H */
