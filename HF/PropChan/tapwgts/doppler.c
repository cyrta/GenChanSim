/*---------------------------------------------------------------------------*/
/* doppler.c - Introduce a doppler shift on the chosen IIR filter output as a*/
/*         complex multiply by a rotating phasor                             */
/*---------------------------------------------------------------------------*/
/*                                                                           */
/*

   copyright (c) 1994 The MITRE Corporation Bedford, MA

   $Id: doppler.c,v 2.2 1994/07/25 19:20:47 jjb Exp $
   $Log: doppler.c,v $
 * Revision 2.2  1994/07/25  19:20:47  jjb
 * added copyright notice
 *

*/
#include <math.h>
#include <complex.h>
#include <propchan.h>

#define FRACMASK 0x007fffff  /* 2**23-1 used to mask fractional index*/

static char rcsid[] = "$Id: doppler.c,v 2.2 1994/07/25 19:20:47 jjb Exp $";

void doppler(unsigned int phasor[], unsigned int phaseincr[],
                      float trigtbl[], fcomplex iqbuf[],
                      int ntaps, int start_tap, float doppscal)
{
	float iout, qout;
	int cosndex, sinndex, filtnum;
	float alpha, sinphi, cosphi;
	unsigned int *phasor_ptr, *phaseincr_ptr;

	phasor_ptr = phasor + start_tap;
	phaseincr_ptr = phaseincr + start_tap;
	for (filtnum=0; filtnum < ntaps; ++filtnum) {
		/* rotate phasor by increment */
		/* Increment was replaced by decrement to solve a spectrum inversion
		 * problem in the HFWB facility.
		 * This fudge results in the sign of Doppler being flipped
		 */
	
		*phasor_ptr -= *phaseincr_ptr;
	
		/* get integer part of indices to sin and cos table*/
	
		cosndex = (int) (*phasor_ptr >> TRIGSHFT);
		sinndex = (cosndex + TRIGOFST) & TRIGMASK;
		  
		/* get fractional part of indices to sin and cos table*/
		  
		alpha = (float) (*phasor_ptr & FRACMASK) * doppscal;
		/* calculate sin(phi) and cos(phi) by linear interpolation */
		  
		sinphi = trigtbl[sinndex] + alpha* (trigtbl[sinndex+1] -
						   trigtbl[sinndex]);
		cosphi = trigtbl[cosndex] + alpha* (trigtbl[cosndex+1] -
						   trigtbl[cosndex]);
	
		/* next 3 lines for debug only*/
	
		/* do complex mult to implement actual phase rotation needed*/
		  
		iout = cosphi * iqbuf[filtnum].r -
		sinphi * iqbuf[filtnum].i;
		qout = cosphi * iqbuf[filtnum].i +
		sinphi * iqbuf[filtnum].r;
		iqbuf[filtnum].r = iout;
		iqbuf[filtnum].i = qout;
	
		phasor_ptr++;
		phaseincr_ptr++;
	}
}
