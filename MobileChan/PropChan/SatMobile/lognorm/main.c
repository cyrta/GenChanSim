/************************************************************************************
 *
 * main() - main program to test the clognorm function
 *
 ************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>
#include <seed.h>
#include <pathfile.h>

double me = 3;
double sigma = .2;
int nsamps = 5000;
float lonodata[5000];

void main()
{
  FILE *fid;
  int i;
  int seed;

  printf("MAIN: nsamps is %d.\n",nsamps);
  printf("MAIN: sigma and me are %g and %g.\n\n",sigma,me);

  /* Initialize */
  for(i=0;i<nsamps;i++){
    lonodata[i]=0.0;
  }
  seed = srand48(GetSeed());		/* get seed from file */
  /*  printf("MAIN: seed is %16d\n",seed);*/

  lognorm(&lonodata[0],nsamps,me,sigma);
  printf("MAIN: lonodata samples output from lognorm:\n");
  for (i=0;i<10;i++){
    printf("%d     %g\n",i,lonodata[i]);
  }
 
  SaveSeed(lrand48());

  fid=fopen("lonodata.bin","w");
  fwrite(&lonodata[0],sizeof(float),2*nsamps,fid);
  fclose(fid);
}

