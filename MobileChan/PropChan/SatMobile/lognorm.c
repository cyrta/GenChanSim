/* Generates lognormally distributed random variables. */

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <seed.h>

void
lognorm(
	 float data[], /* log-normal process returned */
	 int nsamples, /* number of real samples asked for */
	 double mean,  /* mean of Gaussian */
	 double std)   /* standard deviation of Gaussian */
{
  int i;
  float lono;

  /* simple (possibly unnecessary) check for error */
  if(nsamples == 0) {
    printf("Error from lognorm(): number of variables requested is zero\n");
    exit(1);
  }

  /*  printf("CLOGNORM: nsamples is %d.\n",nsamples);*/
  /*printf("CLOGNORM: std and mean are %g and %g.\n",std,mean);*/

  gauss48(&data[0],nsamples);  /* Obtain mean 0, variance 1 Gaussian rv's */

  for(i=0; i<nsamples; i++) {
    data[i]=std*data[i]+mean;  /* convert from mean 0, variance 1 */
                               /* to mean mean, variance std^2 */
    lono=pow(10,data[i]/20.0); /* Make log-normal */
    data[i]=lono;              /* Put result in output data stream */
    /*printf("LOGNORM: lognormal variable is %g.\n",data[i]);*/
  }
}


