/* Propagation model as described in 'Generic Channel Simulator,' by 
 * Phil Bello, Chris Nissen, and Jeff Blanchard which was
 * derived from the model described in 'Wideband HF Propagation, 
 * Narrowband Interference, and Atmospheric Noise Models for Link Performance 
 * Evaluation', Phil Bello, MTR 93B0000086 and 'Software Wide Band HF Simulation
 * User's Manual', Richard Coutts, MTR93B0000122  
 * 
 * r.coutts

   copyright (c) 1994 The MITRE Corporation Bedford, MA

   $Id: lin.c,v LL 0.0 12/1996 $
   $Log: lin.c,v $

 * The general program mixedchan.c was used to degenerate a flat
 * fading model: There are multiple discrete paths, if desired, but
 * typically only one direct path is specified; there is one complex
 * Gaussian scatter path.  The strength of the scatter path with
 * respect to the direct path depends on the environment, and is left
 * as an input parameter in this version.  The Doppler power spectrum
 * is Gaussian if the mobile is in an open area and the Doppler power
 * spectrum is flat if the mobile is in a built-up area.  The general
 * data file for power spectrum is available in this program.  This
 * program is the third building block in the Markov model Satellite
 * to Mobile channel programs. There are three possible channel states:
 * blocked, shadowed, and clear.  C. M. Keller, MIT Lincoln Laboratory

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
#include <flatfade.h>
#include <seed.h>
#include <pathfile.h>
#include <fracdly.h>

static char rcsid[] = "$Id: lin.c,v LL 0.0 1996 cmk Exp $";
static char revstring[] = "$Revision: LL 0.0 $";

void main(int argc, char **argv) {
	char 		inFile[120];				/* input file  */
	char		outFile[120];				/* output file */
	#ifdef DEBUG
		char    tapFile[120] = "tapwgts.dbg";           /* tapwgt file */
		char    fadingFile[120] = "fading.dbg";         /* tapwgt file */
	
		FILE    *fadfp;    	              /* fading coeff file ptr */
		FILE    *tapfp;	                      /* taps samples file ptr */
	#endif
	int	 bufferDelay[NPATHS];    /* delay buffer before line */
	fcomplex sum[NPATHS];	         /* sum of each path */
	fcomplex outBuffer[OUTBUFFER];   /* sum of all taps */
	fcomplex *wgts[NPATHS];	         /* weights */
	fcomplex *sample[NPATHS];        /* samples of each path */
	fcomplex s;		         /* input sample */
	fcomplex sdummy;   	         /* dummy input sample */
        float    lono;                   /* lognormal fading coefficient*/
	int	 i,ii;	                 /* index */
	int	 j;	                 /* index */
	FILE	 *infp; 	         /* input samples file ptr */
	FILE	 *outfp;		 /* output samples file ptr */
        float    revNo;                  /* software rev number */
	int	 nSamples = 0;	         /* input samples processed */
	int	 b = 0;		         /* outBuffer[] index */
	int      minDelay = MAXINT;      /* min bulk delay */
	float    tmpDelay;               /* temporary storage */
	float    fractDelay;             /* temporary storage */
	int      updateCoefFlag = 0;     /* flag to update coef */
	int      fracDlyFlag = 0;        /* frac. dly. flag */
	float	 normDplShift[NPATHS];   /* Dpl shift of discrete paths */
	float    distDplShift;     	 /* Dpl shift of scatter path */
        float    distscalsig;            /* scaled sigma_eta for scatter path */
	float	 normGain[NPATHS];       /* relative gain of discrete paths */
	float	 gain[NPATHS][NUMSTATES]; /* relative gains of all paths for states */
	float	 gainpass[NPATHS];       /* relative gains of all paths for one state */
        int      numofsamps;             /* Number of samples to spend in the
                                            currently chosen state. */
        int      sampnum;                /* Count of how many samples processed
                                            in the current state. */
        float    sampspermeter;          /* sample-rate/vehicle-speed */
	float	 normBulkDelay[NPATHS];  /* bulk delay of discrete paths (mS) */
	float	 distBulkDelay;  	 /* bulk delay of scatter path(mS) */
	float	 bulkDelay[NPATHS];	 /* bulk delay of all paths (mS) */
	char	 *helpStr;		 /* help string */
	float	 tau[NPATHS];		 /* 'tau' param for slow fading */
        char     dop_data_fname[120];    /*  data filename */
	float	 r[NPATHS];		 /* 'r' parameter for slow fading */
	float	 fading[NPATHS];	 /* slow fading factor */
        float    numer;                  /* Intermediate calculation variable */
        float    denom;                  /* Intermediate calculation variable */
        float    scale;                  /* scale factor to apply to standard deviation
                                            of Gaussian IIR filter to go from
                                            normalized to true. */
	float probb[NUMSTATES];     /* probability system is in each state */
        float transprobb[NUMTRANS]; /* transition probability matrix */
	float distance;             /* state transition distance */
        float lonomean[NUMSTATES];  /* blocked state lognormal mean */
        float lonostd[NUMSTATES];   /* blocked state lognormal standard deviation */
	float distGain[NUMSTATES];  /* relative gain of scatter path */
        float convert;              /* Conversion from base e to base 10 */
        float mean1;                /* mean for base e lognormal */
        float sigma1;               /* sigma for base e lognormal */
	int  sysstate = CLEAR;      /* flag for Lin's system state*/
	int  flatSpectrum = 0;      /* flat doppler spectrum flag */
	int  gaussianSpectrum = 0;  /* Gaussian doppler spectrum flag */
	int  dataSpectrum = 0;      /* general doppler spectrum flag */
	int  warmupInterp = 0;      /* flag to warmup interpolators */
	int  loadInterpWarmup = 0;  /* flag to load interpolator warmup files */
	int  genInterpWarmup = 0;   /* flag to gen. interpolator warmup files */
	int  interpStep = 1;        /* interpolator step size for buffer */
	float   rmsSlope;           /* RMS slope of surface */
        float elev;                 /* elevation angle to the satellite in degrees */
        float rad;                  /* elevation angle to the satellite in radians*/
	float           exptau;     /* time constant for exponential decay */
	int		seed;	    /* r.n.g. seed */
	int  slowFading = 0;	    /* slow fading flag */
	CmdInfo		*cmdSeed;   /* CmdLine information */
	TAPWGTS		tw;	    /* tapwgts() data */
	
	/* print copyright notice */

        sscanf(revstring+10,"%f",&revNo);
        fprintf(stdout,"Lin Model Channel Simulator ver. %7.3f\n",revNo);
	fprintf(stdout,"copyright (c) 1994 The MITRE Corporation Bedford, MA\n");
	fprintf(stdout,"revised (1997) by the MIT Lincoln Laboratory, Lexington, MA\n");

	/* clear memory */
	for(i = 0; i < NPATHS; i++) {
	  for(j = 0; j < NUMSTATES; j++) gain[i][j] = 0;
          /*printf("LIN: gain for state %d and path %i is %g\n",j,i,(double) gain[i][j]);*/
	  bulkDelay[i] = 0;
	}

	/* initial values to ensure all were entered */
	tw.nPathsNorm = 1; /* init number of normal paths */
	tw.nPathsDist = 1; /* init number of disturbed paths */
	tw.thinPhScr = 0;	/* this is no longer optional */
	tw.thinPhaseC = 0.25;			/* C parameter */
	tw.thinPhaseThresh = 0.05;		/* tail threshold parameter */
	tw.scatterSampleRate = 1;
	tw.weightLimit = -70;   
        for(i=0;i<NUMSTATES;i++) distGain[i] = -3;
        distBulkDelay = 0;
        rmsSlope = 0.5;
        elev = 30;

	tw.model = LIN_MODEL;   /* select the LIN MODEL */

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
	CmdFloat("-vs Vehicle_Speed (m/s)", &tw.VehSpd, cmdMan);

	CmdSpace();
	CmdInt("-norm Number_of_Discrete_Paths", &tw.nPathsNorm, cmdDef);
	CmdFloat("-nd Fractional_Dop_Shft_([-1,1])", normDplShift, NPATHS, cmdMan);
	CmdFloat("-nx Rel_Discrete_Path_Gain_(dB)", normGain, NPATHS, cmdMan);
	CmdFloat("-nb Discrete_Bulk_Delay_(nS)", normBulkDelay, NPATHS, cmdMan);
	CmdSpace();

        tw.nPathsDist = 1;  /* Fix for 1 scatter path */
        tw.scatterSampleRate = 1;  /* in MHz */
	CmdFloat("-dx State RMS Scatter Path Gain (dB)", &distGain, NUMSTATES, cmdMan);
	CmdFloat("-db Scatter_Path_Bulk_Delay_(nS)", &distBulkDelay, cmdMan);

	CmdSpace();
	CmdFlag("-flat flat",&flatSpectrum);
	CmdFlag("-gaussian Gaussian",&gaussianSpectrum);
	CmdFlag("-datadopp data",&dataSpectrum);
	CmdSpace();

        CmdFloat("-rms RMS Slope of Surface",&rmsSlope,cmdMan);
        CmdFloat("-el Satellite Elev. Ang. (deg)",&elev,cmdMan);
	CmdFile("-dopfile Dop_Pow_Spec_File", dop_data_fname, 120, cmdMan);
        exptau = 1000.0;  /* Fixed so exptau*tw.scatterSampleRate=1 */
	CmdSpace();

	CmdFloat("-prob State Probs.", &probb, NUMSTATES, cmdMan);
        CmdFloat("-trans State Transition Probs.",&transprobb, NUMTRANS, cmdMan);
	CmdFloat("-mean State Lognormal m (dB)", &lonomean, NUMSTATES, cmdMan);
	CmdFloat("-std State Lognormal Sigma (dB)", &lonostd, NUMSTATES, cmdMan);
        CmdFloat("-dis State Transition Distance (m)", &distance, cmdMan);

	cmdSeed = CmdInt("-seed R.N.G._Seed", &seed, cmdOpt);
	CmdSpace();

	CmdDep(&gaussianSpectrum,&rmsSlope,&elev,cmdSensitive);
	CmdDep(&dataSpectrum,dop_data_fname,cmdSensitive);
	CmdDep(&tw.nPathsNorm, normDplShift, normGain, normBulkDelay, cmdArray);
        CmdDep(&flatSpectrum,&gaussianSpectrum,&dataSpectrum,cmdRadioBox);
	CmdDep(&warmupInterp,&loadInterpWarmup,&genInterpWarmup,cmdRadioBox);
        
	CmdHelp(helpStr = PathFile("/MobileChan/PropChan/SatMobile/lin.hlp"));

	CmdArgs(argc, argv);
	CmdUnMan();
	free(helpStr);

        /****************************************************************
	* Compute the mean and variance parameters for the lognormal    *
        * distribution based on the mean and RMS values of the          *
        * lognormally-distributed discrete path.  See eqs. 12.9, 12.10, *
        * 12.11, 12.12, 12.17, and 12.18 of Generic Channel Simulator,  *
        * Interim Report, by Phil Bello.                                *
        ****************************************************************/

        convert=20.0/log(10.0);
        for(i=0;i<NUMSTATES;i++)
	{
          /*printf("LIN: entered lonomean for state %d in dB is %g.\n",i,(double) lonomean[i]);
          printf("LIN: entered lonostd for state %d in dB is %g.\n",i,(double) lonostd[i]);*/
	  lonomean[i] = pow(10.0, lonomean[i]/20.0);		/* dB->voltage */
	  lonostd[i] = pow(10.0, lonostd[i]/20.0);		/* dB->voltage */
          /*printf("LIN: entered lonomean for state %d undBed is %g.\n",i,(double) lonomean[i]);
          printf("LIN: entered lonostd for state %d undBed is %g.\n",i,(double) lonostd[i]);*/
          sigma1 = log(1.0 + (lonostd[i]/lonomean[i])*(lonostd[i]/lonomean[i]));
          mean1=log(lonomean[i]) - lonostd[i]*lonostd[i]/2.0;
	  /*printf("LIN: m1 for state %d is %g.\n",i,(double) mean1);
          printf("LIN: sigma1 for state %d is %g.\n",i,(double) sigma1);*/
          lonomean[i]=convert * mean1;
          lonostd[i]=convert * sigma1;
	  /*printf("LIN: calcd lonomean for state %d is %g.\n",i,(double) lonomean[i]);
          printf("LIN: calcd lonostd for state %d is %g.\n",i,(double) lonostd[i]);
          printf("\n");*/
	}

        /*********************************************************
        * Set up parameters for the Lin states. All three states *
        * have the same path structure, but with different       *
        * parameters.                                            *
        *********************************************************/

	tw.nPaths = tw.nPathsNorm+tw.nPathsDist;        /* total number of paths 
                                                           for clear state. */
	if(tw.nPaths > NPATHS || tw.nPaths <= 0)       	/* check if too many paths
                                                           were entered */
	{
	  printf("Error from LIN: total number of discrete and scatter paths\n");
	  printf("is ZERO or greater than %d.\n", NPATHS);
	  exit(1);
	}

        /******************************************************
	* Independent of which state, pack Direct and Scatter *
        * mode parameters into a single array.                *
        * Put the scatter path first in this program          *
        * because this was copied over from lutz.c            *
        ******************************************************/

	j = 0;			              /* index to packed arrays */

        /***************************************
	* The one and only SCATTER PATH FIRST: *
        ***************************************/
	distDplShift = ((tw.carFreq * ONE_MHZ * tw.VehSpd) / LIGHT_SPEED);
	tw.dplShift[j] = distDplShift;	      /* Dpl shift */
          /* Relative gain initialization */
        for(i=0; i<NUMSTATES; i++)
        {
          gain[j][i] = distGain[i];
          /*printf("LIN: scatter path gain for state %d is %g\n",i,(double) gain[j][i]);*/
	  tw.stategain[j][i] = gain[j][i];    /* In tw struct for display purposes */
        }
        /*************************************************
        * Later when the system state is known,          *
        * tw.gain[j] is set to tw.stategain[j][sysstate] *
        *************************************************/
	bulkDelay[j] = distBulkDelay;	      /* bulk delay (nS) */
	tw.bulkDelay[j] = distBulkDelay;      /* In tw struct for display purposes */
        tw.multiPathSpr[j] = 0.0;	      /* multipath spread (n/a) */

        /******************************************************
	* Set parameter specifying doppler power spectrum.    *
        ******************************************************/
	if ( gaussianSpectrum )
	  tw.powSpectrum = GAUSSIAN_SPECTRUM;   /* exchanged for jakes 7/1996 */
	else if ( flatSpectrum )
	  tw.powSpectrum = FLAT_SPECTRUM;
	else
	  tw.powSpectrum = DATA_SPECTRUM;      /* new type of powSpectrum added 
                                                  Nov.1995 LR MIT Lincoln Lab */
	       
        /******************************************************
        * Set warmup flag.                                    *
        ******************************************************/
	if (tw.nPathsDist == 0 )
	  tw.warmup = DO_WARMUP;    /* don't load or save since interp not used */
	else if (warmupInterp == 1)
	  tw.warmup = DO_WARMUP;
	else if (loadInterpWarmup == 1)
	  tw.warmup = LOAD_WARMUP;
	else if (genInterpWarmup == 1)
	  tw.warmup = GEN_WARMUP;
	else
	  tw.warmup = DO_WARMUP;

        /**********************************************************
        * Calculate sigma_eta, one standard for the Gaussian      *
        * Doppler Power Spectrum and scale it properly according  *
        * to how the Gaussian IIR filter was designed.            *
	* (1) 0.5 is the normalized frequency                     *
        * (2) sqrt(-2*log(sqrt(2*PI)*(.001))) is where Gaussian   *
        *     is down -30 dB for Gaussian IIR of unit standard    *
        *     deviation.                                          *
        /*********************************************************/
        if ( gaussianSpectrum )
        {
          numer=2 * rmsSlope * tw.VehSpd;
          denom=(LIGHTSPEED/(tw.carFreq * ONE_MHZ));      /* wavelength */
          scale=1.0/sqrt(- 2.0 * log (sqrt(2*PI)*(.001))); 
          rad=(PI/180.0)*elev;
          distscalsig=(numer/denom) * sin(rad)/scale;
          /*printf("elev is %g and sin(rad) is %g\n",(double) elev,(double) sin(rad));*/
          /* Equation (2) in Progress Report 2 */
          /*printf("scale for Doppler is %g.\n",(double) scale);
          printf("sigmanu for Doppler is %g.\n",(double) distscalsig*scale);
          printf("distscalsig is %g.\n",(double) distscalsig);*/
        }
        else 
        {
          distscalsig=distDplShift;  /* For the flat power spectral density */
        }
        tw.scaledsigma[j]=distscalsig;
        /*printf("tw.scaledsigma is %g.\n",(double) tw.scaledsigma[j]);*/

	tw.mode[j] = DIST;			/* set to disturbed mode */
	tw.groupDelay[j] = 0.0;			/* group delay (n/a) */
	tw.expTau[j] = exptau/1e9;              /* exp. time constant */
	j++;	  			        /* index to packed array */

        /*************************
        * DISCRETE PATHS NEXT:   *
        *************************/
	for(i = 0; i < tw.nPathsNorm; i++)    
        {
	  normDplShift[i] = ((tw.carFreq * ONE_MHZ * tw.VehSpd) / LIGHT_SPEED) 
		  * normDplShift[i];	      /* Fractional Dpl shift for Discrete */
	  tw.dplShift[j] = normDplShift[i];   /* Put in tw structure */
          for(ii=0; ii<NUMSTATES; ii++)
          {
            gain[j][ii] = normGain[i];
            /*printf("LIN: gain for discrete path %d for state %d is %g\n",i,ii,(double) gain[j][ii]);*/
	    tw.stategain[j][ii] = gain[j][ii]; /* In tw struct for display purposes */
            /*printf("LIN: tw.stategain for discrete path %d for state %d is %g\n",i,ii,(double) tw.stategain[j][ii]);*/
          }
          /*************************************************
	  * Later when the system state is known,          *
          * tw.gain[j] is set to tw.stategain[j][sysstate] *
          *************************************************/
	  bulkDelay[j] = normBulkDelay[i];    /* bulk delay (nS) */
          tw.bulkDelay[j] = normBulkDelay[i]; /* In tw struct for display purposes */
	  tw.dplSpread[j] = 0.0;	      /* n/a to norm mode*/
	  tw.groupDelay[j] = 0.0;	      /* group delay */
	  tw.multiPathSpr[j] = 0.0;	      /* multipath spread (n/a) */
          tw.scaledsigma[j]=0.0;              /* not needed for discrete paths */
	  tw.mode[j] = NORM;		      /* set to normal mode */
	  tw.expTau[j] = 0.0;                 /* exp. time constant */
	  j++;				      /* increase index to packed array */
        }

        /**********************************
        * End of packing paths into array *
        **********************************/

        /************************
	* check rng seed stuff  *
        ************************/
	if(cmdSeed->entered) 			/* if seed entered... */
        {
		srand48(seed);			/* init rng */
	} 
        else 
        {
		srand48(GetSeed());		/* else, get seed from file */
	}

        /**********************
        * Do some conversions *
        **********************/
	tw.sampleRate *= 1000000;		         /* MHz -> Hz */
	tw.scatterSampleRate *= 1e6;	                 /* MHz -> Hz */
	tw.weightLimit = pow(10.0, tw.weightLimit/10.0); /* dB->power */

        /****************************************************
        * calculate some coefficients (snapshot rate, etc.) *
        ****************************************************/
	cancoeff(&tw);

        /***********************************
        * convert bulk delay (mS->samples) *
        ***********************************/
        /*************************************************
        * There is no power normalization in this progam *
        *************************************************/
	minDelay = MAXINT;
	for(i = 0; i < tw.nPaths; i++) 
        {
	  tmpDelay =  tw.sampleRate * bulkDelay[i]/1e9;     /* nS->samples */
	  bufferDelay[i] = tmpDelay;
          /* check if there is a fractional delay */
	  fractDelay = tmpDelay - (float)(bufferDelay[i]);
	  if (fractDelay > (1-MIN_FRACT_DELAY)) bufferDelay[i]++; /* round up on delay */
          /* compute gain */
          /*printf("LIN: NUMSTATES is %d\n",NUMSTATES);*/
          for(ii=0; ii<NUMSTATES; ii++)
	  {
	    gain[i][ii] = pow(10.0, gain[i][ii]/20.0);  /* dB->voltage */
            /*printf("LIN: gain for path %d of state %d is %g\n",i,ii,(double) gain[i][ii]);*/
          } 
	}

        /******************
	* initialize taps *
        ******************/
 	for(i=0; i<tw.nPaths; ++i)
        {
          ctapwgtslin(&tw, wgts, i, 1,dop_data_fname);
        }

        /*********************************************************
	* check if fractional delay and setup software if needed *
        * Do this set up assuming systate = 0.                   * 
        *********************************************************/
   
        for(i=0; i<tw.nPaths; i++) gainpass[i]=gain[i][0];
        FracTapDelay(&minDelay, &fracDlyFlag,
	             &outBuffer[b], updateCoefFlag, &tw,
		     gainpass, wgts, sample[0]);
 	updateCoefFlag = 1;

        /**********************************************
	* adjust bulk delay values if frac delay used *
        **********************************************/
	if (fracDlyFlag == 1) 
        {
	  printf("FRACTIONAL DELAY FILTERS ARE IN USE\n");
	  if (minDelay < 0)
	  {
	    minDelay = 0;
          }
	  for(i=0; i<tw.nPaths; ++i)
	  {
          bufferDelay[i] = minDelay;
          }
	}

        /****************************************
	* if slowfading is requested by user    *
        ****************************************/
	if(slowFading)
        {
          printf("LIN: slowFading flag is set.\n");
          fade(fading, tau, r, tw.nPaths, 1);	/* init slow fading */
	} 
        else 
        {
	  for(i = 0; i < NPATHS; i++) fading[i] = 1.0;	/* don't scale */
	}

        /****************************************************************
	* Initialize first system state before entering data processing *
        * while loop.                                                   *
        ****************************************************************/
        sysstate = CLEAR; /* System state */
        numofsamps = 0;    /* Number of samples to be in this system state */
        sampspermeter = tw.sampleRate / tw.VehSpd;  /* samples per meter travelled */

        /*printf("LIN: P(clear state) is %g.\n",(double) probb[0]);
        printf("LIN: P(shadowed state) is %g.\n",(double) probb[1]);
        printf("LIN: P(blocked state) is %g.\n",(double) probb[2]);
        printf("LIN: Transition Probability Q[0] is %g.\n",(double) transprobb[0]);
        printf("LIN: Transition Probability Q[1] is %g.\n",(double) transprobb[1]);
        printf("LIN: Transition Probability Q[2] is %g.\n",(double) transprobb[2]);
        printf("LIN: Transition Probability Q[3] is %g.\n",(double) transprobb[3]);
        printf("LIN: Transition Probability Q[4] is %g.\n",(double) transprobb[4]);
        printf("LIN: Transition Probability Q[5] is %g.\n",(double) transprobb[5]);
        printf("LIN: Transition Probability Q[6] is %g.\n",(double) transprobb[6]);
        printf("LIN: Transition Probability Q[7] is %g.\n",(double) transprobb[7]);
        printf("LIN: Transition Probability Q[8] is %g.\n",(double) transprobb[8]);
        printf("LIN: state transition distance is %g.\n",(double) distance);
        printf("LIN: samples per m traveled is %g.\n",(double) sampspermeter);*/

        initlinstate(&sysstate,&numofsamps,probb,transprobb,distance,sampspermeter);

        for (j=0;j<tw.nPaths;j++)
        {
          tw.gain[j]=tw.stategain[j][sysstate];
          /*printf("LIN: New path gain for state %d is %g\n",j,tw.gain[j]);*/
        }
        /*printf("LIN: sysstate = %d.\n",sysstate);
        printf("LIN: numofsamps = %d.\n",numofsamps);*/

        sampnum = 0;

        /********************************************************
	* No matter which state, all of the tw.nPaths are used. *
        * The lognormal multiplication factor for the discrete  *
        * path is dependent on which state the system is in.    *
        ********************************************************/
        lognorm(&lono,1,lonomean[sysstate],lonostd[sysstate]);  
        printf("LIN: The initial state is %d.\n",sysstate);
        printf("      The duration is %d samples.\n",numofsamps);
        printf("      The lognormal amplitude is %g\n",lono);

        /*************************************************************
	* Initialize buffer memory size to use the maximum number of *
	* paths.  This first call to buffer does not affect the      *
	* samples written to the output file because the outBuffer   *
        * index is not incremented here.  The while(fread()) loop    *
        * later keeps strict tabs on how many samples are processed  *
        * and it is this many samples that are written to the output *
        * file.                                                      *
        *************************************************************/
        sdummy.r = 0.0;
        sdummy.i = 0.0;
        buffer(bufferDelay, tw.nPaths, sample, sdummy);  /* pass dummy sample into
                                                              delay buffer */
        /*printf("LIN: sdummy is %g +i%g\n",sdummy.i,sdummy.r);*/
        /*for (i=0; i<tw.nPaths;i++)
        {
          printf("LIN: sample is %g +i%g\n",sample[i]->r,sample[i]->i);
        }*/

        /*************
	* Open files *
        **************/
	#ifdef DEBUG
          tapfp = FOpen(tapFile, "w");	    /* open tap-weight file */
	  fadfp = FOpen(fadingFile, "w");   /* open fading data file */
	#endif
	infp = FOpen(inFile, "r");	    /* open input data file */
	outfp = FOpen(outFile, "w");	    /* open output output file */

        /****************************************
	* loop once for ea complex input sample *
        ****************************************/
	printf("\nWriting output file '%s'...\n", outFile); 
        fflush(stdout);

	j = 0;			        /* number of tapwgts() calls */

        /*************************************************************
	* main loop for reading and processing complex input samples *
        **************************************************************/
	while(fread(&s, sizeof(fcomplex), 1, infp) != 0) 
        {                                     
          if(sampnum >= numofsamps)                  
	  {                            /* Time to select a new system state */
            /*printf("LIN: sysstate before linstate call = %d.\n",sysstate);*/
            linstate(transprobb,distance,sampspermeter,&sysstate,&numofsamps);
            /*printf("LIN: sysstate = %d.\n",sysstate);
            printf("LIN: numofsamps = %d.\n",numofsamps);*/
            for (j=0;j<tw.nPaths;j++)
            {
              tw.gain[j]=tw.stategain[j][sysstate];
              /*printf("LIN: New path gain for state %d is %g\n",j,tw.gain[j]);*/
            }
            sampnum = 0;               /* Begin counting samples inside a state */
            lognorm(&lono,1,lonomean[sysstate],lonostd[sysstate]);  
                                               /* Calculate lognormal multiplier */
            printf("\nLIN: The new sysstate is %d.\n",sysstate);
            printf("      The duration is %d samples.\n",numofsamps);
            printf("      The lognormal amplitude is %g\n",lono);
	  }  /* Done picking a new system state */
          /*printf("LIN: sysstate is %d.\n",sysstate);*/

          sampnum++;                                  /* Increment counter for # of 
                                                         samples in the current state */
          buffer(bufferDelay, tw.nPaths, sample, s);  /* pass sample into delay buffer */

          /*printf("LIN: s is %g +i%g\n",s.i,s.r);
          for (i=0; i<tw.nPaths;i++)
          {
            printf("LIN: sample is %g +i%g\n",sample[i]->r,sample[i]->i);
          }*/

          if(nSamples % FADE_UPDATE == 0)             /* get new fading coeff
                                                         every 'FADE_UPDATE' samples */
          {
            if(nSamples == 0)                         /* print to screen */
            {
              printf("%d samples per tick: \n", FADE_UPDATE); 
              fflush(stdout);
	    } 
            else if(nSamples % (50 * FADE_UPDATE) == 0) 
            {
	      printf("%d", nSamples);
	    } 
            else 
            {
	       printf("."); fflush(stdout);
	    }
            if(slowFading) 
            {
	      fade(fading, tau, r, tw.nPaths, 0);       /* get fading parameters */
              printf("LIN: slowFading is set.\n");
            }
	    #ifdef DEBUG
	      fwrite(&fading[0], sizeof(float), tw.nPaths, fadfp);
	    #endif
	  }

          /************************************************
	  * read in tap weights if snapshot rate dictates *
          ************************************************/
	  for(i = 0; i < tw.nPaths; i++) 
          {
            /* test if 'nSamples' stepped over multiplicand of snapshot */
	    interpStep = (int)(nSamples*tw.snapshot[i]/tw.sampleRate)
		          - (int)((nSamples-1)*tw.snapshot[i]/tw.sampleRate);
	    if ( (nSamples != 0) && (interpStep != 0) ) 
            {                        
              /* get new delay weights */
	      ctapwgtslin(&tw, wgts, i, interpStep,dop_data_fname); 
              updateCoefFlag = 1;
              #ifdef DEBUG
		 fwrite(wgts[i], sizeof(fcomplex), tw.nTaps[i], tapfp);
              #endif
	     }
	  }

          /******************************************************
          * No matter which state the system is in, a lognormal *
          * parameter is needed to multiply the direct discrete *
          * path.  Cannot simply multiply the input sample by   *
          * the parameter as in lutz.c, but only multiply the   *
          * first discrete path by the parameter out of the tap *
          * delay line.  The first discrete path is the second  *
          * path packed in the structure.                       *
          ******************************************************/

          /*************************************************************
	  * if fractional delay then handle as one large tap delay and *
          * call the fractapdelay subroutine                           *
          *************************************************************/
	  if (fracDlyFlag == 1) 
          {
            for(i=0; i<tw.nPaths; i++)
            {
              gainpass[i]=gain[i][sysstate];
              /*printf("LIN: gain passed to FracTapDelay for path %d is %g\n",i,gainpass[i]);*/
            }
            FracTapDelay(&minDelay, &fracDlyFlag, &outBuffer[b], updateCoefFlag, 
                         &tw, gainpass, wgts, sample[0]); 
            updateCoefFlag = 0;
	  }
          else 
          {
	    for(i = 0; i < tw.nPaths; i++)  /* get output of each tap delay line */
            {
	      tapdelay(&sum[i], i, tw.nTaps[i], wgts[i], sample[i]);
	    }
	    /* tally outputs of tap delay lines and multiply by gain and fading */
	    outBuffer[b].r = outBuffer[b].i = 0;
	    for(i = 0; i < tw.nPaths; i++)
            {
              if(i==0) /* No lognormal gain multiplier for scatter path */
	      {
	        outBuffer[b].r += fading[i] * gain[i][sysstate] * sum[i].r;
	        outBuffer[b].i += fading[i] * gain[i][sysstate] * sum[i].i;
              }
              else /* Multiply all discrete paths by lognormal gain */
	      {
	        outBuffer[b].r += fading[i] * gain[i][sysstate] * lono * sum[i].r;
	        outBuffer[b].i += fading[i] * gain[i][sysstate] * lono * sum[i].i;
              }
	    }
	  }
          /*printf("nSample just processed was %d\n",nSamples);*/
	  nSamples++;  	/* incr number of sample processed */
	  b++;	        	/* incr outBuffer[] index */
	  if(nSamples%OUTBUFFER == 0)  /* output a block of data */
          {
            /*printf("LIN: writing output buffer block and resetting b to 0.\n");*/
	    fwrite(outBuffer, sizeof(fcomplex), b, outfp);
	    b = 0;
	  }
        }  /* Done with main while loop that reads and processes each sample */

	printf("done.\n");
	printf("%d samples written to %s\n", nSamples, outFile);
	/* output any residual data */
	fwrite(outBuffer, sizeof(fcomplex), b, outfp);
	SaveSeed(lrand48());			/* save seed to file for next run */
}	

/* 'fopen' file and check for error */
FILE *FOpen(char *file, char *mode) {
	FILE	*fp;

	if((fp = fopen(file, mode)) == NULL) 
        {
	  printf("Error from LIN:  '%s' not opened", file);
	  if(mode[0] == 'w') printf(" for writing");
	  else if(mode[0] == 'r') printf(" for reading");
	  printf(".\n");
	  exit(1);
	}
	return fp;
}





