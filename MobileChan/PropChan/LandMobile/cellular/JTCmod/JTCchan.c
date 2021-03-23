/* Propagation model as described in 'Wideband HF Propagation, 
 * Narrowband Interference, and Atmospheric Noise Models for Link Performance 
 * Evaluation', Phil Bello, MTR 93B0000086 and 'Software Wide Band HF Simulation
 * User's Manual', Richard Coutts, MTR93B0000122  
 * 
 * r.coutts

   copyright (c) 1994 The MITRE Corporation Bedford, MA

   $Id: JTCchan.c,v 3.0 1994/08/31 19:09:08 jjb Exp $
   $Log: JTCchan.c,v $

 * LL Revision  Feb. 1996 C.M.K. Can input a data file with
 * filter coefficients for the Doppler power spectrum.
 *
 * Revision 3.0  1994/08/31  19:09:08  jjb
 * added fractional delays for rate conversion.
 * added load/generate filter warmup files.
 * added new mixed scatter/discrete (macrocell) model
 *
 * Revision 2.2  1994/07/25  18:57:02  jjb
 * added copyright notice
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
 */
#include <stdio.h>
#include <math.h>
#include <complex.h>
#include <values.h>
/* local includes */
#include <Cmdline.h>
#include <JTCchan.h>
#include <seed.h>
#include <pathfile.h>
#include <fracdly.h>

static char rcsid[] = "$Id: JTCchan.c,v 3.0 1994/08/31 19:09:08 jjb Exp $";
static char revstring[] = "$Revision: 3.0 $";

/*
#define	DEBUG
 */
void main(int argc, char **argv) {
	char 		inFile[120];				/*input file */
	char		outFile[120];				/* output file */
	#ifdef DEBUG
		char		tapFile[120] = "tapwgts.dbg";/* tapwgt file */
		char		fadingFile[120] = "fading.dbg";/* tapwgt file */
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
        float           revNo;                                         /* software rev number */
	int			nSamples = 0;				/* input samples processed */
	int			b = 0;						/* outBuffer[] index */
	int             minDelay = MAXINT;              /* min bulk delay */
	float           tmpDelay;                       /* temporary storage */
	float           fractDelay;                       /* temporary storage */
	int             updateCoefFlag = 0;             /* flag to update coef */
	int             fracDlyFlag = 0;                 /* frac. dly. flag */
	float		normDplShift[NPATHS];		/* Dpl shift of normal paths */
	float		distDplShift[NPATHS];		/* Dpl shift of disturbed paths */
	float		normGain[NPATHS];			/* relative gain of norm paths */
	float		distGain[NPATHS];			/* relative gain of norm paths */
	float		gain[NPATHS];				/* relative gain of all paths */
	float		gainFactor;					/* used to adjust path gains */
	float		normBulkDelay[NPATHS];		/* bulk delay of norm paths (mS) */
	float		distBulkDelay[NPATHS];		/* bulk delay of dist paths (mS) */
	float		bulkDelay[NPATHS];			/* bulk delay of all paths (mS) */
/*	float		dplSpread[NPATHS];	*/		/* Dpl spread of disturbed paths */
/*	float		groupDelay[NPATHS];	*/		/* group delay or normal paths */
/*	float		multiPathSpr[NPATHS];	*/	/* rms multipath spread (uS) */
	char		*helpStr;					/* help string */
	float		tau[NPATHS];				/* 'tau' param for slow fading */
    char        dop_data_fname[120];            /*  data filename */
	float		r[NPATHS];					/* 'r' parameter for slow fading */
	float		fading[NPATHS];				/* slow fading factor */
	int  flatSpectrum = 0;       /* flat doppler spectrum flag */
	int  jakesSpectrum = 0;      /* jakes doppler spectrum flag */
	int  dataSpectrum = 0;      /* general doppler spectrum flag */
	int  warmupInterp = 0;       /* flag to warmup interpolators */
	int  loadInterpWarmup = 0;   /* flag to load interpolator warmup files */
	int  genInterpWarmup = 0;    /* flag to gen. interpolator warmup files */
	int  interpStep = 1;         /* interpolator step size for buffer */
        int  notused1 = 0;             /* dummy input for ctapwgts */
        char * notused2 = NULL;                 /* dummy input for ctapwgts */
	float   expTau[NPATHS];      /* time constant for exponential decay */
	int			seed;						/* r.n.g. seed */
	int			slowFading = 0;				/* slow fading flag */
	CmdInfo		*cmdSeed;					/* CmdLine information */
	TAPWGTS		tw;							/* tapwgts() data */
	
	/* print copyright notice */

        sscanf(revstring+10,"%f",&revNo);
        fprintf(stdout,"Mobile Channel Simulator ver. %7.3f\n",revNo);
	fprintf(stdout,"copyright (c) 1994 The MITRE Corporation Bedford, MA\n");
	fprintf(stdout,"revised (1996) by the MIT Lincoln Laboratory, Lexington, MA\n");

	/* clear memory */
	for(i = 0; i < NPATHS; i++) {
		gain[i] = 0;
		bulkDelay[i] = 0;
	}

	/* initial values to ensure all were entered */
	tw.nPathsNorm = 0; /* init number of normal paths */
	tw.nPathsDist = 0; /* init number of disturbed paths */
	tw.thinPhScr = 0;	/* this is no longer optional */
	tw.thinPhaseC = 0.25;			/* C parameter */
	tw.thinPhaseThresh = 0.05;		/* tail threshold parameter */
	tw.scatterSampleRate = 10e6;
	tw.weightLimit = -30;   

	/* setup model to use based on compiler constant MODEL */
	if ( MODEL == JTC_MODEL )
	  tw.model = JTC_MODEL;   /* select JTC model */
	else if (MODEL == MACROCELL_MODEL)
	  tw.model = MACROCELL_MODEL;   /* select macrocell model */
	else {
	  fprintf(stderr,"JTCchan compiled with incorrect model\n");
	  fprintf(stderr,"must recompile code\n");
	  exit(1);
	}

	/* set command line args for interface */
	CmdFile("-i Input_File", inFile, 120, cmdMan);
	CmdFloat("-s Sample_Rate_(MHz)", &tw.sampleRate, cmdMan);
	CmdFile("-o Output_File", outFile, 120, cmdMan);
	CmdSpace();

	if ( tw.model == JTC_MODEL ) {
	  CmdFloat("-vs Vehicle_Speed (m/s)", &tw.VehSpd, cmdMan);
	  CmdFloat("-fc Carrier Frequency (MHz)", &tw.carFreq, cmdMan);
          CmdFlag("-datadopp data",&dataSpectrum);
	  CmdFlag("-flat flat_doppler_spectrum",&flatSpectrum);
	  CmdFlag("-classic classic_doppler_spectrum",&jakesSpectrum);

	  CmdFile("-dopfile Dop_Pow_Spec_File", dop_data_fname, 120, cmdMan);

	  CmdFloat("-tl Tap_weight_threshold(dB)", &tw.weightLimit, cmdOpt);
	  CmdFlag("-w warmup_filters",&warmupInterp);
	  CmdFlag("-wld load_filter_warmup_files",&loadInterpWarmup);
	  CmdFlag("-wgen generate_filter_warmup_files",&genInterpWarmup);
	  CmdSpace();
	  CmdInt("-dist Number_of_JTC_Paths", &tw.nPathsDist, cmdDef);
/*	  CmdFloat("-dd Doppler_Shift_(Hz)", distDplShift, NPATHS, cmdMan);   not needed, calculated later RCB */
	  CmdFloat("-dx Relative_Path_Gain_(dB)", distGain, NPATHS, cmdMan);
	  CmdFloat("-db Bulk_Delay_(nS)", distBulkDelay, NPATHS, cmdMan);
	  CmdSpace();
	  cmdSeed = CmdInt("-seed R.N.G._Seed", &seed, cmdOpt);
	  CmdSpace();

	  /* setup dependencies */

	  CmdDep(&tw.nPathsDist, distGain, distBulkDelay, cmdArray);  
  	  CmdDep(&dataSpectrum,dop_data_fname,cmdSensitive);
	  CmdDep(&flatSpectrum,&jakesSpectrum,&dataSpectrum,cmdRadioBox);
	  CmdDep(&warmupInterp,&loadInterpWarmup,&genInterpWarmup,cmdRadioBox);

	} else if ( tw.model == MACROCELL_MODEL ) {
	  flatSpectrum = 1;  /* force flat spectrum */
	  CmdFloat("-vs Vehicle_Speed (m/s)", &tw.VehSpd, cmdMan);
	  CmdFloat("-fc Carrier Frequency (MHz)", &tw.carFreq, cmdMan);
	  CmdFloat("-tl Tap_weight_threshold(dB)", &tw.weightLimit, cmdOpt);
	  CmdFlag("-w warmup_filters(no warmup files)",&warmupInterp,cmdOpt);
	  CmdFlag("-wld load_filter_warmup_files",&loadInterpWarmup,cmdOpt);
	  CmdFlag("-wgen generate_filter_warmup_files",&genInterpWarmup,cmdOpt);
	  CmdSpace();
	  CmdInt("-norm Number_of_Discrete_Paths", &tw.nPathsNorm, cmdDef);
	  CmdFloat("-nd Discrete_Doppler_Angle_(deg)", normDplShift, NPATHS, cmdMan);
	  CmdFloat("-nx Rel_Discrete_Path_Gain_(dB)", normGain, NPATHS, cmdDef);
	  CmdFloat("-nb Discrete_Bulk_Delay_(nS)", normBulkDelay, NPATHS, cmdDef);
/*	  CmdFloat("-g Delay_Distortion_(uS/MHz)", groupDelay, NPATHS, cmdMan); */
	  CmdSpace();                                             
	  CmdInt("-dist Number_of_Scatter_Paths", &tw.nPathsDist, cmdDef);
	  CmdFloat("-ds Scatter_Path_Sample_Rate", &tw.scatterSampleRate, cmdMan);
/*	  CmdFloat("-dd Doppler_Shift_(Hz)", distDplShift, NPATHS, cmdMan);   not needed, calculated later RCB */
	  CmdFloat("-dx Rel_Scatter_Path_Gain_(dB)", distGain, NPATHS, cmdMan);
	  CmdFloat("-db Scatter_Bulk_Delay_(nS)", distBulkDelay, NPATHS, cmdMan);
	  CmdFloat("-et Scatter_Exp_Tau(ns)", expTau, NPATHS,cmdMan);
	  CmdSpace();
	  cmdSeed = CmdInt("-seed R.N.G._Seed", &seed, cmdOpt);
	  CmdSpace();

	  CmdDep(&tw.nPathsDist, distGain, distBulkDelay, cmdArray);  
	  CmdDep(&tw.nPathsDist, expTau, cmdArray);
	  CmdDep(&tw.nPathsDist, &tw.scatterSampleRate, cmdSensitive);
	  CmdDep(&tw.nPathsNorm, normDplShift, normGain, normBulkDelay, cmdArray);
	  CmdDep(&warmupInterp,&loadInterpWarmup,&genInterpWarmup,cmdRadioBox);

	} else {
	  cmdSeed = 0;  /* to keep compiler warning quiet */
	  fprintf(stderr,"incorrect model\n");
	  exit(1);
	}


/*	CmdFloat("-ds Doppler_Spread_(Hz)", dplSpread, NPATHS, cmdMan);     not needed for simulation */
/*	CmdFloat("-ms Multipath_Spread_(uS)", multiPathSpr, NPATHS, cmdMan); not needed for simulation */
/*	CmdFloat("-tc Thin_Phase_C", &tw.thinPhaseC, cmdDef);
	CmdFloat("-tt Thin_Phase_Theshold", &tw.thinPhaseThresh, cmdDef);
	CmdFlag("-sf Slow_Fading", &slowFading);
	CmdFloat("-t Time_Constant_(S)", tau, NPATHS, cmdMan);
	CmdFloat("-f Fade_RMS_Value_(dB)", r, NPATHS, cmdMan);       

	The thin pahse stuff not needed for this mode of operation RCB 19April94                       */

	/* set dependencies between args */

/* Changed dependencies for the mobile comm simulator due to the changes in the
   parameter types required.  Here is the original set....
	CmdDep(&slowFading, tau, r, cmdSensitive);
	CmdDep(&tw.nPathsDist, &tw.thinPhaseC,  &tw.thinPhaseThresh, cmdSensitive);
	CmdDep(&tw.nPathsNorm, &groupDelay, normDplShift, normGain,
		normBulkDelay, normDplShift, cmdArray);
	CmdDep(&tw.nPathsDist, distDplShift, distGain, dplSpread,
		distBulkDelay, multiPathSpr, cmdArray);  

   and here are the modified versions .... */

	/* invoke interface */
	if ( tw.model == MACROCELL_MODEL )
	  CmdHelp(helpStr = PathFile("/MobileChan/PropChan/LandMobile/cellular/JTCmod/macromobilechan.hlp"));
	else
	  CmdHelp(helpStr = PathFile("/MobileChan/PropChan/LandMobile/cellular/JTCmod/JTCchan.hlp"));
	CmdArgs(argc, argv);
	CmdUnMan();
	free(helpStr);

	tw.nPaths = tw.nPathsNorm+tw.nPathsDist;	/* total number of paths */

	/* check if too many paths were entered */
	if(tw.nPaths > NPATHS || tw.nPaths <= 0) {
		printf("Error from JTCchan: total number of quiet and disturbed paths "
		" is ZERO or greater than %d.\n", NPATHS);
		exit(1);
	}

	/* set parameter specifying doppler spectrum */
	if ( jakesSpectrum )
	  tw.powSpectrum = JAKES_SPECTRUM;
	else if ( flatSpectrum )
	  tw.powSpectrum = FLAT_SPECTRUM;
	else
	  tw.powSpectrum = DATA_SPECTRUM; /* new type of powSpectrum added
                                             Feb. 1996 CMK MIT Lincoln Lab */

	/* check to make sure if exponential then also flat */
	if ( (tw.model == MACROCELL_MODEL) && (tw.powSpectrum != FLAT_SPECTRUM ) ) {
	        printf("Error from JTCchan: when in macrocell model\n");
		printf("only flat doppler spectrum is allowed\n");
		exit(1);
	}
	       
	/* set warmup flag */
	if (tw.nPathsDist == 0 )
	  tw.warmup = DO_WARMUP;  /* don't load or save since interp not used */
	else if (warmupInterp == 1)
	  tw.warmup = DO_WARMUP;
	else if (loadInterpWarmup == 1)
	  tw.warmup = LOAD_WARMUP;
	else if (genInterpWarmup == 1)
	  tw.warmup = GEN_WARMUP;
	else
	  tw.warmup = DO_WARMUP;

	/* pack some 'normal(Discrete)' and 'disturbed' mode parameters into a single array */
	j = 0;											/* index to packed arrays */
	for(i = 0; i < tw.nPathsNorm; i++) {
		normDplShift[i] = ((tw.carFreq * ONE_MHZ * tw.VehSpd) / LIGHT_SPEED) 
		  * cos(normDplShift[i] * PI/180.0);			/* Dpl shift for Discrete */
		tw.dplShift[j] = normDplShift[i];
		gain[j] = normGain[i];						/* relative gain path */
		tw.gain[j] = gain[j];				/* Added to tw struct for display purposes */
		tw.bulkDelay[j] = normBulkDelay[i];		/* "" */
		bulkDelay[j] = normBulkDelay[i];			/* bulk delay (mS) */
		tw.dplSpread[j] = 0.0;						/* n/a to norm mode*/
		tw.groupDelay[j] = 0.0;			/* group delay */
		tw.multiPathSpr[j] = 0.0;					/* multipath spread (n/a) */
		tw.mode[j] = NORM;							/* set to normal mode */
		tw.expTau[j] = 0.0;     /* exp. time constant */
		j++;										/* index to packed array */
	}
	for(i = 0; i < tw.nPathsDist; i++) {
/*		if(dplSpread[i] == 0) {
			printf("Error from JTCchan: Doppler Spread must be > 0.\n");
			exit(1);
		}                                                                           */
		distDplShift[i] = ((tw.carFreq * ONE_MHZ * tw.VehSpd) / LIGHT_SPEED);
		tw.dplShift[j] = distDplShift[i];		/* Dpl shift */
		gain[j] = distGain[i];					/* relative gain path */
		tw.gain[j] = gain[j];				/* Added to tw struct for display purposes */
		tw.bulkDelay[j] = distBulkDelay[i];		/* "" */
		bulkDelay[j] = distBulkDelay[i];		/* bulk delay (nS) */
/*		tw.dplSpread[j] = dplSpread[i];			   Dpl spread */  
		tw.multiPathSpr[j] = 0.0;	                /* multipath spread (uS) */
		tw.mode[j] = DIST;						/* set to disturbed mode */
		tw.groupDelay[j] = 0.0;					/* group delay (n/a) */
		tw.expTau[j] = expTau[i]/1e9;     /* exp. time constant */
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
	tw.scatterSampleRate *= 1e6;	        /* MHz -> Hz */
	tw.weightLimit = pow(10.0, tw.weightLimit/10.0); /* dB->power */

	cancoeff(&tw);		/* calculate some coefficients (snapshot rate, etc.) */

	/* convert bulk delay (mS->samples) */
	gainFactor = 0;
	minDelay = MAXINT;
	for(i = 0; i < tw.nPaths; i++) {
	        tmpDelay =  tw.sampleRate * bulkDelay[i]/1e9;	                /* nS->samples */
		bufferDelay[i] = tmpDelay;

		/* check if fractional delay */
		fractDelay = tmpDelay - (float)(bufferDelay[i]);
		if ( fractDelay > (1-MIN_FRACT_DELAY))
		  bufferDelay[i]++;   /* round up on delay */

		/* compute gain */
		gain[i] = pow(10.0, gain[i]/20.0);					/* dB->voltage */
		gainFactor += gain[i] * gain[i];
	}
	gainFactor = 1.0 / sqrt(gainFactor);
	/* adjust gains to maintain unity output power */
	for(i = 0; i < tw.nPaths; i++) gain[i] *= gainFactor;

	/* initialize taps */
 	for(i=0; i<tw.nPaths; ++i)
	  ctapwgts(&tw, wgts, i, 1, notused1, notused2, dop_data_fname);

	/* check if fractional delay and setup software if needed */
	FracTapDelay(&minDelay, &fracDlyFlag,
		    &outBuffer[b], updateCoefFlag, &tw,
		    gain, wgts, sample[0]);
	updateCoefFlag = 1;

	/* adjust bulk delay values if frac delay used */
	if (fracDlyFlag == 1) {

	  printf("FRACTIONAL DELAY FILTERS ARE IN USE\n");
	  if (minDelay < 0)
	    minDelay = 0;

	  for(i=0; i<tw.nPaths; ++i)
	    bufferDelay[i] = minDelay;

	}

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
/*********** * read in tap weights if snapshot rate dictates **********************************/
		for(i = 0; i < tw.nPaths; i++) {

		  /* test if 'nSamples' stepped over multiplicand of snapshot */
		  interpStep = (int)(nSamples*tw.snapshot[i]/tw.sampleRate)
		    - (int)((nSamples-1)*tw.snapshot[i]/tw.sampleRate);
		  if ( (nSamples != 0) && (interpStep != 0)   ) {
		    ctapwgts(&tw, wgts, i, interpStep, notused1, notused2, dop_data_fname); 	/* get new delay weights */
		    updateCoefFlag = 1;
#ifdef DEBUG
		    fwrite(wgts[i], sizeof(fcomplex), tw.nTaps[i], tapfp);
#endif
		  }
		}
		/* if fractional delay then handle as one large tap delay and
		   call the fractapdelay subroutine */
		if (fracDlyFlag == 1) {

		  FracTapDelay(&minDelay, &fracDlyFlag,
			      &outBuffer[b], updateCoefFlag, &tw,
			      gain, wgts, sample[0]);
		  updateCoefFlag = 0;
 
		} else {
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
		printf("Error from JTCchan:  '%s' not opened", file);
		if(mode[0] == 'w') printf(" for writing");
		else if(mode[0] == 'r') printf(" for reading");
		printf(".\n");
		exit(1);
	}
	return fp;
}





