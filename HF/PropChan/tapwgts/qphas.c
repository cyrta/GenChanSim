/* qphas.c   February 9      Hoa Ho & Mark Sandler

   copyright (c) 1994 The MITRE Corporation Bedford, MA

   $Id: qphas.c,v 2.2 1994/07/25 19:20:47 jjb Exp $
   $Log: qphas.c,v $
 * Revision 2.2  1994/07/25  19:20:47  jjb
 * added copyright notice
 *

 * Generate quadrature phase coefficients 
 * return two arrays ampreal[nfilter] and ampimag[nfilter]     
 */

#include <math.h>
#include <complex.h>
#include <propchan.h>

static char rcsid[] = "$Id: qphas.c,v 2.2 1994/07/25 19:20:47 jjb Exp $";

void qphas(
	int ntaps[],		/* # taps in ea partition */
	float group_delay[],/* group delay of each partition */
	int freqWinTransLen,/* number of sample for freq window transition */
	int timeWinTransLen,/* number of sample for time window transition */
	fcomplex normresp[],/* normal response */
	int sttapnum[],		/* starting tap # */
	int parnum,			/* partition # */
	float fs)			/* sampling rate (Hz) */
{
	int i,j;			/* temporary*/
	int lb;				/* lower index bound of impulse response*/
	int index1,index2;	/* temporary*/
	float alpha;		/* quad phas characteristic is alpha*omega*omega*/
	float theta,f;		/* temp float var*/
	float window;		/* saves the result of a window function */
	float *cmplxdat;	/* work area for fft */
	float *cdatptr1;	/* first pointer to real and imag part of cmplxdat;*/
	float *cdatptr2;	/* second pointer to real and imag part of cmplxdat;*/
	fcomplex *normresp_ptr;

	cmplxdat = (float *) calloc(2*NPTS, sizeof(float));

	if (cmplxdat == 0) {
		printf("Error from propchan in qphas(): Allocation error for cmplxdat");
		exit(1);
	}

	/* find desired length of quad phase response */
	alpha = group_delay[parnum] * 1.e-12;
	cdatptr1 = cmplxdat;

	for (i = 0; i<= (HNPTS-freqWinTransLen); i++) {
		f = (i*fs)/NPTS;
		theta = (PI*alpha*f*f);
		*(cdatptr1++) = cos(theta);	/* real part */
		*(cdatptr1++) = -sin(theta);	/* imag part */
	}
	for (j = freqWinTransLen-1; j >= 0; j--) {
		i = HNPTS - j;
		f = ((float) i) * (fs/NPTS);
		theta = (PI*alpha*f*f);
		window = ((float)(2*j+1))/((float)(2*freqWinTransLen));
		*(cdatptr1++) = window * cos(theta);
		*(cdatptr1++) = window * -sin(theta);
	}
	/* set negative freq components equal to positive freq components*/
	cdatptr1 = cmplxdat+2*NPTS-2;
	cdatptr2 = cmplxdat+2;
	for (i = 1; i < HNPTS ; i++) {
		*cdatptr1 = *(cdatptr2++);		/* real component*/
		*(cdatptr1+1) = *(cdatptr2++);	/* imag component*/
		cdatptr1 = cdatptr1 - 2;
	}
	/* take inverse fft, routine is based on an offset of one instead of zero */
	four1(cmplxdat-1, NPTS, -1);

	/* select significant time response*/
	lb = -ntaps[parnum] / 2;
	index1 = lb;

	/* Store coefficients */
	normresp_ptr = normresp + sttapnum[parnum];
	for (i=0; i<ntaps[parnum]; ++i) {
		index2 = (index1++) & (NPTS - 1);
		if (i<timeWinTransLen) {
			window = (i+0.5)/timeWinTransLen;
		} else {
			j = (ntaps[parnum] - 1) - i;
			if (j < timeWinTransLen) {
				window = (1.0*j+0.5)/timeWinTransLen;	    
			} else {
				window = 1;
			}
		}
		normresp_ptr->r = window * cmplxdat[2*index2]/(float)NPTS;
		normresp_ptr->i = window * cmplxdat[2*index2+1]/(float)NPTS;
		normresp_ptr++;
	}
	free(cmplxdat);
}
