/* Generate complex fading coefficients by passing gaussian noise through a
 * low-pass filter.  This routine uses a single filter to generate several
 * Doppler Spreads.  The sole filter uses has a Doppler Spread of 20.0 KHz @
 * 1.0 MHz.  The Doppler spreads are selected randomly using the input to 
 * determine the maximum value.
 * r.coutts

   copyright (c) 1994 The MITRE Corporation Bedford, MA

   $Id: iirfade.c,v 2.2 1994/07/25 19:06:52 jjb Exp $
   $Log: iirfade.c,v $
 * Revision 2.2  1994/07/25  19:06:52  jjb
 * added copyright notice
 *

 */
/* includes */
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <complex.h>
#include <narrowband.h>
#include <iirfade.h>

/*
#define	DEBUG
 */

static char rcsid[] = "$Id: iirfade.c,v 2.2 1994/07/25 19:06:52 jjb Exp $";

void iirfade(
	fcomplex	data[],			/* complex fading (returned) */
	int			nsamples,		/* number of samples in data[] */
	int			interp,			/* interpolation factor */
	DELAY_4		*delay[],		/* filter delays */
	fcomplex	*firInit[])		/* fir interpolation filter initial states */
{
	/* filter bank file name */
	#ifdef DEBUG
		char	filename[100];		/* output file */
		static int	call = 0;		/* call to function count */
		void WriteDebug(char *, fcomplex [], int);
	#endif
	int			npts;				/* npts in signal */
	int			lininterp;			/* linear interpolation factor */
	int			filtinterp;			/* filtered interpolation factor */

	if(interp > 64) {			/* if too much interpolation for interp64()...*/
		filtinterp = 64;		/* only do 64 interpolation with interp64() */
	} else {					/* if not too many... */
		filtinterp = interp;	/* do all interpolations with interp64() */
	}
	lininterp = interp/filtinterp;	/* do the rest with linear interpolator */
	npts = ceil((double)nsamples/(filtinterp*lininterp));/* # pts in signal */

	cgauss48(data, npts);			/* generate gaussian complex number */

	#ifdef DEBUG
		/* create inpulse for scaling filters */
		/*
		if(call == 0) {
			data[0].r = 1; data[0].i = 0;
			for(i = 1; i < npts; i++) data[i].r = 0, data[i].i = 0;
		}
		 */
		sprintf(filename, "gauss%c.dbg", 'a'+call);
		WriteDebug(filename, data, npts);
	#endif

	iirfilter(data, npts, delay);	/* do iir filtering */

	#ifdef DEBUG
		sprintf(filename, "iirfilter%c.dbg", 'a'+call);
		WriteDebug(filename, data, npts);
	#endif

	interp64(data, nsamples, npts, filtinterp, firInit);	/* interpolate */

	npts *= filtinterp;							/* # pts increase by interp'n */

	#ifdef DEBUG
		sprintf(filename, "interp64%c.dbg", 'a'+call);
		WriteDebug(filename, data, npts);
	#endif

	linearinterp(data, nsamples, npts, lininterp);/* interpolate by 'interp' */
	npts *= lininterp;							/* # pts increased */

	#ifdef DEBUG
	#ifdef LINEARINTERP_DBG
		sprintf(filename, "linearinterp%c.dbg", 'a'+call);
		WriteDebug(filename, data, npts);
	#endif
	call++;									/* incr call to function count */
	#endif
}

#ifdef	DEBUG
void WriteDebug(
	char	*filename,			/* 1st part of file name */
	fcomplex	data[],			/* data */
	int			npts)			/* # pts in data[] */
{
	FILE		*dfp;			/* debug file ptr */

	if((dfp = fopen(filename, "w")) == NULL) {
		printf("Warning from iirfade: '%s' not opened\n", filename);
	} else {
		fwrite(data, sizeof(fcomplex), npts, dfp);
		fclose(dfp);
	}
}
#endif


/* perform lowpass IIR filtering on 'data' */
void iirfilter(
	fcomplex	data[],				/* complex data samples (input & output) */
	int			npts,				/* number of pts in data[] */
	DELAY_4		*delay[])			/* filter delay buffers */
{
	int			i;					/* index */
	static int firstcall = 1;		/* 1st call to function flag */
	static FILTER filtbank;			/* LPF filter coefficients */

	if(firstcall) {					/* if 1st call to function... */
		firstcall = 0;				/* enter 'if' statement only once */
		getfiltbank(NBFILTBANK, &filtbank);
	}

	/* pass noise thru LPF */
	for(i = 0; i < npts; i++) {
		iir(&data[i], &filtbank, delay);
	}
}


/* upsample signal for by linear interprelating for 'dec' cycles */
void linearinterp(fcomplex data[], int nsamples, int ndecpts, int dec) {

	/* upsample real part of signal */
	interp(data, nsamples, ndecpts, dec);
	/* upsample imag part of signal */
	interp((fcomplex *)&data[0].i, nsamples, ndecpts, dec);
}

/* interpolate the real component of complex sample (real or imag)
 * this function can be fooled into interpolating the imag portion by passing
 * &data[0].i to data[]
 */
void interp(fcomplex data[],			/* complex samples to interpolate */
	int dim,							/* dimension of data[] */
	int npts,							/* number of points in data[] */
	int n)								/* interpolation factor */
{
	float		delta;					/* amplitude step for interpolation */
	int			i;						/* index */
	int			j;						/* index */
	int			index;					/* index */

	/* if just one sample, copy and return */
	if(npts == 1) {						/* if only one interpolation pt */
		for(i = 1; i < dim; i++) {		/* loop thru all but 1st sample... */
			data[i] = data[0];			/* set equal to 1st samples */
		}
		return;
	}
	/* check if too many samples passed, don't overflow */
	if(n*npts > dim) {					/* if too many points... */
		npts = ceil((float)dim/n)+1;	/* don't process all points */
		i = npts-2;						/* starting at 2nd to last sample... */
		delta = (data[i+1].r-data[i].r)/n;	/* amp step for interpolation */
		for(j = 1; j <= n; j++) {			/* for each hold sample... */
			index = i*n+j;					/* index to write to */
			if(index >= dim) break;			/* if about to overflow...break */
			data[i*n+j].r = data[i].r+j*delta;	/* add step */
		}
	}
	/* upsample rest of signal (don't check for overflow) */
	for(i = npts-3; i >= 0; i--) {			/* starting 2nd to last sample... */
		delta = (data[i+1].r-data[i].r)/n;	/* amp step for interpolation */
		for(j = 1; j <= n; j++) {			/* for each hold sample... */
			data[i*n+j].r = data[i].r+j*delta;	/* add step */
		}
	}
}

/* read in filter bank file */
void getfiltbank(char filename[], FILTER *f) {
	int		i = 0;				/* index */
	FILE	*fp;				/* filename file ptr */
	char	*envpath;			/* path to enviromental variable */
	char	file[100];			/* path and filename */

	/* get path to simulation */
	if((envpath = getenv("HFDIR")) == (char *)NULL) {
		printf("Error from narrowband:  must 'setenv HFDIR ~/hf'\n");
		exit(1);
	}
	strcpy(file, envpath);			/* add path to file name */
	if(file[strlen(file)-1] != '/') strcat(file, "/");
	strcat(file, filename);			/* add filename to file name */

	/* open file */
	if((fp = fopen(file, "r")) == NULL) {
		printf("Error from narrowband(): filter file '%s' not opened\n", file);
		exit(1);
	}
	/* read amplitude */
	fscanf(fp, "%f", &(f->ampl));
	/* read coeff */
	while(fscanf(fp, "%f%f%f%f",
	     &(f->sect[i].a1), &(f->sect[i].a2),
	     &(f->sect[i].b1), &(f->sect[i].b2)) != EOF)
	{
		i++;
	}
	fclose(fp);           /* close file */
	/* blank remaining filter coefficients */
	for(; i < 4; i++) {
		f->sect[i].a1 = f->sect[i].a2 =
		f->sect[i].b1 = f->sect[i].b2 = 0.0;
	}
}

/* Interpolate the signal by 2, 4, 8, 16, 32, or 64 to by using multiple stages
 * of x2 interpolation by zero insertion and filtering.
 */
void interp64(
	fcomplex	data[],		/* complex data samples */
	int			datadim,	/* dimension of data[] */
	int			ndatas,		/* # elements in data[] */
	int			interp,		/* interpolation factor */
	fcomplex	*firInit[])	/* fir interpolation filter initial states */
{
	static int firstcall = 1;			/* flag for 1st call to function */
	static float interp1[NPTSFILTER1];	/* 1st filter coeff */
	static float interp2[NPTSFILTER2];	/* 2nd filter coeff */
	static float interp3[NPTSFILTER3];	/* 3rd filter coeff */
	static float interp4[NPTSFILTER4];	/* 4th filter coeff */
	static float interp5[NPTSFILTER5];	/* 5th filter coeff */
	static float interp6[NPTSFILTER6];	/* 6th filter coeff */

	if(firstcall) {			/* if first call to function... */
		firstcall = 0;		/* only enter this statement once */
		readfilterfile(INTERPFILTER1, interp1, NPTSFILTER1);
		readfilterfile(INTERPFILTER2, interp2, NPTSFILTER2);
		readfilterfile(INTERPFILTER3, interp3, NPTSFILTER3);
		readfilterfile(INTERPFILTER4, interp4, NPTSFILTER4);
		readfilterfile(INTERPFILTER5, interp5, NPTSFILTER5);
		readfilterfile(INTERPFILTER6, interp6, NPTSFILTER6);
	}
	if(interp >= 2) {
		zerofilter(data, datadim, ndatas, interp1, NPTSFILTER1, firInit[0]);
	}
	if(interp >= 4) {
		zerofilter(data, datadim, 2*ndatas, interp2, NPTSFILTER2, firInit[1]);
	}
	if(interp >= 8) {
		zerofilter(data, datadim, 4*ndatas, interp3, NPTSFILTER3, firInit[2]);
	}
	if(interp >= 16) {
		zerofilter(data, datadim, 8*ndatas, interp4, NPTSFILTER4, firInit[3]);
	}
	if(interp >= 32) {
		zerofilter(data, datadim, 16*ndatas, interp5, NPTSFILTER5, firInit[4]);
	}
	if(interp >= 64) {
		zerofilter(data, datadim, 32*ndatas, interp6, NPTSFILTER6, firInit[5]);
	}
}

/* read in interpolator filter coefficients */
void readfilterfile(char filename[], float data[], int npts) {
	int			i;				/* index */
	FILE		*fp;			/* filter file pointer */
	char		*envpath;		/* path to enviromental variable */
	char		file[100];		/* path and filename */

	if((envpath = getenv("HFDIR")) == (char *)NULL) {
		printf("Error from narrowband:  must 'setenv HFDIR ~/hf'\n");
		exit(1);
	}
	strcpy(file, envpath);			/* add path to file name */
	strcat(file, filename);			/* add filename to file name */

	if((fp = fopen(file, "r")) == NULL) {
		printf("Error from narrowband: filter file '%s' not opened\n", file);
		exit(1);
	}

	for(i = 0; i < npts; i++) {
		if(fscanf(fp, "%f", &data[i]) == EOF) {
			printf("Error from narrowband: out of points in interpolator "
				"filter file '%s'.  ", file);
			printf("There should be %d points, but file contains %d points!\n",
				npts, i);
			exit(1);
		}
	}
	if(fscanf(fp, "%f", &data[i]) != EOF) {
		printf("WARNING: found extra filter coefficients in '%s'\n", file);
	}
	fclose(fp);
}


/* Perform zero insertion and filter complex data via a floating point fir
 * filter.  The zero insertion is done by only using every other filter
 * coefficient on a given pass.
 * The number of valid points in the output is the number of input data points.
 * The output is written into the input array data[].
 * The function uses a queue to retain data so that input data isn't stomped on.
 * Zero insertion is done after the points if an there is an even number of 
 * filter coefficients, and before if there is an odd number of coefficients.
 * The function algorithm approach is simular to a convolution.
 *
 * starting configuration:
 *
 * [][][][][][][][][][][][][][][][][][][][][][][][]   data
 *                                     <-- [][][][]   q
 *                                     [][][][][][]   filter
 * [][][][][][][][][][][][][][][][][][][][][][][][]   data
 *
 * Modification: 12/17/93 - multiply each sample by 2.0 to compensate for the
 * power lost due to zero insertion. r.coutts
 */
#define		M				2		/* factor of interpolator */
void zerofilter(
	fcomplex	data[],			/* input data (modified) */
	int			dim,			/* dimension of data[] */
	int			ndatas,			/* # pts in data[] */
	float		filter[],		/* filter coeff */
	int			nFiltPts,		/* # pts in filter[] */
	fcomplex	firInit[])		/* fir interpolation filter initial states */
{
	fcomplex	sum;			/* complex sum of filter pass */
	int			index, i, j, s;	/* indices */
	fcomplex	*q;				/* complex data queue */
	int			r;				/* index to 'q' */
	int			r0;				/* index to front of 'q' */
	int			start;			/* starting data index */
	int			qsize;			/* number of elements in 'q' */
	fcomplex	zero = {0.0, 0.0};	/* 0.0 + i*0.0 */
	static fcomplex	tempFirInit[NPTSFILTER1-1];	/* temporary storage of initial
								 * states for next section of interferer.  Note
								 * that NPTSFILTER1 is assumed to be the largest
								 * filter and is, therefore, used to dimension
								 * the array. */
 
	/* set some variables */
	qsize = ceil((double)nFiltPts/M);	/* size of 'q' */
	start = ndatas-qsize;		/* number of filter passes */
	/* alloc queue memory */
	q = (fcomplex *)malloc(qsize*sizeof(fcomplex));
	if(q == NULL) {
		printf("Oh, oh... big error from zerofilter(): memory not properly "
			"allocated for queue.\n");
		exit(1);
	}
	/* multiply values by 2.0 to compensate for lost power by zero insertion */
	for(i = 0; i < ndatas; i++) {
		data[i].r *= 2.0;
		data[i].i *= 2.0;
	}
	/* fill queue with the last data pts (leave q[0] empty) */
	r = 1;						/* set q index to 2nd element */
	for(i = start+1; i < ndatas; i++) {
		q[r++] = data[i];		/* fill queue with end of data */
	}
	/* save initial states for next section of interferer */
	if(firInit != NULL) {		/* if initial states exist... */
		/* save end of data buffer as init states for next section of interf */
		j = 0;
		for(i = ndatas-nFiltPts+1; i < ndatas; i++) {
			tempFirInit[j++] = data[i];
		}
	}
	r0 = 0;										/* zero is front of q */
	for(i = start; i > -qsize; i--) {			/* for ea data point... */
		if(i >= 0) {							/* if 'i' is in data... */
			q[r0] = data[i];					/* add data to queue */
		} else {								/* if 'i' is NOT in data... */
			q[r0] = zero;						/* data is zero (empty) */
			if(firInit != NULL) {				/* if initial states exist... */
				q[r0] = firInit[nFiltPts-1+i];	/* get inital state */
				/* -1 because firInit[] contains nfilterPts-1 points */
			}
		}
		for(s = 0; s < M; s++) {				/* two passes in one local */
			sum.r = 0.0;						/* clear real part */
			sum.i = 0.0;						/* clear imag part */
			r = r0;								/* init q intex to front */
			/* index every other filter[] coeff to simulate zero insertion */
			for(j = nFiltPts-1-s; j >= 0; j -= M) {	/* for ea filter coeff... */
				sum.r += q[r].r*filter[j];		/* (real)x(filt coeff) */
				sum.i += q[r].i*filter[j];		/* (imag)x(filt coeff) */
				if(++r == qsize) r = 0;			/* incr q index, wrap? */
			}
			index = (i+qsize)*M-1-s;			/* index to output (input) */
			if(index >= dim) continue;			/* if overflow, cont */
			data[index].r = sum.r;		/* store the real output in data[].r */
			data[index].i = sum.i;		/* store the imag output in data[].i */
		}
		if(--r0 < 0) r0 = qsize-1;	/* move front of queue local */
	}
	/* copy initial states from temporary storage */
	if(firInit != NULL) {		/* if initial states exist... */
		/* save end of data buffer as init states for next section of interf */
		for(i = 0; i < nFiltPts-1; i++) {
			firInit[i] = tempFirInit[i];
		}
	}
	free(q);							/* free queue memory */
}
