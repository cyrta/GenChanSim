#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>
#include <airairchan.h>
#define VERTICAL 0

void
relpow(float carFreq,
	   float *air_hts,
	   float dielConst,
	   float surfCon,
	   float rmsHt,
	   int polar,
	   float antGain,
	   float *gsps,        /* returned value */
	   float *gscs)        /* returned value */
{

  /* Numbers in ()'s in comments after an equation refer to the
     equation number in Phil Bello's Interim Report, Feb. 1996. */


  fcomplex z,ezero,Ctemp,sinth,costh,gamma;
  float term1,term2,gamma_abssqr,wl;
  float gainratio,scatgain,specgain;

  wl = LIGHTSPEED/carFreq;  /* wavelength in meters */
  /*printf("RELPOW: wavelength in meters is = %g\n",wl);
  
  printf("RELPOW: air heights pointer: %x\n",air_hts);
  printf("RELPOW: air_hts %f %f\n",air_hts[0],air_hts[1]);*/
  term1 = (air_hts[0] + air_hts[1]) * (air_hts[0] + air_hts[1]);
  /* printf("RELPOW: term1 = %g\n",term1);*/

  term2 = (air_hts[1] - air_hts[0]) * (air_hts[1] - air_hts[0]);
  /*  printf("RELPOW: term2 = %g\n",term2);*/

/* Although this operation is real only, need complex storage for future use. */

  sinth.r = (air_hts[0] + air_hts[1])/
	        ((float)sqrt(1.0 + term1));  /* (8.34) */
  sinth.i = 0;
  /*  printf("RELPOW: sinth = %g\n",sinth.r);*/

  costh.r = 1.0/((float)sqrt(1.0 + term1));  /*  */
  /*  printf("RELPOW: costh = %g\n",costh.r);*/
  costh.i = 0;

/*
 * ezero is a COMPLEX
 */

  ezero.r = dielConst;
  /*  printf("RELPOW: Real part of ezero = %g\n",ezero.r);*/
  ezero.i = -60.0*surfCon*wl;
  /*  printf("RELPOW: Imag part of ezero = %g\n",ezero.i);*/

  Ctemp.r = ezero.r - (costh.r*costh.r);
  Ctemp.i = ezero.i;

  z = Csqrt(Ctemp);  /* horizontal polarization */
  /*  printf("RELPOW: Horizontal z = %g, %g\n",z.r,z.i);*/

  if(polar == 0)  /* Vertical polarization */
	 z =  Cdiv(z,ezero);
  /*  printf("RELPOW: Vertical z = %g, %g\n",z.r,z.i);*/

  gamma  = Cdiv(Csub(sinth,z),Cadd(sinth,z));

  /*  printf("RELPOW: gamma = %g, %g\n",gamma.r,gamma.r);*/

  gamma_abssqr = (float)((double)(gamma.r*gamma.r) + 
						   (double)(gamma.i*gamma.i));
  /*  printf("RELPOW: abs(gamma)^2 = %g\n",gamma_abssqr);*/

/*
 * finished with complex operations - real only from here
 */

/*
 * fill gsps and gscs locations before return
 */

   specgain = gamma_abssqr*antGain*(1 + term2)/(1 + term1)
		   * exp(-(4*PI*rmsHt/wl)*(4*PI*rmsHt/wl)*term1/(1 + term1));
   *gsps = specgain;
   /*   printf("RELPOW: gsps = %g\n",*gsps);*/

   scatgain = gamma_abssqr*antGain*(1 + term2)/(1 + term1)
		   * (1 - exp(-(4*PI*rmsHt/wl)*(4*PI*rmsHt/wl)*term1/(1 + term1)));
   *gscs = scatgain;

   /*   printf("RELPOW: gscs = %g\n",*gscs);*/

   gainratio = specgain/scatgain;

   /*   printf("\n RELPOW: Ratio of specular to scatter path gains: %g\n\n",gainratio);*/


/* Convert to dB: */

   *gsps = 20*(float)log10(*gsps);
   *gscs = 20*(float)log10(*gscs);
   /*   printf("RELPOW: gsps and gscs in dB = %g %g\n",*gsps,*gscs);*/

}
		   
	   
