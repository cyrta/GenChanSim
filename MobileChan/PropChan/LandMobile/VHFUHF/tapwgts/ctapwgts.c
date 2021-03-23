/* generate tap weights for a single snapshot. 

   copyright (c) 1994 The MITRE Corporation Bedford, MA

   $Id: ctapwgts.c,v 3.0 1994/08/30 18:34:51 jjb Exp $
   $Log: ctapwgts.c,v $

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
#include <mixedchan.h>
void NewPrintTapWgts();

static char rcsid[] = "$Id: ctapwgts.c,v 3.0 1994/08/30 18:34:51 jjb Exp $";

void 
ctapwgts(
	 TAPWGTS *tw,		/* tapwgts structure of parameters */
	 fcomplex *wgts[],	/* weights of ea tap */
	 int path,              /* path number */
	 int interpStep,        /* amount to step thru interp buffer */
	 int weight_type,
	 char *data_fname,
	 char *filt_fname,
	 int whicha)
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

		case JAKES_SPECTRUM:{
		  if(whicha==A25) strcat(fbfile, FILTBANK25); /* filter bank file name */
		  else if(whicha==A50) strcat(fbfile, FILTBANK50);
		  else if(whicha==A75) strcat(fbfile, FILTBANK75);
		  else strcat(fbfile, FILTBANK100);
		  break;}

		case FLAT_SPECTRUM:
		  strcat(fbfile, FLAT_FILTBANK); /* filter bank file name */
		  break;

		case DATA_SPECTRUM:
		  strcpy(fbfile,filt_fname); /* filter bank file name */
		  break;

		default:
		  strcat(fbfile, FILTBANK100);	/* filter bank file name */
		  break;
		}
                printf("Using filter file: %s\n",fbfile);

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
	chfchan(tw, wgts, path, interpStep,weight_type,data_fname,whicha);/* get tap-weight values */

	if (firstcall) {	/* if first call to function... */
		NewPrintTapWgts(tw,weight_type,data_fname);	/* print out tap-weight stuff */

		firstcall = NO;	/* don't do this again */
	}
}

void 
NewPrintTapWgts(TAPWGTS * tw,int weight_type,char *data_fname)
{
	int             i;	/* index */
        printf("\n");
	printf("   Sample Rate (Hz): %8.3f\n", tw->sampleRate);
	if ( tw->model == MACROCELL_MODEL )
	  printf("Scat Samp Rate (Hz): %8.3f\n", tw->scatterSampleRate);
	printf("    Number of paths: %8d\n", tw->nPaths);
	printf("Vehicle Speed (m/s): %8.5f\n",tw->VehSpd);
	printf("Carrier Freq. (MHz): %8.3f\n", tw->carFreq);
	printf("------path------- ");
	for (i = 0; i < tw->nPaths; i++)
		printf("----%d--- ", i + 1);
	printf("\n");
	printf("             mode ");
	for (i = 0; i < tw->nPaths; i++) {
	  if ( tw->model == MACROCELL_MODEL ) {
	    if (tw->mode[i] == NORM)
	      printf("Discrete ");
	    else if (tw->mode[i] == DIST)
	      printf(" Scatter ");
	  } else {  /* JTC model */
	    printf("    JTC  ");
	  }
	}
	printf("\n");

	  if(weight_type == 0){
	  /* printout exponential parameters */
	  printf("     Exp. Tau(ns) ");
	  for(i = 0; i < tw->nPaths; i++) {
	    if(tw->mode[i] == DIST) {
	      printf(" %7.3f ", tw->expTau[i] * 1e9);
	    } else {
	      printf("     N/A ");
	    }
	  }
	  printf("\n");
	}
	  else{
/* printf("weight_type is %d, about to print names\n",weight_type); */
		/* printout filenames being used */
	  printf("   Delay Power Spectrum Data Filenames:\n ");
/* printf("npaths is %d\n",tw->nPaths); */
	  for(i = 0; i < tw->nPaths; i++) {
	    if(tw->mode[i] == DIST) {
	      printf(" %s\n",data_fname+i*120); /* names are each up to
											   120 chars long*/
	    } else {
	      printf("     N/A ");
	    }
	  }		
	}
	/* both discrete and scatter modes */
	printf("   number of taps ");
	for (i = 0; i < tw->nPaths; i++)
	  printf("%8d ", tw->nTaps[i]);
	printf("\n");
	printf("    snapshot (Hz) ");
	for (i = 0; i < tw->nPaths; i++)
		printf("%8.3f ", tw->snapshot[i]);
	printf("\n");
	printf("    Dpl shift(Hz) ");
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
	printf("  Bulk Delay (ns) ");
	for (i = 0; i < tw->nPaths; i++) {
		if (tw->mode[i] == DIST)
			printf("%8.3f ", tw->bulkDelay[i]);
		else
			printf("%8.3f ", tw->bulkDelay[i]);
	}
	printf("\n");
	printf("   Rel. Gain (dB) ");
	for (i = 0; i < tw->nPaths; i++) {
		if (tw->mode[i] == DIST)
			printf("%8.3f ", tw->gain[i]);
		else
			printf("%8.3f ", tw->gain[i]);
	}
	printf("\n");
	printf("\n");
}

