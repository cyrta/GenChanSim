/* Gen Power delay spectrum weights for exponential model
 *
 * 'exp_thresh' truncation thresh for exponential model
 *     it is the value where the tails are truncated
 * NOTE: currently 'exp_thresh' is only used in cancoeff.c to
 *      compute the number of taps since it is deterministic where
 *      the threshold will truncate the exponential

   copyright (c) 1994 The MITRE Corporation Bedford, MA

   $Id: expweight.c,v 3.0 1994/08/30 18:31:22 jjb Exp $
   $Log: expweight.c,v $
 * Revision 3.0  1994/08/30  18:31:22  jjb
 * created for version 3.0 of software.
 *

 */

#include <stdio.h>
#include <math.h>
#include <JTCchan.h>

static char rcsid[] = "$Id: expweight.c,v 3.0 1994/08/30 18:31:22 jjb Exp $";

void expweight(
	       float weight_buff[],  /* weights (returned) */
	       int sttapnum[],	     /* starting tap number inside 'weight_buff' */
	       int ntaps[],	     /* # taps of each path */
	       float sample_rate,    /* sample rate of simulator */
	       float exp_tau,	     /* exponential time constant */
	       int parnum)	     /* current partition (path) number */
{
  int i;
  float *buff_ptr;
  float buff[MAXTAPS], total_power;

  /* simple (possibly unnecessary) check for error */
  if(ntaps[parnum] == 0) {
    printf("Error from expweight(): number of taps is zero\n");
    exit(1);
  }

  total_power = 0.;

  /* compute the exponential and its power */


  for(i=0; i<ntaps[parnum]; ++i) {
    buff[i] = exp(-1.0 * (double)i / (exp_tau * sample_rate));
    total_power += buff[i];
  }
  /* transfer normalized delay weights to output buffer*/

  buff_ptr = weight_buff + sttapnum[parnum];
  for (i = 0; i < ntaps[parnum]; i++) 
    *buff_ptr++ = sqrt(buff[i]/total_power);

}
