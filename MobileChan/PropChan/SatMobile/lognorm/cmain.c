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

double me = 30.0;
double sigma = 10.0;
int nsamps = 5000;
fcomplex lonodata[5000];

void main()
{
  FILE *fid;
  int i;
  int seed;

  printf("MAIN: nsamps is %d.\n",nsamps);
  printf("MAIN: sigma and me are %g and %g.\n\n",sigma,me);

  /* Initialize */
  for(i=0;i<nsamps;i++){
    lonodata[i].r=0.0;
    lonodata[i].i=0.0;
  }
  seed = srand48(GetSeed());		/* get seed from file */
  /*  printf("MAIN: seed is %16d\n",seed);*/

  clognorm(&lonodata[0],nsamps,me,sigma);
  printf("MAIN: lonodata samples output from clognorm:\n");
  for (i=0;i<10;i++){
    printf("%d     %g + i%g\n",i,lonodata[i].r,lonodata[i].i);
  }
 
  SaveSeed(lrand48());

  fid=fopen("clonodata.bin","w");
  fwrite(&lonodata[0],sizeof(float),2*nsamps,fid);
  fclose(fid);
}

