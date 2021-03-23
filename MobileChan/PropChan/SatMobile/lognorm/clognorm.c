/* Generates complex lognormally distributed random variables. */

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <complex.h>
#include <seed.h>

void
clognorm(
	 fcomplex data[], /* complex log-normal process returned */
	 int nsamples, /* number of complex samples asked for */
	 double mean,     /* mean of complex Gaussian */
	 double std)  /* standard deviation of complex Gaussian */
{
  int i;
  const double CONST=8.68588963806504; /* 20*log10(e) */
  fcomplex lono;

  /* simple (possibly unnecessary) check for error */
  if(nsamples == 0) {
    printf("Error from clognorm(): number of complex variables requested is zero\n");
    exit(1);
  }

  /*printf("CLOGNORM: nsamples is %d.\n",nsamples);
  printf("CLOGNORM: std and mean are %g and %g.\n",std,mean);*/

  cgauss48(&data[0],nsamples);  /* Obtain mean 0, variance 1 complex Gaussian rv's */

  for(i=0; i<nsamples; i++) {
    data[i].r=std*data[i].r+mean;  /* convert from mean 0, variance 1 */
    data[i].i=std*data[i].i+mean;  /* to mean mean, variance std^2 */
    lono.r=pow(10,data[i].r/20.0)*cos((float)data[i].i/CONST); /* Make log-normal */
    lono.i=pow(10,data[i].r/20.0)*sin((float)data[i].i/CONST); /* Make log-normal */
    data[i].r=lono.r;    /* Put result in output data stream */
    data[i].i=lono.i;    /* Put result in output data stream */
    /*printf("CLOGNORM: lognormal variable is %g +i%g.\n",data[i].r,data[i].i);*/
  }
}


