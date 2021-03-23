/* 
  function to compute the fractional delay filter coefficients

  input: delay (0<delay < 1)
  output: dlycoeff - FIR filter coefficients

  copyright (c) 1994 The MITRE Corporation Bedford, MA

  $Id: fracdelay.c,v 3.0 1994/08/30 18:47:58 jjb Exp $
  $Log: fracdelay.c,v $
 * Revision 3.0  1994/08/30  18:47:58  jjb
 * new addition to rev 3.0 of code
 *

*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
/* local include files */
#include <complex.h>
#include <airairchan.h>
#include <fracdly.h>

static char rcsid[] = "$Id: fracdelay.c,v 3.0 1994/08/30 18:47:58 jjb Exp $";

void fracDelayFilter(float delay,float dlycoeff[])
{
  char  *envpath;             /* path name returned from getenv() */
  char  fname[120];           /* filename */
  FILE *fpcoef;               /* pointer to file of coeff. */
  static int firstcall = 1;   /* indicates if first time called */
  float fullcoeff[ORIG_FRAC_FILT_SIZE];  /* orig. filter coeff. */
  static float coeff[FRAC_INTERP_RATE+1][FRAC_FILT_SIZE];
                              /* frac. delay filters */
  int i,j,k;                  /* loop indexes */

  if ( (delay < 0) || (delay >=1.0 ) ) {
    fprintf(stderr,"bad delay\n");
    exit(1);
  }
  

  /* first time through read in coefficients */

  if ( firstcall == 1 ) {
    firstcall = 0;
     /* set up file paths using env variable defined in '.cshrc' */
    if ((envpath = getenv("HFDIR")) == (char *) NULL) {
      printf("Error from tapwgts(): must 'setenv HFDIR ~/hf'\n");
      exit(1);
    }
    strcpy(fname, envpath);
    strcat(fname, FRACDLY_FILTER);

   /* read in coefficients */
    if ( (fpcoef=fopen(fname,"r")) == NULL ) {
      fprintf(stderr,"unable to open coefficient file\n");
      exit(-1);
    }

    if ( (i=fread(fullcoeff,sizeof(float),ORIG_FRAC_FILT_SIZE,fpcoef)) != ORIG_FRAC_FILT_SIZE ) {
      fprintf(stderr,"error only able to read %d coefficients not %d\n",i,ORIG_FRAC_FILT_SIZE);
      fclose(fpcoef);
      exit(-1);
    }

    fclose(fpcoef);

    /* create the polyphase filters */

    j=0;
    for (i=0; i<FRAC_FILT_SIZE; i++) {
      for (k=FRAC_INTERP_RATE-1; k>=0; k--)
	coeff[k][i]=fullcoeff[j++];
    }

    /* determine coeff[FRAC_INTERP_RATE] by shifting coeff[0] */
    coeff[FRAC_INTERP_RATE][0] = 0.0;
    for(i=1; i<FRAC_FILT_SIZE; ++i ) {
      coeff[FRAC_INTERP_RATE][i] = coeff[0][i-1];
    }

  }

  /* interpolate coefficients */
  delay=delay*FRAC_INTERP_RATE;
  for(j=0; j<FRAC_INTERP_RATE; ++j) {
    if ( delay < (float)(j+1)) {
      for (i=0; i<FRAC_FILT_SIZE; ++i) {
	dlycoeff[i]=coeff[j][i] + (delay-(float)j)*(coeff[j+1][i] - coeff[j][i]);
      }
      break;
    }
  }

}
