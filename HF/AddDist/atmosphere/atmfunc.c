/* functions.c supports atmosphere.c
 * r.coutts

   copyright (c) 1994 The MITRE Corporation Bedford, MA

   $Id: atmfunc.c,v 2.2 1994/07/25 18:03:14 jjb Exp $
   $Log: atmfunc.c,v $
 * Revision 2.2  1994/07/25  18:03:14  jjb
 * Added Copyright notices
 *

 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <malloc.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <sys/uio.h>
#include <sys/mtio.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <atmosphere.h>
#include <complex.h>
#define	BUFFSIZE	512	/* buffer size */

static char rcsid[] = "$Id: atmfunc.c,v 2.2 1994/07/25 18:03:14 jjb Exp $";

static float C;		/* global value of c for integral function */

/* functions (system can't find) */
double drand48();	/* random number generator (not in math.h) */

/* add member to stroke queue */
STROKE *AddStroke(STROKE *s) {
	STROKE *out;	/* new memory */

	out = (STROKE *)malloc(sizeof(STROKE));
	if(s != NULL) s->next = out;
	out->next = (STROKE *)NULL;
	return(out);
}


/* sort the strokes queue by time */
void sortstrokes(STROKE *s0) {
	STROKE		*s;					/* stroke inside queue */
	STROKE		*smin = NULL;		/* min stroke inside queue */
	STROKE		*above;				/* member above min member */
	STROKE		*prev;				/* previous member */

	while(s0 != NULL && s0->next != NULL) { /* s0 is the largest SORTED value */
		above = prev = s0; 			/* save previous member */
		s = s0->next;				/* move down one and start search */
		if(s != NULL) {				/* init min search */
			smin = s;				/* init sort */
			s = s->next;
		}
		while(s != NULL) {
			if(s->time < smin->time) {
				smin = s;			/* save member to be sorted (min) */
				above = prev;		/* save member above one to be sorted */
			}
			prev = s; 				/* save previous member */
			s = s->next;
		}
		/* swap min value with current location */
		above->next = smin->next;	/* skip the member sorted */
		smin->next = s0->next;		/* */
		s0->next = smin;			/* point largest sorted value to new sort */
		s0 = s0->next;				/* update largest sorted value */
	}
}


/* print out stroke queue */
void printstrokes(STROKE *s0) {

	printf(" f   s  seconds length    rms\n");
	printf("--- --- ------- ------- -------\n");
	while(s0 != NULL) {
		printf("%3d %3d %7.4f %7.4f %7.4f\n", s0->flash, s0->stroke, s0->time,
			s0->duration, s0->rms);
		s0 = s0->next;
	}
}


/* determine stroke strength using power-rayleigh statistics */
float StrokeRms(float c) {
	static float area;	/* value returned by integral */
	extern float C;		/* external value of c for f(c) */
	static float fc;	/* f(c) integral in phil's paper */
	float	out;		/* output of this function (rms value) */
	float	rn;			/* uniform random number (0,1) */
	static float lastc = -1; /* last value of c to function */

	if(c != lastc) {
		lastc = c;
		C = c;
		area = qromb(&f, 0.0, INF);
		fc = sqrt(2*area);
	}
	rn = drand48();
	out = pow(-log(rn), 1/c);
	out /= fc;
	return(out);
}

/* function to integrate in StrokeRms() */
float f(float x) {
	extern float	C;
	float	out;

	out = x*exp(-pow(x, C));
	return(out);
}


/* blank complex array */
void fcomplexclear(fcomplex out[], int n) {
	int		i;

	for(i = 0; i < n; i++) {
		out[i].r = 0.0;
		out[i].i = 0.0;
	}
}


/* print a symbol every time a flash is created */
void PrintFlash(float number) {
	static int firstcall = YES;
	char string[100];
	int			rn;		/* random number */
	static int	n = 1;	/* number of ticks */
	int			i;		/* index */
	#define NRAYS 5			/* max number of rays in flash */
	#define	LASTCHAR	75	/* char loc in string[] of last char */
	static struct {int loc; char sym;} ray[NRAYS];

	if(firstcall == YES) {
		firstcall = NO;
		ray[0].loc = 40;
		ray[0].sym = '/';
	}
	for(i = 0; i < n; i++) {
		rn = rand()%10;
		if(rn%5 == 0 && n < NRAYS) {
			/* add right */
			ray[n].loc = ray[i].loc;
			ray[n++].sym = ray[i].sym;
		} else if(rn%5 == 1 && n > 1) {
			/* delete */
			n--;
		} 
		if(rn%3 == 0 || (ray[i].loc == 0 && ray[i].sym == '/') ||
		   (ray[i].loc == LASTCHAR-1 && ray[i].sym == '\\'))
		{
			if(ray[i].sym == '/') ray[i].sym = '\\';
			else ray[i].sym = '/';
		} else { /* same dir */
			if(ray[i].sym == '\\') ray[i].loc++;
			else ray[i].loc--;
		}
	}
	printf("%5.2f", number);							/* print seconds */

	for(i = 0; i < LASTCHAR; i++) string[i] = ' ';		/* blank string */
	string[i++] = '\n';									/* terminate string */
	string[i] = NULL;									/* terminate string */

	for(i = 0; i < n; i++) {
		if(ray[i].loc >= 0 && ray[i].loc < LASTCHAR) {
			string[ray[i].loc] = ray[i].sym;
		}
	}
	printf(string);
}

/* return the random time duration of a stroke (seconds).
 * value is uniform random number in range (10.0(msec), 1000.0(msec))
 * rcoutts
 */
float StrokeDuration(float min, float max) {
	float	out;

	/* get uniform random number (0,1) */
	out = drand48();
	/* get output (min(sec), max(sec)) */
	out *= max-min;
	out += min;
	return(out);
}

/* scale a complex array by scalar
 */
void ScaleComplex(float m, fcomplex a[], int n) {
	int		i;

	for(i = 0; i < n; i++) {
		a[i].r *= m;
		a[i].i *= m;
	}
}

/* determine random number of strokes in flash implicitly */
int	NumStrokes() {
#define	NSTROKEBINS	15
	static int strokes[NSTROKEBINS] =
		{472, 242, 152, 197, 273, 108, 80, 85, 28, 70, 32, 22, 15, 14, 18};
									/* histogram a strokes/flash */
	static int cdf[NSTROKEBINS];	/* unnormalized CDF of strokes[] */
	static firstcall = YES;			/* first call flag */
	int		rn;						/* random number */
	int		bin = 0;				/* strokes index */

	if(firstcall) {
		firstcall = NO;
		/* create CDF */
		cdf[0] = strokes[0];
		for(bin = 1; bin < NSTROKEBINS; bin++) {
			cdf[bin] = cdf[bin-1]+strokes[bin];
		}
	}
	/* get uniform random number and scale by total number of strokes */
	rn = drand48()*cdf[NSTROKEBINS-1]+1;
	/* find which bin rn falls into for CDF */
	bin = 0;
	while(rn > cdf[bin]) bin++;
	return(bin+1);
}

/* determine time interval (seconds) for next stroke implicitly*/
float NextStroke() {
#undef	NSTROKEBINS
#define	NSTROKEBINS	26
	static int strokes[NSTROKEBINS] =
		{5, 107, 200, 214, 226, 184, 138, 95, 73, 63,
		 38, 34, 31, 15, 20, 11, 5, 7, 7, 4, 2, 2, 1, 1, 1, 1};
									/* histogram a strokes/flash */
	static int cdf[NSTROKEBINS];	/* unnormalized CDF of strokes[] */
	static firstcall = YES;			/* first call flag */
	int		rn;						/* random number */
	int		bin = 0;				/* strokes index */
	float	seconds;				/* output in seconds */

	if(firstcall) {
		firstcall = NO;
		/* create CDF */
		cdf[0] = strokes[0];
		for(bin = 1; bin < NSTROKEBINS; bin++) {
			cdf[bin] = cdf[bin-1]+strokes[bin];
		}
	}
	/* get uniform random number and scale by total number of strokes */
	rn = drand48()*cdf[NSTROKEBINS-1]+1;
	/* find which bin rn falls into for CDF */
	bin = 0;
	while(rn > cdf[bin]) bin++;
	/* each bin is 10.0 msec wide, so return center of bin in seconds */
	seconds = ((bin+1)*10.0-5.0)*1.0e-3;
	return(seconds);
}

/* add stroke to the output queue */
void IntegrateStroke(
	int sumloc,					/* sample location in infinite array */
	fcomplex data[],			/* signal to integrate */
	int nelements,				/* # elements in data[] */
	fcomplex sum[],				/* queue representing inf array */
	int queuesize)				/* size of sum[] */
{
	int		i;		/* index */
	int		j;		/* index */
	int		n;		/* number of times to loop */

	j = sumloc%queuesize;	/* get location inside queue */
	if(queuesize-j < nelements) n = queuesize-j; /* loop until end of queue */
	else n = nelements;		/* loop until end of data */
	for(i = 0; i < n; j++, i++) {
		sum[j].r += data[i].r;
		sum[j].i += data[i].i;
	}
	/* loop below is executed if the end of the queue was reached */
	for(j = 0; i < nelements; i++, j++) {
		sum[j].r += data[i].r;
		sum[j].i += data[i].i;
	}
}

/* Write to output file from queue and blank written part of queue.
 * If it's the first call, 'mode' = output queue size.  'mode' is ignored for
 * subsequent calls unless mode == 0 inwhich case it is the last call and the
 * buffer in 'FWriteBuffer()' is emptied.
 * 'Output()' returns the number of points written.  Note that output is
 * written in blocks and that the number written may be less than the number
 * sent (see the FWriteBuffer() description).
 */
int Output(
	char		filename[],		/* output file name */
	ATMOSSTATS	*as,			/* some statistics */
	int			n,				/* loc in inf array to output */
	fcomplex	out[],			/* output queue representing inf array */
	int			mode)			/* size of out[] */
{
	static int	queuesize;	/* size of queue */
	static int	lastn = 0;	/* last queue location */
	static FILE	*fp;		/* file descriptor */
	static int	buffsize = BUFFSIZE;	/* buffer size for FWriteBuffer() */
	static int		i;		/* index */
	int		rtn = 0;		/* return value from FWriteBuffer() */
	int		npts = 0;		/* # pts written to file */
	/* tape params */
	static int firstcall = YES;	/* first call flag */

	if(firstcall) { /* open file */
		firstcall = NO;
		queuesize = mode;	/* size of output queue */
		if(filename[0] != NULL && (fp = fopen(filename, "w")) == NULL) {
			printf("Error from Output(): `%s' not opened\n", filename);
			exit(1);
		}
	} 

	if(mode == 0) buffsize = 0; /* tells FWriteBuffer() this is the last call */
	n %= queuesize;	/* mod sample location to fall within queue */
	if(n >= lastn) {	/* output to file (no queue wrap around) */
		if(fp != NULL) {
			rtn = FWriteBuffer((char *)&out[lastn], sizeof(fcomplex), n-lastn,
			fp, buffsize);
		}
		if(rtn == -1) {
			printf("Oh, oh, 'FWriteBuffer' returned error status: exiting\n");
			exit(1);
		}
		BinHist(&out[lastn], n-lastn, as);
		npts += rtn;
		/* blank portion of queue written to */
		for(i = lastn; i < n; i++) {
			out[i].r = 0.0;
			out[i].i = 0.0;
		}
	} else {		/* output to file (queue wrap around) */
		/* output end of queue */
		if(fp != NULL) {
			rtn = FWriteBuffer((char *)&out[lastn], sizeof(fcomplex),
			queuesize-lastn, fp, buffsize);
		}
		if(rtn == -1) {
			printf("Oh, oh, 'FWriteBuffer' returned error status: exiting\n");
			exit(1);
		}
		BinHist(&out[lastn], queuesize-lastn, as);
		npts += rtn;
		/* output beginning of queue */
		if(filename[0] != NULL) {
			rtn = FWriteBuffer((char *)&out[0], sizeof(fcomplex), n, fp, 
				buffsize);
		}
		if(rtn == -1) {
			printf("Oh, oh, 'FWriteBuffer' returned error status: exiting\n");
			exit(1);
		}
		BinHist(&out[0], n, as);
		npts += rtn;
		/* blank portion of queue written to */
		for(i = lastn; i < queuesize; i++) {
			out[i].r = 0.0;
			out[i].i = 0.0;
		}
		for(i = 0; i < n; i++) {
			out[i].r = 0.0;
			out[i].i = 0.0;
		}
	}
	if(mode == 0) if(fp != NULL) fclose(fp); 		/* close file */
	lastn = n;
	return npts;
}

/* generate a rand poisson process */
float RandPoisson(float lamda) {
	float	rn;
	float	f;

	rn = drand48();
	SaveSeed(rn);
	f = -log(rn)/lamda;
	return(f);
}

/* initialize values atmosphere statistic structure 'ATMOSSTATS *as' 
 */
void InitStats(ATMOSSTATS *as) {
	int			bin;		/* bin index */

	as->max = BINMAX;		/* max bin value */
	as->min = BINMIN;		/* min bin value */
	as->step = BINSTEP;		/* bin step size */
	as->span = as->max-as->min;		/* width of hist */
	as->nbins = as->span/as->step+1; /* # of bins (including underflow) */
	/* alloc mem */
	as->x = (float *)calloc(as->nbins, sizeof(float)); /* init to zeros */
	as->n = (int *)calloc(as->nbins, sizeof(int)); /* init to zeros */

	bin = 1;				/* bin 0 is overflow */
	as->x[bin] = as->min;		/* init first bin */
	/* assign x values as equal increments of step */
	while(as->x[bin] < as->max) {
		as->x[bin+1] = as->x[bin]+as->step; /* bin x values */
		bin++;
	}

	/* zero out some values */
	as->c = 0.0;
	as->sigma = 0.0;
	as->ampsum = 0.0;
	as->ampsqsum = 0.0;
	as->rmssum = 0.0;
	as->rmssqsum = 0.0;
	as->nnonzeros = 0;
	as->nzeros = 0;
	as->nstrokes = 0;
	as->nflashes = 0;
	as->npts = 0;
}

/* print the contents of the atmosphere statistics structure '*as' to file.
 */
void OutputStats(ATMOSSTATS *as, int mode, float lamdaF) {
	float		ampsqavg;		/* average amplitude squared value */
	float		ampavg;			/* average amplitude value */
	char		outfile[100];	/* output file name */
	char		file[100];		/* file name */
	int			i;				/* index */
	FILE		*fp;			/* output file ptr */

	/* calc some stuff */
	ampsqavg = as->ampsqsum/(as->nnonzeros+as->nzeros);	/* avg(|n(t)|^2) */
	ampavg = as->ampsum/(as->nnonzeros+as->nzeros); 	/* avg(|n(t)|) */
	as->vd = 20.0*log10(sqrt(ampsqavg)/ampavg);			/* Vd */

	/* create output file name and output */
	sprintf(file, "atm%.0f", lamdaF);	/* output file 'atm(flashrate)' */
	strcpy(outfile, file);
	strcat(outfile, ".stat");		/* stat file is atm##.stat,eg atm100.stat */
	if((fp = fopen(outfile, "w")) == NULL) {
		printf("Error from fprintstats(): `%s' not opened\n", outfile);
		exit(1);
	}
	
	fprintf(fp, "                         STATISTICS\n");
	fprintf(fp, "avg(|n(t)|^2):%-20.3f ", ampsqavg);
	fprintf(fp, "     avg(a^2):%-20.3f\n",as->rmssqsum/as->nflashes);
	fprintf(fp, "  avg(|n(t)|):%-20.3f ", ampavg);
	fprintf(fp, "       avg(a):%-20.3f\n", as->rmssum/as->nflashes);
	fprintf(fp, "\n");
	fprintf(fp, "sum(|n(t)|^2):%-20.3f ", as->ampsqsum);
	fprintf(fp, "     sum(a^2):%-20.3f\n", as->rmssqsum);
	fprintf(fp, "  sum(|n(t)|):%-20.3f ", as->ampsum);
	fprintf(fp, "       sum(a):%-20.3f\n", as->rmssum);
	fprintf(fp, "\n");
	fprintf(fp, "      flashes:%-20d ", as->nflashes);
	fprintf(fp, "    non-zeros:%-20d\n", as->nnonzeros);
	fprintf(fp, "      strokes:%-20d ", as->nstrokes);
	fprintf(fp, "        zeros:%-20d\n", as->nzeros);
	fprintf(fp, "\n");
	fprintf(fp, "    pts filed:%-20d ", as->npts);
	fprintf(fp, "            c:%-20.3f\n", as->c);
	fprintf(fp, "           Vd:%-20.3f ", as->vd);
	fprintf(fp, "        sigma:%-20.3f\n", as->sigma);
	fprintf(fp, "\n");
	fprintf(fp, "                           HISTOGRAM\n");
	fprintf(fp,
	"---x--- ---n--- ---x--- ---n--- ---x--- ---n--- ---x--- ---n---\n");
	for(i = 0; i < as->nbins; i++) {
		fprintf(fp, "%7.3f %7d ",as->x[i], as->n[i]);
		if((i+1)%4 == 0) fprintf(fp, "\n");
	}
	printf("run statistics written to `%s'\n", outfile);
	fclose(fp);

	/* histogram file */
	strcpy(outfile, file);			/* use same name as .stat file */
	strcat(outfile, ".hst");		/* history file ends in .hst */
	if((fp = fopen(outfile, "w")) == NULL) {
		printf("Error from OutputStats(): `%s' not opened", outfile);
		exit(1);
	}
	if(mode) {	/* if in calibration mode... */
		/* print out a limited histogram */
		/* print the number of zeros as the first bin */
		fprintf(fp, "%d\t%d\n", 0, as->nzeros);
		for(i = 0; i < as->nbins; i++) {
			if(as->n[i] != 0.0) {			/* if bin has data in it... */
				/* convert 'x' back to voltage */
				fprintf(fp, "%.0f\t%d\n", rint(pow(10.0, as->x[i]/20.0)),
					as->n[i]);
			}
		}
	} else {	/* if NOT calibration mode... */
		/* print whole histogram */
		for(i = 0; i < as->nbins; i++) {
			fprintf(fp, "%.3f\t%d\n", as->x[i], as->n[i]);
		}
	}
	fclose(fp);
	printf("%d bins written to `%s'\n", as->nbins, outfile);
}


/* Bin data into histogram defined in 'ATMOSSTATS *as'.  The is an overflow bin
 * for number that are too small.  The is NO overflow bin for numbers that
 * are too big.  Histogram values are initialized in InitStats().
 */
void BinHist(
	fcomplex data[],			/* data to be binned */
	int ndatas,					/* # pts in data[] */
	ATMOSSTATS *as)				/* some statistics */
{
	int		i;					/* index */
	int		bin;				/* bin number */
	float	amp;				/* amp value */
	float	ampsquared;			/* amp^2 */

	for(i = 0; i < ndatas; i++) {
		ampsquared = data[i].r*data[i].r+data[i].i*data[i].i;
		amp = sqrt(ampsquared);
		if(amp != 0.0) {
			as->ampsqsum += ampsquared;
			as->ampsum += amp;
			as->nnonzeros++;
			amp = 20*log10(amp);
			/* calc bin number.  -1 is to not count the underflow bin */
			bin = (as->nbins-1)*((amp+as->step/2.0-as->min)/as->span);
			if(bin < 0) as->n[0]++; 					/* underflow */
			else if(bin+1 < as->nbins) as->n[bin+1]++;	/* +1 for underflow */
			/* note: no overflow for number too large */
		} else as->nzeros++;
	}
}


/* interpolate into C & Sigma table.
 *                    FlashRate
 *                ...             ...
 *  Vd ... | C11 & Sigma11 | C12 & Sigma12 | ...
 *     ... | C21 & Sigma21 | C22 & Sigma22 | ...
 *                ...             ...
 */
VD InterpCandSigma(
	VD		vdTable[][NFLASHRATES],	/* c & sigma table */
	float	v,						/* vd for interp'n */
	float	f)						/* flash rate for interp'n */
{
	int		v1;					/* index of smallest Vd of interp */
	int		v2;					/* index of largest Vd of interp */
	int		f1;					/* index of smallest flash rate of interp */
	int		f2;					/* index of larglest flash rate of interp */
	float	c1;					/* c interpolated from 1st column */
	float	sigma1;				/* sigma interpolated from 1st column */
	float	c2;					/* c interpolated from 2nd column */
	float	sigma2;				/* sigma interpolated from 2nd column */
	float	vRem;				/* remainder for interp'n */
	float	fRem;				/* remainder for interp'n */
	VD		out;				/* output */
	
	v1 = v - VDMIN;				/* get integer index */
	vRem = v - VDMIN - v1;		/* get remainder */
	if(v == VDMAX) {
		v2 = v1;
	} else {
		v2 = v1 + 1;
	}
	f1 = (f-FLASHRATEMIN)/FLASHRATESTEP;		/* get index */
	fRem = (f-FLASHRATEMIN)/FLASHRATESTEP - f1;	/* get remainder */
	if(f == FLASHRATEMAX) {
		f2 = f1;
	} else {
		f2 = f1 + 1;
	}
	/* interpolate 3 times */
	/* interpolate 1st column */
	c1 = Interp(v1 + vRem, v1, vdTable[v1][f1].c, v2, vdTable[v2][f1].c);
	sigma1 = Interp(v1 + vRem, v1, vdTable[v1][f1].sigma, v2,
		vdTable[v2][f1].sigma);
	/* interpolate 2nd column */
	c2 = Interp(v1 + vRem, v1, vdTable[v1][f2].c, v2, vdTable[v2][f2].c);
	sigma2 = Interp(v1 + vRem, v1, vdTable[v1][f2].sigma, v2,
		vdTable[v2][f2].sigma);
	/* interpolate betw columns */
	out.c = Interp(f1 + fRem, f1, c1, f2, c2);
	out.sigma = Interp(f1 + fRem, f1, sigma1, f2, sigma2);
	return out;
}

/* linearly interpolate two vertices.  Function return the interplated y.
 * Note that the x does not have to lie inside the vertices.  This is simple
 * y = mx+b stuff.
 */
float Interp(
	float		x,			/* x to be interpolated */
	float		x1,			/* x of 1st vertice */
	float		y1,			/* y of 1st vertice */
	float		x2,			/* x of 2nd vertice */
	float		y2)			/* y of 2nd vertice */
{
	float		m;			/* slope */
	float		b;			/* y intercept */

	if(x1 == x2) {
		if(y1 != y2) {
			printf("Warning from Interp(): same 'X' value [%f], "
				"but different Y's [%f, %f]\n", x1, y1, y2);
			exit(1);
		}
		return y1;
	}
	m = (y2-y1)/(x2-x1);	/* slope */
	b = y1-m*x1;			/* y intercept */
	return m*x+b;			/* return y = mx+b */
}

/* read in C and Sigma table from file */
void ReadCandSigma(VD vdTable[][NFLASHRATES]) {
	int		v, f;			/* Vd and flash rate index */
	FILE	*fp;			/* filename file ptr */
	char	*envpath;		/* path to enviromental variable */
	char	file[100];		/* path and filename */

	/* get path to simulation */
	if((envpath = getenv("HFDIR")) == (char *)NULL) {
		printf("Error from atmosphere:  must 'setenv HFDIR ~/hf'\n");
		exit(1);
	}
	strcpy(file, envpath);			/* add path to file name */
	if(file[strlen(file)-1] != '/') strcat(file, "/");
	strcat(file, CANDSIGMAFILE);	/* add filename to file name */

	/* open file */
	if((fp = fopen(file, "r")) == NULL) {
		printf("Error from atmosphere: C & Sigma file '%s' not opened\n", file);
		exit(1);
	}
	for(v = 0; v < NVDS; v++) {
		for(f = 0; f < NFLASHRATES; f++) {
			fscanf(fp, "%f%f", &vdTable[v][f].c, &vdTable[v][f].sigma);
		}
	}
}
