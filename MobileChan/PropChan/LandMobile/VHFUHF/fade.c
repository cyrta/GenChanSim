/* Generate slowly fading weights documented in section 2.2 of MTR "Wideband
 * HF Propagation, Narrowband Inteference, and Atmospheric Noise Models for
 * Link Peformance Evaluation" Phil Bello July 93
 *
 * Routine returns vector 'mult_fade' (one element per partition)
 
   copyright (c) 1994 The MITRE Corporation Bedford, MA

   $Id: fade.c,v 3.0 1994/08/30 18:49:58 jjb Exp $
   $Log: fade.c,v $
 * Revision 3.0  1994/08/30  18:49:58  jjb
 * updated rev number.
 *
 * Revision 2.2  1994/07/25  18:57:02  jjb
 * added copyright notice
 *

 *
 * Modified 8/9/93 Mark Sandler
 * Debugged 11/27/93 Rich Coutts
 * Debugged 12/1/93 Rich Coutts
 */

#include <math.h>
#include <mixedchan.h>
#define KSCALE 1000

static char rcsid[] = "$Id: fade.c,v 3.0 1994/08/30 18:49:58 jjb Exp $";

void fade(
	float mult_fade[],         /* path amplitude value vector */
	float tau_fade[],          /* tau parameter for fading channel */
	float r_fade[],            /* r parameter for fading channel */
	int npars,                 /* number of partitions to process */
	int init_flag)             /* 0 for run time, 1 for init */
{
	int				i;
	float			y;
	static float	yPrev = 0;	/* previous value of y */
	float			a;
	static float	w[NPATHS];
	static float	alpha_fade;
	static float	alpha_scale[NPATHS];
	static float	r_scale[NPATHS];
	static float	expon;
	static float	a_exp[NPATHS];

	if (init_flag != 0) {
		/* Initialization */
		if(npars > NPATHS) {
			printf("Error from 'propchan' in fade(): too many paths "
				"[%d > %d]\n", npars > NPATHS);
			exit(1);
		}
		expon = 1.0/ (float) KSCALE;
		for (i = 0; i < npars; i++) {
			a = pow(r_fade[i] * log(10.0), 2.0) / 40.0;
			a_exp[i] = pow(10.0, -a/20);
			alpha_fade = exp(-0.128/tau_fade[i]);
			alpha_scale[i] = pow(alpha_fade, expon);
			r_scale[i] = r_fade[i] * 
				sqrt(1.0 - (double)pow(alpha_fade, 2*expon));
		}
	} else {
		gauss48(w, npars);    /* get npars real Gaussian random samples */
		for (i = 0; i < npars; i++) {
			/* update path amplitude values */
			y = alpha_scale[i] * yPrev + r_scale[i] * w[i];
			yPrev = y;			/* save for next call */
			mult_fade[i] = a_exp[i] * pow(10.0, y/20.0);
		}
	}
}
