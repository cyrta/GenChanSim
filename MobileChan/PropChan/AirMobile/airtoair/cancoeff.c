/* calculates some of the coefficients in channel
 * r.coutts 

   copyright (c) 1994 The MITRE Corporation Bedford, MA

   $Id: cancoeff.c,v 0.0 1996/04/30 17:30:00 cmk Exp $
   $Log: cancoeff.c,v $

 * Lincoln Lab Revision LL0.0
 * Changed so does not depend on MACROCELL vs JTC model (since these
 * are not applicable for air to air!) and changed expTau
 * parameter to be the tau for the bessel function delay power
 * spectrum form.
 * 
 * Revision 3.0 1994/08/30 18:45:53 jjb added MACROCELL model.
 *
 * Revision 2.2  1994/07/25  18:57:02  jjb
 * added copyright notice
 *

 */
#include <airairchan.h>
#include <math.h>

/*static char rcsid[] = "$Id: cancoeff.c,v 3.0 1994/08/30 18:45:53 jjb Exp $";*/
static char rcsid[] = "$Id: cancoeff.c,v 0.0 1996/04/30 17:30:00 cmk Exp $";

void cancoeff(TAPWGTS *tw) {
	int		i;		/* index */
	int totalTaps = 0;

	/* init freq and time window transition lengths */
	tw->freqWinTransLen = 120.0;
	tw->timeWinTransLen = 30.0 * tw->sampleRate / ONE_MHZ;

	/* determine the number of taps required */
	for(i = 0; i < tw->nPaths; i++) {
		if(tw->mode[i] == DIST) {	/* if scatter path... */

		  /* set ntaps so that delay power spectrum is covered */
		  tw->nTaps[i] = tw->scatterSampleRate * tw->Tau + 1;
                  if(tw->nTaps[i] < 3) tw->nTaps[i] = 3;
		  if(tw->nTaps[i] > 350) tw->nTaps[i] = 350;
		} else { 		/* else discrete path */
		        tw->nTaps[i] = 1;
		}
		if(tw->nTaps[i]/2 == (float)tw->nTaps[i]/2) tw->nTaps[i]++;
		totalTaps += tw->nTaps[i];
	}

	/* check if too many taps requested */
	if ( totalTaps > MAXTAPS ) {
	  fprintf(stderr," too many taps. need %d,but max = %d\n",
		  totalTaps,MAXTAPS);
	  for(i=0; i<tw->nPaths; ++i) {
	    if ( tw->mode[i] == DIST ) {
		fprintf(stderr,"path# %d, scatter, no. of taps=%d\n",
			i,tw->nTaps[i]);
	    } else
	      fprintf(stderr,"path# %d, discrete, no. of taps=%d\n",
		      i,tw->nTaps[i]);
	  }
	  exit(1);
	}

	/* calc snap shot rate for ea path */
	for(i = 0; i < tw->nPaths; i++) {
		if(tw->mode[i] == DIST) {	
				/* For mobile comm, snapshot rate is based only on
			        Doppler shift. IIR16 is sampled at 4X, then the signal
					              goes through a 16X interpolator */
			tw->snapshot[i] = 4.0 * 16.0 * tw->scaledsigma[i];
		} else {					
		  tw->snapshot[i] = 50.0 * tw->scaledsigma[i];
		}
	}
}



