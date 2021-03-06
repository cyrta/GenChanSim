/* some structures lifted from propchan.h */
/* Delay line element structure for 8th order IIR filter */
/*
   copyright (c) 1994 The MITRE Corporation Bedford, MA

   $Id: filter.h,v 2.2 1994/07/25 19:06:52 jjb Exp $
   $Log: filter.h,v $
 * Revision 2.2  1994/07/25  19:06:52  jjb
 * added copyright notice
 *

*/
#ifndef _FILTER_H
#define _FILTER_H

typedef struct delay_4 {
  float dr1;
  float dr2;
  float dr3;
  float dr4;
  float di1;
  float di2;
  float di3;
  float di4;
} DELAY_4;

/* coefficients of a single biquad section*/
typedef struct biquad { 
	float a1;
	float a2;
	float b1;
	float b2;
} BIQUAD;

/* IIR filter Structure */
#define	MAXSECT		4		/* N section for a and b */
typedef struct FILTER {
	float ampl;
	BIQUAD sect[MAXSECT];
} FILTER;

#endif
