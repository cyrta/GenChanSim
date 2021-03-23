/* Propagation model for the air to air channel as described in the
 * 'Generic Channel Simulator Quarterly Progress Report No. 2', July
 * 1995, under contract MDA904-95-C-2078 by Phillip A. Bello.  The
 * software is derived from the mixedchan mixed model sotware
 * based on a report 'Generic Channel Simulator,' by Phil Bello, Chris
 * Nissen, and Jeff Blanchard.  This, in turn, was derived from the model
 * described in 'Wideband HF Propagation, Narrowband Interference, and
 * Atmospheric Noise Models for Link Performance Evaluation', Phil
 * Bello, MTR 93B0000086 and 'Software Wide Band HF Simulation User's
 * Manual', Richard Coutts, MTR93B0000122
 * 
 * Linda Riehl and Cathy Keller, MIT Lincoln Laboratory, March, 1996.

   Based on mixedchan.c, formerly known as macromobilichan., a
   simulator program with copyright (c) 1994 The MITRE Corporation
   Bedford, MA.

   $Id: airairchan.c,v LL0.0 3/1996 $
   $Log: airairchan.c,v $

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
reexport restrictions.  */
#include <stdio.h>
#include <math.h>
#include <complex.h>
#include <values.h>
/* local includes */
#include <Cmdline.h>
#include <airairchan.h>
#include <seed.h>
#include <pathfile.h>
#include <fracdly.h>

static char rcsid[] = "$Id: airairchan.c,v LL0.0 3/1996 $";
static char revstring[] = "$Revision: LL0.0 $";

/*
#define	DEBUG
*/ 
void main(int argc, char **argv) {
	char 		inFile[120];	/*input file */
	char		outFile[120];   /* output file */
	#ifdef DEBUG
	  char	tapFile[120] = "tapwgts.dbg";  /* tapwgt file */
	  char	fadingFile[120] = "fading.dbg";/* tapwgt file */
	
	  FILE		*fadfp;			/* fading coeff file ptr */
	  FILE		*tapfp;			/* taps samples file ptr */
	#endif

	int		bufferDelay[NPATHS];	/* delay buffer before line */
	fcomplex	sum[NPATHS];		/* sum of each path */
	fcomplex	outBuffer[OUTBUFFER];/* sum of all taps */
	fcomplex	*wgts[NPATHS];		/* weights */
	fcomplex	*sample[NPATHS];	/* samples of each path */
	fcomplex	s;				    /* input sample */
	int		i;				        /* index */
	int		j;				        /* index */
	FILE		*infp;				/* input samples file ptr */
    FILE		*outfp;			    /* output samples file ptr */
    float       revNo;              /* software rev number */
	int		nSamples = 0;		    /* input samples processed */
	int		b = 0;			       	/* outBuffer[] index */
	int     minDelay = MAXINT;      /* min bulk delay */
	float   tmpDelay;               /* temporary storage */
	float   fractDelay;             /* temporary storage */
	int     updateCoefFlag = 0;     /* flag to update coef */
	int     fracDlyFlag = 0;        /* frac. dly. flag */
    float   DirectDplShift;         /* Dpl shift of direct path */
    float   SpecularDplShift;       /* Dpl shift of specular path */
    float   normDplShift[NPATHS];   /* Dpl shift of other discrete paths */
    float   distDplShift;	   /* Dpl shift of scatter paths */
        float   directscalsig;        /* scaled sigma_eta for direct path (not needed)*/
        float   specularscalsig;      /* scaled sigma_eta for specular path (not needed)*/
        float   normscalsig[NPATHS]; /* scaled sigma_eta for discrete paths (not needed)*/
        float   distscalsig; /* scaled sigma_eta for scatter path */

	float   velInplane[2];/* aircraft velocities in vertical plane (m/s) */
	float	velPerp[2];  /* aircraft velocities perpendicular to 
								vertical plane (m/s) */
	/*	float   scatSampRate;*/ /* scatter path sample rate (MHz) */
	float   rmsSlope;    /* RMS slope of surface */
        float   airheightsmeters[2]; /* aircraft heights in meters */
	float   airDist;     /* distance between aircraft */
	int     directPath;  /* Is there a direct path? */
	int     specularPath; /* Is there a specular path? */
	int     scatterPath; /* Is there a scatter path? */
	int     othDiscretePaths; /* number of discrete paths other
				     than the direct  path */

	int     relPathStrength;/* Should the program compute the
				   relative path strenghts? */
	float   gd;             /* strength of the direct path (dB) */
	float   gsps;           /* relative strength of specular path (dB) */
	float   gscs;           /* relative strength of scatter path (dB) */
	float   gdiscrete[NPATHS]; /* relative strengths of other discrete paths (dB); */
	float   antGain;        /* relative antenna gain product */
	float   surfCon;        /* surface conductivity (mhos/m) */
	int     polar;          /* polarization (vert = 0 , hor= 1 ) */
	float   dielConstant;   /* dielectric constant */
	float   rmsHeight;      /* RMS height of surface (m) */

	float	gain[NPATHS];   /* relative gain of all paths */
	float	gainFactor;	    /* used to adjust path gains */
        float   DirectPathDelay; /* delay of direct path (ns) */
        float   specularPathDelay; /* delay of specular path 
                                      with respect to direct path (ns) */
	float   normPathDelay[NPATHS];	 /* delays of other discrete path (ns) */
	float	distBulkDelay;   	 /* bulk delay of scatter path (ns) */
	float	bulkDelay[NPATHS];		 /* bulk delay of all paths (ns) */
	char   *helpStr;		 /* help string */
	float	tau[NPATHS];     /* 'tau' param for slow fading */
	float	r[NPATHS];		 /* 'r' parameter for slow fading */
	float	fading[NPATHS];	 /* slow fading factor */
        float termp; /* Intermediate calculation variable */
        float termi; /* Intermediate calculation variable */
        float heightsqr; /* Intermediate calculation variable */
        float termh; /* Intermediate calculation variable */
        float numer; /* Intermediate calculation variable */
        float denom; /* Intermediate calculation variable */
        float scale; /* scale factor to apply to standard deviation of Gaussian
                        IIR filter to go from normalized to true. */
	/*	int  gaussianSpectrum = 0;*/   /* Gaussian doppler spectrum flag */
	float heightsadd,heightsmult; /* intermediate Tau calculation variables */
	float kconst,bconst; /* k and b terms in Bello's equations */
	float srb,srbinv,temp,coeff; /* intermediate Tau calculation variables */
	float Tau; /* The approximate decay exponent of the delay power spectrum */
	int  warmupInterp = 0;       /* flag to warmup interpolators */
	int  loadInterpWarmup = 0;   /* flag to load interpolator warmup files */
	int  genInterpWarmup = 0;    /* flag to gen. interpolator warmup files */
	int  interpStep = 1;         /* interpolator step size for buffer */
	int	 seed;	        	     /* r.n.g. seed */
	int	 slowFading = 0;		 /* slow fading flag */
	CmdInfo		*cmdSeed;		 /* CmdLine information */
	TAPWGTS		tw;				 /* tapwgts() data */

	float  term1;                /* temporary term for large equations */
	
	/* print copyright notice */

    sscanf(revstring+12,"%f",&revNo);
    fprintf(stdout,"Air to Air Channel Simulator ver. %7.3f\n",revNo);
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
	tw.scatterSampleRate = 10;
	tw.weightLimit = -70;   

	tw.model = AIRAIR_MODEL;   /* select air to air channel model */

	/* set command line args for interface */
	CmdFile("-i Input_File", inFile, 120, cmdMan);
	CmdFloat("-s Sample_Rate_(MHz)", &tw.sampleRate, cmdMan);
	CmdFile("-o Output_File", outFile, 120, cmdMan);
	CmdSpace();

	CmdFlag("-w none",&warmupInterp,cmdOpt);
	CmdFlag("-wld load",&loadInterpWarmup,cmdOpt);
	CmdFlag("-wgen generate",&genInterpWarmup,cmdOpt);
	CmdSpace();

	CmdFloat("-fc Carrier Frequency (MHz)", &tw.carFreq, cmdMan);
	CmdFloat("-vhoriz Velocities (in-plane) (m/s)",velInplane,2,cmdMan);
	CmdFloat("-vperp Velocities_(perp) (m/s)",velPerp,2,cmdMan);
	CmdFloat("-hts Aircraft_Heights_(m)",airheightsmeters,2,cmdMan); 
	CmdFloat("-dis Range (m)",&airDist,cmdMan);
	CmdFloat("-rms RMS Slope of Surface",&rmsSlope,cmdMan); 
	CmdFlag("-dp Direct",&directPath);
        CmdFlag("-disp Specular",&specularPath);
        CmdFlag("-scatp Scatter",&scatterPath);

	CmdFloat("-gd Relative Path Gains (dB)",&gd,cmdDef);
	CmdFloat("-gsp Relative Path Gains (dB)",&gsps,cmdDef);
	CmdFloat("-gsc Relative Path Gains (dB)",&gscs,cmdDef);
	CmdFloat("-scatrate Scat_Path_Samp_Rate (MHz)",&tw.scatterSampleRate,cmdDef);
        CmdDep(&scatterPath,&tw.scatterSampleRate,cmdSensitive);
	/*CmdDep(&directPath,&gd,cmdSensitive);*/
	CmdDep(&specularPath,&gsps,cmdSensitive);
        CmdDep(&scatterPath,&gscs,cmdSensitive);

	CmdInt("-othdisp #_Other_Discrete_Paths",&othDiscretePaths,cmdDef);
	CmdFloat("-gdist Other_Disc_Path_Gains_(dB)",gdiscrete,NPATHS,cmdDef);
	CmdFloat("-nd Other_Disc_Doppler_Shfts_(Hz)", normDplShift, NPATHS, cmdMan);
	CmdFloat("-nb Other_Disc_Path_Delays_(nS)", normPathDelay, NPATHS, cmdDef);
        /* CmdDep(&specularPath,&othDiscretePaths,cmdSensitive); */
        CmdDep(&othDiscretePaths,&gdiscrete,cmdArray);
        CmdDep(&othDiscretePaths,&normDplShift,cmdArray);
        CmdDep(&othDiscretePaths,&normPathDelay,cmdArray);

	CmdFloat("-rmsht RMS Surface Height (m)",&rmsHeight,cmdDef);
	CmdFlag("-comprel Compute_Strengths?",&relPathStrength);
	CmdFloat("-diel Dielectric Constant",&dielConstant,cmdDef);
	CmdFloat("-surfcon Surface Conduct. (mhos/m)",&surfCon,cmdDef);
	CmdInt("-polar Polarization",&polar,cmdDef);
	CmdFloat("-antGain Rel_Ant_Gain_Prod",&antGain,cmdDef);


	CmdDep(&relPathStrength,&rmsHeight,cmdSensitive);
	CmdDep(&relPathStrength,&dielConstant,cmdSensitive);
	CmdDep(&relPathStrength,&surfCon,cmdSensitive);
	CmdDep(&relPathStrength,&polar,cmdSensitive);
	CmdDep(&relPathStrength,&antGain,cmdSensitive);

	/*CmdDep(&relPathStrength,&gd,gscs,gsps,cmdSensitive);*/

	CmdDep(&warmupInterp,&loadInterpWarmup,&genInterpWarmup,cmdRadioBox);


	cmdSeed = CmdInt("-seed R.N.G._Seed", &seed, cmdOpt);
	CmdSpace();

	CmdHelp(helpStr = PathFile("/MobileChan/PropChan/AirMobile/airtoair/airairchan.hlp"));

    CmdArgs(argc, argv);
	CmdUnMan();
	free(helpStr);

        /* Compute normalized aircraft heights with respect to aircraft range: */
        tw.airHeights[0] = airheightsmeters[0]/airDist;
        tw.airHeights[1] = airheightsmeters[1]/airDist;
        /*printf("First relative aircraft height is: %g.\n",tw.airHeights[0]);
        printf("Second relative aircraft height is: %g.\n",tw.airHeights[1]);*/

	tw.alpha = rmsSlope;

	/* Compute the number of paths in the model to run: */
        if (relPathStrength == 1){ /* Overwrite how many paths there are */
           directPath = 1;
           specularPath = 1;
           scatterPath = 1;
           othDiscretePaths = 0;}
        /* total number of discrete paths */
        tw.nPathsNorm = directPath + specularPath + othDiscretePaths; 
        /* total number of scatter paths */
        tw.nPathsDist = scatterPath;               
	tw.nPaths = tw.nPathsNorm+tw.nPathsDist;    /* total number of paths */
	/*printf("Total number of paths is %d.\n",tw.nPaths);*/

	/* check if too many paths were entered */
	if(tw.nPaths > NPATHS || tw.nPaths <= 0) {
		printf("Error from airairchan: total number of discrete and scatter paths "
		" is ZERO or greater than %d.\n", NPATHS);
		exit(1);
	}
	       
	/* set parameter specifying doppler spectrum */
	tw.powSpectrum = GAUSSIAN_SPECTRUM;

        /* set other parameters that are needed in the subroutine besselweight.c */
	tw.airDist = airDist;

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

	/* Calculate the relative gain parameters gd, gsps, and gscs if not entered */
        if (relPathStrength == 1){ /* Overwrite the relative path powers */
	   gd = 0.0;  /* in dB */     
	   /* Initialize: */
	   gsps = 0.0;
	   gscs = 0.0;
           /*printf("Gain of direct path is %g\n",gd);
	   printf("tw.airHeights %f %f\n",tw.airHeights[0],tw.airHeights[1]);
	   printf("%x\n",tw.airHeights);*/
	   relpow(tw.carFreq * ONE_MHZ,
		  tw.airHeights,
		  dielConstant,
		  surfCon,
		  rmsHeight,
		  polar,
		  antGain,
		  &gsps,
		  &gscs);
	   /*printf("gsps is %g dB\n",gsps);
	   printf("gscs is %g dB\n",gscs);*/
	   } /* if (relPathStrength == 1) */

	term1  =   LIGHTSPEED
	  * sqrt( 1 + (tw.airHeights[0]+tw.airHeights[1])
	  *(tw.airHeights[0]+tw.airHeights[1])); 
	/*printf("term1 is %g.\n",term1);*/

	DirectPathDelay = 0.0; /* Initialize, because these parameters are */
	DirectDplShift = 0.0;  /* needed in equations even if directPath not= 1 */

	/* pack the direct, discrete, and scatter mode parameters into a single array */
	j = 0;		/* index to packed arrays */

        /* If the direct path exists, put these parameters in first */
        if (directPath == 1){        /* There is a direct path */
              DirectDplShift = (velInplane[1]-velInplane[0]) *
				tw.carFreq *
				  ONE_MHZ / (LIGHTSPEED * 
				 sqrt( 1 + (tw.airHeights[0]-tw.airHeights[1])*
			         (tw.airHeights[0]-tw.airHeights[1]) ));     
/* Equation (4) in Progress Report 2 */
	      /*printf("DirectDplShift is %g.\n",DirectDplShift);*/


              tw.dplShift[j] = 0.0; /* with respect to Direct path */
              gain[j] = gd;
              tw.gain[j] = gain[j];
              DirectPathDelay = ( airDist / LIGHTSPEED) * 1e9
                               * sqrt(1+( tw.airHeights[0] - tw.airHeights[1] )
                                 * ( tw.airHeights[0] - tw.airHeights[1] )); /*(ns)*/
/* Equation (9) in Progress Report 2 */
	      /*printf("DirectPathDelay is %g.\n",DirectPathDelay);*/

              bulkDelay[j] = 0.0;  /* With respect to DirectPathDelay */
              tw.bulkDelay[j] = bulkDelay[j];
              tw.groupDelay[j] = 0.0;
              directscalsig = 0.0;    /* not needed for direct paths */
              tw.scaledsigma[j]=directscalsig;  /* not needed for discrete paths */
              tw.mode[j] = NORM ;     /* The direct path is a discrete path */
              j++;                   /* index to packed array */
	      /*printf("Direct path is finished, j index is %d.\n",j);*/
      }

	if(specularPath == 1) {                 /* There is a specular path */
	  SpecularDplShift = (velInplane[1]-velInplane[0])
		  * tw.carFreq * ONE_MHZ / (LIGHTSPEED 
			  *sqrt( 1 + (tw.airHeights[0]+tw.airHeights[1]) 
                 *(tw.airHeights[0]+tw.airHeights[1]) ))
                 - DirectDplShift; /* with respect to the direct path */
/* Equation (3) in Progress Report 2 */
	  /*printf("SpecularDplShift is %g.\n",SpecularDplShift);*/

		tw.dplShift[j] = SpecularDplShift;
		gain[j] = gsps;		/* relative path gain of specular path */
		tw.gain[j] = gain[j];	/* Added to tw struct for display purposes */
		specularPathDelay = ( airDist / LIGHTSPEED) * 1e9
                           * sqrt( 1 + (tw.airHeights[0] + tw.airHeights[1] )
                           * ( tw.airHeights[0] + tw.airHeights[1] ))
                           - DirectPathDelay; /*(ns) with respect to DirectPathDelay*/
/* Equation (6) in Progress Report 2 */
		/*printf("specularPathDelay is %g.\n",specularPathDelay);*/

        bulkDelay[j] = specularPathDelay;
	tw.bulkDelay[j] = bulkDelay[j];
        tw.groupDelay[j] = 0.0;
        specularscalsig = 0.0;    /* not needed for discrete paths */
        tw.scaledsigma[j]=specularscalsig;  /* not needed for discrete paths */
	tw.mode[j] = NORM ;     /* Discrete paths */
	j++;			/* index to packed array */
        /*printf("Specular path, j index is %d.\n",j);*/
	}

	for(i = 0; i < othDiscretePaths; i++) {
	  /*printf(" i index is %d\n",i);
	  printf("normDplShift is %g.\n",normDplShift[i]);*/
	  tw.dplShift[j] = normDplShift[i];
	  gain[j] = gdiscrete[i];		/* relative path gain */
	  tw.gain[j] = gain[j];	/* Added to tw struct for display purposes */
	  /*printf("normPathDelay is %g.\n",normPathDelay[i]);*/

	  bulkDelay[j] = normPathDelay[i];
	  tw.bulkDelay[j] = bulkDelay[j];
	  tw.groupDelay[j] = 0.0;
	  normscalsig[i] = 0.0;    /* not needed for discrete paths */
	  tw.scaledsigma[j]=normscalsig[i];  /* not needed for discrete paths */
	  tw.mode[j] = NORM ;     /* Discrete paths */
	  j++;			/* index to packed array */
	  /*printf("Discrete paths, j index is %d.\n",j);*/
	}

        if(scatterPath == 1){
	  distDplShift = (velInplane[1]-velInplane[0]) 
		 * tw.carFreq * ONE_MHZ 
           / (LIGHTSPEED * sqrt( 1 + (tw.airHeights[0]+tw.airHeights[1]) 
                                * (tw.airHeights[0]+tw.airHeights[1]) ))
                 - DirectDplShift; /* with respect to the direct path */
/* Equation (3) in Progress Report 2 */
	  /*printf("distDplShift is %g.\n",distDplShift);*/

	  tw.dplShift[j] = distDplShift;  /* Dpl shift */
	  gain[j] = gscs;		           /* relative path gain */
	  tw.gain[j] = gain[j];	 /* Added to tw struct for display purposes */
	  distBulkDelay = ( airDist / LIGHTSPEED) *1e9
                            * sqrt( 1 + ( tw.airHeights[0] + tw.airHeights[1] )
                                 * ( tw.airHeights[0] + tw.airHeights[1] ))
                           - DirectPathDelay; /*(ns) with respect to DirectPathDelay*/
/* Equation (6) in Progress Report 2 */
	  /*printf("distBulkDelay is %g.\n",distBulkDelay);*/

	bulkDelay[j] = distBulkDelay;    /* bulk delay (nS) */
	tw.bulkDelay[j] = bulkDelay[j];     /* "" */
        tw.groupDelay[j] = 0.0;

/* Calculate sigma_eta, one standard for the Doppler Power Spectrum and
scale it properly according to how the Gaussian IIR filter was designed. */
        termp=(velPerp[0]/tw.airHeights[0] + velPerp[1]/tw.airHeights[1]);
        termp=termp * termp;
        termi=(velInplane[0]/tw.airHeights[0] + velInplane[1]/tw.airHeights[1]);
        termi=termi*termi;
        heightsqr=(tw.airHeights[0]+tw.airHeights[1]) 
                 * (tw.airHeights[0]+tw.airHeights[1]) ;
        termh=(1+heightsqr);
        numer=2 * rmsSlope * tw.airHeights[0] * tw.airHeights[1];
        denom=(LIGHTSPEED/(tw.carFreq * ONE_MHZ)) * sqrt(termh);
	/*        scale=(0.5)/sqrt(- 2.0 * log (sqrt(2*PI)*(.001))); */
        scale=1.0/sqrt(- 2.0 * log (sqrt(2*PI)*(.001))); 
              /* scale to match Gaussian IIR filter design */
	/* 0.5 is the normalized frequency */
        /* sqrt(-2*log(sqrt(2*PI)*(.001))) is where Gaussian is down -30 dB
           for Gaussian IIR of unit standard deviation */
        distscalsig=(numer/denom) * sqrt(termp + (1 / termh) * termi)/scale;
/* Equation (2) in Progress Report 2 */
        /*printf("scale for Doppler is %g.\n",scale);
        printf("sigmanu for Doppler is %g.\n",distscalsig*scale);
        printf("distscalsig is %g.\n",distscalsig);*/

        tw.scaledsigma[j]=distscalsig;
        /*printf("tw.scaledsigma is %g.\n",tw.scaledsigma[j]);*/
  
	/* Compute an approximate decay exponent, Tau, of the
           delay power spectrum form to use to determine the
           number of taps for the scatter path (needed in cancoeff.c) */
	heightsadd = tw.airHeights[0] + tw.airHeights[1];
        heightsmult = tw.airHeights[0]*tw.airHeights[1];
	kconst = heightsmult/ heightsadd  * (1 / (1 + heightsadd*heightsadd));       
	bconst = heightsadd*heightsadd * (1 + heightsadd*heightsadd);
        /*printf("bconst is equal to %g.\n",bconst);*/
	srb = sqrt(bconst);
        srbinv = 1/srb;
	temp = (srb + srbinv)/2;
	coeff = LIGHTSPEED/(kconst*rmsSlope*rmsSlope*airDist);
        /*printf("\ncoeff is %g\n",coeff);*/

	/* Tau = 2*(srb+srbinv)/coeff; */ /* Based on CMK's calculation */
        if(bconst > 1.0/bconst) Tau=3.0*srb/coeff; /* Based on PB's calculation */
        else Tau=3.0*srbinv/coeff;
	tw.Tau=Tau;
        /*printf("Tau is %g.\n",Tau);
        printf("tw.Tau is %g.\n",tw.Tau);*/

	tw.mode[j] = DIST;   		    /* set to scatter mode */
	j++;	 			    /* index to packed array */
        /*printf("Scatter paths, j index is %d.\n",j);*/
	}
        /*printf("Outside of loops\n");*/

	/* check rng seed stuff */
	if(cmdSeed->entered) {		/* if seed entered... */
		srand48(seed);		/* init rng */
	} else {
		srand48(GetSeed());	/* else, get seed from file */
	}
        /*printf("\nGot seed: %d\n\n",seed);*/

	/* do some conversions */
	tw.sampleRate *= 1000000;		/* MHz -> Hz */
	tw.scatterSampleRate *= 1e6;	        /* MHz -> Hz */
        /*printf("Assigning tw.weightLimit");*/
	tw.weightLimit = pow(10.0, tw.weightLimit/10.0); /* dB->power */
        /*printf("Done assigning tw.weightLimit\n");*/

        /*printf("Entering cancoeff\n");*/
	cancoeff(&tw);		/* calculate some coefficients (snapshot rate, etc.) */
        /*printf("Exiting cancoeff\n");*/

	/* convert bulk delay (ns->samples) */
	gainFactor = 0;
	minDelay = MAXINT;
	for(i = 0; i < tw.nPaths; i++) {
	    tmpDelay =  tw.sampleRate * bulkDelay[i]/1e9; /* nS->samples */
		bufferDelay[i] = tmpDelay;

		/* check if fractional delay */
		fractDelay = tmpDelay - (float)(bufferDelay[i]);
		if ( fractDelay > (1-MIN_FRACT_DELAY))
		  bufferDelay[i]++;   /* round up on delay */

		/* compute gain */
		gain[i] = pow(10.0, gain[i]/20.0);	/* dB->voltage */
		gainFactor += gain[i] * gain[i];
	}
	gainFactor = 1.0 / sqrt(gainFactor);
	/* adjust gains to maintain unity output power */

	for(i = 0; i < tw.nPaths; i++) gain[i] *= gainFactor;

	/* initialize taps */
        /*printf("Before ctapwgts call.\n");*/
 	for(i=0; i<tw.nPaths; ++i) {
          /*printf("i index of ctapwgts loop is: %d\n",i);*/
	  ctapwgts(&tw, wgts, i, 1);
        }
        /*printf("After ctapwgts call.\n");*/

	/* check if fractional delay and setup software if needed */
        /*printf("Before FracTapDelay call.\n");*/
	FracTapDelay(&minDelay, &fracDlyFlag,
		    &outBuffer[b], updateCoefFlag, &tw,
		    gain, wgts, sample[0]);
	updateCoefFlag = 1;
        /*printf("After FracTapDelay call.\n");*/

	/* adjust bulk delay values if frac delay used */
	if (fracDlyFlag == 1) {

	  printf("FRACTIONAL DELAY FILTERS ARE IN USE\n");
	  if (minDelay < 0)
	    minDelay = 0;

	  for(i=0; i<tw.nPaths; ++i)
	    bufferDelay[i] = minDelay;

	}


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
/************ read in tap weights if snapshot rate dictates ***************/
		for(i = 0; i < tw.nPaths; i++) {

		  /* test if 'nSamples' stepped over multiplicand of snapshot */
		  interpStep = (int)(nSamples*tw.snapshot[i]/tw.sampleRate)
		    - (int)((nSamples-1)*tw.snapshot[i]/tw.sampleRate);
		  if ( (nSamples != 0) && (interpStep != 0)   ) {
			/* last 2 args of call to ctapwgts() added by LR Nov 1995 */
		    ctapwgts(&tw, wgts, i, interpStep);	   /* get new delay weights */
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
		printf("Error from airairchan:  '%s' not opened", file);
		if(mode[0] == 'w') printf(" for writing");
		else if(mode[0] == 'r') printf(" for reading");
		printf(".\n");
		exit(1);
	}
	return fp;
}






