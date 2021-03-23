/*
 * Generate one snapshot of the scatter path impulse response
 * 
 *

   copyright (c) 1994 The MITRE Corporation Bedford, MA

   $Id: chfchan.c,v 3.0 1994/08/30 18:37:45 jjb Exp $
   $Log: chfchan.c,v $

 * Call the doppler subroutine for both discrete and scatter paths. 7/1/97 CMK
 *
 * Lincoln Lab Revision 12/1995 Linda Riehl and Cathy Keller
 * fixed comments to reflect the mixed model channel terminology 
 * rather than the HF terminology
 * added ability to enter arbitrary data files for the delay power
 * spectrum and for the doppler power spectrum IIR filter coefficients
 * added DATA_INTERP_WRM_FILE possibility
 *
 * Revision 3.0  1994/08/30  18:37:45  jjb
 * fixed glitch that occurred on interpolator block boundaries.
 * eliminated loop around interpolator warmup. This was not
 * needed after glitch was fixed.
 * Added capability to load or store interpolator and IIR filter
 * states in two files to eliminate warmup if desired.
 * Added "interStep" to handle cases requiring the interpolator be stepped
 * thru with greater than 1 as step size.
 * Added macrocell model (call to expweight and normal(discrete) modes).
 * Fixed error in calling doppler when normal mode.
 *
 * Revision 2.2  1994/07/25  18:47:02  jjb
 * added copyright notice
 *

 * /
/* defines */

#include <stdio.h>
#include <math.h>
#include <complex.h>
#include <mixedchan.h>
#include <nb.h>
#include <ifde.h>


#define NJUMPINGJACKS 200	/* # loops to warm-up IIR filter (increased
				 * to 200 for the 32nd order Jakes spectrum
				 * IIR filter */
#define EXP 0
#define DATA 1
#define JAKES 2
#define FLAT 0

extern void     WriteFloat(char[], char *, float *, int);
extern void     WriteComplex(char[], char *, fcomplex *, int);
extern void     expweight(float[], int[], int[], float, float, int);
extern int      dataweight(float[], int[], int,char[]);
static char rcsid[] = "$Id: chfchan.c,v 3.0 1994/08/30 18:37:45 jjb Exp $";

void
chfchan(
	TAPWGTS * tw,		/* stuct of tapwgts() parameters */
	fcomplex * wgts[],	/* weights of ea path */
	int path,		/* path to assign weights to */
	int interpStep,         /* amount to step thru interp buffer */
	int weight_type,
	char *data_fname,
	int whicha)

{		
 static struct bufstruct 
{
   fcomplex intout[16000];
} *interpbuf;
 
static  fcomplex firptr2[MAXTAPS][NPTSFILTER1-1];
static  fcomplex firptr4[MAXTAPS][NPTSFILTER2-1];
static  fcomplex firptr8[MAXTAPS][NPTSFILTER3-1];
static  fcomplex firptr16[MAXTAPS][NPTSFILTER4-1];

	static unsigned int phasor[MAXTAPS];
	static          firstcall = YES;	/* first call to function
						 * flag */
	static          firstpath[NPATHS];	/* flag for 'warming-up'
						 * filters */
	static float    trigtbl[TRIGLEN + 1];	/* trig tables for Doppler
						 * shift */
	static float    weight_buff[MAXTAPS];	/* */
	static fcomplex normresp[MAXTAPS];	/* */
	static unsigned int phaseincr[MAXTAPS];	/* */
	static int      sttapnum[NPATHS];	/* start tap number of path */
	static float    lpgscal[4];	/* Gaussian random number scale
					 * factors */
	static float    doppscal;	/* 2**-TRIGSHFT scale factor in
					 * doppler */
	static fcomplex iqbuf[MAXTAPS];	/* tap weights */
/*      static char     twfile[100]="classtws.dat";
        static FILE     *twfp;					For Testing */
	static int      intercnt[NPATHS];	/* Counter for interpolater
						 * buffer */
	int             loop;
        int             i;
	int             j;/* indices */
        int             k;
	int             p;	/* path index */
	fcomplex       *ptr1, *ptr2;
	double          pscale;
	FILE                *fp;                /* file pointer */
	char                *envpath;           /* path to enviromental variable */
	static char         warmFile[120];	/* path and filename */

	if (firstcall) {	/* Do global initialization */
		firstcall = NO;

		if ((envpath = getenv("SIMDIR")) == (char *) NULL) {
		  printf("Error from narrowband:  must 'setenv SIMDIR appropriately'\n");
		  exit(1);
		}
		strcpy(warmFile, envpath);	/* add path to file name */
		if ( tw->powSpectrum == FLAT_SPECTRUM )
		  strcat(warmFile, FLAT_INTERP_WRM_FILE);
		if( tw->powSpectrum == JAKES_SPECTRUM ){
   /* whicha means which ratio of vehicle speeds has been chosen LR Feb 1996 */
		  if (whicha == A25)
			strcat(warmFile, INTERP_CLASS25_WRM_FILE);
		  if (whicha == A50)
			strcat(warmFile, INTERP_CLASS50_WRM_FILE);
		  if (whicha == A75)
			strcat(warmFile, INTERP_CLASS75_WRM_FILE);
		  if (whicha == A100)
			strcat(warmFile, INTERP_CLASS100_WRM_FILE);
		}
		if( tw->powSpectrum == DATA_SPECTRUM )
 		  strcat(warmFile, DATA_INTERP_WRM_FILE);

		for (j = 0; j < NPATHS; j++){
			intercnt[j] = 16000;	/* index into interpolation
						 * buffers (can) */
		firstpath[j] = YES;	/* set flags */
		}
		doppscal = (float) ldexp(1., -TRIGSHFT);
		for (j = 0; j < 4; j++) {
			lpgscal[j] = ldexp(1.0, 6 * j - 22);	/* scale factor for
								 * table lookup */
		}
		/* set up trig table */
		for (j = 0; j < TRIGLEN; j++) {
			trigtbl[j] = (float) cos(PI * 2 * j / TRIGLEN);
		}
		/*
		 * next line needed so that if trigtbl[index] is in range it
		 * remains valid for trigtbl[inex1+1] (required by doppler.c)
		 */
		trigtbl[TRIGLEN] = trigtbl[0];
		/* find starting filter number of each path */
		sttapnum[0] = 0;
		for (p = 1; p < tw->nPaths; p++) {
			sttapnum[p] = sttapnum[p - 1] + tw->nTaps[p - 1];
		}

		for (p = 0; p < tw->nPaths; p++) {
			if (tw->mode[p] == NORM) {	/* discrete mode */
				qphas(tw->nTaps, tw->groupDelay, tw->freqWinTransLen,
				      tw->timeWinTransLen, normresp, sttapnum, p, tw->sampleRate);
			} else {/* scatter mode */
				if (tw->model == MACROCELL_MODEL) {
				  if(weight_type == EXP) /* exp delay power spectrum */
					expweight(weight_buff, sttapnum, 
						  tw->nTaps, tw->scatterSampleRate,
					       tw->expTau[p], p);
				  else{/* data sample delay power spectrum */
                     /*printf("before call to dataweight ntaps is %d\n",tw->nTaps[p]);*/
					tw->nTaps[p] = dataweight(weight_buff, sttapnum, 
						   p,data_fname);
                     /*printf("after: %d\n",tw->nTaps[p]);*/
				  }
				} else if (tw->thinPhScr) {
					weight(weight_buff, sttapnum, tw->multiPathSpr,
					       tw->nTaps, tw->thinPhaseC, tw->thinPhaseThresh, p);
				} else {
					for (j = 0; j < MAXTAPS; j++)
						weight_buff[j] = 1.0;
				}
			}
			/* phaseincr calculation for Doppler */
			for (j = sttapnum[p]; j < sttapnum[p] + tw->nTaps[p]; j++) {
				pscale = ldexp(1.0, 32) / tw->snapshot[p];
				phasor[j] = 0;
				phaseincr[j] = (long int) (tw->dplOffset[j] * pscale);
			}
		}

		/* set pointers to weights within 'iqbuf' */
		for (j = 0; j < tw->nPaths; j++) {
			wgts[j] = &iqbuf[sttapnum[j]];
		}

		/* copy weight into array for use in vartapdelay */
		p=0;
		for(i=0; i<tw->nPaths; ++i ) {
		  if ( tw->mode[i] == NORM) {
		    for(j=0; j<tw->nTaps[i]; ++j) {
		      tw->weighting[p] = 1.0;
		      p++;
		    }
		  } else {
		    for(j=0; j<tw->nTaps[i]; ++j) {
		      tw->weighting[p] = weight_buff[p];
		      p++;
		    }
		  }
		}
		    

		/* check if to generate and store warmup */
		if ( tw->warmup == GEN_WARMUP ) {
		  /* force values that will cause MAXTAPS of scatter path
		     warmup values to be performed. NOTE: code stops after
		     warmup file are stored, therefore this does not cause
		     any errors */
		  tw->nPaths = 1;
		  tw->mode[0] = DIST;
		  tw->nTaps[0] = MAXTAPS;

		}

		/* adjust filter bank values for more taps */

		p = 0;
		for (i = 0; i < tw->nPaths; i++) {
			for (j = 0; j < tw->nTaps[i]; j++) {
				tw->filterBank[p].ampl = tw->filterBank[0].ampl;
				for (k = 0; k < MAXSECT; k++) {
					tw->filterBank[p].sect[k].a = tw->filterBank[0].sect[k].a;
					tw->filterBank[p].sect[k].b = tw->filterBank[0].sect[k].b;
				}
				p++;
			}
		}

		/* allocate memory for interpolator buffer */
		j = tw->nPaths - 1;
		i= sttapnum[j] + tw->nTaps[j];
		interpbuf = (struct bufstruct *) calloc(i,sizeof(struct bufstruct));
		if ( interpbuf == NULL ) {
		  fprintf(stderr,"unable to allocate memory for interpolator buffer\n");
		  exit(1);
		}

		/* check if to generate and store warmup or load warmup */
		if (tw->warmup == LOAD_WARMUP) {

		  if ( (fp=fopen(warmFile,"r")) == NULL ) {
		    fprintf(stderr,"unable to open Interpolator warmup file %s\n",
			    warmFile);
		    exit(-1);
		  }
		  if ( (i=fread(firptr2,sizeof(fcomplex),
				(MAXTAPS*(NPTSFILTER1-1)),fp)) 
		      != (MAXTAPS*(NPTSFILTER1-1)) ) {
		    fprintf(stderr,"unable to read 1st Interpolator warmup file %s\n",
			    warmFile);
		    fprintf(stderr,"only able to read in %d items when should be %d\n",
			    i,(MAXTAPS*(NPTSFILTER1-1)));
		    fclose(fp);
		    exit(-1);
		  }
		  if ( (i=fread(firptr4,sizeof(fcomplex),
				(MAXTAPS*(NPTSFILTER2-1)),fp)) 
		      != (MAXTAPS*(NPTSFILTER2-1)) ) {
		    fprintf(stderr,"unable to read 2nd Interpolator warmup file %s\n",
			    warmFile);
		    fprintf(stderr,"only able to read in %d items when should be %d\n",
			    i,(MAXTAPS*(NPTSFILTER2-1)));
		    fclose(fp);
		    exit(-1);
		  }
		  if ( (i=fread(firptr8,sizeof(fcomplex),
				(MAXTAPS*(NPTSFILTER3-1)),fp)) 
		      != (MAXTAPS*(NPTSFILTER3-1)) ) {
		    fprintf(stderr,"unable to read 3rd Interpolator warmup file %s\n",
			    warmFile);
		    fprintf(stderr,"only able to read in %d items when should be %d\n",
			    i,(MAXTAPS*(NPTSFILTER3-1)));
		    fclose(fp);
		    exit(-1);
		  }
		  if ( (i=fread(firptr16,sizeof(fcomplex),
				(MAXTAPS*(NPTSFILTER4-1)),fp)) 
		      != (MAXTAPS*(NPTSFILTER4-1)) ) {
		    fprintf(stderr,"unable to read 4th Interpolator warmup file %s\n",
			    warmFile);
		    fprintf(stderr,"only able to read in %d items when should be %d\n",
			    i,(MAXTAPS*(NPTSFILTER4-1)));
		    fclose(fp);
		    exit(-1);
		  }
		  fclose(fp);

		  /* set firstpath[] to NO so warmup is skipped */
		  for(i=0; i<tw->nPaths; ++i)
		    firstpath[i] = NO;

		}

	      }
	/********The following code updates the channel Doppler spectra ********/


	/* Generate next snapshot */
	if (tw->mode[path] == DIST) {	/* scatter path mode */
		if (firstpath[path] == YES) {	/* if first path weights... */

			/* warm-up filter */
			for (j = 0; j < NJUMPINGJACKS; j++) {
				cgauss48(&iqbuf[sttapnum[path]], tw->nTaps[path]);
                                iir32(tw, 0, &iqbuf[sttapnum[path]], &tw->filterBank[sttapnum[path]], tw->nTaps[path], sttapnum[path],whicha);
			}
		/* 	for (j = 0; j < 100; j++) {  orig value */
			for (j = 0; j < 1; j++) {	/* Warm-up the
							 * interpolator */
				for (loop = 0; loop < 1000; loop++) {	/* Loop through enough
									 * times to fill
									 * interpolator input
									 * vec */

					cgauss48(&iqbuf[sttapnum[path]], tw->nTaps[path]);
                                        iir32(tw, 0, &iqbuf[sttapnum[path]], &tw->filterBank[sttapnum[path]], tw->nTaps[path],sttapnum[path],whicha);

					for (i=0; i<tw->nTaps[path]; ++i) {
					  (interpbuf +sttapnum[path]+i)->intout[loop].r = iqbuf[sttapnum[path]+i].r;
					  (interpbuf+sttapnum[path]+i)->intout[loop].i = iqbuf[sttapnum[path]+i].i;  
					}     
				}	/* End for loop = 0:1000 */
				for(i=0;  i<tw->nTaps[path]; ++i)
				  interp64((interpbuf+sttapnum[path]+i)->intout, 
					   16000, 1000, 16, 
					   &firptr2[sttapnum[path]+i][0],
					   &firptr4[sttapnum[path]+i][0],
					   &firptr8[sttapnum[path]+i][0],
					   &firptr16[sttapnum[path]+i][0]);
			}	/* End interpolation warm-up */
			firstpath[path] = NO;

			if ( tw->warmup == GEN_WARMUP ) {

			  if ( (fp=fopen(warmFile,"w")) == NULL ) {
			    fprintf(stderr,"unable to open Interpolator warmup file %s\n",
				    warmFile);
			    exit(-1);
			  }
			  if ( (i=fwrite(firptr2,sizeof(fcomplex),
					 (MAXTAPS*(NPTSFILTER1-1)),fp)) 
			      != (MAXTAPS*(NPTSFILTER1-1)) ) {
			    fprintf(stderr,"unable to write 1st Interpolator warmup file %s\n",
				    warmFile);
			    fprintf(stderr,"only able to write %d items when should be %d\n",
				    i,(MAXTAPS*(NPTSFILTER1-1)));
			    fclose(fp);
			    exit(-1);
			  }
			  if ( (i=fwrite(firptr4,sizeof(fcomplex),
					 (MAXTAPS*(NPTSFILTER2-1)),fp)) 
			      != (MAXTAPS*(NPTSFILTER2-1)) ) {
			    fprintf(stderr,"unable to write 2nd Interpolator warmup file %s\n",
				    warmFile);
			    fprintf(stderr,"only able to write %d items when should be %d\n",
				    i,(MAXTAPS*(NPTSFILTER2-1)));
			    fclose(fp);
			    exit(-1);
			  }
			  if ( (i=fwrite(firptr8,sizeof(fcomplex),
					 (MAXTAPS*(NPTSFILTER3-1)),fp)) 
			      != (MAXTAPS*(NPTSFILTER3-1)) ) {
			    fprintf(stderr,"unable to write 3rd Interpolator warmup file %s\n",
				    warmFile);
			    fprintf(stderr,"only able to write %d items when should be %d\n",
				    i,(MAXTAPS*(NPTSFILTER3-1)));
			    fclose(fp);
			    exit(-1);
			  }
			  if ( (i=fwrite(firptr16,sizeof(fcomplex),
					 (MAXTAPS*(NPTSFILTER4-1)),fp)) 
			      != (MAXTAPS*(NPTSFILTER4-1)) ) {
			    fprintf(stderr,"unable to write 4th Interpolator warmup file %s\n",
				    warmFile);
			    fprintf(stderr,"only able to write %d items when should be %d\n",
				    i,(MAXTAPS*(NPTSFILTER4-1)));
			    fclose(fp);
			    exit(-1);
			  }
			  fclose(fp);

			  /* call iir32 to store its warmup values */
			  iir32(tw, 1, NULL, NULL, 0,0,whicha);


			  exit(0);
			}

		}

		/*
		 * Modify for the mobile channel so that only 1 fading
		 * coefficient is generated for each path.  Thus, we will
		 * simply set nTaps = 1 for each call to cgauss48 and iir32
		 * and then copy the returned value to as many taps as
		 * actually exist for the path  3/14/94 c.a.n.  Here is the
		 * original code:
		 * 
		 * cgauss48(&iqbuf[sttapnum[path]], tw->nTaps[path]);
		 * iir32(&iqbuf[sttapnum[path]],
		 * &weight_buff[sttapnum[path]],
		 * &tw->filterBank[sttapnum[path]], tw->nTaps[path], path,whicha);
		 */
		/* Begin modification */
		/*
		 * First, we have to set-up a buffer since iir32's output is
		 * run through a 16X interpolator.  We will do this by
		 * calling iir32/cgauss48 1000 times, and then passing the
		 * inerpolator this 1000 length vector.  It returns a 16000
		 * point vector which we then use to feed the simulator
		 * whenever a channel snapshot is requested.  When the 16000
		 * point buffer is empty, we fill it again.  Note that this
		 * must be done for each PATH since they all have independent
		 * fading. Thus, we will set up a two-dimensional array
		 * [NPATH][16000]. 3/15/94 c.a.n.
		 */


		if ((intercnt[path] + interpStep-1) >= 16000) {	/* If interpolation buffer is
						 * empty... */

	/**	        twfp = FOpen(twfile,"w"); **/
			for (loop = 0; loop < 1000; loop++) {	/* Loop through enough
								 * times to fill
								 * interpolator input
								 * vec */

                        cgauss48(&iqbuf[sttapnum[path]], tw->nTaps[path]);
			iir32(tw, 0, &iqbuf[sttapnum[path]], &tw->filterBank[sttapnum[path]], tw->nTaps[path], sttapnum[path],whicha);
			for( i=0; i<tw->nTaps[path]; ++i ) {
			  (interpbuf+sttapnum[path]+i)->intout[loop].r = iqbuf[sttapnum[path]+i].r;
			  (interpbuf+sttapnum[path]+i)->intout[loop].i = iqbuf[sttapnum[path]+i].i;
			}
/**	BEFORE INTERPOLATION:			fwrite(interpbuf[path].intout[loop],sizeof(fcomplex),1,twfp); **/
		      }	/* End for loop = 0:1000 */

		/**	fclose(twfp);	**/				/*** THIS LINE FOR TESTING ***/

			intercnt[path] += interpStep - 1 - 16000;
			
			for( i=0; i<tw->nTaps[path]; ++i ) {
			  interp64((interpbuf+sttapnum[path]+i)->intout, 
				   16000, 1000, 16,
				   &firptr2[sttapnum[path]+i][0],
				   &firptr4[sttapnum[path]+i][0],
				   &firptr8[sttapnum[path]+i][0],
				   &firptr16[sttapnum[path]+i][0]);
	/* interpolate */

			  iqbuf[sttapnum[path]+i].r = (interpbuf+sttapnum[path]+i)->intout[intercnt[path]].r;
			  iqbuf[sttapnum[path]+i].i = (interpbuf+sttapnum[path]+i)->intout[intercnt[path]].i;
			}
			intercnt[path]++;
		} else {
			intercnt[path] += interpStep - 1;
			for( i=0; i<tw->nTaps[path]; ++i ) {
			  iqbuf[sttapnum[path]+i].r = (interpbuf+sttapnum[path]+i)->intout[intercnt[path]].r;
			  iqbuf[sttapnum[path]+i].i = (interpbuf+sttapnum[path]+i)->intout[intercnt[path]].i;
			}
			intercnt[path]++;

		}		/* End fetch/generate interpolation buffer
				 * value */


		/*
		 * Set all taps in this path equal to the one returned by
		 * iir32
		 */
		/* Weight the taps external to iir32: */
		for (j = 0; j < tw->nTaps[path]; j++) {
			iqbuf[sttapnum[path] + j].r = iqbuf[sttapnum[path]+j].r * weight_buff[sttapnum[path] + j];
			iqbuf[sttapnum[path] + j].i = iqbuf[sttapnum[path]+j].i * weight_buff[sttapnum[path] + j];

		}

		/* End Modification */


	} else {		/* discrete path */
		ptr1 = &normresp[sttapnum[path]];
		ptr2 = &iqbuf[sttapnum[path]];
		for (j = 0; j < tw->nTaps[path]; j++) {
			ptr2->r = ptr1->r;
			ptr2->i = ptr1->i;
			ptr1++;
			ptr2++;
		}

	}
	doppler(phasor, phaseincr, trigtbl, &iqbuf[sttapnum[path]], tw->nTaps[path],
		sttapnum[path], doppscal,interpStep);

}
/*********************************************************************************/

void
WriteComplex(
	     char *filename,	/* 1st part of file name */
	     char *mode,	/* "w", "r", "a", etc. */
	     fcomplex data[],	/* data */
	     int npts)
{				/* # pts in data[] */
	FILE           *dfp;	/* debug file ptr */

	printf("writing '%s'...", filename);
	fflush(stdout);
	if ((dfp = fopen(filename, mode)) == NULL) {
		printf("Warning from WriteComplex: '%s' not opened\n", filename);
	} else {
		fwrite(data, sizeof(fcomplex), npts, dfp);
		fclose(dfp);
	}
	printf("done\n");
}

void
WriteFloat(
	   char *filename,	/* 1st part of file name */
	   char *mode,		/* "w", "r", "a", etc. */
	   float data[],	/* data */
	   int npts)
{				/* # pts in data[] */
	FILE           *dfp;	/* debug file ptr */

	printf("writing '%s'...", filename);
	fflush(stdout);
	if ((dfp = fopen(filename, mode)) == NULL) {
		printf("Warning from WriteFloat: '%s' not opened\n", filename);
	} else {
		fwrite(data, sizeof(float), npts, dfp);
		fclose(dfp);
	}
	printf("done\n");
}

/* Append interpolation functions *************************************************/
/*
 * Interpolate the signal by 2, 4, 8, 16, 32, or 64 to by using multiple
 * stages of x2 interpolation by zero insertion and filtering.
 */
void
interp64(
	 fcomplex data[],	/* complex data samples */
	 int datadim,		/* dimension of data[] */
	 int ndatas,		/* # elements in data[] */
	 int interp,		/* interpolation factor */
	 fcomplex firInit2[],
	 fcomplex firInit4[],
	 fcomplex firInit8[],
	 fcomplex firInit16[])
{				/* fir interpolation filter initial states */
	static int      firstcall = 1;	/* flag for 1st call to function */
	static float    interp1[NPTSFILTER1];	/* 1st filter coeff */
	static float    interp2[NPTSFILTER2];	/* 2nd filter coeff */
	static float    interp3[NPTSFILTER3];	/* 3rd filter coeff */
	static float    interp4[NPTSFILTER4];	/* 4th filter coeff */
	static float    interp5[NPTSFILTER5];	/* 5th filter coeff */
	static float    interp6[NPTSFILTER6];	/* 6th filter coeff */

	if (firstcall) {	/* if first call to function... */
		firstcall = 0;	/* only enter this statement once */
		readfilterfile(INTERPFILTER1, interp1, NPTSFILTER1);
		readfilterfile(INTERPFILTER2, interp2, NPTSFILTER2);
		readfilterfile(INTERPFILTER3, interp3, NPTSFILTER3);
		readfilterfile(INTERPFILTER4, interp4, NPTSFILTER4);
		readfilterfile(INTERPFILTER5, interp5, NPTSFILTER5);
		readfilterfile(INTERPFILTER6, interp6, NPTSFILTER6);
	}
	if (interp >= 2) {
		zerofilter(data, datadim, ndatas, interp1, NPTSFILTER1, firInit2);
	}
	if (interp >= 4) {
		zerofilter(data, datadim, 2 * ndatas, interp2, NPTSFILTER2, firInit4);
	}
	if (interp >= 8) {
		zerofilter(data, datadim, 4 * ndatas, interp3, NPTSFILTER3, firInit8);
	}
	if (interp >= 16) {
		zerofilter(data, datadim, 8 * ndatas, interp4, NPTSFILTER4, firInit16);
	}
/* commented out on 8/18/94 jjb. not needed to go above 16 
	if (interp >= 32) {
		zerofilter(data, datadim, 16 * ndatas, interp5, NPTSFILTER5, firInit);
	}
	if (interp >= 64) {
		zerofilter(data, datadim, 32 * ndatas, interp6, NPTSFILTER6, firInit);
	}
*/
}

/* read in interpolator filter coefficients */
void
readfilterfile(char filename[], float data[], int npts)
{
	int             i;	/* index */
	FILE           *fp;	/* filter file pointer */
	char           *envpath;/* path to enviromental variable */
	char            file[120];	/* path and filename */

	if ((envpath = getenv("SIMDIR")) == (char *) NULL) {
		printf("Error from narrowband:  must 'setenv SIMDIR appropriately'\n");
		exit(1);
	}
	strcpy(file, envpath);	/* add path to file name */
	strcat(file, filename);	/* add filename to file name */

	if ((fp = fopen(file, "r")) == NULL) {
		printf("Error from narrowband: filter file '%s' not opened\n", file);
		exit(1);
	}
	for (i = 0; i < npts; i++) {
		if (fscanf(fp, "%f", &data[i]) == EOF) {
			printf("Error from narrowband: out of points in interpolator "
			       "filter file '%s'.  ", file);
			printf("There should be %d points, but file contains %d points!\n",
			       npts, i);
			exit(1);
		}
	}
	if (fscanf(fp, "%f", &data[i]) != EOF) {
		printf("WARNING: found extra filter coefficients in '%s'\n", file);
	}
	fclose(fp);
}


/*
 * Perform zero insertion and filter complex data via a floating point fir
 * filter.  The zero insertion is done by only using every other filter
 * coefficient on a given pass. The number of valid points in the output is
 * the number of input data points. The output is written into the input
 * array data[]. The function uses a queue to retain data so that input data
 * isn't stomped on. Zero insertion is done after the points if an there is
 * an even number of filter coefficients, and before if there is an odd
 * number of coefficients. The function algorithm approach is simular to a
 * convolution.
 * 
 * starting configuration:
 * 
 * [][][][][][][][][][][][][][][][][][][][][][][][]   data <-- [][][][] q
 * [][][][][][]   filter [][][][][][][][][][][][][][][][][][][][][][][][]
 * data
 * 
 * Modification: 12/17/93 - multiply each sample by 2.0 to compensate for the
 * power lost due to zero insertion. r.coutts
 */
#define		M				2	/* factor of
							 * interpolator */
void
zerofilter(
	   fcomplex data[],	/* input data (modified) */
	   int dim,		/* dimension of data[] */
	   int ndatas,		/* # pts in data[] */
	   float filter[],	/* filter coeff */
	   int nFiltPts,	/* # pts in filter[] */
	   fcomplex firInit[])
{				/* fir interpolation filter initial states */
	fcomplex        sum;	/* complex sum of filter pass */
	int             index, i, j, s;	/* indices */
	fcomplex       *q;	/* complex data queue */
	int             r;	/* index to 'q' */
	int             r0;	/* index to front of 'q' */
	int             start;	/* starting data index */
	int             qsize;	/* number of elements in 'q' */
	fcomplex        zero = {0.0, 0.0};	/* 0.0 + i*0.0 */
	static fcomplex tempFirInit[NPTSFILTER1 - 1];	/* temporary storage of
							 * initial states for
							 * next section of
							 * interferer.  Note
							 * that NPTSFILTER1 is
							 * assumed to be the
							 * largest filter and
							 * is, therefore, used
							 * to dimension the
							 * array. */

	/* set some variables */
	qsize = ceil((double) nFiltPts / M);	/* size of 'q' */
	start = ndatas - qsize;	/* number of filter passes */
	/* alloc queue memory */
	q = (fcomplex *) malloc(qsize * sizeof(fcomplex));
	if (q == NULL) {
		printf("Oh, oh... big error from zerofilter(): memory not properly "
		       "allocated for queue.\n");
		exit(1);
	}
	/*
	 * multiply values by 2.0 to compensate for lost power by zero
	 * insertion
	 */
	for (i = 0; i < ndatas; i++) {
		data[i].r *= 2.0;
		data[i].i *= 2.0;
	}
	/* fill queue with the last data pts (leave q[0] empty) */
	r = 1;			/* set q index to 2nd element */
	for (i = start + 1; i < ndatas; i++) {
		q[r++] = data[i];	/* fill queue with end of data */
	}
	/* save initial states for next section of interferer */
	if (firInit != NULL) {	/* if initial states exist... */
		/*
		 * save end of data buffer as init states for next section of
		 * interf
		 */
		j = 0;
		for (i = ndatas - nFiltPts + 1; i < ndatas; i++) {
			tempFirInit[j++] = data[i];
		}
	}
	r0 = 0;			/* zero is front of q */
	for (i = start; i > -qsize; i--) {	/* for ea data point... */
		if (i >= 0) {	/* if 'i' is in data... */
			q[r0] = data[i];	/* add data to queue */
		} else {	/* if 'i' is NOT in data... */
			q[r0] = zero;	/* data is zero (empty) */
			if (firInit != NULL) {	/* if initial states exist... */
				q[r0] = firInit[nFiltPts - 1 + i];	/* get inital state */
				/*
				 * -1 because firInit[] contains nfilterPts-1
				 * points
				 */
			}
		}
		for (s = 0; s < M; s++) {	/* two passes in one local */
			sum.r = 0.0;	/* clear real part */
			sum.i = 0.0;	/* clear imag part */
			r = r0;	/* init q intex to front */
			/*
			 * index every other filter[] coeff to simulate zero
			 * insertion
			 */
			for (j = nFiltPts - 1 - s; j >= 0; j -= M) {	/* for ea filter
									 * coeff... */
				sum.r += q[r].r * filter[j];	/* (real)x(filt coeff) */
				sum.i += q[r].i * filter[j];	/* (imag)x(filt coeff) */
				if (++r == qsize)
					r = 0;	/* incr q index, wrap? */
			}
			index = (i + qsize) * M - 1 - s;	/* index to output
								 * (input) */
			if (index >= dim)
				continue;	/* if overflow, cont */
			data[index].r = sum.r;	/* store the real output in
						 * data[].r */
			data[index].i = sum.i;	/* store the imag output in
						 * data[].i */
		}
		if (--r0 < 0)
			r0 = qsize - 1;	/* move front of queue local */
	}
	/* copy initial states from temporary storage */
	if (firInit != NULL) {	/* if initial states exist... */
		/*
		 * save end of data buffer as init states for next section of
		 * interf
		 */
		for (i = 0; i < nFiltPts - 1; i++) {
			firInit[i] = tempFirInit[i];
		}
	}
	free(q);		/* free queue memory */
}

