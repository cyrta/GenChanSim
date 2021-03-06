/*
 * iir.c - Implement an 8 pole IIR filter as 4 cascaded canonic biquad
 *         sections, where one section looks like:
 *
 *                          1
 *  y0 ---+-----------+----->-----+--- y1
 *        |           |           |
 *        |           |  -1       |
 *        |           V Z         |
 *        |           |           |
 *        |    -b1    |     a1    |
 *        +-----<-----+----->-----+
 *        |           |           |
 *        |           |  -1       |
 *        |           V Z         |
 *        |           |           |
 *        |    -b2    |     a2    |
 *        `-----<-----+----->-----'
 *
 *  Each call to iir computes one iteration of one filter.
 *
 *  These equations are being implemented:
 *  y(0,n) = x(n)
 *  for (i = 0; i < 4; i++)
 *  {
 *    d(i,n) = y(i-1,n) - b2(i) * d(i,n-2) - b1(i) * d(i,n-1)
 *    y(i,n) = a2(i) * d(i,n-2) + a1(i) * d(i,n-1) + d(i,n)
 *  }
 *  y(n) = y(3,n)
 *

   copyright (c) 1994 The MITRE Corporation Bedford, MA

   $Id: iir.c,v 2.2 1994/07/25 19:06:52 jjb Exp $
   $Log: iir.c,v $
 * Revision 2.2  1994/07/25  19:06:52  jjb
 * added copyright notice
 *

 *
 * written by: mark sandler
 * modified by: rich coutts 8/21/92
 */

#include <math.h>
#include <complex.h>
#include <filter.h>

static char rcsid[] = "$Id: iir.c,v 2.2 1994/07/25 19:06:52 jjb Exp $";

void iir(
	fcomplex	*iqbuf,			/* complex point to be filtered */
	FILTER		*filtbank,		/* filter bank coeff */
	DELAY_4		*d[])			/* delay buffers */
{
	/* filter states */
	static float yr0 = 0, yr1 = 0, yr2 = 0, yi0 = 0, yi1 = 0, yi2 = 0;
	FILTER		*f;				/* pointer to filter */
	DELAY_4		*temp;			/* temporary ptr to delay */

	/* init pointers */
	f = filtbank;

	/* do one iteration of one filter */
	yr0 = f->ampl * iqbuf->r;
	yi0 = f->ampl * iqbuf->i;

	d[0]->dr1 = yr0 - f->sect[0].b2 * d[1]->dr1 - f->sect[0].b1 * d[2]->dr1; 
	yr1 = f->sect[0].a2 * d[1]->dr1 + f->sect[0].a1 * d[2]->dr1 + d[0]->dr1;
	d[0]->dr2 = yr1 - f->sect[1].b2 * d[1]->dr2 - f->sect[1].b1 * d[2]->dr2; 
	yr2 = f->sect[1].a2 * d[1]->dr2 + f->sect[1].a1 * d[2]->dr2 + d[0]->dr2;

	d[0]->di1 = yi0 - f->sect[0].b2 * d[1]->di1 - f->sect[0].b1 * d[2]->di1; 
	yi1 = f->sect[0].a2 * d[1]->di1 + f->sect[0].a1 * d[2]->di1 + d[0]->di1;
	d[0]->di2 = yi1 - f->sect[1].b2 * d[1]->di2 - f->sect[1].b1 * d[2]->di2; 
	yi2 = f->sect[1].a2 * d[1]->di2 + f->sect[1].a1 * d[2]->di2 + d[0]->di2;

	iqbuf->r = yr2;
	iqbuf->i = yi2;

	/* rotate pointers for next call */
	temp = d[0];
	d[0] = d[1];
	d[1] = d[2];
	d[2] = temp;
}
