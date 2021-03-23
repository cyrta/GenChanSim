/* generate tap weights for a single snapshot. 

   copyright (c) 1994 The MITRE Corporation Bedford, MA

   $Id: ctapwgtslin.c,v 4.0 1996/07/09 18:34:51 cmk Exp $
   $Log: ctapwgtslin.c,v $

 * Lincoln Lab Revision 7/96 cmk
 * took out weight_type and data_fname, but kept filt_fname.
 *
 * Lincoln Lab Revision 12/95 Linda Riehl and Cathy Keller
 * added weight_type, data_fname, filt_fname for allowing data sample file
 * for delay power spectrum and filter coefficient file for doppler
 * power spectrum
 *
 * Revision 3.0  1994/08/30  18:34:51  jjb
 * added interpstep as argument passed onto chfchan.
 * modified interpolator filter selection to allow more than
 * two filters to be used in the future.
 * modified printing of tap information to handle different models.
 *
 * Revision 2.2  1994/07/25  18:47:02  jjb
 * added copyright notice
 *

 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>
#include <flatfade.h>
void NewPrintTapWgts();

static char rcsid[] = "$Id: ctapwgtslin.c,v 4.0 1996/07/09 18:34:51 cmk Exp $";

void 
ctapwgtslin(
	 TAPWGTS *tw,		/* tapwgts structure of parameters */
	 fcomplex *wgts[],	/* weights of ea tap */
	 int path,              /* path number */
	 int interpStep,        /* amount to step thru interp buffer */
	 char *filt_fname)
{	
	char            fbfile[120];	/* filter bank file name */
	char           *envpath;/* path name returned from getenv() */
	FILE           *fbfp = NULL;	/* filter bank fp */
	static int      firstcall = YES;	/* flag for first call */
	int             totalntaps;	/* sum of ntaps[] */
	int             i;	/* index */
	int             j;	/* index */
	int             k;	/* index */
	int             t;	/* index */

	if (firstcall) {
		/* set up file paths using env variable defined in '.cshrc' */
		if ((envpath = getenv("SIMDIR")) == (char *) NULL) {
		  printf("Error from tapwgts(): must 'setenv SIMDIR appropriately'\n");
			exit(1);
		}
		strcpy(fbfile, envpath);
		switch(tw->powSpectrum) {

		case GAUSSIAN_SPECTRUM:
		  strcat(fbfile, GAUSSIAN_FILTBANK);	/* filter bank file name */
		  break;

		case FLAT_SPECTRUM:
		  strcat(fbfile, FLAT_FILTBANK); /* filter bank file name */
		  break;

		case DATA_SPECTRUM:
		  strcpy(fbfile,filt_fname); /* filter bank file name */
		  break;

		default:
		  strcat(fbfile, GAUSSIAN_FILTBANK);	/* filter bank file name */
		  break;
		}
                /*printf("Using filter file: %s\n",fbfile);*/

		/* set all the taps within a partition to one value */
		for (i = 0, j = 0; i < tw->nPaths; i++) {
			for (k = 0; k < tw->nTaps[i]; k++) {
			  tw->dplOffset[j++] = tw->dplShift[i];
			}
		}

		/* open files */
/* Read in IIR32 tap coefficients for the first time; this is effectively the first iteration */

		if ((fbfp = fopen(fbfile, "r")) == NULL) {
			printf("Error from tapwgts(): fb:'%s' not opened\n", fbfile);
			exit(1);
		}
		/* read in filter bank file */
/* Change from reading in ASCII values in columnar format to reading doubles in
   binary format   c.a.n. 4/94 */


		fread(&tw->filterBank[0].ampl, sizeof(double), 1,fbfp );
		for (i = 0; i < 32; i++){
		  fread(&tw->filterBank[0].sect[i].a,sizeof(double),1,fbfp);
		  fread(&tw->filterBank[0].sect[i].b, sizeof(double),1,fbfp);
/*		printf("a[%d]= %2.64e\n",i,tw->filterBank[0].sect[i].a );
		printf("b[%d]= %2.64e\n",i,tw->filterBank[0].sect[i].b );  */
	
		}
		fclose(fbfp);


		/* blank remaining filter coefficents */
		for (; i < MAXSECT; i++) {	/* 2/23/94 Changed 4 to MAXSECT for IIR16 can */
			tw->filterBank[0].sect[i].a = tw->filterBank[0].sect[i].b = 0.0;
		}

		/* assign filter bank values */
		t = 0;
		for (i = 0; i < tw->nPaths; i++) {
			for (j = 0; j < tw->nTaps[i]; j++) {
				tw->filterBank[t].ampl = tw->filterBank[0].ampl;
				for (k = 0; k < MAXSECT; k++) {	/* 2/23/94 Changed 4 to MAXSECT for IIR16 can */
					tw->filterBank[t].sect[k].a = tw->filterBank[0].sect[k].a;
					tw->filterBank[t].sect[k].b = tw->filterBank[0].sect[k].b;
				}
				t++;
			}
		}

		/* get total number of taps in all partitions */
		for (i = 0, totalntaps = 0; i < tw->nPaths; i++) {
			totalntaps += tw->nTaps[i];
		}
	}
	chfchan(tw, wgts, path, interpStep); /* get tap-weight values */

	if (firstcall) {	/* if first call to function... */
		NewPrintTapWgts(tw);	/* print out tap-weight stuff */
		firstcall = NO;	/* don't do this again */
	}
}

void 
NewPrintTapWgts(TAPWGTS * tw)
{
	int             i;	/* index */
	int             ii;	/* index */
        printf("\n");
	printf("   Sample Rate (Hz): %8.3f\n", tw->sampleRate);
        /*printf("Scat Samp Rate (Hz): %8.3f\n", tw->scatterSampleRate);*/
	printf("    Number of paths: %8d\n", tw->nPaths);
  	printf("Vehicle Speed (m/s): %8.5f\n",tw->VehSpd);
	printf("Carrier Freq. (MHz): %8.3f\n", tw->carFreq);
	printf("-----------path----------- ");
	for (i = 0; i < tw->nPaths; i++)
		printf("----%d--- ", i + 1);
	printf("\n");
	printf("                     mode ");
	for (i = 0; i < tw->nPaths; i++) {
	  if (tw->mode[i] == NORM)
	    printf("Discrete ");
	  else if (tw->mode[i] == DIST)
	    printf(" Scatter ");
	}
	printf("\n");

        /* printout exponential parameters */
	printf("             Exp. Tau(ns) ");
	for(i = 0; i < tw->nPaths; i++) {
	  if(tw->mode[i] == DIST) {
	    /*printf(" %7.3f ", tw->expTau[i] * 1e9);*/
	    printf("     N/A ");
	  } else {
	    printf("     N/A ");
	  }
	}
	printf("\n");

	/* both discrete and scatter modes */
	printf("           number of taps ");
	for (i = 0; i < tw->nPaths; i++)
	  printf("%8d ", tw->nTaps[i]);
	printf("\n");
	printf("            Dpl shift(Hz) ");
	for (i = 0; i < tw->nPaths; i++)
		printf("%8.3f ", tw->dplShift[i]);
	printf("\n");
	/* normal mode */
/*	printf("Distortn (uS/MHz) ");
	for (i = 0; i < tw->nPaths; i++) {
		if (tw->mode[i] == NORM)
			printf("     n/a ");
		else
			printf("     n/a ");
	}
	printf("\n");  commented out by jjb @ 8/11/94 */
	/* disturbed mode */
	printf("          Bulk Delay (ns) ");
	for (i = 0; i < tw->nPaths; i++) {
		if (tw->mode[i] == DIST)
			printf("%8.3f ", tw->bulkDelay[i]);
		else
			printf("%8.3f ", tw->bulkDelay[i]);
	}
	printf("\n");
        for (ii=0; ii < NUMSTATES; ii++)
	{
	  printf("  Rel. Gain (dB) State %2d ",ii);
	  for (i = 0; i < tw->nPaths; i++) 
          {
	    if (tw->mode[i] == DIST)
	      printf("%8.3f ", tw->stategain[i][ii]);
	    else
	      printf("%8.3f ", tw->stategain[i][ii]);
	  }
          printf("\n");
	}
	printf("\n");
	printf("\n");
}

