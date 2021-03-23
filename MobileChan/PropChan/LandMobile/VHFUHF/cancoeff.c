/* calculates some of the coefficients in channel
 * r.coutts 

   copyright (c) 1994 The MITRE Corporation Bedford, MA

   $Id: cancoeff.c,v 3.0 1994/08/30 18:45:53 jjb Exp $
   $Log: cancoeff.c,v $
 * Revision 1997/07/30 cmk
 * fixed so that tw->maxdplShift is used here for
 * Doppler power spectrum limits, and tw->dplShift[i] is
 * freed up for applying overall Doppler Shifts to the
 * Doppler power spectra for the scatter paths.
 *
 * Revision 3.0  1994/08/30  18:45:53  jjb
 * added MACROCELL model.
 *
 * Revision 2.2  1994/07/25  18:57:02  jjb
 * added copyright notice
 *

 */
#include <mixedchan.h>
#include <math.h>

static char rcsid[] = "$Id: cancoeff.c,v 3.0 1994/08/30 18:45:53 jjb Exp $";

void cancoeff(TAPWGTS *tw) {
	int		i;		/* index */
	int totalTaps = 0;

	/* init freq and time window transition lengths */
	tw->freqWinTransLen = 120.0;
	tw->timeWinTransLen = 30.0 * tw->sampleRate / ONE_MHZ;

	/* determine the number of taps required */
	for(i = 0; i < tw->nPaths; i++) {
		if(tw->mode[i] == DIST) {	/* if disturbed mode... */
		  if ( tw->model == MACROCELL_MODEL ) {
/*		         tw->nTaps[i] = tw->scatterSampleRate * tw->expTau[i]
			   * (-1.0 * log(tw->expThresh[i])) + 1; */
		    /* set ntaps so that 5 * Tau of exponent is non-zero */
		    tw->nTaps[i] = tw->scatterSampleRate * tw->expTau[i] * 5 + 1;


		  } else {   /* JTC model */
			/* units = nS*Hz = 1e6*uS/Hz = Seconds/Hz */
			tw->nTaps[i] =
				sqrt(3.0) * tw->multiPathSpr[i] * tw->sampleRate * 1.0e-6; 
		  }
		} else { 					/* else normal mode (discrete) */
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
	      if ( tw->model == MACROCELL_MODEL )
		fprintf(stderr,"path# %d, scatter, no. of taps=%d\n",
			i,tw->nTaps[i]);
	      else
		fprintf(stderr,"path# %d, JTC, no. of taps=%d\n",
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
			tw->snapshot[i] = 4.0 * 16.0 * tw->maxdplShift;

/*			tw->snapshot[i] = 4.0 * 16.0 * tw->dplShift[i] *
				tw->sampleRate / ONE_MHZ;		*/
		} else {					
	/* ??		tw->snapshot[i] = 250.0 * tw->dplShift[i]; */
		  tw->snapshot[i] = 50.0 * tw->maxdplShift;
		}
	}
}
