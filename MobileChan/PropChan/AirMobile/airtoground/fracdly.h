/*
  include file for fractional delay filter 

  copyright (c) 1994 The MITRE Corporation Bedford, MA

  $Id: fracdly.h,v 3.0 1994/08/30 18:52:51 jjb Exp $
  $Log: fracdly.h,v $
 * Revision 3.0  1994/08/30  18:52:51  jjb
 * new addition to rev 3.0 of software.
 *

*/

#define FRACDLY_FILTER "/MobileChan/PropChan/AirMobile/airtoground/fracdly10-12.coef"
#define FRAC_FILT_SIZE 12
#define FRAC_INTERP_RATE 10
#define ORIG_FRAC_FILT_SIZE (FRAC_FILT_SIZE * FRAC_INTERP_RATE)

#define MIN_FRACT_DELAY  0.001
#define FRAC_FILT_DLY_BIAS 5

void fracDelayFilter(float, float[]);
void FracTapDelay(int*, int*, fcomplex*,int,TAPWGTS *, float[], 
		 fcomplex**,fcomplex*);
