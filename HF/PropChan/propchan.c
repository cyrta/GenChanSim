/* Propagation model as described in 'Wideband HF Propagation, 
 * Narrowband Interference, and Atmospheric Noise Models for Link Performance 
 * Evaluation', Phil Bello, MTR 93B0000086 and 'Software Wide Band HF Simulation
 * User's Manual', Richard Coutts, MTR93B0000122
 * 

   copyright (c) 1994 The MITRE Corporation Bedford, MA

   $Id: propchan.c,v 2.2 1994/07/25 19:29:19 jjb Exp $
   $Log: propchan.c,v $
 * Revision 2.2  1994/07/25  19:29:19  jjb
 * added copyright notice.
 *


		GENERAL PUBLIC LICENSE
		----------------------

	The MITRE Corporation (MITRE) provides this software to you
without charge to use, copy, modify or enhance for any legitimate
purpose provided you reproduce MITRE's copyright notice in any copy or
derivative work of this software.

	This software is the copyright work of MITRE. No ownership or
other proprietary interest in this software is granted you other than
what is granted in this license.

	Any modification or enhancement of this software must identify
the part of this software that was modified, by whom and when, and
must inherit this license including its warranty disclaimers.

	MITRE IS PROVIDING THE PRODUCT "AS IS" AND MAKES NO WARRANTY,
EXPRESS OR IMPLIED, AS TO THE ACCURACY, CAPABILITY, EFFICIENCY OR
FUNCTIONING OF THIS SOFTWARE AND DOCUMENTATION. IN NO EVENT WILL MITRE
BE LIABLE FOR ANY GENERAL, CONSEQUENTIAL, INDIRECT, INCIDENTAL,
EXEMPLARY OR SPECIAL DAMAGES, EVEN IF MITRE HAS BEEN ADVISED OF THE
POSSIBILITY OF SUCH DAMAGES.

	You, at your expense, hereby indemnify and hold harmless
MITRE, its Board of Trustees, officers, agents and employees, from any
and all liability or damages to third parties, including attorney's
fees, court costs, and other related costs and expenses, arising out
of your use of this software irrespective of the cause of said
liability.

	The export from the United States or the subsequent reexport
of this software is subject to compliance with United States export
control and munitions control restrictions. You agree that in the
event you seek to export this software or any derivative work thereof,
you assume full responsibility for obtaining all necessary export
licenses and approvals and for assuring compliance with applicable
reexport restrictions.
 * r.coutts
 */
#include <stdio.h>
#include <math.h>
#include <complex.h>
/* local includes */
#include <Cmdline.h>
#include <propchan.h>
#include <seed.h>
#include <pathfile.h>

/*
#define	DEBUG
 */

static char rcsid[] = "$Id: propchan.c,v 2.2 1994/07/25 19:29:19 jjb Exp $";

void main(int argc, char **argv) {
	char 		inFile[100];				/*input file */
	char		outFile[100];				/* output file */
	#ifdef DEBUG
		char		tapFile[100] = "tapwgts.dbg";/* tapwgt file */
		char		fadingFile[100] = "fading.dbg";/* tapwgt file */
		FILE		*fadfp;					/* fading coeff file ptr */
		FILE		*tapfp;					/* taps samples file ptr */
	#endif
	int			bufferDelay[NPATHS];		/* delay buffer before line */
	fcomplex	sum[NPATHS];				/* sum of each path */
	fcomplex	outBuffer[OUTBUFFER];		/* sum of all taps */
	fcomplex	*wgts[NPATHS];				/* weights */
	fcomplex	*sample[NPATHS];			/* samples of each path */
	fcomplex	s;							/* input sample */
	int			i;							/* index */
	int			j;							/* index */
	FILE		*infp;						/* input samples file ptr */
	FILE		*outfp;						/* output samples file ptr */
	int			nSamples = 0;				/* input samples processed */
	int			b = 0;						/* outBuffer[] index */
	float		normDplShift[NPATHS];		/* Dpl shift of normal paths */
	float		distDplShift[NPATHS];		/* Dpl shift of disturbed paths */
	float		normGain[NPATHS];			/* relative gain of norm paths */
	float		distGain[NPATHS];			/* relative gain of norm paths */
	float		gain[NPATHS];				/* relative gain of all paths */
	float		gainFactor;					/* used to adjust path gains */
	float		normBulkDelay[NPATHS];		/* bulk delay of norm paths (mS) */
	float		distBulkDelay[NPATHS];		/* bulk delay of dist paths (mS) */
	float		bulkDelay[NPATHS];			/* bulk delay of all paths (mS) */
	float		dplSpread[NPATHS];			/* Dpl spread of disturbed paths */
	float		groupDelay[NPATHS];			/* group delay or normal paths */
	float		multiPathSpr[NPATHS];		/* rms multipath spread (uS) */
	char		*helpStr;					/* help string */
	float		tau[NPATHS];				/* 'tau' param for slow fading */
	float		r[NPATHS];					/* 'r' parameter for slow fading */
	float		fading[NPATHS];				/* slow fading factor */
	int			seed;						/* r.n.g. seed */
	int			slowFading = 0;				/* slow fading flag */
	CmdInfo		*cmdSeed;					/* CmdLine information */
	TAPWGTS		tw;							/* tapwgts() data */
	
	/* print copyright notice */

	fprintf(stdout,"copyright (c) 1994 The MITRE Corporation Bedford, MA\n");

	/* clear memory */
	for(i = 0; i < NPATHS; i++) {
		gain[i] = 0;
		bulkDelay[i] = 0;
	}

	/* initial values to ensure all were entered */
	tw.nPathsNorm = 0; /* init number of normal paths */
	tw.nPathsDist = 0; /* init number of disturbed paths */
	tw.thinPhScr = 1;	/* this is no longer optional */
	tw.thinPhaseC = 0.25;			/* C parameter */
	tw.thinPhaseThresh = 0.05;		/* tail threshold parameter */

	/* set command line args for interface */
	CmdFile("-i Input_File", inFile, 100, cmdMan);
	CmdFloat("-s Sample_Rate_(MHz)", &tw.sampleRate, cmdMan);
	CmdFile("-o Output_File", outFile, 100, cmdMan);
	CmdSpace();
	CmdInt("-norm Number_of_Normal_Paths", &tw.nPathsNorm, cmdDef);
	CmdFloat("-nd Doppler_Shift_(Hz)", normDplShift, NPATHS, cmdMan);
	CmdFloat("-nx Relative_Path_Gain_(dB)", normGain, NPATHS, cmdDef);
	CmdFloat("-nb Bulk_Delay_(mS)", normBulkDelay, NPATHS, cmdDef);
	CmdFloat("-g Delay_Distortion_(uS/MHz)", groupDelay, NPATHS, cmdMan);
	CmdSpace();
	CmdInt("-dist Number_of_Disturbed_Paths", &tw.nPathsDist, cmdDef);
	CmdFloat("-dd Doppler_Shift_(Hz)", distDplShift, NPATHS, cmdMan);
	CmdFloat("-dx Relative_Path_Gain_(dB)", distGain, NPATHS, cmdDef);
	CmdFloat("-db Bulk_Delay_(mS)", distBulkDelay, NPATHS, cmdDef);
	CmdFloat("-ds Doppler_Spread_(Hz)", dplSpread, NPATHS, cmdMan);
	CmdFloat("-ms Multipath_Spread_(uS)", multiPathSpr, NPATHS, cmdMan);
	CmdFloat("-tc Thin_Phase_C", &tw.thinPhaseC, cmdDef);
	CmdFloat("-tt Thin_Phase_Theshold", &tw.thinPhaseThresh, cmdDef);
	CmdFlag("-sf Slow_Fading", &slowFading);
	CmdFloat("-t Time_Constant_(S)", tau, NPATHS, cmdMan);
	CmdFloat("-f Fade_RMS_Value_(dB)", r, NPATHS, cmdMan);
	cmdSeed = CmdInt("-seed R.N.G._Seed", &seed, cmdOpt);
	CmdSpace();
	/* set dependencies between args */
	CmdDep(&slowFading, tau, r, cmdSensitive);
	CmdDep(&tw.nPathsDist, &tw.thinPhaseC,  &tw.thinPhaseThresh, cmdSensitive);
	CmdDep(&tw.nPathsNorm, &groupDelay, normDplShift, normGain,
		normBulkDelay, normDplShift, cmdArray);
	CmdDep(&tw.nPathsDist, distDplShift, distGain, dplSpread,
		distBulkDelay, multiPathSpr, cmdArray);
	/* invoke interface */
	CmdHelp(helpStr = PathFile("HF/PropChan/propchan.hlp"));
	CmdArgs(argc, argv);
	CmdUnMan();
	free(helpStr);

	tw.nPaths = tw.nPathsNorm+tw.nPathsDist;	/* total number of paths */

	/* check if too many paths were entered */
	if(tw.nPaths > NPATHS || tw.nPaths <= 0) {
		printf("Error from propchan: total number of quiet and disturbed paths "
		" is ZERO or greater than %d.\n", NPATHS);
		exit(1);
	}
	/* pack some 'normal' and 'disturbed' mode parameters into a single array */
	j = 0;											/* index to packed arrays */
	for(i = 0; i < tw.nPathsNorm; i++) {
		tw.dplShift[j] = normDplShift[i];			/* Dpl shift */
		gain[j] = normGain[i];						/* relative gain path */
		bulkDelay[j] = normBulkDelay[j];			/* bulk delay (mS) */
		tw.dplSpread[j] = 0.0;						/* n/a to norm mode*/
		tw.groupDelay[j] = groupDelay[i];			/* group delay */
		tw.multiPathSpr[j] = 0.0;					/* multipath spread (n/a) */
		tw.mode[j] = NORM;							/* set to normal mode */
		j++;										/* index to packed array */
	}
	for(i = 0; i < tw.nPathsDist; i++) {
		if(dplSpread[i] == 0) {
			printf("Error from propchan: Doppler Spread must be > 0.\n");
			exit(1);
		}
		tw.dplShift[j] = distDplShift[i];		/* Dpl shift */
		gain[j] = distGain[i];					/* relative gain path */
		bulkDelay[j] = distBulkDelay[j];		/* bulk delay (mS) */
		tw.dplSpread[j] = dplSpread[i];			/* Dpl spread */
		tw.multiPathSpr[j] = multiPathSpr[i];	/* multipath spread (uS) */
		tw.mode[j] = DIST;						/* set to disturbed mode */
		tw.groupDelay[j] = 0.0;					/* group delay (n/a) */
		j++;									/* index to packed array */
	}

	/* check rng seed stuff */
	if(cmdSeed->entered) {				/* if seed entered... */
		srand48(seed);					/* init rng */
	} else {
		srand48(GetSeed());				/* else, get seed from file */
	}

	/* do some conversions */
	tw.sampleRate *= 1000000;		/* MHz -> Hz */

	coeff(&tw);		/* calculate some coefficients (snapshot rate, etc.) */

	/* convert bulk delay (mS->samples) */
	gainFactor = 0;
	for(i = 0; i < tw.nPaths; i++) {
		bufferDelay[i] = tw.sampleRate * bulkDelay[i]/1000;	/* mS->samples */
		gain[i] = pow(10.0, gain[i]/20.0);					/* dB->voltage */
		gainFactor += gain[i] * gain[i];
	}
	gainFactor = 1.0 / sqrt(gainFactor);
	/* adjust gains to maintain unity output power */
	for(i = 0; i < tw.nPaths; i++) gain[i] *= gainFactor;

	#ifdef DEBUG
		tapfp = FOpen(tapFile, "w");	/* open tap-weight file */
		fadfp = FOpen(fadingFile, "w");	/* open fading data file */
	#endif

	infp = FOpen(inFile, "r");			/* open input data file */
	outfp = FOpen(outFile, "w");		/* open output output file */

	/* loop once for ea complex input sample */
	printf("Writing output file '%s'...", outFile); fflush(stdout);
	j = 0;						/* number of tapwgts() calls */

	if(slowFading) {		/* if request by user */
		fade(fading, tau, r, tw.nPaths, 1);	/* init slow fading */
	} else {
		for(i = 0; i < NPATHS; i++) fading[i] = 1.0;	/* don't scale */
	}

	while(fread(&s, sizeof(fcomplex), 1, infp) != 0) {
		/* pass sample into delay buffer */
		buffer(bufferDelay, tw.nPaths, sample, s);
		/* get new fading coeff every 'FADE_UPDATE' samples */
		if(nSamples % FADE_UPDATE == 0) {
			/* print something to screen */
			if(nSamples == 0) {
				printf("%d samples per tick: ", FADE_UPDATE); fflush(stdout);
			} else if(nSamples % (50 * FADE_UPDATE) == 0) {
				printf("%d", nSamples);
			} else {
				printf("."); fflush(stdout);
			}
			if(slowFading) {
				fade(fading, tau, r, tw.nPaths, 0);	/* get fading parameters */
			}
			#ifdef DEBUG
				fwrite(&fading[0], sizeof(float), tw.nPaths, fadfp);
			#endif
		}
		/* read in tap weights if snapshot rate dictates */
		for(i = 0; i < tw.nPaths; i++) {
			if(nSamples == 0 ||
			   /* test if 'nSamples' stepped over multiplicand of snapshot */
			   (int)((nSamples-1)*tw.snapshot[i]/tw.sampleRate) !=
			   (int)(nSamples*tw.snapshot[i]/tw.sampleRate))
			{
				tapwgts(&tw, wgts, i);			/* get new delay weights */
				# ifdef DEBUG
					fwrite(wgts[i], sizeof(fcomplex), tw.nTaps[i], tapfp);
				# endif
			}
		}
		/* get output of each tap delay line */
		for(i = 0; i < tw.nPaths; i++) {
			tapdelay(&sum[i], i, tw.nTaps[i], wgts[i], sample[i]);
		}
		/* tally outputs of tap delay lines and multiply by gain and fading */
		outBuffer[b].r = outBuffer[b].i = 0;
		for(i = 0; i < tw.nPaths; i++) {
			outBuffer[b].r += fading[i] * gain[i] * sum[i].r;
			outBuffer[b].i += fading[i] * gain[i] * sum[i].i;
		}
		nSamples++;	/* incr number of sample processed */
		b++;		/* incr outBuffer[] index */
		/* output a block of data */
		if(nSamples%OUTBUFFER == 0) {
			fwrite(outBuffer, sizeof(fcomplex), b, outfp);
			b = 0;
		}
	}
	printf("done.\n");
	printf("%d samples written to %s\n", nSamples, outFile);
	/* output any residual data */
	fwrite(outBuffer, sizeof(fcomplex), b, outfp);
	SaveSeed(lrand48());			/* save seed to file for next run */
}

/* 'fopen' file and check for error */
FILE *FOpen(char *file, char *mode) {
	FILE	*fp;

	if((fp = fopen(file, mode)) == NULL) {
		printf("Error from propchan:  '%s' not opened", file);
		if(mode[0] == 'w') printf(" for writing");
		else if(mode[0] == 'r') printf(" for reading");
		printf(".\n");
		exit(1);
	}
	return fp;
}
