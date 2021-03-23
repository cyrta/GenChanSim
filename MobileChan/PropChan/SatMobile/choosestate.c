/* Determines the initial system state */

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <seed.h>
#include "flatfade.h"

void
choosestate(
	    int *blstate,  /* output = YES for blocked state, = NO for clear state     */
	    double Aprob)  /* input = probability that system is in blocked state      */
{
  double prob;

  /*printf("CHOOSESTATE: P(blocked state) is %f.\n",Aprob);*/

  prob = drand48();  /* Select a number from a uniform [0,1) distribution. */
  /*printf("CHOOSESTATE: state prob = %g.\n",prob);*/

  *blstate = NO;                       /* Initialize to clear state condition */
  if (prob <= Aprob) *blstate = YES;   /* Blocked state condition */

}
