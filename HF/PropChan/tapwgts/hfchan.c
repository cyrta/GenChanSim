/* Generate one snapshot of the impulse response of a
 * disturbed HF channel

   copyright (c) 1994 The MITRE Corporation Bedford, MA

   $Id: hfchan.c,v 2.4 1994/08/30 19:04:48 jjb Exp $
   $Log: hfchan.c,v $
 * Revision 2.4  1994/08/30  19:04:48  jjb
 * fixed error in passing arguments to doppler routine. Always
 * passed buffer containing 1st paths.
 *
 * Revision 2.3  1994/07/26  15:41:55  jjb
 * incorrectly commented out filterbank update after weight functions
 * were called. The code is now uncommented. Problem occurred when
 * weight function changed the number of taps for a given path.
 *
 * Revision 2.2  1994/07/25  19:20:47  jjb
 * added copyright notice
 *

 */
#include <stdio.h>
#include <math.h>
#include <complex.h>
#include <propchan.h>

/* defines */
#define NJUMPINGJACKS 100	/* # loops to warm-up IIR filter */

extern void WriteFloat(char [], char *, float *, int);
extern void WriteComplex(char [], char *, fcomplex *, int);

static char rcsid[] = "$Id: hfchan.c,v 2.4 1994/08/30 19:04:48 jjb Exp $";

void hfchan(
	TAPWGTS *tw,		/* stuct of tapwgts() parameters */
	fcomplex *wgts[],	/* weights of ea path */
	int path)		
	/* path to assign weights to */
{
	static	unsigned int phasor[MAXTAPS];
	static	firstcall = YES;				/* first call to function flag */
	static	firstpath[NPATHS];				/* flag for 'warming-up' filters */
	static	float trigtbl[TRIGLEN+1];		/* trig tables for Doppler shift*/
	static	float weight_buff[MAXTAPS];		/* */
	static	fcomplex normresp[MAXTAPS];		/* */
	static	unsigned int phaseincr[MAXTAPS];/* */
	static	int sttapnum[NPATHS];			/* start tap number of path */
	static	float lpgscal[4];		/* Gaussian random number scale factors */
	static	float doppscal;			/* 2**-TRIGSHFT scale factor in doppler */
	static	fcomplex iqbuf[MAXTAPS];/* tap weights */
	int             i;
	int		j;				/* indices */
	int             k;
	int		p;						/* path index */
	fcomplex *ptr1, *ptr2;
	double	pscale;

	if (firstcall) {							/* Do global initialization */
		firstcall = NO;
		for(j = 0; j < NPATHS; j++) firstpath[j] = YES;		/* set flags */
		doppscal = (float) ldexp (1., -TRIGSHFT);
		for (j = 0; j < 4; j++) {
			lpgscal[j] = ldexp(1.0, 6*j-22);   /*scale factor for table lookup*/
		}
		/* set up trig table */
		for (j = 0; j < TRIGLEN; j++) {
			trigtbl[j] = (float) cos(PI*2*j/TRIGLEN);
		}
		/* next line needed so that if trigtbl[index] is in range it remains
		 * valid for trigtbl[inex1+1] (required by doppler.c) */
		trigtbl[TRIGLEN] = trigtbl[0];
		/* find starting filter number of each path */
		sttapnum[0] = 0;
		for (p = 1; p < tw->nPaths; p++) {
			sttapnum[p] = sttapnum[p-1] + tw->nTaps[p-1];
		}

		for(p = 0; p < tw->nPaths; p++) {
			if(tw->mode[p] == NORM) { /* normal mode */
				qphas(tw->nTaps, tw->groupDelay, tw->freqWinTransLen,
				tw->timeWinTransLen, normresp, sttapnum, p, tw->sampleRate);
			} else { /* disturbed mode */
				if (tw->thinPhScr) {
					weight(weight_buff, sttapnum, tw->multiPathSpr,
						tw->nTaps, tw->thinPhaseC, tw->thinPhaseThresh, p);
				} else {
					for (j = 0; j < MAXTAPS; j++) weight_buff[j] = 1.0; 
				}
			}
			/* phaseincr calculation for Doppler*/
			for (j = sttapnum[p]; j < sttapnum[p] + tw->nTaps[p]; j++) {
				pscale = ldexp(1.0, 32)/tw->snapshot[p];
				phasor[j] = 0;
				phaseincr[j] = (long int)(tw->dplOffset[j] * pscale);
			}
		}

		/* set pointers to weights within 'iqbuf' */
		for(j = 0; j < tw->nPaths; j++) {
			wgts[j] = &iqbuf[sttapnum[j]];
		}

		/* adjust filter bank values for more taps */

		p = 0;
		for(i = 0; i < tw->nPaths; i++) {
			for(j = 0; j < tw->nTaps[i]; j++) {
				tw->filterBank[p].ampl = tw->filterBank[0].ampl;
				for(k = 0; k < 4; k++) {
					tw->filterBank[p].sect[k].a1 = tw->filterBank[0].sect[k].a1;
					tw->filterBank[p].sect[k].a2 = tw->filterBank[0].sect[k].a2;
					tw->filterBank[p].sect[k].b1 = tw->filterBank[0].sect[k].b1;
					tw->filterBank[p].sect[k].b2 = tw->filterBank[0].sect[k].b2;
				}
				p++;
			}
		}

	}
	/* Generate next snapshot */
	if(tw->mode[path] == DIST) {				/* disturbed mode */
		if(firstpath[path]) {					/* if first path weights... */
			/* warm-up filter */
			for(j = 0; j < NJUMPINGJACKS; j++) {
				cgauss48(&iqbuf[sttapnum[path]], tw->nTaps[path]);
				iir4(&iqbuf[sttapnum[path]], &weight_buff[sttapnum[path]], 
					&tw->filterBank[sttapnum[path]], tw->nTaps[path], path);
			}
			firstpath[path] = NO;
		}
		cgauss48(&iqbuf[sttapnum[path]], tw->nTaps[path]);

		iir4(&iqbuf[sttapnum[path]], &weight_buff[sttapnum[path]], 
			&tw->filterBank[sttapnum[path]], tw->nTaps[path], path);

	} else {		/* normal channel */
		ptr1 = &normresp[sttapnum[path]];
		ptr2 = &iqbuf[sttapnum[path]];
		for(j = 0; j < tw->nTaps[path]; j++) {
			ptr2->r = ptr1->r;
			ptr2->i = ptr1->i;
			ptr1++;
			ptr2++;
		}
	}
	doppler(phasor, phaseincr, trigtbl, &iqbuf[sttapnum[path]], 
		tw->nTaps[path], sttapnum[path], doppscal);
}

void WriteComplex(
	char		*filename,	/* 1st part of file name */
	char		*mode,		/* "w", "r", "a", etc. */
	fcomplex	data[],		/* data */
	int			npts)		/* # pts in data[] */
{
	FILE		*dfp;		/* debug file ptr */

	printf("writing '%s'...", filename); fflush(stdout);
	if((dfp = fopen(filename, mode)) == NULL) {
		printf("Warning from WriteComplex: '%s' not opened\n", filename);
	} else {
		fwrite(data, sizeof(fcomplex), npts, dfp);
		fclose(dfp);
	}
	printf("done\n");
}

void WriteFloat(
	char		*filename,	/* 1st part of file name */
	char		*mode,		/* "w", "r", "a", etc. */
	float		data[],		/* data */
	int			npts)		/* # pts in data[] */
{
	FILE		*dfp;		/* debug file ptr */

	printf("writing '%s'...", filename); fflush(stdout);
	if((dfp = fopen(filename, mode)) == NULL) {
		printf("Warning from WriteFloat: '%s' not opened\n", filename);
	} else {
		fwrite(data, sizeof(float), npts, dfp);
		fclose(dfp);
	}
	printf("done\n");
}
