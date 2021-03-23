/* atmospheric noise header file

   copyright (c) 1994 The MITRE Corporation Bedford, MA

   $Id: atmosphere.h,v 2.2 1994/07/25 18:06:33 jjb Exp $
   $Log: atmosphere.h,v $
 * Revision LL 0.0 1996/10/10 cmk
 * fixed the path to c_sigma.table
 *
 * Revision 2.2  1994/07/25  18:06:33  jjb
 * added copyright notices
 *

 */

#ifndef _ATMOSPHERE_H
#define _ATMOSPHERE_H

#include <complex.h>

#define	YES				1
#define	NO				0
#define	MAXRAND			2147483647.0	/* max random number */
#define	INF				10000.0			/* infinity for integration */
#define	SHORTESTSTROKE	10e-3			/* shortest stroke duration (mS) */
#define	LONGESTSTROKE	4				/* longest stroke duration (mS) */
#define	K				4.2536			/* average # strokes per flash */
#define	BINMIN			-50				/* min bin value of histogram */
#define	BINMAX			40				/* max bin value of histogram */
#define	BINSTEP			1.0				/* histogram bin step */
#define	VDMIN			3.0				/* minimum calibrated Vd */
#define	VDMAX			11.0			/* minimum calibrated Vd */
#define NVDS			9				/* number of vd values in table */
#define	FLASHRATEMIN	50.0			/* minimum calibrated flash rate */
#define	FLASHRATEMAX	200.0			/* minimum calibrated flash rate */
#define	FLASHRATESTEP	25.0			/* res'n betw flash rates */
#define	NFLASHRATES		7				/* # of flash rate value in table */
#define	CANDSIGMAFILE	"HF/AddDist/atmosphere/c_sigma.table"	/* calibrated table of 
													 * C & Sigma */

typedef struct VD {float c, sigma;} VD;

typedef struct ATMOSSTATS {
	double		ampsum;			/* sum of the output amplitudes */
	double		ampsqsum;		/* sum of the output amplitudes squared */
	double		rmssum;			/* sum of the stroke amplitudes */
	double		rmssqsum;		/* sum of the stroke amplitudes squared */
	float		c;				/* c value from p.bello's paper */
	float		sigma;			/* sigma value from p.bello's paper */
	float		vd;				/* calc Vd value from p.bello's paper */
	int			nnonzeros;		/* number of nonzero amplitudes */
	int			nzeros;			/* number of zero amplitudes */
	int			nstrokes;		/* # of strokes */
	int			nflashes;		/* # of flashes */
	int			npts;			/* number of points filed */
    /* histogram parameters */
    float    	min;			/* min bin value (dB) */
    float    	max;			/* max bin value (dB) */
	float		span;			/* span of histogram (dB) */
    float    	step;			/* bin step value (dB) */
	float		*x;				/* ea x value of bins */
	int			*n;				/* count value of bins */
	int			nbins;			/* number of bins */
} ATMOSSTATS;

typedef struct STROKE {			/* stroke information queue */
	int		flash;				/* flash number (0, #flashes) */
	int		stroke;				/* stroke number (0, #strokes) */
	float	time;				/* start time of stroke (seconds) */
	float	duration;			/* duration of stroke (seconds) */
	float	rms;				/* stength of stroke (volts) */
	struct STROKE	*next;		/* next stroke in queue */
} STROKE;

/* functions */
VD InterpCandSigma(VD [][NFLASHRATES], float, float);
float Interp(float, float, float, float, float);
extern void InitStats(ATMOSSTATS *);
extern void OutputStats(ATMOSSTATS *, int, float);
extern STROKE *AddStroke(STROKE *);
extern int NumStrokes();
extern float NextStroke();
extern float StrokeDuration(float, float);
extern void IntegrateStroke(int, fcomplex *, int, fcomplex *, int);
extern int Output(char [], ATMOSSTATS *, int, fcomplex *, int);
extern float RandPoisson(float);
extern float StrokeRms(float);
extern void ScaleComplex(float, fcomplex [], int);
extern void PrintFlash(float);
extern void BinHist(fcomplex [], int, ATMOSSTATS *);
extern int FWriteBuffer(char *, int, int, FILE *, int);
extern void ReadCandSigma(VD [][NFLASHRATES]);
extern float qromb();
extern float f(float);

#endif
