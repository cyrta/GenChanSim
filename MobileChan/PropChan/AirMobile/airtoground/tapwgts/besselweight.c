/* Gen Power delay spectrum weights for air-to-air bessel-function model
 * for the scatter path.
 *
   Derived from expweight.c, which is copyright (c) 1994 The MITRE
   Corporation Bedford, MA, by the MIT Lincoln Laboratory, December
   1995

   $Id: besselweight.c,v 1.0 1995/11/09 cmk Bessel $
   $Log: besselweight.c,v $
 * created for version LL of the software.
 *
 */

#include <stdio.h>
#include <math.h>
#include <airairchan.h>
extern float expbess(float, float, float);

static char rcsid[] = "   $Id: besselweight.c,v LL0.0 12/1995 cmk Bessel $";

void besselweight(
	       float weight_buff[],  /* weights (returned) */
	       int sttapnum[],	 /* starting tap number inside 'weight_buff' */
	       int ntaps[],	     /* # taps of each path */
	       float sample_rate,/* sample rate of simulator */
	       float airheight[2],     /* altitudes of aircrafts normalized to d */
	       float d,          /* ground separation of two aircraft (meters) */
	       float alpha,      /* RMS slope of the surface */
	       int parnum)	     /* current partition (path) number */
{
  int i;
  float *buff_ptr;
  float buff[MAXTAPS], total_power;
  float heights;
  float kconst;
  float bconst;
  float srb;
  float srbinv;
  float coeff;
  float tempbuff,retfloat,tempfloat;
  float zeta;
  int count;
 
  total_power = 0.;

  /* compute the bessel function form and its power */

  heights = airheight[0] + airheight[1];
  kconst = (airheight[0] * airheight[1]) / heights  * (1 / (1 + heights*heights));
  bconst = heights*heights * (1 + heights*heights);
  srb = sqrt(bconst);
  srbinv = 1 / srb;
  tempbuff = (srb + srbinv) / 2;
  tempfloat = fabs(srb - srbinv)/2;
  coeff = LIGHTSPEED /(kconst*alpha*alpha*d);

  /*printf("BESSELWEIGHT: kconst = %g, bconst = %g\n",kconst,bconst);
  printf("BESSELWEIGHT: a = %g, b = %g\n",tempbuff,tempfloat);*/

  for(count=0; count<ntaps[parnum]; count++){
    /* printf("\nBESSELWEIGHT: count is %d\n",count);*/
    zeta = coeff * (float)count / sample_rate;
    /* printf("BESSELWEIGHT: zeta = %g\n",zeta);*/
    retfloat = expbess(tempbuff,tempfloat,zeta);
    /* printf("BESSELWEIGHT: Exponential Bessel Function Output: %g\n",retfloat);*/
    buff[count] = coeff * retfloat;
    /* printf("BESSELWEIGHT: Delay Power Spectrum Output: %g\n",buff[count]);*/
    total_power += buff[count];
  }
  /*printf("\nBESSELWEIGHT: Last zeta = %g\n",zeta);
  printf("BESSELWEIGHT: Last Exponential Bessel Function Output: %g\n",retfloat);
  printf("BESSELWEIGHT: Last Delay Power Spectrum Output: %g\n",buff[count]);*/

  /* transfer normalized delay weights to output buffer*/

  buff_ptr = weight_buff + sttapnum[parnum];
  for (i = 0; i < ntaps[parnum]; i++) 
    *buff_ptr++ = sqrt(buff[i]/total_power);

}
