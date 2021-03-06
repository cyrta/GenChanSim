/* Atmospheric Noise model as described in 'Wideband HF Propagation, 
 * Narrowband Interference, and Atmospheric Noise Models for Link Performance 
 * Evaluation', Phil Bello, MTR 93B0000086 and 'Software Wide Band HF Simulation
 * User's Manual', Richard Coutts, MTR93B0000122
 *
 * The algorithm is implimented by: generating the stats for ea stroke
 * (rms, time, duration), sorting the strokes by time, integrating one stroke
 * at a time, and outputing fragments of the integration (plus noise) as ea
 * new stroke is processed.
 *
 * When calibrating c & sigma, no gaussian series is generated and a value
 * of unity is used as the flash amplitude.
 *
 * r.coutts

 $Id: atmosphere.c,v 2.2 1994/07/25 18:03:14 jjb Exp $
 $Log: atmosphere.c,v $
 * Revision 2.2  1994/07/25  18:03:14  jjb
 * Added Copyright notices
 *

   copyright (c) 1994 The MITRE Corporation Bedford, MA

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
 */
#include <stdio.h>
#include <math.h>
#include <malloc.h>
#include <string.h>
#include <atmosphere.h>
#include <Cmdline.h>
#include <seed.h>
#include <pathfile.h>
#include <complex.h>

static char rcsid[] = "$Id: atmosphere.c,v 2.2 1994/07/25 18:03:14 jjb Exp $";

void main(int argc, char **argv) {
	char		outfile[100] = ""; /* output data file */
	CmdInfo		*cmdOutfile;	/* cmdline structure */
	float		tF;				/* time of flash (seconds) */
	float		tT = 1.0;		/* total time (seconds) */
	float		tS;				/* time of stroke (seconds) */
	int			nstrokes;		/* number of strokes in flash */
	int			i;				/* stroke index */
	float		lamdaF;			/* lamda flash */
	fcomplex	*out;			/* output */
	fcomplex	*signal;		/* output data */
	float		rms;			/* rms value of stroke */
	int			freq = 1;		/* sample rate of output (Hz) */
	int			nsamples;		/* number of samples in signal */
	float		minstroke = SHORTESTSTROKE;	/* minimum stroke length */
	float		maxstroke = LONGESTSTROKE;	/* maximum stroke length */
	int			totalnstrokes = 0;	/* total number of strokes */
	int			printlightning = NO;/* flag to print lightning to screen */
	float		v = 0;			/* user input of Vd */
	int			calibrate = 0;	/* flag to calibrate c and sigma */
	int			vd_entered = 0;	/* flag for vd */
	int			c_sigma_entered = 0;/* flag for c and sigma */
	int			outqueue;		/* output queue size */
	int			seedrng;		/* random number generator seed */
	CmdInfo		*cmdSeedRNG;	/* structure to input argument */
	float		tlast = 0.0;	/* last time */
	STROKE		*s = (STROKE *)NULL;		/* current stroke in queue */
	STROKE		*s0 = NULL;		/* top of queue */
	STROKE		*prev;			/* previous member of queue */
	float		timegap = 0.0;	/* largest gap in time between strokes */
	ATMOSSTATS	as;				/* run statistic */
	VD			vdInput;		/* C & Sigma from user or table */
	VD			vdTable[NVDS][NFLASHRATES]; /* C & Sigma table */
	char		*helpStr;		/* help string */


	/* print copyright notice */

	fprintf(stdout,"copyright (c) 1994 The MITRE Corporation Bedford, MA\n");

	InitStats(&as);	/* initialize statistics structure */

	/* get command-line args */
	CmdFloat("-t Total_Time_(seconds)", &tT, cmdMan);
	CmdInt("-s Sample_Rate_(MHz)", &freq, cmdDef);
	CmdFloat("-f Flashes_Per_Second", &lamdaF, cmdMan);
	CmdFlag("-cal Calibrate_C_&_Sigma", &calibrate);
	CmdFlag("-vd Enter_Vd", &vd_entered);
	CmdFlag("-cs Enter_C_&_Sigma", &c_sigma_entered);
	CmdFloat("-v Vd", &v, cmdMan);
	CmdFloat("-c Power_Rayleigh_C", &vdInput.c, cmdMan);
	CmdFloat("-sigma Sigma_Of_Noise", &vdInput.sigma, cmdMan);
	CmdFloat("-min Min_Stroke_Length_(mS)", &minstroke, cmdDef);
	CmdFloat("-max Max_Stroke_Length_(mS)", &maxstroke, cmdDef);
	cmdSeedRNG = CmdInt("-S R.N.G._Seed", &seedrng, cmdOpt);
	cmdOutfile = CmdFile("-o Output_File", outfile, 100, cmdOpt);
	CmdFlag("-p Screen_Saver", &printlightning);
	CmdDep(&calibrate, &vd_entered, &c_sigma_entered, cmdRadioBox);
	CmdDep(&vd_entered, &v, cmdSensitive);
	CmdDep(&c_sigma_entered, &vdInput.c, &vdInput.sigma, cmdSensitive);
	CmdHelp(helpStr = PathFile("HF/AddDist/atmosphere/atmosphere.hlp"));
	CmdArgs(argc, argv);
	CmdUnMan();
	free(helpStr);

	/* conversions */
	freq *= 1000000;				/* sampling rate (Hz->MHz) */
	minstroke /= 1000;				/* min stroke length (mS->seconds) */
	maxstroke /= 1000;				/* max stroke length (mS->seconds) */

	if(vd_entered) {	/* if Vd was entered... */
		if(lamdaF < 50 || lamdaF > 200) {
			printf("Error from atmophere: Vd only calibrated for the "
				"flash rates of range (50, 200) flashes/second\n");
			exit(1);
		}
		if(v < 3 || v > 11) {
			/* check input */
			printf("Error from atmosphere: Vd must be in range (3,11)\n");
			exit(1);
		}
		ReadCandSigma(vdTable);
		vdInput = InterpCandSigma(vdTable, v, lamdaF);
		printf("Calibrated values:  C=%f sigma=%f\n", vdInput.c, vdInput.sigma);
	}
	as.c = vdInput.c;				/* value attained from user or table */
	as.sigma = vdInput.sigma;		/* value attained from user or table */
	
	if(cmdOutfile->entered == 0) {			/* if no file entered... */
		outfile[0] = '\0';					/* don't write a data file */
		printf("NOTE: NO DATA FILE OUTPUT REQUESTED\n");
	}
	/* generate stroke statistics */
	printf("generating stroke statistics..."); fflush(stdout);
	tF = 0.0;
	if(cmdSeedRNG->entered) {		/* if user input a seed... */
		srand48(seedrng);			/* ...set the r.n.g. */
	} else {						/* else no seed specified... */
		srand48(GetSeed());			/* get old seed */
	}
	while(tF < tT) {
		tS = tF;					/* 1st stroke is at start of flash */
		if(calibrate) {				/* if calibrating... */
			rms = 1.0;				/* rms amplitude is unity */
		} else {					/* else, running simulation, so... */
			rms = StrokeRms(as.c);	/* generate random strength of flash */
		}
		as.rmssum += rms;			/* accumulate rms values */
		as.rmssqsum += rms*rms;		/* accumulate rms values */
		nstrokes = NumStrokes();	/* get number of strokes in flash */
		for(i = 0; i < nstrokes; i++) {
			/* add stroke to queue */
			s = AddStroke(s);		/* add stroke to list */
			if(s0 == NULL) {		/* if 1st stroke... */
				s0 = s;				/* set s0 to top of list of strokes */
			}
			s->duration = StrokeDuration(minstroke, maxstroke);	/* seconds */
			s->time = tS;			/* time of stroke (seconds) */
			s->rms = rms;			/* rms value of stroke (seconds) */
			s->flash = as.nflashes; /* flash number */
			s->stroke = i;			/* stroke number */
			tS += NextStroke();		/* get next stroke time */
			if(tS >= tT) break;		/* end of run */
		}
		as.nstrokes += i+1;			/* i+1 = # strokes processed */
		totalnstrokes += nstrokes;
		tF += RandPoisson(lamdaF); /* get next flash time */
		as.nflashes++;
	}
	printf("done\n"); /* done generating stroke statistics... */
	printf("sorting strokes by time..."); fflush(stdout);
	sortstrokes(s0); /* sort the strokes by time */
	printf("done\n");

	/* get required size of integration queue and alloc memory */
	prev = s0;		/* 'prev' is previous member of queue */
	s = s0->next;	/* go to top of queue */
	/* get largest time gap between strokes */
	while(s != NULL) {
		if(timegap < s->time - prev->time) timegap = s->time - prev->time;
		prev = s;
		s = s->next;
	}
	/* set queue size = max(timegap,maxstroke)*freq */
	if(timegap < tT - prev->time) timegap = tT - prev->time; /* gap to end */
	if(timegap > maxstroke) outqueue = timegap*freq;/* gap dictates q size */
	else outqueue = maxstroke*freq; 				/* stroke dictates q size */
	outqueue += 10;									/* add a little slop */
	out = (fcomplex *)calloc(outqueue, sizeof(fcomplex));
	signal = (fcomplex *)calloc(outqueue, sizeof(fcomplex));
	if(out == NULL || signal == NULL) {
		printf("Error from atmosphere: can't allocate enough memory to "
			"accomodate maximum stroke length.  "
			"[%.3f(seconds)]x[%.3f(Hz)]x[8(bytes/sample)] = %d bytes\n",
			maxstroke, freq, (int)(maxstroke*freq));
		exit(1);
	}

	/* generate strokes */
	printf("generating output file..."); fflush(stdout);
	if(printlightning) printf("\n"); /* CR after '...' because lightning */
	if(calibrate) {								/* if calibrating c & sigma...*/
		for(i = 0; i < outqueue; i++) {			/* for ea sample... */
			signal[i].r = 1; 					/* set signal = unity amp */
			signal[i].i = 0;					/* ditto */
		}
	}
	s = s0;										/* set s = top of stroke list */
	while(s != NULL) {							/* while not bottom of list...*/
		if(printlightning) PrintFlash(s->time); /* print screen character */
		/* add noise to output and file */
		if(!calibrate) {						/* if NOT calibrating... */
			nsamples = (s->time-tlast)*freq;		/* # samples to output */
			if(outqueue < nsamples) {				/* if queue too small... */
				printf("Error from atmosphere: too many samples ");
				exit(1);							/* exit program */
			}
			cgauss48(signal, nsamples);				/* noise to add to output */
			ScaleComplex(as.sigma, signal, nsamples);/* scale by std.dev. */
			/* scale to unity real power */
			ScaleComplex(sqrt(2.0), signal, nsamples);
			/* add noise to output queue */
			IntegrateStroke(tlast*freq, signal, nsamples, out, outqueue);
		}
		as.npts += Output(outfile, &as, s->time*freq, out, outqueue); /* file */
		/* generate next stroke */
		nsamples = s->duration*freq;				/* # samples in stroke */
		if(!calibrate) {							/* if NOT calibrating... */
			cgauss48(signal, nsamples);				/* gaussian signal */
			ScaleComplex(s->rms, signal, nsamples);	/* scale by stroke power */
		}
		IntegrateStroke(s->time*freq, signal, nsamples, out, outqueue); /* add*/
		tlast = s->time;						/* update last stroke output */
		s = s->next;
	}
	/* write noise to the end of the file */
	if(!calibrate) {							/* if NOT calibrating... */
		nsamples = (tT-tlast)*freq;				/* number samples to file */
		if(outqueue < nsamples) {				/* if queue too small... */
			printf("Error from atmosphere: too many samples\n");
			exit(1);							/* exit software */
		}
		cgauss48(signal, nsamples);				/* noise */
		ScaleComplex(as.sigma, signal, nsamples);/* scale by std.dev. */
		IntegrateStroke(tlast*freq, signal, nsamples, out, outqueue); /* add */
	}
	as.npts += Output(outfile, &as, tT*freq, out, 0); /* last write to file */
	printf("done\n");
	OutputStats(&as, calibrate, lamdaF);		/* fprint statistics */
	SaveSeed(lrand48());						/* save seed for next run */
}
