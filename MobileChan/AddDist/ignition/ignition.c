/* Ignition noise as described in "Generic Channel Simulator" by
 * Phil Bello
 *
 * The algorithm is implimented by:
 * (1) finding at what times impulsive noise is to be added according to the
 *     Poisson distribution.
 * (2) Generating the amplitudes of the noise at those instants according to
 *     a Weibul distribution.
 * (3) Generating the phase of the noise at those instants according to
 *     a uniform distribition.
 * C. M. Keller

 $Id: ignition.c,v 0.0 1997/02/20 13:20:14 cmk Exp $
 $Log: ignition.c,v $ */


#include <stdio.h>
#include <math.h>
#include <malloc.h>
#include <string.h>
#include <ignition.h>
#include <Cmdline.h>
#include <seed.h>
#include <pathfile.h>
#include <complex.h>

/*#define STATS*/  /* To write statistics on amplitude and interarrival times */

static char rcsid[] = "$Id: ignition.c,v 0.0 1997/02/20 13:20:14 cmk Exp $";

void main(int argc, char **argv) {

	char		outfile[120] = "";   /* output data file */
	CmdInfo		*cmdOutfile;	     /* cmdline structure */
        float           tT;                  /* total time (secs) of noise simulation */
        float           samprate;            /* simulation sample rate in samples 
                                                per sec */
        float           lamda;               /* For the Poisson distribution */
        float           mweibull;            /* m of the Weibull distribution 
                                                for amplitude */
        float           kweibull;            /* k of the Weibull distribution */
        float           Fa;                  /* normalized power density */
        float           Ni;                  /* complex power density in dBm/kHz converted to Watts/Hz */
        float           centfreq;            /* center frequency of the simulation */
        int             numimpulses;         /* count of the number of non-zero impulses */
        int             currentsample;       /* current sample in output sequence */
	int	        nsamples;  	     /* number of samples in signal */
        double          drand48();
        double          uniformrv;           /* sample of the uniform [0,1)
                                                distribution function */
	float		timeint;	     /* current time interval
                                                between impulses (sec)*/
        int             sampleint;           /* current sample interval between impulses */
        double          xval,yval;           /* temporary variables */
        double          theta;               /* sample of the uniform [0,2*PI)
                                                distribution function */
        double          ampli;                /* sample of the Weibull distribution 
                                                function */
	fcomplex	*out;		     /* output data */
	int		seedrng;	     /* random number generator seed */
	CmdInfo		*cmdSeedRNG;	     /* structure to input argument */
	char		*helpStr;	     /* help string */
        FILE            *outfp;              /* output samples file pointer */
#ifdef STATS
        float           *amps;                /* Amplitude statistics */
        float           *ints;                /* Interval statistics */
	char ampstatfile[120] = "ampstats.bin";   /* Amplitude statistic data file */
        FILE *outampstats;                        /* Amplitude statistic file pointer */
	char intstatfile[120] = "intstats.bin";   /* Interval statistic data file */
        FILE *outintstats;                        /* Interval statistic file pointer */
#endif STATS

	/* print copyright notice */



	/* get command-line args */
	CmdFloat("-t Total_Time_(seconds)", &tT, cmdMan);
	CmdFloat("-s Sample_Rate_(MHz)", &samprate, cmdDef);
	CmdFloat("-lam Pulses_Per_Second", &lamda, cmdMan);
	CmdFloat("-Wm Weibull_parameter_m", &mweibull, cmdMan);
	CmdFloat("-freq Center_Frequency (MHz)", &centfreq, cmdMan);
	cmdSeedRNG = CmdInt("-S R.N.G._Seed", &seedrng, cmdOpt);
	cmdOutfile = CmdFile("-o Output_File", outfile, 100, cmdOpt);
	CmdHelp(helpStr = PathFile("MobileChan/AddDist/ignition/ignition.hlp"));
	CmdArgs(argc, argv);
	CmdUnMan();
	free(helpStr);

	/* conversion */
	samprate *= 1000000;			/* sampling rate (Hz->MHz) */
        /*printf("sample rate is %g\n",samprate);*/
	
	if(cmdOutfile->entered == 0) {		/* if no file entered... */
  	  outfile[0] = '\0';	                /* don't write a data file */
	  printf("NOTE: NO DATA FILE OUTPUT REQUESTED\n");
	}

        nsamples = tT * samprate;               /* Total number of samples */
        /*printf("Total number of samples is %d.\n",nsamples);*/

        /* Initialize the output vector to all 0's */
        out = (fcomplex *)calloc(nsamples, sizeof(fcomplex));
        /*printf("out[0] is %g + i %g \n",out[0].r,out[0].i);
        printf("out[100] is %g + i %g \n",out[100].r,out[100].i);*/

        #ifdef STATS  /* Allocate space for statistics */
          amps = (float *)calloc(nsamples, sizeof(float));
          ints = (float *)calloc(nsamples, sizeof(float));
        #endif

	if(cmdSeedRNG->entered) {		/* if user input a seed... */
		srand48(seedrng);		/* ...set the r.n.g. */
	} else {			        /* else no seed specified... */
		srand48(GetSeed());		/* get old seed */
	}

        /* Generate the necessary parameter k for the Weibull distribution */
        if (centfreq > 30 && centfreq < 200)
          Fa = 36.0 - 22.3*log10(centfreq/10.0);
        else if (centfreq >= 200 && centfreq < 2000)
          Fa = 7.0 - 12.25*log10(centfreq/200.0);
        else
	{
          printf("\nIGNITION: centfreq out of range.");
          printf("\n          Setting Fa = 0.\n\n");
          Fa = 0;
        }
        Ni = Fa - 144.0 + 3;                  /* Relative to Nonrelative + 3 dB for complex */
        printf("\nPower density is %g dBm/kHz.\n",Ni);
        Ni = pow(10.0, Ni/10.0);              /* Ni in mW/kHz */
        /*printf("Power density is %g mW/kHz.\n",Ni); */
        Ni = Ni/1000.0;                       /* Ni in mW/Hz  */
        /*printf("Power density is %g mW/Hz.\n",Ni); */
        Ni = Ni/1000.0;                       /* Ni in Watts per Hz */
        printf("Power density is %g Watts/Hz.\n\n",Ni);
 
        xval = exp(gamma(1.0+2.0/mweibull))*(lamda/Ni);  /* Equation 11.6 */
        yval = mweibull/2.0;                           
        kweibull = pow( xval, yval);

        printf("exp(gamma(1+2/mweibull))*(lamda/Ni) is %g\n",xval);
        printf("mweibull/2.0 is %g\n",yval);
        printf("The k parameter of the Weibull dist is %g\n",kweibull);

	printf("Generating non-zero impulses...\n\n"); fflush(stdout);

        numimpulses = 0;                        /* Initialize total number of impulses */
        currentsample = 0;                      /* Initialize current sample */
	while(currentsample < nsamples) 
        {
          uniformrv = drand48();                /* Sample from the [0,1) uniform dist */
          /*printf("Uniform random variable is %g\n",uniformrv);*/
          timeint = log(1.0/uniformrv)/lamda;   /* Interval between impulses in secs */
          #ifdef STATS
            ints[numimpulses] = timeint;
          #endif
          /*printf("Time interval is seconds is %g\n",timeint);*/
          sampleint = (int) (timeint*samprate); /* Interval between impulses in samples */
          /*printf("Time interval in samples is %d\n",sampleint);*/
          currentsample += sampleint;           /* Current sample in the output stream */
          if (currentsample <= nsamples)
          {
	    /*printf("Sample number of impulse is %d\n",currentsample);*/
             numimpulses += 1;                  /* Increment counter */
             uniformrv = drand48();
             xval = log(1.0/uniformrv)/kweibull;
             yval = 1.0/mweibull;
             /*printf("log(1/uniformrv)/kweibull is %g\n", xval);*/
             /*printf("1/mweibull is %g\n",yval);*/
             ampli = pow(log(1/uniformrv)/kweibull,1.0/mweibull); /* Amplitude of 
                                                                     the impulse */
             #ifdef STATS
               amps[numimpulses-1] = ampli;
             #endif
             /*printf("ampli is %g \n",ampli);*/
             theta = 2 * PI * drand48();                       /* Phase of the impulse */
             /*printf("theta is %g \n",theta);*/
             out[currentsample].r = ampli * cos(theta);
             out[currentsample].i = ampli * sin(theta);
             /*printf("Sample value is %g+i*%g \n\n",out[currentsample].r,out[currentsample].i);*/
          }
	}
        printf("Total number of non-zero impulses is %d\n\n",numimpulses);
	printf("done\n"); /* done generating pulses... */

        if(cmdOutfile->entered != 0)    /* If an output file is requested */
	{
          outfp = fopen(outfile, "w");		/* open output file */
	  printf("%d samples written to %s\n\n", nsamples, outfile);
          fwrite(out, sizeof(fcomplex), nsamples, outfp);
        }

	SaveSeed(lrand48());			/* save seed for next run */

        #ifdef STATS
          outampstats = fopen(ampstatfile, "w");            /* Open statistic file */
          fwrite(amps, sizeof(float), numimpulses, outampstats);
          outintstats = fopen(intstatfile, "w");            /* Open statistic file */
          fwrite(ints, sizeof(float), numimpulses, outintstats);
        #endif

}

