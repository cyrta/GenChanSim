/* calculates some of the coefficients in channel
 * r.coutts

   copyright (c) 1994 The MITRE Corporation Bedford, MA

   $Id: coeff.c,v 2.2 1994/07/25 19:29:19 jjb Exp $
   $Log: coeff.c,v $
 * Revision 2.2  1994/07/25  19:29:19  jjb
 * added copyright notice.
 *

 */
#include <propchan.h>
#include <math.h>

static char rcsid[] = "$Id: coeff.c,v 2.2 1994/07/25 19:29:19 jjb Exp $";

void coeff(TAPWGTS *tw) {
	int		i;		/* index */

	/* init freq and time window transition lengths */
	tw->freqWinTransLen = 120.0;
	tw->timeWinTransLen = 30.0 * tw->sampleRate / ONE_MHZ;

	/* determine the number of taps required */
	for(i = 0; i < tw->nPaths; i++) {
		if(tw->mode[i] == DIST) {	/* if disturbed mode... */
			/* units = uS*Hz = 1e6*uS/Hz = Seconds/Hz */
			tw->nTaps[i] =
				sqrt(3.0) * tw->multiPathSpr[i] * tw->sampleRate * 1.0e-6;
		} else { 					/* else normal mode */
			/* number of taps increases with the square of sample rate */
			tw->nTaps[i] = (1.2 * tw->sampleRate * 1e-6 * tw->groupDelay[i] + 
				100.0) * tw->sampleRate * 1e-6;
			if (tw->nTaps[i]>NPTS) {
				printf("Warning from propchan: group delay for path %d is too "
					"large.  Shrunk from %d taps to %d taps.\n", i+1, 
					tw->nTaps[i], NPTS);
				tw->nTaps[i] = NPTS;
			}
		}
		if(tw->nTaps[i]/2 == (float)tw->nTaps[i]/2) tw->nTaps[i]++;
	}

	/* calc snap shot rate for ea path */
	for(i = 0; i < tw->nPaths; i++) {
		if(tw->mode[i] == DIST) {	/* if disturbed mode... */

/* Removed dependence on sample rate 4/12/94 c.a.n. tw->snapshot[i] = 50.0 * sqrt((tw->dplSpread[i] * 
				tw->dplSpread[i]) +  2 * tw->dplShift[i] * tw->dplShift[i]) *
				tw->sampleRate / ONE_MHZ; */

			tw->snapshot[i] = 50.0 * sqrt((tw->dplSpread[i] * 
				tw->dplSpread[i]) +  2 * tw->dplShift[i] * tw->dplShift[i]);

		} else {					/* if normal mode... */
/* Removed dependence on sample rate 4/12/94 c.a.n. tw->snapshot[i] = 50.0 * tw->dplShift[i] *
				tw->sampleRate / ONE_MHZ; */

			tw->snapshot[i] = 50.0 * tw->dplShift[i];
		}
	}
}
