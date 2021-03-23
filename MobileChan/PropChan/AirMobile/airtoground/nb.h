/* narrowband interference header file
 
   copyright (c) 1994 The MITRE Corporation Bedford, MA

   $Id: nb.h,v 3.0 1994/08/30 18:51:15 jjb Exp $
   $Log: nb.h,v $
 * Revision 3.0  1994/08/30  18:51:15  jjb
 * updated rev number.
 *
 * Revision 2.2  1994/07/25  18:57:02  jjb
 * added copyright notice
 *

 */

#ifndef _NARROWBAND_H
#define _NARROWBAND_H

#include <stdio.h>
#include <math.h>
#include <complex.h>

#define	RE			0				/* real portion of array */
#define	IM			1				/* imaginary portion of array */
#define	MAXINTERFLEN	1000000		/* max # of samples in interferer */
#define	COMPLEX		2				/* RE and IM */
#define	YES			1
#define	NO			0
#define	BUFFSIZE	512				/* size of fwritebuffer() buffer */
#define	PARETOEXP	0.45			/* Pareto exponent 'a' */
#define SEED    	0				/* r.n.g. seed (zero mean no seed is set) */
#define	NBFILTBANK	"narrowband/ds25hz.coef"	/* filter bank file */
#define	BANDWIDTH	0.15625			/* Doppler spread of NBFILTBANK (2.5/16) */
#define	BMIN		0.000001		/* min bandwidth (stops div zero error) */
#define	WMIN		0.000001		/* min Doppler spread (stops div 0 error) */
#define	NINTERPFILTERS	6			/* # of interpolation filters */
#define	INTERPFILTER1	"/MobileChan/PropChan/AirMobile/airtoground/tapwgts/interp1.coef"	/* 1st filter 64x interp */
#define	INTERPFILTER2	"/MobileChan/PropChan/AirMobile/airtoground/tapwgts/interp2.coef"	/* 2nd filter 64x interp */
#define	INTERPFILTER3	"/MobileChan/PropChan/AirMobile/airtoground/tapwgts/interp3.coef"	/* 3rd filter 64x interp */
#define	INTERPFILTER4	"/MobileChan/PropChan/AirMobile/airtoground/tapwgts/interp4.coef"	/* 4th filter 64x interp */
#define	INTERPFILTER5	INTERPFILTER4				/* 5th filter 64x interp */
#define	INTERPFILTER6	INTERPFILTER4				/* 6th filter 64x interp */
#define	NPTSFILTER1	28				/* # pts in INTERPFILTER1 */
#define	NPTSFILTER2	11				/* # pts in INTERPFILTER2 */
#define	NPTSFILTER3	8				/* # pts in INTERPFILTER3 */
#define	NPTSFILTER4	6				/* # pts in INTERPFILTER4 */
#define	NPTSFILTER5	NPTSFILTER4		/* # pts in INTERPFILTER5 */
#define	NPTSFILTER6	NPTSFILTER4		/* # pts in INTERPFILTER6 */
#define	INTERP		64				/* max interpolation factor */
/* number of gaussian samples to warm-up FIRs */
#define NFIRWARMUPS	37
#define	ONE_MHZ		1000000			/* one million (for scaling) */
#define	NIIRDELAYS	3				/* # delays in filter delay */

extern void IntegrateInterf(int, fcomplex [], int, fcomplex [], int);
extern void MultComplex(fcomplex *, fcomplex *, int);
extern void iirfade();
extern int InterpFactor(float);
extern float AttainableFreq(int);
extern float InterfAmp(float, float);
extern float InterfLen(float);
extern float RandPoisson(float);
extern float InterfFreq(float);
extern float InterfPhase();
extern char *getenv(char *);
/* some functions outside of narrowband directory without header files */
extern double drand48();

#endif	/* _NARROWBAND_H */
