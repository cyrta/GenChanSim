/* Narrowband Interferenc model as described in 'Wideband HF Propagation, 
 * Narrowband Interference, and Atmospheric Noise Models for Link Performance 
 * Evaluation', Phil Bello, MTR 93B0000086 and 'Software Wide Band HF Simulation
 * User's Manual', Richard Coutts, MTR93B0000122
 *
 * r.coutts

   copyright (c) 1994 The MITRE Corporation Bedford, MA

   $Id: narrowband.c,v 2.2 1994/07/25 19:06:52 jjb Exp $
   $Log: narrowband.c,v $
 * Revision 2.2  1994/07/25  19:06:52  jjb
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

#include <math.h>
#include <stdio.h>
#include <string.h>
/* local includes */
#include <complex.h>
#include <iirfade.h>
#include <Cmdline.h>
#include <narrowband.h>
#include <seed.h>
#include <pathfile.h>

static char rcsid[] = "$Id: narrowband.c,v 2.2 1994/07/25 19:06:52 jjb Exp $";

void main (int argc, char **argv) {
	fcomplex	*queue;						/* output in 'double' format */
	float		dynRngDb;					/* input dynamic range (c.l.arg) */
	float		tT;							/* total time of experiment */
	float		arrivalRate;				/* number of interferers */
	float		avgLen;						/* avg length of interf (seconds) */
	float		avgNInterfs;				/* average number of interferers */
	char		outFile[100] = "";			/* file identifier name (c.l.arg) */
	char		fileId[100] = "nb";			/* user input file ID */
	char		interfFile[100] = "";		/* user input file ID */
	long		seed;						/* r.n.g. seed */
	float		sampleRate;					/* sampleRate (Hz) */
	float		maxInterfFreq;				/* max interf freq (normalized)*/
	int			q;							/* number of samples in q */
	int			qLen;						/* size of queue */
	INTERF		*interf;					/* linked list of interferers */
	char		*c;							/* ptr to '.' in 'fileId' */
	int			dataFile = 0;				/* flag to create .dat file */
	int			useInterfFile = 0;			/* flag to use interferer file */
	int			useSimulation = 0;			/* flag to use simulation */
	CmdInfo		*cmdSeed;					/* cmdline ptr */
	CmdInfo		*cmdMaxFreq;				/* cmdline ptr */
	float		wMax = 0.0;					/* max Doppler spread */
	char		*helpStr;					/* help string */
	float		bMax = 0.0;					/* max bandwidth */
	float		a = PARETOEXP;				/* pareto exponent value */

	/* print copyright notice */

	fprintf(stdout,"copyright (c) 1994 The MITRE Corporation Bedford, MA\n");

	/* get command line variables */
	CmdFlag("-intf Interferer_File", &useInterfFile);
	CmdFlag("-sim Simulation", &useSimulation);
	CmdString("-if Interferer_File", interfFile, 100, cmdMan);
	CmdFloat("-t Time_(seconds)", &tT, cmdMan);
	CmdFloat("-s Sample_Rate_(MHz)", &sampleRate, cmdMan);
	cmdMaxFreq = CmdFloat("-f Maximum_Interferer_Frequency_(MHz)",
		&maxInterfFreq, cmdOpt);
	CmdFloat("-d Dynamic_Range_(dB)", &dynRngDb, cmdMan);
	CmdFloat("-m Average_Number_of_Interferers", &avgNInterfs, cmdMan);
	CmdFloat("-i Interferers_per_Second", &arrivalRate, cmdMan);
	CmdFloat("-wmax Max_Doppler_Spread_(Hz)", &wMax, cmdDef);
	CmdFloat("-bMax Max_Bandwidth_(Hz)", &bMax, cmdDef);
	CmdFloat("-a Pareto_Exponent", &a, cmdDef);
	cmdSeed = CmdInt("-S R.N.G._Seed", &seed, cmdOpt);
	CmdString("-o Output_File_ID", fileId, 100, cmdDef);
	CmdFlag("-file Create_Sample_File", &dataFile);
	/* if 'interfFile' entered, make 'dynRngDb', 'ninterf', etc. insensitive */
	CmdDep(&useInterfFile, interfFile, cmdSensitive);
	CmdDep(&useSimulation, &dynRngDb, &arrivalRate, &avgNInterfs, &wMax, &bMax,
		&a, &maxInterfFreq, cmdSensitive);
	CmdDep(&useSimulation, &useInterfFile, cmdRadioBox);
	CmdHelp(helpStr = PathFile("HF/AddDist/narrowband/narrowband.hlp"));
	CmdArgs(argc, argv);
	CmdUnMan();
	free(helpStr);

	if(cmdMaxFreq->entered) {			/* if max interf freq entered... */
		if(maxInterfFreq > sampleRate) {	/* if user screwed up... */
			printf("Error from narrowband:  Maximum Interferer Frequency [%f]"
				" cannot exceed the sampling rate [%f]\n", maxInterfFreq, 
				sampleRate);
			exit(1);
		}
		/* normalize maxInterfFreq to the sampling rate */
		maxInterfFreq = maxInterfFreq/sampleRate;
	} else {							/* else, since max freq not entered...*/
		maxInterfFreq = 1.0;			/* set to sampling rate */
	}
		
	/* normalize frequencies to Hz */
	sampleRate *= ONE_MHZ;					/* sampling rate MHz -> Hz */

	/* normalize 'bMax' and 'wMax' to a 1.0Hz sampling rate */
	bMax /= sampleRate;		/* norm bandwidth */
	wMax /= sampleRate;		/* norm Doppler spread */

	/* test if 'bMax' and 'wMax' are too big for filter */
	if(bMax > BANDWIDTH || wMax > BANDWIDTH) {
		printf("Error from narrowband: Doppler spread or bandwidth too large.");
		printf("  Current filter in file '%s' defined as having passband of", 
			NBFILTBANK);
		printf(" %.1f(Hz) @ %.1f(Hz)\n", BANDWIDTH*sampleRate, sampleRate);
		exit(1);
	}

	/* check random seed */
	if(cmdSeed->entered) {					/* if seed entered... */
		srand48(seed);						/* set seed */
	} else {								/* if seed not entered... */
		srand48(GetSeed());					/* set seed */
	}

	/* strip '.' off fileId (if user input one) */
	if((c = strchr(fileId, '.')) != NULL) {		/* if '.' in 'fileId'... */
		*c = '\0';								/* remove '.' */
	}
	if(dataFile) {							/* if output '.dat' file... */
		strcpy(outFile, fileId);			/* copy fileId to outFile */
		strcat(outFile, ".dat");			/* add '.dat' to 'outFile' */
	}

	/* generate interferer statistics */
	if(useInterfFile) {						/* if input file entered... */
		/* read int test interferers */
		interf = ReadInterf(interfFile, sampleRate);/* get 'interf' from file */
	} else {								/* if NO input file entered... */
		/* get 'interf' from simulation */
		avgLen = avgNInterfs/arrivalRate;	/* interf length (seconds) */
		printf("Average Interferer Length: %.3f (Seconds)\n", avgLen);
		interf = GetInterfStats(tT, sampleRate, maxInterfFreq, dynRngDb, a,
			arrivalRate, bMax, wMax, avgLen);
	}
	WriteInterf(fileId, interf);				/* print linked list to file */
	BreakUpInterf(interf, sampleRate, MAXINTERFLEN); /* break up interferers */
	interf = SortInterf(interf);				/* sort interferers by time */
	/* output data to file if requested */
	queue = AllocData(interf, sampleRate, tT, &qLen);/* alloc for data[] */
	do {
		q = GetInterfData(interf, sampleRate, queue, qLen); /* get data */
		OutputData(outFile, queue, q, qLen);	/* output to file */
	} while(q != -1);							/* while still data */
	q = sampleRate*tT;							/* goto last sample */
	OutputData(outFile, queue, q, 0);			/* output and close file */
	printf(".done\n");
	SaveSeed(lrand48());						/* save seed */
}


/* Because interferers can get huge (a 10 second interferer @ 1 MHz @ 8 bytes
 * per sample results in 80 MB), interferers must be broken down into smaller
 * interferers.  This must be done while keeping track of the phase of the
 * interferer and maintaining the filter coefficients for the fading.  This 
 * function accomplishes this.
 * The function operates be traversing a linked list, taking interferers that
 * are to big, breaking them in two, adding a chunk onto the end of the list,
 * and continuing the traversal eventually into the chunk and perhaps breaking
 * it up again if it's too big.  
 * The algorithm would break up a list of three number to a maximum value of 10
 * as:
 *      25 >10  10  10  10  10  10
 *       8   8  >8   8   8   8   8
 *      12  12  12 >10  10  10  10
 *          15  15  15 >10  10  10
 *                   2   2  >2   2
 *                       5   5  >5
 */
void BreakUpInterf(
	INTERF	*in,							/* top of linked list of interfs */
	float	sampleRate,						/* sample rate (Hz) */
	int		maxSamples)						/* max allowable samples */
{
	INTERF	*newInterf;						/* new member of interferer list */
	INTERF	*in0;							/* top of linke list of interfs */
	float	len;							/* length of shortened interferer */
	int		nSamples;						/* # samples of shortened interf */

	in0 = in;								/* save top of linked list */
	while(in != NULL) {						/* while still in list... */
		if(in->nSamples > maxSamples) {		/* if interferer is too big... */
			newInterf = AddInterf(in, 0);	/* add interferer to list */
			InterfCopy(newInterf, in);		/* copy statistics into new interf*/
			len = maxSamples/sampleRate;	/* length of shortened interferer */
			nSamples = sampleRate * len;	/* # samples in shortened interf */
			newInterf->len = in->len - len;	/* length of new interferer */
			newInterf->nSamples = in->nSamples - nSamples;	/* " */
			in->len = len;					/* length of shortened interferer */
			in->nSamples = nSamples;		/* " */
			newInterf->time = in->time + len;	/* new interf start time */
		}
		in = in->next;						/* get next interferer */
	}
}


/* perform bubble sort of linked list in time (small -> large).  The function 
 * returns the top of the list which may change during the sort.
 */
INTERF *SortInterf(
	INTERF	*in0)							/* highest unsorted member */
{
	INTERF	*prev = NULL;					/* previous member */
	INTERF	*temp;							/* temporary ptr */
	INTERF	*in;							/* member of list */
	INTERF	*top;							/* top of list (returned) */

	if(in0 == NULL) return NULL;			/* return if list is empty */
	top = in0;								/* set top of list */
	while(in0->next != NULL) {				/* while still sorting... */
		in = in0;							/* sort from in0 down */
		while(in->next != NULL) {			/* while not last member... */
			if(in->time > in->next->time) {	/* if next member is larger... */
				temp = in->next;			/* rethread linked list */
				in->next = in->next->next;	/* " */
				temp->next = in;			/* " */
				in = temp;					/* " */
				if(prev == NULL) {			/* if top of list is sorted... */
					top = temp;				/* new top of linked list */
				} else {					/* else, if not top of list... */
					prev->next = temp;		/* rethread */
				}
			}
			prev = in;						/* save previous member */
			in = in->next;					/* get next member */
		}
		in0 = in0->next;					/* get next member */
	}
	return top;								/* return top of list */
}


/* copy contents of one interfer into another */
void InterfCopy(
	INTERF	*dest,							/* destination interf */
	INTERF	*src)							/* source interf */
{
	/* copy parameters from source to destination */
	dest->time = src->time;
	dest->amp = src->amp;
	dest->len = src->len;
	dest->nSamples = src->nSamples;
	dest->freq = src->freq;
	dest->phase = src->phase;
	dest->ds = src->ds;
	dest->bw = src->bw;
	dest->dsInt = src->dsInt;
	dest->bwInt = src->bwInt;
	dest->dsInitIIR = src->dsInitIIR;
	dest->bwInitIIR = src->bwInitIIR;
	
	/* share interferer segments filter memory so filter states are passed from
	 * from one segment to another */
	dest->dsInitIIR = src->dsInitIIR; /* copy IIR filter initial states */
	dest->bwInitIIR = src->bwInitIIR; /* copy IIR filter initial states */
	dest->dsInitFIR = src->dsInitFIR; /* copy FIR filter initial states */
	dest->bwInitFIR = src->bwInitFIR; /* copy FIR filter initial states */
}

/* print interferer stats to history (.hst) file */
void WriteInterf(char *fileId, INTERF *in) {
	char		fileName[100];	/* fileName */
	FILE		*fp;			/* output file ptr */
	int			i = 0;			/* interferer counter */

	strcpy(fileName, fileId);			/* copy 'fileId' to 'fileName' */
	strcat(fileName, ".hst");			/* tag '.dat' to end of fileId */

	if((fp = fopen(fileName, "w")) == NULL) {
		printf("ERROR from narrowband: output statistics file '%s' not opened\n"
			, fileName);
		exit(1);
	}
	printf("writing history file '%s'...", fileName); fflush(stdout);
	fprintf(fp,
	"Tstart    amp(dB)  duration   freq   phase(rad) DS(Hz)   BW(Hz)\n");
	fprintf(fp,
	"-------- --------- -------- ---------- ------- -------- --------\n");
	while(in != NULL) {							/* while not end of list... */
		fprintf(fp, "%8.5f ", in->time);
		fprintf(fp, "%9.3f ", 10 * log10(in->amp * in->amp * 0.5));	/* dB */
		fprintf(fp, "%8.5f ", in->len);
		fprintf(fp, "%10.8f ", in->freq/(2*PI));/* normalized (1.0=SampleRate)*/
		fprintf(fp, "%7.5f ", in->phase);
		fprintf(fp, "%8.1f ", in->ds*ONE_MHZ);
		fprintf(fp, "%8.1f ", in->bw*ONE_MHZ);
		fprintf(fp, "\n");
		i++;									/* counter interferers */
		in = in->next;							/* get next in list */
	}
	fclose(fp);									/* close output file */
	printf("%d interferer(s) documented\n", i);
}

/* get the statistics for each and every interferer in the experiment */
INTERF *GetInterfStats(
	float		tT,					/* total time of experiment */
	float		sampleRate,			/* sampleRate (Hz) */
	float		maxInterfFreq,		/* maximum interferer frequency (Hz) */
	float		dynRngDb,			/* dynamic range (db) */
	float		a,					/* Pareto exponent */
	float		ninterfs,			/* avg number of interferers per second */
	float		bMax,				/* max bandwidth */
	float		wMax,				/* max Doppler spread */
	float		avgLen)				/* avg interf length (seconds) */
{
	float		tI;					/* start time of ea interferer */
	INTERF		*in0 = NULL;		/* top of interferer list */
	INTERF		*in = NULL;			/* interferer list */
	float		rn;					/* random number */

	/* generate interferer statistics */
	printf("generating statistics..."); fflush(stdout);
	tI = 0.0;							/* init time */
	while(tI < tT) {					/* while < total time of experiment...*/
		in = AddInterf(in, 1);			/* add interf mem to list */
		if(in0 == NULL) in0 = in;		/* set top of list */
		in->freq = InterfFreq(maxInterfFreq);	/* calc freq of interferer */
		in->phase = InterfPhase();		/* calc phase of interferer */
		in->time = tI;					/* save time of interferer */
		in->amp = InterfAmp(dynRngDb, a);	/* get strength of intererer */
		in->len = InterfLen(avgLen);	/* duration of interferer */
		if(in->len > tT-tI) {			/* if interf will go to end of run... */
			in->len = tT-tI;			/* crop it */
		}
		in->nSamples = in->len*sampleRate;	/* # samples in interferer */
		/* calculate doppler spread */
		if(wMax == 0) {					/* if no Doppler spread... */
			in->ds = 0;					/* Doppler spread is zero Hz */
			in->dsInt = 0;				/* no interpolation */
		} else {						/* if there is Doppler spread... */
			rn = drand48();				/* uniform random number (0.0,1.0) */
			in->ds = rn*wMax;			/* ideal doppler spread */
			if(in->ds < WMIN) {			/* if Dpl spr is below min... */
				in->ds = WMIN;			/* don't want div zero error */
			}
			in->dsInt = InterpFactor(in->ds);	/* attainable interp'n factor */
			in->ds = AttainableFreq(in->dsInt);	/* set attainable doppler spr */
		}
		if(bMax == 0) {					/* if no bandwidth... */
			in->bw = 0;					/* bandwidth is zero Hz */
			in->bwInt = 0;				/* no interpolation */
		} else {						/* if there is bandwidth... */
			rn = drand48();				/* uniform random number (0.0,1.0) */
			in->bw = rn*bMax;			/* ideal doppler spread */
			if(in->bw < BMIN) in->bw = BMIN;	/* don't want div zero error */
			in->bwInt = InterpFactor(in->bw);	/* attainable interp'n factor */
			in->bw = AttainableFreq(in->bwInt);	/* set attainable bandwidth */
		}
		tI += RandPoisson(ninterfs);	/* get next interferer time */
	}
	printf("done\n"); 					/* print 'done' to screen */
	return in0;							/* return top of list */
}

/* determine the attainable Doppler spread or bandwidth according to the 
 * interpolation factor */
float AttainableFreq(int interp) {

	if(interp == 0) {				/* if no Doppler spread... */
		return 0.0;					/* set Doppler spread to zero */
	} else {						/* if there is a Doppler spread */
		return BANDWIDTH/interp;	/* set attainable doppler spr */
	}
}

/* find attainable interpolation factor */
int	InterpFactor(float spread) {
	float	desired;		/* desired interp'n factor */
	int		filt;			/* filter interp'n factor */
	int		lin;			/* linear interp'n factor */

	if(spread == 0.0) return 0;			/* if no spread, return no interp */
	desired = BANDWIDTH/spread;			/* desired interp'n factor */
	if(desired >= 48) {					/* if desired > max filt interp'n */
		filt = 64;						/* filter interp'n factor */
		lin = rint(desired/64);			/* linear interp'n factor */
	} else {							/* else, find interp'n (no linear)*/
		if(desired >= 24) filt = 32;
		else if(desired >= 12) filt = 16;
		else if(desired >= 6) filt = 8;
		else if(desired >= 3) filt = 4;
		else if(desired >= 1.5) filt = 2;
		else filt = 1;
		lin = 1;						/* linear interp'n factor */
	}
	return lin*filt;					/* return total interp'n factor */
}


/* get the statistics for each and every interferer from a file */
INTERF *ReadInterf(
	char		*fileName,			/* file name of .hst file */
	float		sampleRate)			/* sampling rate (Hz) */
{
	INTERF		*in0 = NULL;		/* top of interferer list */
	INTERF		*in = NULL;			/* interferer list */
	INTERF		*inlast;			/* previous interferer list */
	char		unused[100];		/* 1st 2 lines */
	int			rtn;				/* return from fscanf() */
	int			ninterfs = 0;		/* counter */
	FILE		*fp;				/* input file pointer */

	/* open file */
	if((fp = fopen(fileName, "r")) == NULL) {
		printf("Error from narrowband: input statistics '%s' not opened\n",
			fileName);
		exit(1);
	}
	printf("reading interferer file '%s'...", fileName);

	/* read lines of file until the '----' line is hit */
	do {
		fgets(unused, 100, fp);		/* read line but don't save */
	} while(strncmp(unused, "-----", 5) != 0);

	do {
		inlast = in;						/* save previous 'in' */
		in = AddInterf(in, 1);				/* add interf mem to list */
		if(in0 == NULL) in0 = in;			/* set top of list */
		rtn = fscanf(fp, "%f", &(in->time));/* read start time (sec) */
		fscanf(fp, "%f", &(in->amp));		/* read amplitude */
		in->amp = pow(10.0, in->amp/10.0);	/* dB->power */
		in->amp = sqrt(2.0*in->amp);		/* power->amplitude */
		fscanf(fp, "%f", &(in->len));		/* read length (sec) */
		in->nSamples = in->len*sampleRate;	/* generate 'nSamples' */
		fscanf(fp, "%f", &(in->freq));		/* read freq (1.0=SampleRate) */
		if(in->freq > 1.0) {
			printf("Error from narrowband: interferer frequency specified in "
				"file '%s' has normalized frequency > 1.0, e.g. [%f].  "
				"Frequency must be normalized to the sampling rate.\n",
				fileName, in->freq);
			exit(1);
		}
		in->freq *= 2*PI;					/* normalize to radians */
		fscanf(fp, "%f", &(in->phase));		/* read phase (radians) */
		fscanf(fp, "%f", &(in->ds));		/* read Doppler spread */
		in->ds = in->ds/ONE_MHZ;			/* normalize Doppler spread */
		in->dsInt = InterpFactor(in->ds);	/* attainable interp'n factor */
		in->ds = AttainableFreq(in->dsInt);	/* get attainable Dpl spread */
		fscanf(fp, "%f", &(in->bw));		/* read bandwidth */
		in->bw = in->bw/ONE_MHZ;			/* normalize bandwidth */
		in->bwInt = InterpFactor(in->bw);	/* attainable interp'n factor */
		in->bw = AttainableFreq(in->bwInt);	/* get attainable bandwidth */
		ninterfs++;							/* incr counter */
	} while(rtn != EOF);
	printf("%d interferer(s) read\n", ninterfs-1);
	if(inlast != NULL) inlast->next = NULL;	/* remove last element */
	return in0;
}

/* calculate the frequency of the interferer.  'max' is the maximimum interfer
 * frequency in Hz where 1.0 Hz is the sampling rate. */
float InterfFreq(float max) {

	return 2.0 * max * M_PI * drand48();	/* frequency (0.0->2*max*PI) */
}

/* calculate the phase of the interferer */
float InterfPhase() {

	return 2.0*M_PI*drand48();		/* generate phase (0.0->2PI) */
}


/* amplitude of interferer (Power).  Derived from the equ'n:
 *
 *                      d
 *      E(u) = -------------------
 *             [1+u*(d^a-1)]^(1/a)
 */
float InterfAmp(
	float	dynRngDb,				/* dynamic range (dB) */
	float	a)						/* Pareto exponent */
{
	float	u;						/* uniform random number (0.0, 1.0) */
	float	den;					/* denominator of exponent */
	float	d;						/* dynamic range (power) */
	float	power;					/* signal strength (power) */

	d = pow(10.0, dynRngDb/10.0);	/* convert dynamic range (dB->power) */
	u = drand48();					/* generate uniform r.n. */
	/* generate denominator of exponent */
	den = pow(1.0+u*(pow(d, a)-1.0), 1.0/a);
	power = d/den;					/* calc signal strength (power) */
	return sqrt(2.0 * power);		/* convert to amplitude and return */
}


/* length of interferer (seconds) */
float InterfLen(float avgLen) {
	double	u;			/* uniform random number (0.0, 1.0) */

	do {
		u = drand48();		/* get uniform r.n. (0,1) */
	} while(u <= 0.0);		/* if not greater than zero, get another */
	return -avgLen*log(u);	/* calc interferer length */
}

/* alloc the memory required for the output data array as dictated by the 
 * length of the interferers or the time between them
 */
fcomplex *AllocData(
	INTERF		*in,			/* interferer list */
	float		freq,			/* sampling freq of data */
	float		tT,				/* total time of experiment */
	int			*nSamples)		/* number of samples */
{
	float		prevTime = 0.0;	/* previous interferer elem time */
	float		timeGap = 0;	/* gap between inteferers (seconds) */
	fcomplex	*out;			/* data memory location allocated */
	float		maxLen = 0.0;	/* largest value of 'len' in list */

	/* determine the size of the output array */
	while(in != NULL) {
		/* get largest time gap betw start of interferers */
		if(timeGap < in->time - prevTime) timeGap = in->time - prevTime;
		/* get longest 'len' of interferer */
		if(maxLen < in->len) maxLen = in->len;
		prevTime = in->time;
		in = in->next;
	}
	if(timeGap < tT - prevTime) timeGap = tT - prevTime;	/* gap to end */
	if(timeGap > maxLen) *nSamples = timeGap*freq;	/* gap dictates q size */
	else *nSamples = maxLen*freq; 					/* interf dictates q size */
	*nSamples += 10;								/* add a little slop */
	printf("allocating queue memory...", *nSamples); fflush(stdout);
	out = (fcomplex *)calloc(*nSamples, sizeof(fcomplex));
	if(out == NULL) {
		printf("Error from narrowband: not enough memory for run.  Tried to ");
		printf("allocate for %d samples\n", *nSamples);
		exit(1);
	}
	printf("done\n");
	return out;
}

/* generate a rand poisson process */
float RandPoisson(float lamda) {
	float   u;
	float   f;
 
	do {
		u = drand48();		/* get uniform r.n. (0,1) */
	} while(u <= 0.0);		/* if not greater than zero, get another */
	f = -log(u)/lamda;		/* calculate the time to next arrival */
	return f;
}

/* add member to interfer list.  If 'newInterf' != 0, then it is a new interfrr
 * being created and memory for delay buffers is allocated.  If it is not new, 
 * it is an existing interfer being split because it is too big for memory
 */
INTERF *AddInterf(
	INTERF	*in,								/* member of interferer list */
	int		newInterf)							/* flag for new interferer */
{
	INTERF	*out;								/* new memory */
 
	out = (INTERF *)malloc(sizeof(INTERF));		/* alloc memory */
	if(in != NULL) {							/* if member exists... */
		while(in->next != NULL) in = in->next;	/* goto bottom on list */
		in->next = out;							/* add 'out' to linked list */
	}
	if(newInterf) {									/* if new interferer... */
		/* allocate memory for IIR initial conditions */
		out->dsInitIIR = AllocInitIIR(NIIRDELAYS);
		out->bwInitIIR = AllocInitIIR(NIIRDELAYS);
		/* alloc array of Doppler spread FIR filter initial conditions */
		out->dsInitFIR = (fcomplex **)calloc(NINTERPFILTERS,sizeof(fcomplex *));
		out->dsInitFIR[0] = (fcomplex *)calloc(NPTSFILTER1, sizeof(fcomplex));
		out->dsInitFIR[1] = (fcomplex *)calloc(NPTSFILTER2, sizeof(fcomplex));
		out->dsInitFIR[2] = (fcomplex *)calloc(NPTSFILTER3, sizeof(fcomplex));
		out->dsInitFIR[3] = (fcomplex *)calloc(NPTSFILTER4, sizeof(fcomplex));
		out->dsInitFIR[4] = (fcomplex *)calloc(NPTSFILTER5, sizeof(fcomplex));
		out->dsInitFIR[5] = (fcomplex *)calloc(NPTSFILTER6, sizeof(fcomplex));
		/* alloc array of bandwidth FIR filter initial conditions. */
		out->bwInitFIR = (fcomplex **)calloc(NINTERPFILTERS,sizeof(fcomplex *));
		out->bwInitFIR[0] = (fcomplex *)calloc(NPTSFILTER1, sizeof(fcomplex));
		out->bwInitFIR[1] = (fcomplex *)calloc(NPTSFILTER2, sizeof(fcomplex));
		out->bwInitFIR[2] = (fcomplex *)calloc(NPTSFILTER3, sizeof(fcomplex));
		out->bwInitFIR[3] = (fcomplex *)calloc(NPTSFILTER4, sizeof(fcomplex));
		out->bwInitFIR[4] = (fcomplex *)calloc(NPTSFILTER5, sizeof(fcomplex));
		out->bwInitFIR[5] = (fcomplex *)calloc(NPTSFILTER6, sizeof(fcomplex));
		WarmUpFilters(out);		/* warm up filter to eliminate transient time */
	}
	out->next = (INTERF *)NULL;					/* terminate list */
	return out;									/* return list elem */
}

/* 'warm-up' the filters to get rid of the transcient time.
 */
void WarmUpFilters(
	INTERF	*in)			/* interferer */
{
	static fcomplex data[NFIRWARMUPS * INTERP];

	printf("."); fflush(stdout);	/* print dot for ea interferer warmed up */
	WarmUpIIR(in->dsInitIIR);					/* warm-up Doppler spr IIR */
	/* warm up interpolation filters */
	iirfade(data, INTERP*NFIRWARMUPS, INTERP, in->dsInitIIR, in->dsInitFIR);
	WarmUpIIR(in->bwInitIIR);					/* warm-up bandwidth IIR */
	/* warm up interpolation filters */
	iirfade(data, INTERP*NFIRWARMUPS, INTERP, in->bwInitIIR, in->bwInitFIR);
}

/* 'warm-up' an IIR filter to get rid of the transcient time */
void WarmUpIIR(
	DELAY_4		**filter)					/* IIR filter bank */
{
	/* gaussian samples to warm-up filter */
	static fcomplex samples[NJUMPINGJACKS];

	cgauss48(samples, (int)NJUMPINGJACKS);

	iirfilter(samples, NJUMPINGJACKS, filter);
}

/* The following function is not used.  It was written to knock a few seconds
 * off the time it takes to warm up the interpolation filters.  However, it
 * is a little buggy, as the algorithm proved to be more complex than 
 * anticipated.  It is provided in the interest of future development.
 */
/* 'warm-up' an FIR filter to get rid of the transcient time.  The function 
 * operates by first passing through enough points to get rid of the trasient
 * time, and then passing enough points to get valid points in order to warm
 * up the next filter.
 * Note that the IIR filter must be warm-up prior to this call.
 */
void WarmUpFIR(
	DELAY_4		*delay[],		/* filter delays */
	fcomplex	*firInit[])		/* fir interpolation filter initial states */
{
	/* gaussian data to warm filter - allocated 2x largest fir */
	static fcomplex data[NFIRWARMUPS * INTERP];
	fcomplex *d = &data[0];					/* pointer inside data[] */
	int	datadim = NFIRWARMUPS * INTERP;		/* npts in d[] */
	int		ngood;							/* # of good values in data[] */
	int		njunk;							/* # of bad (transient) pts in d[]*/
	static int firstcall = 1;				/* flag for 1st call to function */
	static float interp1[NPTSFILTER1];		/* 1st filter coeff */
	static float interp2[NPTSFILTER2];		/* 2nd filter coeff */
	static float interp3[NPTSFILTER3];		/* 3rd filter coeff */
	static float interp4[NPTSFILTER4];		/* 4th filter coeff */
	static float interp5[NPTSFILTER5];		/* 5th filter coeff */
	static float interp6[NPTSFILTER6];		/* 6th filter coeff */

	cgauss48(data, NFIRWARMUPS);			/* generate complex gaussian */

	iirfilter(data, NFIRWARMUPS, delay);	/* do iir filtering */

	if(firstcall) {			/* if first call to function... */
		firstcall = 0;		/* only enter this statement once */
		readfilterfile(INTERPFILTER1, interp1, NPTSFILTER1);
		readfilterfile(INTERPFILTER2, interp2, NPTSFILTER2);
		readfilterfile(INTERPFILTER3, interp3, NPTSFILTER3);
		readfilterfile(INTERPFILTER4, interp4, NPTSFILTER4);
		readfilterfile(INTERPFILTER5, interp5, NPTSFILTER5);
		readfilterfile(INTERPFILTER6, interp6, NPTSFILTER6);
	}
	/* Kindof interpolate - do just enough sample to warm filters.
	 * do zero insertion and filtering for just enough samples to warm fir.
	 * In hindsight, some arrays would have been nice here to make a simple 
	 * function instead of pasting code over and over and over. */
	{	/* warm up 1st stage of interpolation */
		ngood = NFIRWARMUPS;	/* # good pts in d[] */
		zerofilter(d, datadim, ngood, interp1, NPTSFILTER1, firInit[0]);
	}
	{	/* warm up 2nd stage of interpolation */
		njunk = 2 * NPTSFILTER1;
		ngood = 2 * ngood - njunk;
		d = &d[njunk];			/* move array ptr to skip over transient pts */
		datadim -= njunk;		/* adjust dimension of d[] */
		zerofilter(d, datadim, ngood, interp2, NPTSFILTER2, firInit[1]);
	}
	{	/* warm up 3rd stage of interpolation */
		njunk = 2 * NPTSFILTER2;
		ngood = 2 * ngood - njunk;
		d = &d[njunk];			/* move array ptr to skip over transient pts */
		datadim -= njunk;		/* adjust dimension of d[] */
		zerofilter(d, datadim, ngood, interp3, NPTSFILTER3, firInit[2]);
	}
	{	/* warm up 4th stage of interpolation */
		njunk = 2 * NPTSFILTER2;
		ngood = 2 * ngood - njunk;
		d = &d[njunk];			/* move array ptr to skip over transient pts */
		datadim -= njunk;		/* adjust dimension of d[] */
		zerofilter(d, datadim, ngood, interp4, NPTSFILTER4, firInit[3]);
	}
	{	/* warm up 5th stage of interpolation */
		njunk = 2 * NPTSFILTER3;
		ngood = 2 * ngood - njunk;
		d = &d[njunk];			/* move array ptr to skip over transient pts */
		datadim -= njunk;		/* adjust dimension of d[] */
		zerofilter(d, datadim, ngood, interp5, NPTSFILTER5, firInit[4]);
	}
	{	/* warm up 6th stage of interpolation */
		njunk = 2 * NPTSFILTER4;
		ngood = 2 * ngood - njunk;
		d = &d[njunk];			/* move array ptr to skip over transient pts */
		datadim -= njunk;		/* adjust dimension of d[] */
		zerofilter(d, datadim, ngood, interp6, NPTSFILTER6, firInit[5]);
	}
}

/* allocate memory for initial conditions of iir filter */
DELAY_4 **AllocInitIIR(
	int		nDelays)			/* number of delays in iir filter */
{
	int			i;				/* index */
	DELAY_4		**out;			/* output memory (returned) */

	out = (DELAY_4 **)malloc(sizeof(DELAY_4 *) * nDelays);
	for(i = 0; i < nDelays; i++) {
		out[i] = (DELAY_4 *)calloc(sizeof(DELAY_4), nDelays);
	}
	return out;
}


/* generate the data associated with ea interferer.  This function generates
 * data from interferer start point to interferer start point so that any
 * overlap will be output in subsequent calls.  The return is the index to the
 * last sample processed in an infinite array.
 */
int GetInterfData(
	INTERF		*interf,			/* top of the interferers list */
	int			sampleRate,			/* sample rate (Hz) */
	fcomplex	*queue,				/* block of data returned */
	int			qLen)				/* size of queue */
{
	static int ninterfs = 0;		/* number of interferers processed */
	static INTERF *in;				/* interferers list member */
	static int firstcall = YES;		/* 1st call to function flag */
	static fcomplex *signal;		/* interferer signal */
	static fcomplex *fading;		/* fading of interferer signal */
	static float tlast;				/* previous interferer time */
	int		arraysize;				/* number of samples in interferer */
	float	maxLen;					/* largest value of 'len' in list */
	
	if(firstcall) {					/* if first call to function... */
		firstcall = NO;				/* reset flag */
		in = interf;				/* set 'in' to top of list */
		maxLen = InterfMaxLen(interf);	/* get largest 'len' in list */
		/* allocate memory by the largest interferer */
		arraysize = maxLen*sampleRate;	/* number of elements needed in array */
		printf("allocating signal and fading memory..."); fflush(stdout);
		signal = (fcomplex *)calloc(arraysize, sizeof(fcomplex));
		if(signal == NULL) {
			printf(
			"OUT OF MEMORY while allocating for sample length %.3f seconds.\n",
				(float)arraysize/sampleRate);
			exit(1);
		}
		fading = (fcomplex *)calloc(arraysize, sizeof(fcomplex));
		if(signal == NULL) {
			printf(
			"OUT OF MEMORY while allocating for fading length %.3f seconds.\n",
				(float)arraysize/sampleRate);
			exit(1);
		}
		printf("done\n");
		printf("generating interferers"); fflush(stdout);
	} else {						/* else (not the 1st call)... */
		in = in->next;				/* get next list elem */
	}
	if(in == NULL) {				/* if end of the list... */
		return -1;					/* return no data */
	}

	/* output some dots to captivate the user */
	if(++ninterfs%10 == 0) {		/* if # interfs processed is mult of 10...*/
		printf("*"); fflush(stdout);	/* print # interfs processed */
	} else {						/* if NOT a multiple... */
		printf("."); fflush(stdout);/* print a dot */
	}

	/* generate next interferer */
	OneInterferer(in, signal);						/* generate interferer */

	/* doppler spread fading */
	if(in->dsInt != 0) {							/* if interpolation... */
		/* Doppler spread fading */
		iirfade(fading, in->nSamples, in->dsInt, in->dsInitIIR, in->dsInitFIR);
		MultComplex(signal, fading, in->nSamples);	/* mult signal by fading */
	}

	/* bandwidth spread */
	if(in->bwInt != 0) {							/* if interpolation... */
		/* bandwidth fading */
		iirfade(fading, in->nSamples, in->bwInt, in->bwInitIIR, in->bwInitFIR);
		MultComplex(signal, fading, in->nSamples);	/* mult signal by fading */
	}

	/* integrate signal to output */
	IntegrateInterf(in->time*sampleRate, signal, in->nSamples, queue, qLen);

	tlast = in->time;							/* update last interf output */
	return tlast*sampleRate;					/* last sample processed */
}

/* return the largest value of 'len' in list */
float InterfMaxLen(INTERF *in) {
	float		max = 0.0;						/* return value */

	while(in != NULL) {							/* loop 'til end of list */
		if(max < in->len) max = in->len;		/* assign in largest so far */
		in = in->next;							/* get next element in list */
	}
	return max;									/* return max length 'len' */
}

/* generate one interferer and put it in 'data'.  The interferer is generated
 * by spinning a phasor with a 2x2 rotational matrix.  The 1st point
 * generated is cos(phase) + i*sin(phase).
 *
 * rotational matrix:
 *                     | cos(freq)  -sin(freq) |
 *                 A = |                       |
 *                     | sin(freq)   cos(freq) |
 *
 * so,  data[i] = A * data[i-1]
 */
void OneInterferer(
	INTERF		*in,				/* member of interference list */
	fcomplex	*data)				/* complex output of interferer */
{
	double	sinDelta;				/* -A12, A21 */
	double	cosDelta;				/* A11, A22 */
	double	sinNew;					/* imag(phasor) */
	double	cosNew;					/* real(phasor) */
	double	temp;					/* temp storage of imag(phasor) */
	int		j;						/* point index */

	/* time increment */
	cosDelta = cos(in->freq);				/* A11, A22 */
	sinDelta = sin(in->freq);				/* A21, -A12 */
	cosNew = cos(in->phase);				/* starting real part of phasor */
	sinNew = sin(in->phase);				/* starting imag part of phasor */
	data[0].r = in->amp * cosNew;			/* real part of phase */
	data[0].i = in->amp * sinNew;			/* image part of phase */
	for (j = 1; j < in->nSamples; j++) {	/* starting at 2nd point... */
		temp = sinNew * cosDelta + cosNew * sinDelta;
		cosNew = cosNew * cosDelta - sinNew * sinDelta;
		sinNew = temp;
		data[j].i = in->amp*sinNew;			/* imag */
		data[j].r = in->amp*cosNew;			/* real */
	}
}
