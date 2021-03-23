/*

   copied for the subroutine with
   copyright (c) 1994 The MITRE Corporation Bedford, MA

   $Id: iir32.c,v LL0.0 12/1995 cmk Exp $
   $Log: iir32.c,v $

 * Lincoln Lab Revision 12/1995 cmk
 * use Gaussian shaped IIR filter for the air to air channel
 *
 * Revision 3.0  1994/08/30  18:30:24  jjb
 * added option to load/store filter states in warmup files.
 *
 * Revision 2.2  1994/07/25  18:47:02  jjb
 * added copyright notice
 *

*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>
#include <airairchan.h>

/** This program implements a 32nd order IIR filter used to generate a
*   Gaussian * shaped fading spectrum for the air to air channel.  The
*   tap weight coefficients are read in as doubles.  They are stored
*   in the file
*   "GenChanSim/MobileChan/PropChan/AirMobile/airtoair/tapwgts/gaussian_filt.coef"
*   The first coefficient is the "amplitude" (gain factor) which is
*   tap b(1) as generated in Matlab.  a(1)=1 in Matlab is not written.
*   The remainder of the coefficients are stored in interleaved order:
*   a(2), b(2), a(3), b(3) .... where the indices indicated here are
*   those used by Matlab.  Thus, a new fading spectrum can easily be
*   defined by using the 'yulewalk' function (or any other IIR ilter
*   design function) in Matlab and storing the coefficients in the
*   file in interleaved format and double precision.
*
*   Assuming the coefficients are stored in vectors a and b in Matlab,
*   this is one way to properly write them to the file:
*   
*   c=zeros(1,65);
*   c(1)=b(1);
*   c(2:2:length(c))=a(2:1:length(a));
*   c(3:2:length(c))=b(2:1:length(b));
*   fid=fopen('gaussian_filt.coef','w')
*   fwrite(fid,c,'double')
*   
*   Written by:  C. A. Nissen
*		 The MITRE Corporation
* April, 1994 
*   Edited by C. M. Keller, MIT Lincoln Laboratory*/

/* static char rcsid[] = "$Id: iir32.c,v 3.0 1994/08/30 18:30:24 jjb Exp $"; */
static char rcsid[] = "$Id: iir32.c,v LL0.0 12/1995 cmk Exp $";

void 
iir32(
     TAPWGTS *tw,               /* tap weight info */
     int storeFlag,             /* flag to indicate store delay line
				   values and return */
     fcomplex iqbuf[],		/* filtered number (modified) */
     FILTER f[],		/* filter bank */
     int ntaps,                  /* number of taps in the path */
     int partnum)		/* Called once for each Tapped Delay Line 
				   (i.e. partition ) */
{	

	static DELAY_32      *d;	        /* Delay memory elements */
	static int           firstcall = 1;	/* 1st call to function flag */
	DELAY_32	     *dm;		/* Pointer to delay line */
	int                  i, j;	        /* index */
	double		     yr;		/* Real and Imag output values */
	double		     yi;
	int                  filtnum;		/* index to filter */

	static int	    count;

	FILE                *fp;                /* file pointer */
	char                *envpath;           /* path to enviromental variable */
	static char         warmFile[120];	/* path and filename */

/* If first time through */

	if (firstcall) {
		firstcall = 0;

		if ((envpath = getenv("SIMDIR")) == (char *)NULL) {
		  printf("Error from narrowband:  must 'setenv SIMDIR appropriately'\n");
		  exit(1);
		}
		strcpy(warmFile, envpath);	/* add path to file name */
		if ( tw->powSpectrum == GAUSSIAN_SPECTRUM )
		  strcat(warmFile, GAUSSIAN_IIR_WRM_FILE);

		d = (DELAY_32 *) calloc(sizeof(DELAY_32), MAXTAPS);

		if ( tw->warmup == LOAD_WARMUP ) {
		  if ( (fp=fopen(warmFile,"r")) == NULL ) {
		    fprintf(stderr,"unable to open IIR warmup file %s\n",
			    warmFile);
		    exit(-1);
		  }
		  if ( (i=fread(d,sizeof(DELAY_32),MAXTAPS,fp)) != MAXTAPS ) {
		    fprintf(stderr,"unable to read IIR warmup file %s\n",
			    warmFile);
		    fprintf(stderr,"only able to read in %d items when should be %d\n",
			    i,MAXTAPS);
		    fclose(fp);
		    exit(-1);
		  }
		  fclose(fp);

		} else {
		  for (i = 0; i < MAXTAPS; i++) {	/* Set all memory elements to 0 */
		    dm = &d[i];
		    for(j = 0; j < 32; j++){
		      dm->dr[j] = 0.0;
		      dm->di[j] = 0.0;

		    }
		  }
		}
/***CUT HERE ** This FOR loop is just for testing 
		  for(j = 0; j < 32; j++){
			printf("iir32: filter bank a[%d]: %2.64e\n",j,f->sect[j].a);
			printf("iir32: filter bank b[%d]: %2.64e\n",j,f->sect[j].b);
		  } CUT HERE  ***/
	}

/* check if need to store delay line for warmup */

	if ( storeFlag == 1 ) {
	  if ( tw->warmup == GEN_WARMUP ) {
	    if ( (fp=fopen(warmFile,"w")) == NULL ) {
	      fprintf(stderr,"unable to open IIR warmup file %s\n",
		      warmFile);
	      exit(-1);
	    }
	    if ( (i=fwrite(d,sizeof(DELAY_32),MAXTAPS,fp)) != MAXTAPS ) {
	      fprintf(stderr,"unable to write IIR warmup file %s\n",
		      warmFile);
	      fprintf(stderr,"only able to write %d items when should be %d\n",
		      i,MAXTAPS);
	      fclose(fp);
	      exit(-1);
	    }
	    fclose(fp);

	    return;
	  }
	}

/* Standard processing if not the first call */

	dm = &d[partnum];		/* Init pointer for this path */

	for (filtnum = 0; filtnum < ntaps; ++filtnum) {   /* Normally, ntaps = 1 for mobile channel */
	  /**      printf("INPUT TO Gaussian filter, = %f %f\n",iqbuf[filtnum].r,iqbuf[filtnum].i);  **/


	  yr = f->ampl * iqbuf[filtnum].r + dm->dr[0];    /* Calc the output samples */
	  yi = f->ampl * iqbuf[filtnum].i + dm->di[0];

	    for (i = 0; i < 31; i++){			  /* Update the memory elements */
	      dm->dr[i] = (f->sect[i].b * iqbuf[filtnum].r) - (f->sect[i].a * yr) + dm->dr[i+1];
	      dm->di[i] = (f->sect[i].b * iqbuf[filtnum].i) - (f->sect[i].a * yi) + dm->di[i+1];
/*	      printf("b[%d] = %f \t a[%d] = %f\n",i,f->sect[i].b,i,f->sect[i].a);       */
	    }
	  dm->dr[31] = (f->sect[31].b * iqbuf[filtnum].r) - (f->sect[31].a * yr);
	  dm->di[31] = (f->sect[31].b * iqbuf[filtnum].i) - (f->sect[31].a * yi);

/* That finished up the 31st (last) delay element */

		iqbuf[filtnum].r = yr;
		iqbuf[filtnum].i = yi;
	        dm++;
	        f++;
		count++;
/*	        printf("output of Gaussian filter, count %d %2.64e\n", count, yr);		*/
	}
}


		

	
