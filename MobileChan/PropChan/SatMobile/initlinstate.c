/* Determines the initial system state and the duration, in samples for the Lin model */

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <seed.h>
#include "flatfade.h"

void
initlinstate(int *sysstate,   /* output=CLEAR, SHADOWED, or BLOCKED                     */
  	     int *num,        /* output=duration of state in samples                    */
             float *Aprob,    /* input=probability that system is in state              */
             float *Qprob,    /* input=state transition probabilities                   */
             double distance, /* input=state transition distance (m)                    */
             double samsm)    /* input=samples per meter traveled (samplerate/velocity) */
{
  double prob;
  double Dis;

  /*printf("INITLINSTATE: state transition distance is %g.\n",distance);
  printf("INITLINSTATE: samples per m traveled is %g.\n",samsm);
  printf("INITLINSTATE: P(clear state) is %g.\n",Aprob[0]);
  printf("INITLINSTATE: P(shadowed state) is %g.\n",Aprob[1]);
  printf("INITLINSTATE: P(blocked state) is %g.\n",Aprob[2]);
  printf("INITLINSTATE: Transition Probability Q[0] is %g.\n",Qprob[0]);
  printf("INITLINSTATE: Transition Probability Q[1] is %g.\n",Qprob[1]);
  printf("INITLINSTATE: Transition Probability Q[2] is %g.\n",Qprob[2]);
  printf("INITLINSTATE: Transition Probability Q[3] is %g.\n",Qprob[3]);
  printf("INITLINSTATE: Transition Probability Q[4] is %g.\n",Qprob[4]);
  printf("INITLINSTATE: Transition Probability Q[5] is %g.\n",Qprob[5]);
  printf("INITLINSTATE: Transition Probability Q[6] is %g.\n",Qprob[6]);
  printf("INITLINSTATE: Transition Probability Q[7] is %g.\n",Qprob[7]);
  printf("INITLINSTATE: Transition Probability Q[8] is %g.\n",Qprob[8]);*/
  fflush(stdout);

  *sysstate = CLEAR;  /* Initialize state */

  prob = drand48();  /* Select a number from a uniform [0,1) distribution. */
  /*printf("INITLINSTATE: state prob = %g.\n",prob);*/
  fflush(stdout);

  if (prob <=  (double) Aprob[0])
  {
    *sysstate = CLEAR;  /* Clear state condition */
    Dis = distance / (1.0 - (double) Qprob[0]); /* Av. dist. traveled in clear state. */
  }
  else if (prob > (double)(Aprob[0] + Aprob[1]))
  {
    *sysstate = BLOCKED; /* Blocked state condition */
    Dis = distance / (1.0 - (double) Qprob[8]); /* Av. dist. traveled in blocked state. */
  }
  else
  {
    *sysstate = SHADOWED; /* Shadowed state condition */
    Dis = distance / (1.0 - (double) Qprob[4]); /* Av. dist. traveled in shadowed state. */
  }

  /***********************************************************
  * Calculate the number of samples the system spends in the *
  * state, where the duration in seconds is exponentially    *
  * distributed.                                             *
  ***********************************************************/
  /*printf("INITLINSTATE: sysstate = %d.\n",*sysstate);
  printf("INITLINSTATE: E(m traveled in state %d) is %g.\n",*sysstate,Dis);*/

  prob = drand48();  /* Select a new number from a uniform [0,1) distribution. */
  /*printf("INITLINSTATE: duration prob = %g.\n",prob);*/

  *num = rint( - (samsm) * Dis * log( 1.0-prob ) );

  /*printf("INITLINSTATE: num = %d.\n",*num);*/

}
