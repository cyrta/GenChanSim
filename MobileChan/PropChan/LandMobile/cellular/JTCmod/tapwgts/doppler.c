/*---------------------------------------------------------------------------*/
/* doppler.c - Introduce a doppler shift on the chosen IIR filter output as a*/
/*         complex multiply by a rotating phasor                             */
/*---------------------------------------------------------------------------*/
/*                                                                           */
/*

   copyright (c) 1994 The MITRE Corporation Bedford, MA

   $Id: doppler.c,v 3.0 1994/08/30 18:32:40 jjb Exp $
   $Log: doppler.c,v $
 * Revision 3.0  1994/08/30  18:32:40  jjb
 * added step as argument to allow code to handle steps of greater than
 * one.
 * cleaned up sine table lookup usage to indicate that table gives -sine
 * not sine.
 *
 * Revision 2.2  1994/07/25  18:47:02  jjb
 * added copyright notice
 *

*/
#include <math.h>
#include <complex.h>
#include <JTCchan.h>

#define FRACMASK 0x007fffff  /* 2**23-1 used to mask fractional index*/

static char rcsid[] = "$Id: doppler.c,v 3.0 1994/08/30 18:32:40 jjb Exp $";

void doppler(unsigned int phasor[], unsigned int phaseincr[],
                      float trigtbl[], fcomplex iqbuf[],
                      int ntaps, int start_tap, float doppscal, int step)
{
	float iout, qout;
	int cosndex, sinndex, filtnum;
	float alpha, sinphi, cosphi;
	unsigned int *phasor_ptr, *phaseincr_ptr;

	phasor_ptr = phasor + start_tap;
	phaseincr_ptr = phaseincr + start_tap;
	for (filtnum=0; filtnum < ntaps; ++filtnum) {
		/* rotate phasor by increment */

		*phasor_ptr += (*phaseincr_ptr) * (unsigned int)step;
	
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
	
		/* negate sine since table lookup really found -sine */

		sinphi = -1.0 * sinphi;
	
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
