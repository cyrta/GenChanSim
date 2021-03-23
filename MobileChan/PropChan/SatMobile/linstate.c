/* Determines the system state and the duration, in samples, of the state */

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <seed.h>
#include "flatfade.h"

void
linstate(   float *Q,        /* input = state transition probabilities               */
	    double distance, /* input = state transition distance (m)                */
            double samsm,    /* input = samples per meter traveled (samplerate/velocity) */
            int *sysstate,   /* input/output = CLEAR, SHADOWED, or BLOCKED           */
  	    int *num)        /* output = duration of state in samples                */
{
  double prob;
  double A[2];  /* Transition probabilities to the next state */
  double Dis;

  /*printf("LINSTATE: Current state is %d.\n",*sysstate);
  printf("LINSTATE: state transition distance is %g.\n",distance); 
  printf("LINSTATE: samples per m traveled is %g.\n",samsm);*/

  prob = drand48();  /* Select a number from a uniform [0,1) distribution. */
  /*printf("LINSTATE: state prob = %g.\n",prob);*/

  /* Calculate state transition probabilites depending on current state */
  if(*sysstate == CLEAR)
  {
    A[0] = (double) (Q[1]/(1-Q[0]));  /* clear to shadowed */
    A[1] = (double) (Q[2]/(1-Q[0]));  /* clear to blocked  */
    /*printf("LINSTATE: Prob from clear to shadowed is %g\n",A[0]);
    printf("LINSTATE: Prob from clear to blocked is %g\n",A[1]);*/
    if (prob <= A[0])
    {
      *sysstate = SHADOWED; 
      Dis = distance / (1.0 - (double) Q[4]); /* Av. dist. traveled in shadowed state. */
    }
    else
    {
      *sysstate = BLOCKED;  
      Dis = distance / (1.0 - (double) Q[8]); /* Av. dist. traveled in blocked state. */
    } 
  }
  else if (*sysstate == SHADOWED)
  {
    A[0] = (double) (Q[3]/(1-Q[4]));  /* shadowed to clear */
    A[1] = (double) (Q[5]/(1-Q[4]));  /* shadowed to blocked  */
    /*printf("LINSTATE: Prob from shadowed to clear is %g\n",A[0]);
    printf("LINSTATE: Prob from shadowed to blocked is %g\n",A[1]);*/
    if (prob <= A[0])
    {
      *sysstate = CLEAR; 
      Dis = distance / (1.0 - (double) Q[0]); /* Av. dist. traveled in clear state. */
    }
    else
    {
      *sysstate = BLOCKED;  
      Dis = distance / (1.0 - (double) Q[8]); /* Av. dist. traveled in blocked state. */
    } 
  }
  else
  {
    A[0] = (double) (Q[6]/(1-Q[8]));  /* blocked to clear */
    A[1] = (double) (Q[7]/(1-Q[8]));  /* blocked to shadowed  */
    /*printf("LINSTATE: Prob from blocked to clear is %g\n",A[0]);
    printf("LINSTATE: Prob from blocked to shadowed is %g\n",A[1]);*/
    if (prob <= A[0])
    {
      *sysstate = CLEAR; 
      Dis = distance / (1.0 - (double) Q[0]); /* Av. dist. traveled in clear state. */
    }
    else
    {
      *sysstate = SHADOWED;  
      Dis = distance / (1.0 - (double) Q[4]); /* Av. dist. traveled in shadowed state. */
    } 
  }

  /***********************************************************
  * Calculate the number of samples the system spends in the *
  * state, where the duration in seconds is exponentially    *
  * distributed.                                             *
  ***********************************************************/

  prob = drand48();  /* Select a new number from a uniform [0,1) distribution. */
  /*printf("LINSTATE: duration prob = %g.\n",(double) prob);*/

  *num = rint( - (samsm) * Dis * log( 1.0-prob ) );

  /*printf("LINSTATE: num = %d.\n",*num);*/

}
