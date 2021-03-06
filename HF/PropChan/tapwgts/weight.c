/* Gen Power delay spectrum weights for thin screen phase model
 *
 * 'thinphase_thresh' truncation thresh for thin phase screen model
 *     it is the ratio beteween the peak weight and the smallest weight
 *     before the tails are truncated

   copyright (c) 1994 The MITRE Corporation Bedford, MA

   $Id: weight.c,v 2.2 1994/07/25 19:20:47 jjb Exp $
   $Log: weight.c,v $
 * Revision 2.2  1994/07/25  19:20:47  jjb
 * added copyright notice
 *

 */

#include <stdio.h>
#include <math.h>
#include <complex.h>
#include <propchan.h>

#define TAU 8.e-7

static char rcsid[] = "$Id: weight.c,v 2.2 1994/07/25 19:20:47 jjb Exp $";

void weight(
	float weight_buff[],		/* weights (returned) */
	int sttapnum[],				/* starting tap number inside 'weight_buff' */
	float delay_spread[],		/* delay spread */
	int ntaps[],				/* # taps of each path */
	float thinphase_c,			/* thin phase screen C value */
	float thinphase_thresh,		/* thin phase threshold */
    int parnum)					/* current partition (path) number */
{
	int i, j, search, numneg, numpos, peakindex, numtaps;
	float w, psi, *buff_ptr, *temp_buff_ptr;
	float neg_buff[MAXTAPS], pos_buff[MAXTAPS], total_power, peak, lastw;
	float qsc(float psi, float c);
	int	p;						/* next partition index */

	total_power = 0.;
	peak = 2.1;
	peakindex = 0;
	lastw =0;
	search = 1;

	/* do positive values of delay */
	i = 0;
	psi = 0;
	w = qsc(psi,thinphase_c);
	while ((w/peak >= thinphase_thresh) && (i < MAXTAPS)) {
		psi = i*TAU/(delay_spread[parnum]*1.e-6);
		lastw = w;
		w = qsc(psi,thinphase_c);
		total_power += w;
		pos_buff[i] = w;

		/* we started with peak=2. The fuction is montonically increasing then
		 * monotonically decreasing. As soon as we reach the decreasing portion
		 * lastw will be greater than w and we set peak = w and end the search
		 */

		if ((search == 1) && (lastw > w)) {
			search = 0;
			peak = lastw;
			peakindex = i;
		}
		i++;
	}
	numpos = i;

	/* do negative values of delay */
	i = 0;
	psi = -(i+1)*TAU/(delay_spread[parnum]*1.e-6);
	w = qsc(psi,thinphase_c);
	while ((w/peak >= thinphase_thresh) && (i < MAXTAPS)) {
		psi = -(i+1)*TAU/(delay_spread[parnum]*1.e-6);
		w = qsc(psi,thinphase_c);
		total_power += w;
		neg_buff[i] = w;
		i++;
	}
	numneg = i;

	/* Check if total number of weights is is greater than MAXTAPS. 
	 * If total is greater, truncate accordingly */

	numtaps = numneg + numpos;
	if (numtaps > MAXTAPS) {
		/* trucate monotonically increasing section */
		i = numneg + peakindex;
		numneg = i * ((float) MAXTAPS / (float) numtaps) - peakindex;

		/* trucate monotonically decreasing section */
		j = numpos - peakindex;
		numpos = j * ((float) MAXTAPS / (float) numtaps) + peakindex;
		numtaps = numneg+numpos;
	}

	/* transfer negative delay weights in reverse order to output buffer*/

	temp_buff_ptr = neg_buff+numneg;
	buff_ptr = weight_buff + sttapnum[parnum];
	for (j = 0; j < numneg; j++) *buff_ptr++ = *(--temp_buff_ptr);

	/* transfer positive delay weights in forward order to output byffer*/

	for (j = 0; j < numpos; j++) *buff_ptr++ = pos_buff[j];

	/* normalize amplitude weighting for unity total power */
	buff_ptr = weight_buff + sttapnum[parnum];
	for (j = 0; j < numtaps; j++) {
		*buff_ptr = sqrt(*buff_ptr/total_power);
		buff_ptr++;
	}
	/* simple (possibly unnecessary) check for error */
	if(ntaps[parnum] == 0) {
		printf("Error from weight(): number of taps is zero\n");
		exit(1);
	}
	/* ajust remaining partitions if the number of taps was changed */
	if(numtaps != ntaps[parnum]) {	/* if increasing taps... */
		/* adjust the rest of the buffer */
		p = parnum + 1;						/* index to next set partition */
		while(sttapnum[p] && p < NPATHS) {	/* partition exist? */
			sttapnum[p] += numtaps - ntaps[parnum];	/* increase tap index */
			p++;
		}
	}
	/* adjust the number of taps */
	ntaps[parnum] = numtaps;
}
