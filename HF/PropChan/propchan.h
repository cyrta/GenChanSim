/* propagation channel simulator header file */
/*

   copyright (c) 1994 The MITRE Corporation Bedford, MA

   $Id: propchan.h,v 2.2 1994/07/25 19:29:19 jjb Exp $
   $Log: propchan.h,v $
 * Revision 2.2  1994/07/25  19:29:19  jjb
 * added copyright notice.
 *

*/
#ifndef _PROPCHAN_H
#define _PROPCHAN_H

#include <complex.h>
#include <stdio.h>

#define	NO			0
#define	YES			1
#define	NPATHS		10		/* number of tapped delay lines */
#define ONE_MHZ		1000000	/* one million (used for scaling MHz <-> Hz) */
/* size of output buffer.  MUST BE COMPATIBLE WITH DESTINATION DEVICE! */
#define OUTBUFFER	1024	/* ^ */
#define NORM		1		/* normal mode of path */
#define DIST		2		/* normal mode of path */
#define	MAXTAPS		1280	/* max number of taps per mode */
#define	NUMGAUSS	2048	/* length of Gaussian Inverse table */
#define	MAXSECT		4		/* N section for a and b */
#define PI			3.1415926535
#define	NPTS		4096	/* length of fft */
#define	HNPTS		2048	/* half length of fft in qphas.c */
#define	NORMCHAN	0		/* normal channel */
#define	DISTCHAN	1		/* disturbed channel */
#define	MIN(a,b)	((a) < (b) ? (a) : (b))	/* minimum */
#define	MAX(a,b)	((a) > (b) ? (a) : (b))	/* maximum */
#define	TRIGLEN		512		/* length of trig table used for doppler shift */
#define	TRIGOFST	128		/* offset to convert cos index to sin index */
#define	TRIGMASK	511		/* # used to mask off int portion of index */
#define	TRIGSHFT	23		/* # of bits to shift index to get int portion */
#define	FILTBANK	"/HF/PropChan/tapwgts/ds10hz.coef"	/* filter bank file */
/* number of samples between generating new slow fading coefficients */
#define	FADE_UPDATE	1000	/* ^ */


/* Delay line element structure for 8th order IIR filter */
typedef struct delay_4 {
  float dr1;
  float dr2;
  float dr3;
  float dr4;
  float di1;
  float di2;
  float di3;
  float di4;
} DELAY_4;

/* coefficients of a single biquad section */
typedef struct biquad { 
	float a1;
	float a2;
	float b1;
	float b2;
} BIQUAD;

/* IIR filter Structure */
typedef struct filter {
	float ampl;
	BIQUAD sect[MAXSECT];
} FILTER;

typedef struct TAPWGTS {			/* structure for tapwgts() */
	float	dplShift[NPATHS];		/* doppler shift */
	float	snapshot[NPATHS];		/* snap shot rate of ea path */
	float	groupDelay[NPATHS];		/* group delay of ea path */
	float	multiPathSpr[NPATHS];	/* rms multipath spread (disturbed) */
	float	sampleRate;				/* sample rate (Hz) */
	float	dplSpread[NPATHS];		/* delay spread of ea path */
	float	thinPhaseC;				/* thin phase screening param c */
	float	thinPhaseThresh;		/* thin phase theshold param */
	int		nPathsNorm;				/* number of normal paths */
	int		nPathsDist;				/* number of disturbed paths */
	int		nPaths;					/* total number of paths (normal+dist) */
	int		thinPhScr;				/* thin phase screening (0 = rect) */
	int		mode[NPATHS];			/* disturbed or normal (DIST, NORM) */
	int		nTaps[NPATHS];			/* # taps/path */
    float   dplOffset[MAXTAPS];		/* doppler offset for ea tap */
    float	freqWinTransLen;		/* freq window transition len */
    float	timeWinTransLen;		/* time window transition len */
    FILTER	filterBank[MAXTAPS];	/* memory */
} TAPWGTS;

/* functions */
extern void hfchan(TAPWGTS *, fcomplex *[], int);
extern void PrintTapWgts(TAPWGTS *);
extern void fade(float [], float [], float [], int, int);
extern FILE *FOpen(char *filename, char *mode);
extern void qphas(int [], float [], int, int, fcomplex [], int [], int, float);
extern void weight(float [], int [], float [], int [], float, float, int);
extern void iir4(fcomplex [], float [], FILTER [], int, int);
extern void doppler(unsigned int [], unsigned int [], float [], fcomplex [],
	int, int, float);
extern void WriteDebug();

#endif
