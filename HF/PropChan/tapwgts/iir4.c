/* iir4.c - Implement an 8 pole IIR filter as 4 cascaded canonic biquad
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

   $Id: iir4.c,v 2.2 1994/07/25 19:20:47 jjb Exp $
   $Log: iir4.c,v $
 * Revision 2.2  1994/07/25  19:20:47  jjb
 * added copyright notice
 *

 *
 * written by: mark sandler
 * modified by: rich coutts 8/21/92
 */

#include <math.h>
#include <complex.h>
#include <propchan.h>

static char rcsid[] = "$Id: iir4.c,v 2.2 1994/07/25 19:20:47 jjb Exp $";

void iir4(
	fcomplex	iqbuf[],		/* filtered numbers (modified) */
	float		w[],			/* weights */
	FILTER		f[],			/* filter bank */
	int			ntaps,			/* number of taps */
	int			path)			/* path index */
{
	static int	d0[NPATHS];		/* index to top delay memory of filter */
	static int	d1[NPATHS];		/* index to middle delay memory of filter */
	static int	d2[NPATHS];		/* index to bottom delay memory of filter */
	static int tap[NPATHS];		/* indices into d.[] arrays (above) */
	static int totalntaps = 0;	/* total number of taps */
	static DELAY_4	*dline[3];	/* memory delays of MAXTAPS filters */
	float yr0, yr1, yr2, yr3, yr4, yi0, yi1, yi2, yi3, yi4; /* filter states */
	DELAY_4 *dm0;				/* pointer to top delay memory */
	DELAY_4	*dm1;				/* pointer to middle delay memory */
	DELAY_4 *dm2;				/* pointer to bottom delay memory */
	int filtnum;				/* index to filter */
	static int firstcall = 1;	/* 1st call to function flag */
	int i;						/* index */

	/* get memory for delays */
	if(firstcall) {
		firstcall = 0;
		for(i = 0; i < 3; i++) {
			dline[i] = (DELAY_4 *)calloc(sizeof(DELAY_4), MAXTAPS);
		}
		for(i = 0; i < NPATHS; i++) {
			d0[i] = 0;
			d1[i] = 2;
			d2[i] = 1;
			tap[i] = -1;		/* index into d.[] arrays (above) */
		}
	}

	if(tap[path] == -1) {		/* if not memory allotted... */
		/* allot some memory */
		tap[path] = totalntaps;
		totalntaps += ntaps;
	}

	/* init pointers to be incremented */
	dm0 = &dline[d0[path]][tap[path]];
	dm1 = &dline[d1[path]][tap[path]];
	dm2 = &dline[d2[path]][tap[path]];
	for (filtnum = 0; filtnum < ntaps; ++filtnum) {
		yr0 = f->ampl * iqbuf[filtnum].r;
		yi0 = f->ampl * iqbuf[filtnum].i;

		dm0->dr1 = yr0 - f->sect[0].b2 * dm2->dr1 - f->sect[0].b1 * dm1->dr1; 
		yr1 = f->sect[0].a2 * dm2->dr1 + f->sect[0].a1 * dm1->dr1 + dm0->dr1;
		dm0->dr2 = yr1 - f->sect[1].b2 * dm2->dr2 - f->sect[1].b1 * dm1->dr2; 
		yr2 = f->sect[1].a2 * dm2->dr2 + f->sect[1].a1 * dm1->dr2 + dm0->dr2;
		dm0->dr3 = yr2 - f->sect[2].b2 * dm2->dr3 - f->sect[2].b1 * dm1->dr3; 
		yr3 = f->sect[2].a2 * dm2->dr3 + f->sect[2].a1 * dm1->dr3 + dm0->dr3;
		dm0->dr4 = yr3 - f->sect[3].b2 * dm2->dr4 - f->sect[3].b1 * dm1->dr4; 
		yr4 = f->sect[3].a2 * dm2->dr4 + f->sect[3].a1 * dm1->dr4 + dm0->dr4;

		dm0->di1 = yi0 - f->sect[0].b2 * dm2->di1 - f->sect[0].b1 * dm1->di1; 
		yi1 = f->sect[0].a2 * dm2->di1 + f->sect[0].a1 * dm1->di1 + dm0->di1;
		dm0->di2 = yi1 - f->sect[1].b2 * dm2->di2 - f->sect[1].b1 * dm1->di2; 
		yi2 = f->sect[1].a2 * dm2->di2 + f->sect[1].a1 * dm1->di2 + dm0->di2;
		dm0->di3 = yi2 - f->sect[2].b2 * dm2->di3 - f->sect[2].b1 * dm1->di3; 
		yi3 = f->sect[2].a2 * dm2->di3 + f->sect[2].a1 * dm1->di3 + dm0->di3;
		dm0->di4 = yi3 - f->sect[3].b2 * dm2->di4 - f->sect[3].b1 * dm1->di4; 
		yi4 = f->sect[3].a2 * dm2->di4 + f->sect[3].a1 * dm1->di4 + dm0->di4;

		iqbuf[filtnum].r = *w * yr4;
		iqbuf[filtnum].i = *w * yi4;

		dm0++;
		dm1++;
		dm2++;
		f++;
		w++;
	}
	d0[path] = ++d0[path] % 3;
	d1[path] = ++d1[path] % 3;
	d2[path] = ++d2[path] % 3;
}
