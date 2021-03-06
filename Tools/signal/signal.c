/* generate test input signals

   copyright (c) 1994 The MITRE Corporation Bedford, MA

   $Id: signal.c,v 2.4 1994/08/26 14:02:23 jjb Exp $
   $Log: signal.c,v $
 * Revision 2.4  1994/08/26  14:02:23  jjb
 * fixed bug when ask for single sample. floating point round off
 * caused variable loop to be set to -1 instead of zero.
 *
 * Revision 2.3  1994/08/25  13:00:59  jjb
 * fixed error in sine wave generation. real and imag were reversed.
 * Also when creating large files, the software continued writing until
 * disk was full.
 *
 * Revision 2.2  1994/07/25  19:34:01  jjb
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
 */

#include <stdio.h>
/* local includes */
#include <complex.h>
#include <Cmdline.h>
#include <pathfile.h>
#include <math.h>

#define	OUTBUFFER	100000
#define	PI		3.1415927

extern void cgauss48(fcomplex [], int);

static char rcsid[] = "$Id: signal.c,v 2.4 1994/08/26 14:02:23 jjb Exp $";

void main(int argc, char **argv) {
	char		outfile[100];		/* output file name */
	float		srate = 1.0;		/* samples rate (1MHz) */
	int			nsamples;			/* number of output samples */
	fcomplex	spike = {1.0, 1.0};	/* signal spike */
	fcomplex	zero = {0.0, 0.0};	/* signal zero */
	fcomplex	block[OUTBUFFER];	/* block of output */
	int			i;					/* index */
	int			j = 0;				/* index */
	float		period = 0;				/* period (mS) */
        CmdInfo         *cmdPeriod;                     /* information of period parameter */
	float		pulselen = 0;		/* length of pulse (uS) */
	float		seconds;			/* length of output (seconds) */
	int			periodsamples;		/* number of samples in period */
	int			pulsesamples;		/* number of samples in pulse */
	int			impulse = 0;		/* impulse flag */
	int			pulse = 0;			/* pulse flag */
	int			sinewave = 0;		/* sinewave flag */
	int			ones = 0;			/* ones flag */
	int			gauss = 0;			/* gaussian flag */
	int			loop;				/* number of loops */
	char		*helpStr;			/* help string */
	double		angle;				/* angle of sine wave */
	double		incr;				/* increment of phase for ea sample */
	float		freq;				/* freq of sine wave */
	FILE		*outfp;				/* output file pointer */

	/* print copyright notice */

	fprintf(stdout,"copyright (c) 1994 The MITRE Corporation Bedford, MA\n");

	/* get command line variables */
	CmdFloat("-f Sample_Rate_(MHz)", &srate, cmdDef);
	CmdFloat("-t Time_(seconds)", &seconds, cmdMan);
	CmdFile("-o Output_File", outfile, 100, cmdMan);
	CmdFlag("-i Impulse", &impulse);
	CmdFlag("-s Pulse", &pulse);
	CmdFlag("-g Gaussian", &gauss);
	CmdFlag("-w Sine_Wave", &sinewave);
	CmdFlag("-ones Ones (I/Q=1)", &ones);
	CmdFloat("-l Pulse_Length_(uS)", &pulselen, cmdMan);
        cmdPeriod = CmdFloat("-p Period_(mS)", &period, cmdOpt);
/*	CmdFloat("-p Period_(mS)", &period, cmdOpt);                 */
	CmdFloat("-freq Frequency(kHz)\n", &freq, cmdMan);
	CmdDep(&pulse, &pulselen, cmdSensitive);
        CmdDep(&impulse, &period, cmdSensitive);
	CmdDep(&impulse, &pulse, &ones, &sinewave, &gauss, cmdRadioBox);
	CmdDep(&sinewave, &freq, cmdSensitive);
	CmdHelp(helpStr = PathFile("Tools/signal/signal.hlp"));
	CmdArgs(argc, argv);
	CmdUnMan();
	free(helpStr);

        if(!cmdPeriod->entered) {               /* if no period entered... */
                period = seconds*1e3;           /* set it equal to the time */
        }

	/* simple conversions */
	srate *= 1000000;				/* Samples rate MHz -> Hz */
	freq *= 1000;					/* kHz -> Hz */
	period *= 1e-3;					/* period (mS -> seconds) */
	pulselen *= 1e-6;				/* pulse length (uS -> seconds) */
	nsamples = seconds*srate;		/* total number of samples */
	pulsesamples = pulselen*srate;	/* pulse length (seconds->samples) */
	periodsamples = period*srate;	/* # samples in period */
	
	/* open output file */
	if((outfp = fopen(outfile, "w")) == NULL) {
		printf("Error from signal:  output file '%s' not opened\n", outfile);
		exit(1);
	}
	printf("writing %d samples to '%s'...", nsamples, outfile); fflush(stdout);
	if(impulse) {
		while(j < nsamples) {
			fwrite(&spike, sizeof(fcomplex), 1, outfp);
			j++;					/* incr output index */
			/* find out how many zeros to write */
			if(periodsamples-1 < nsamples-j) {	/* if NOT near end of output..*/
				loop = periodsamples-1;
			} else {							/* else near end of output...*/
				loop = nsamples-j;
			}
			if ( loop < 0 )
			  loop = 0;
			for(i = 0; i < loop; i++) {
				fwrite(&zero, sizeof(fcomplex), 1, outfp);
			}
			j += loop;				/* incr output index */
		}
	} else if(pulse) {
		while(j < nsamples) {
			/* find out how many ones to write */
			if(pulsesamples < nsamples-j) {		/* if NOT near end of output..*/
				loop = pulsesamples;			/* output pulse samples */
			} else {							/* else near end of output...*/
				loop = nsamples-j;				/* output pulse to end */
			}
			if ( loop < 0 )
			  loop = 0;
			for(i = 0; i < loop; i++) {
				fwrite(&spike, sizeof(fcomplex), 1, outfp);
			}
			j += loop;				/* incr output index */
			/* find out how many zeros to write */
			if(periodsamples-pulsesamples < nsamples-j) {	/* if NOT end... */
				loop = periodsamples-pulsesamples;	/* put all zeros */
			} else {								/* else near end... */
				loop = nsamples-j;					/* put zeros to end */
			}
			if ( loop < 0 )
			  loop = 0;
			for(i = 0; i < loop; i++) {
				fwrite(&zero, sizeof(fcomplex), 1, outfp);
			}
			j += loop;				/* incr output index */
		}
	} else if(sinewave) {
		periodsamples = OUTBUFFER;
		j = 0;
		angle = 0;
		incr = 2 * PI * freq / srate;
		while(j < nsamples) {
			if(periodsamples > nsamples - j) {	/* if near end of output...*/
				periodsamples = nsamples - j;
			}
			for(i = 0; i < periodsamples; i++) {
				block[i].r = cos(angle);
				block[i].i = sin(angle);
				angle += incr;
			}
			fwrite(&block[0], sizeof(fcomplex), periodsamples, outfp);
			j += periodsamples;				/* incr output index */
		}
	} else if(ones) {
		periodsamples = OUTBUFFER;
		for(j = 0; j < periodsamples; j++) {
			block[j].r = 1;				/* Since our complex filters are */
			block[j].i = 1;				/* actually implemented as two realy */
		}						/* filters, set imaginary channel to 1 */
		j = 0;
		while(j < nsamples) {
			if(periodsamples > nsamples - j) {	/* if near end of output...*/
				periodsamples = nsamples - j;
			}
			fwrite(&block[0], sizeof(fcomplex), periodsamples, outfp);
			j += periodsamples;				/* incr output index */
		}
	} else if(gauss) {
		periodsamples = OUTBUFFER;
		j = 0;
		while(j < nsamples) {
			if(periodsamples > nsamples - j) {	/* if near end of output...*/
				periodsamples = nsamples - j;
			}
			cgauss48(&block[0], periodsamples);
			fwrite(&block[0], sizeof(fcomplex), periodsamples, outfp);
			j += periodsamples;				/* incr output index */
		}
	}
	fclose(outfp);
	printf("done\n");
}
