/* Determines the system state duration, in samples */

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <seed.h>
#include "flatfade.h"

void
stateduration(
  	    int *num,     /* output = duration of state in samples                    */
	    int blstate,  /* input = YES for blocked state, = NO for clear state     */
	    double Dg,    /* input = mean distance traveled in clear state (m)        */
	    double Db,    /* input = mean distance traveled in blocked state (m)      */
            double samsm) /* input = samples per meter traveled (samplerate/velocity) */
{
  double prob;

  /*printf("STATEDURATION: E(m traveled in blocked state) is %g.\n",Db);
  printf("STATEDURATION: E(m traveled in clear state) is %g.\n",Dg);
  printf("STATEDURATION: samples per m traveled is %g.\n", samsm);*/

  /***********************************************************
  * Calculate the number of samples the system spends in the *
  * state, where the duration in seconds is exponentially    *
  * distributed.                                             *
  ***********************************************************/

  prob = drand48();  /* Select a number from a uniform [0,1) distribution. */
  /*printf("STATEDURATION: duration prob = %g.\n",prob);*/

  if (blstate)       /* System is in blocked state */
  {
    *num = rint( - samsm * Db * log( 1.0-prob ) );
  }
  else               /* System is in clear state */
  {
    *num = rint( - samsm * Dg * log( 1.0-prob ) );
  }
  /*printf("STATEDURATION: blstate = %d.\n",blstate);
  printf("STATEDURATION: num = %d.\n",*num);*/

}
