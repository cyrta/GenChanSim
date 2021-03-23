/* calculates some of the coefficients in channel
 * r.coutts 

   copyright (c) 1994 The MITRE Corporation Bedford, MA

   $Id: cancoeff.c,v 3.0 1994/08/30 18:45:53 jjb Exp $
   $Log: cancoeff.c,v $
 * Revision LL 0.0 1997 cmk
 * This version of cancoeff.c in this directory forces the number 
 * taps to 1 for scatter as well as discrete paths for the flatfade model.
 *
 * Revision 3.0  1994/08/30  18:45:53  jjb
 * added MACROCELL model.
 *
 * Revision 2.2  1994/07/25  18:57:02  jjb
 * added copyright notice
 *

 */
#include <flatfade.h>
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
	  tw->nTaps[i] = 1; /* Force just one tap to be used for flat fading model */
                   	    /* and one tap for each of the discrete paths */
          /*printf("CANCOEFF: the number of taps for path %d is %d.\n",i,tw->nTaps[i]);*/
  	  if(tw->nTaps[i]/2 == (float)tw->nTaps[i]/2) tw->nTaps[i]++;
	  totalTaps += tw->nTaps[i];
	}
        /*printf("CANCOEFF: the total number of taps is %d.\n",totalTaps);*/

	/* check if too many taps requested */
	if ( totalTaps > MAXTAPS ) 
        {
	  fprintf(stderr," too many taps. need %d,but max = %d\n",
		  totalTaps,MAXTAPS);
	  for(i=0; i<tw->nPaths; ++i) 
          {
	    if ( tw->mode[i] == DIST )
	      fprintf(stderr,"path# %d, scatter, no. of taps=%d\n",
		      i,tw->nTaps[i]);
            else
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
	  } 
          else {					
	    tw->snapshot[i] = 50.0 * tw->scaledsigma[i];
	  }
	}
}

