/* propagation channel simulator header file */
/* Modified 2/94 by C. Nissen to include parameters for mobile channel */
/*

   copyright (c) 1994 The MITRE Corporation Bedford, MA

   $Id: flatfade.h,v 4.0 1996/07/15 19:07:06 cmk Exp $
   $Log: flatfade.h,v $

   Revision 4.0 1996.07/14 cmk
   altered to run with flatfade model

   Revision 3.0  1994/08/31 19:07:06  jjb
   added constants and structure elements for fractional delays
   used in rate conversion, loading/generating filter warmup files,
   mixed scatter/discrete path model (macrocell), and Jakes/flat
   doppler spectrum.

 * Revision 2.2  1994/07/25  18:57:02  jjb
 * added copyright notice
 *

*/
#ifndef _PROPCHAN_H
#define _PROPCHAN_H

#include <complex.h>
#include <stdio.h>

#define	NO			0
#define	YES			1
#define NUMSTATES 3                     /* For Lin */
#define NUMTRANS 9                      /* For Lin */
#define	CLEAR			0       /* For Lin */
#define SHADOWED                1       /* For Lin */
#define BLOCKED                 2       /* For Lin */
#define	NPATHS		20	/* number of tapped delay lines */
#define ONE_MHZ		1000000	/* one million (used for scaling MHz <-> Hz) */
#define LIGHT_SPEED     3.0E8  /* Velocity of Light in Vacuum */
#define LIGHTSPEED 299790000 /* meters per second */
/* size of output buffer.  MUST BE COMPATIBLE WITH DESTINATION DEVICE! */
#define OUTBUFFER	1024	/* ^ */
#define NORM		1	/* normal mode of path */
#define DIST		2	/* normal mode of path */
#define	MAXTAPS		200	/* max number of taps for all modes combined */
#define	NUMGAUSS	2048	/* length of Gaussian Inverse table */
#define	MAXSECT		32	/* N section for a and b */
#define PI		3.1415926535
#define	NPTS		4096	/* length of fft */
#define	HNPTS		2048	/* half length of fft in qphas.c */
#define	NORMCHAN	0	/* normal channel */
#define	DISTCHAN	1	/* disturbed channel */
#define	MIN(a,b)	((a) < (b) ? (a) : (b))	/* minimum */
#define	MAX(a,b)	((a) > (b) ? (a) : (b))	/* maximum */
#define	TRIGLEN		512	/* length of trig table used for doppler
				 * shift */
#define	TRIGOFST	128	/* offset to convert cos index to sin index */
#define	TRIGMASK	511	/* # used to mask off int portion of index */
#define	TRIGSHFT	23	/* # of bits to shift index to get int
				 * portion */
                       /* filter bank files */
#define	GAUSSIAN_FILTBANK "/MobileChan/PropChan/SatMobile/tapwgts/gaussian_filt.coef"	
#define	FLAT_FILTBANK	"/MobileChan/PropChan/SatMobile/tapwgts/flat_filt.coef"

/* define constants for power spectrum selection used in powSpectrum field
   of TAPWGTS structure */

#define GAUSSIAN_SPECTRUM  1 /* gaussian power spectrum selected */
#define FLAT_SPECTRUM   2   /* flat power spectrum selected */
#define DATA_SPECTRUM   3   /* data power spectrum selected */

/* define file names for storing interpolator warmup values */

#define FLAT_IIR_WRM_FILE     "/MobileChan/PropChan/SatMobile/tapwgts/iir_flat_warm.dat"
#define GAUSSIAN_IIR_WRM_FILE    "/MobileChan/PropChan/SatMobile/tapwgts/iir_gaussian_warm.dat"
#define DATA_IIR_WRM_FILE    "/MobileChan/PropChan/SatMobile/tapwgts/iir_data_warm.dat"
#define FLAT_INTERP_WRM_FILE  "/MobileChan/PropChan/SatMobile/tapwgts/interp_flat_warm.dat"
#define GAUSSIAN_INTERP_WRM_FILE "/MobileChan/PropChan/SatMobile/tapwgts/interp_gaussian_warm.dat"
#define DATA_INTERP_WRM_FILE "/MobileChan/PropChan/SatMobile/tapwgts/interp_data_warm.dat"

/* define some constants for warmup mode */

#define DO_WARMUP     0      /* perform warmup when run code */
#define LOAD_WARMUP   1      /* load warmup values from files */
#define GEN_WARMUP    2      /* generate warmup files */

/* define constants for model selection */

#define LUTZ_MODEL        1   /* Lutz Markov model */
#define FLATFADE_MODEL  2   /* flat fading model model */
#define LIN_MODEL        3   /* Lin Markov model */
#define DEBUG_MODEL      4   /* model used for debugging */

/* number of samples between generating new slow fading coefficients */
#define	FADE_UPDATE	1000	/* ^ */


/* Delay line element structure for 32nd order IIR filter */
typedef struct delay_32 {
	double	dr[32];	
	double	di[32];
}               DELAY_32;

/* coefficients of a single biquad section */
typedef struct biquad {
	double           a;
	double           b;
}               CANON;

/* IIR filter Structure */
typedef struct filter {
	double           ampl;
	CANON          sect[MAXSECT];
}               FILTER;

typedef struct TAPWGTS {	/* structure for tapwgts() */
	float           dplShift[NPATHS];	/* doppler shift */
	float		VehSpd;			/* Maximum Vehicle Speed (Not an array since Dshift is the same for all */
	float		carFreq;	/* Carrier Frequency */
	float           snapshot[NPATHS];	/* snap shot rate of ea path */
	float           groupDelay[NPATHS];	/* group delay of ea path */
	float           multiPathSpr[NPATHS];	/* rms multipath spread
						 * (disturbed) */
	float           sampleRate;	/* sample rate (Hz) */
	float           scatterSampleRate;	/* sample rate (Hz) for scatter path */
	float           dplSpread[NPATHS];	/* delay spread of ea path */
	float           thinPhaseC;	/* thin phase screening param c */
	float           thinPhaseThresh;	/* thin phase theshold param */
	float           expTau[NPATHS];         /* exponential time const. tau */
        float           scaledsigma[NPATHS]; /* used to scale IIR filter for power spectrum */
	int             nPathsNorm;	/* number of normal paths */
	int             nPathsDist;	/* number of disturbed paths */
	int             nPaths;         /* total number of paths (normal+dist) */
	int             powSpectrum;    /* used to indicate power spectrum */
	int             thinPhScr;	/* thin phase screening (0 = rect) */
	int             warmup;         /* indicates warmup mode selected */
	int             model;          /* model to run (JTC,FLATFADE) */
	int             mode[NPATHS];	/* disturbed or normal (DIST, NORM) */
	int             nTaps[NPATHS];	/* # taps/path */
	float           weighting[MAXTAPS];     /* weighting array without doppler added */
	float           weightLimit;            /* limit for weighting */
	float           dplOffset[MAXTAPS];	/* doppler offset for ea tap */
	float           freqWinTransLen;	/* freq window transition len */
	float           timeWinTransLen;	/* time window transition len */
	float		bulkDelay[NPATHS];	/* used to print out path gain can 4/94 */
	float		gain[NPATHS];		/* used to print out path gain can 4/94 */
        float           stategain[NPATHS][NUMSTATES]; /* Gains to print out for Lin model */
	FILTER          filterBank[MAXTAPS];	/* memory */
}               TAPWGTS;

/* functions */
extern void     PrintTapWgts(TAPWGTS *);
extern void     fade(float[], float[], float[], int, int);
extern FILE    *FOpen(char *filename, char *mode);
extern void     qphas(int[], float[], int, int, fcomplex[], int[], int, float);
extern void     weight(float[], int[], float[], int[], float, float, int);
extern void     iir4(fcomplex[], float[], FILTER[], int, int);
extern void 
doppler(unsigned int[], unsigned int[], float[], fcomplex[],
	int, int, float, int);
	extern void     WriteDebug();

#endif

