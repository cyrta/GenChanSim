/* narrowband interference header file
 */
/*
   copyright (c) 1994 The MITRE Corporation Bedford, MA

   $Id: narrowband.h,v 2.2 1994/07/25 19:06:52 jjb Exp $
   $Log: narrowband.h,v $
 * Revision LL 0.0 1996/10/10 cmk
 * fixed path to filter and interpolation files
 *
 * Revision 2.2  1994/07/25  19:06:52  jjb
 * added copyright notice
 *

*/

#ifndef _NARROWBAND_H
#define _NARROWBAND_H

#include <stdio.h>
#include <math.h>
#include <complex.h>
#include <filter.h>

#define	RE			0				/* real portion of array */
#define	IM			1				/* imaginary portion of array */
#define	MAXINTERFLEN	1000000		/* max # of samples in interferer */
#define	COMPLEX		2				/* RE and IM */
#define	YES			1
#define	NO			0
#define	BUFFSIZE	512				/* size of fwritebuffer() buffer */
#define	PARETOEXP	0.45			/* Pareto exponent 'a' */
#define SEED    	0				/* r.n.g. seed (zero mean no seed is set) */
#define	NBFILTBANK	"HF/AddDist/narrowband/ds25hz.coef"	/* filter bank file */
#define	BANDWIDTH	0.15625			/* Doppler spread of NBFILTBANK (2.5/16) */
#define	BMIN		0.000001		/* min bandwidth (stops div zero error) */
#define	WMIN		0.000001		/* min Doppler spread (stops div 0 error) */
#define	NINTERPFILTERS	6			/* # of interpolation filters */
#define	INTERPFILTER1	"HF/AddDist/narrowband/interp1.coef"	/* 1st filter 64x interp */
#define	INTERPFILTER2	"HF/AddDist/narrowband/interp2.coef"	/* 2nd filter 64x interp */
#define	INTERPFILTER3	"HF/AddDist/narrowband/interp3.coef"	/* 3rd filter 64x interp */
#define	INTERPFILTER4	"HF/AddDist/narrowband/interp4.coef"	/* 4th filter 64x interp */
#define	INTERPFILTER5	INTERPFILTER4				/* 5th filter 64x interp */
#define	INTERPFILTER6	INTERPFILTER4				/* 6th filter 64x interp */
#define	NPTSFILTER1	28				/* # pts in INTERPFILTER1 */
#define	NPTSFILTER2	11				/* # pts in INTERPFILTER2 */
#define	NPTSFILTER3	8				/* # pts in INTERPFILTER3 */
#define	NPTSFILTER4	6				/* # pts in INTERPFILTER4 */
#define	NPTSFILTER5	NPTSFILTER4		/* # pts in INTERPFILTER5 */
#define	NPTSFILTER6	NPTSFILTER4		/* # pts in INTERPFILTER6 */
#define NJUMPINGJACKS 3000			/* # of samples to warm-up IIR filter */
#define	INTERP		64				/* max interpolation factor */
/* number of gaussian samples to warm-up FIRs */
#define NFIRWARMUPS	37
#define	ONE_MHZ		1000000			/* one million (for scaling) */
#define	NIIRDELAYS	3				/* # delays in filter delay */
#define	PI			3.1415927

typedef struct INTERF {
	struct INTERF *next;			/* ptr to next elem of queue */
	float		time;				/* start time (seconds) */
	float		amp;				/* amplitude */
	float		len;				/* length of interferer (seconds) */
	int			nSamples;			/* number of samples (samplerate*len) */
	float		freq;				/* frequency of interferer (Hz) */
	float		phase;				/* phase of interferer (0.0, 2PI) */
	float		ds;					/* Doppler spread for 1.0Hz */
	float		bw;					/* bandwidth for 1.0Hz */
	int			dsInt;				/* Doppler spread interpolation factor */
	int			bwInt;				/* bandwidth interpolation factor */
	DELAY_4		**dsInitIIR;		/* Doppler spread delay buffers */
	DELAY_4		**bwInitIIR;		/* bandwidth delay buffers */
	fcomplex	**dsInitFIR;		/* fir interpolation filter initial states.
									 * The initial states are used when an
									 * interferer is broken up into segments
									 * to fit into RAM */
	fcomplex	**bwInitFIR;		/* fir interpolation filter initial states*/
} INTERF;

/* function declarations */
extern void OneInterferer(INTERF *, fcomplex *);
extern float InterfMaxLen(INTERF *);
extern void IntegrateInterf(int, fcomplex [], int, fcomplex [], int);
extern void MultComplex(fcomplex *, fcomplex *, int);
extern void iirfade();
extern DELAY_4 **AllocInitIIR(int);
extern INTERF *AddInterf();
extern int InterpFactor(float);
extern float AttainableFreq(int);
extern float InterfAmp(float, float);
extern float InterfLen(float);
extern float RandPoisson(float);
extern float InterfFreq(float);
extern float InterfPhase();
extern char *getenv(char *);
extern void InterfCopy(INTERF *, INTERF *);
extern INTERF *GetInterfStats(float, float, float, float, float, float, float,
	float,float);
extern INTERF *ReadInterf(char [], float);
extern int GetInterfData(INTERF *, int, fcomplex *, int);
extern fcomplex *AllocData(INTERF *, float, float, int *);
extern void WriteInterf(char *, INTERF *);
extern int OutputData(char [], fcomplex [], int, int);
extern void BreakUpInterf(INTERF *, float, int);
extern INTERF *SortInterf(INTERF *);
extern void WarmUpFilters(INTERF *interferer);
extern void WarmUpIIR(DELAY_4 **filterCoef);
extern void WarmUpFIR(DELAY_4 **iirCoef, fcomplex **firCoef);
/* some functions outside of narrowband directory without header files */
extern double drand48();

#endif	/* _NARROWBAND_H */
