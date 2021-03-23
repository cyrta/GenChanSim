/* complex number header file

   copyright (c) 1994 The MITRE Corporation Bedford, MA

   $Id: complex.h,v 2.2 1994/07/25 18:30:41 jjb Exp $
   $Log: complex.h,v $
 * Revision 2.2  1994/07/25  18:30:41  jjb
 * added Copyright notice.
 *

 */
/*
 * Dec. 1995 added Csub, Csqrt and Csub ( functions are in complex.c)
 * Linda Riehl MIT lincoln Lab
 */

#ifndef _COMPLEX_H
#define _COMPLEX_H

typedef struct FCOMPLEX { float	r, i; } fcomplex;
typedef struct DCOMPLEX { double r, i; } dcomplex;
fcomplex Cinit(fcomplex);
fcomplex Cadd(fcomplex, fcomplex);
fcomplex Cmul(fcomplex, fcomplex);
fcomplex Csub(fcomplex, fcomplex);
fcomplex Csqrt(fcomplex);
fcomplex Cdiv(fcomplex, fcomplex);

#endif
