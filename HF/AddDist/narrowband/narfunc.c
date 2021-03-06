/* some functions to support narrowband.c.
 * r.coutts

   copyright (c) 1994 The MITRE Corporation Bedford, MA

   $Id: narfunc.c,v 2.2 1994/07/25 19:06:52 jjb Exp $
   $Log: narfunc.c,v $
 * Revision 2.2  1994/07/25  19:06:52  jjb
 * added copyright notice
 *

 */
#include <stdio.h>
#include <complex.h>
#include <narrowband.h>
#include <fwritebuffer.h>

static char rcsid[] = "$Id: narfunc.c,v 2.2 1994/07/25 19:06:52 jjb Exp $";

/* add interferer to the output queue */
void IntegrateInterf(
	int sumloc,					/* sample location in infinite array */
	fcomplex data[],			/* signal to integrate */
	int nElements,				/* # elements in data[] */
	fcomplex sum[],				/* queue representing inf array */
	int queueSize)				/* size of sum[] */
{
	int		i;		/* index */
	int		j;		/* index */
	int		n;		/* number of times to loop */

	j = sumloc%queueSize;		/* get location inside queue */
	if(queueSize-j < nElements) n = queueSize-j; /* loop until end of queue */
	else n = nElements;			/* loop until end of data */
	for(i = 0; i < n; j++, i++) {
		sum[j].r += data[i].r;
		sum[j].i += data[i].i;
	}
	/* loop below is executed if the end of the queue was reached */
	j = 0;						/* queue index */
	for(; i < nElements; i++) {	/* loop thru rest of elements */
		sum[j].r += data[i].r;
		sum[j].i += data[i].i;
		j++;
	}
}

/* Write to output file from queue and blank written part of queue.
 * If it's the first call, 'mode' = output queue size.  'mode' is ignored for
 * subsequent calls unless mode == 0 in which case it is the last call and the
 * buffer in 'FWriteBuffer()' is emptied.
 * 'OutputData()' returns the number of points written.  Note that output is
 * written in blocks and that the number written may be less than the number
 * sent (see the FWriteBuffer() description).
 */
int OutputData(
	char		fileName[],		/* output file name */
	fcomplex	out[],			/* output queue representing inf array */
	int			n,				/* loc in inf array to output */
	int			mode)			/* size of out[] */
{
	static int	queueSize;		/* size of queue */
	static int	lastN = 0;		/* last queue location */
	static FILE	*fp = NULL;		/* file descriptor */
	static int	buffSize = BUFFSIZE;	/* buffer size for FWriteBuffer() */
	static int		i;			/* index */
	int		rtn = 0;			/* return value from FWriteBuffer() */
	int		npts = 0;			/* # pts written to file */
	/* tape params */
	static int firstCall = YES;	/* first call flag */

	if(firstCall) { /* open file */
		firstCall = NO;
		queueSize = mode;	/* size of output queue */
		if(fileName[0] != NULL && (fp = fopen(fileName, "w")) == NULL) {
			printf("Error from narrowband:  output `%s' not opened\n",
				fileName);
			exit(1);
		}
	} 

	if(n < 0) n = lastN;		/* if invalid number passed, ignore */
	n %= queueSize;	/* mod sample location to fall within queue */
	if(n >= lastN) {	/* output to file (no queue wrap around) */
		if(mode == 0) buffSize = 0; /* tells FWriteBuffer() 'last call' */
		if(fp != NULL) {
			rtn = FWriteBuffer((char *)&out[lastN], sizeof(fcomplex), n-lastN,
			fp, buffSize);
		}
		if(rtn == -1) {
			printf("Oh, oh, error status returned from 'FWriteBuffer' in "
				"OutputData()\n");
			exit(1);
		}
		npts += rtn;
		/* blank portion of queue written to */
		for(i = lastN; i < n; i++) {
			out[i].r = 0.0;
			out[i].i = 0.0;
		}
	} else {		/* output to file (queue wrap around) */
		/* output end of queue */
		if(fp != NULL) {
			rtn = FWriteBuffer((char *)&out[lastN], sizeof(fcomplex),
			queueSize-lastN, fp, buffSize);
		}
		if(rtn == -1) {
			printf("Oh, oh, error status returned from 'FWriteBuffer' in "
				"OutputData()\n");
			exit(1);
		}
		npts += rtn;
		/* output beginning of queue */
		if(mode == 0) buffSize = 0; /* tells FWriteBuffer() 'last call' */
		if(fp != NULL) {
			rtn = FWriteBuffer((char *)&out[0], sizeof(fcomplex), n, fp,
			buffSize);
		}
		if(rtn == -1) {
			printf("Oh, oh, error status returned from 'FWriteBuffer' in "
				"OutputData()\n");
			exit(1);
		}
		npts += rtn;
		/* blank portion of queue written to */
		for(i = lastN; i < queueSize; i++) {
			out[i].r = 0.0;
			out[i].i = 0.0;
		}
		for(i = 0; i < n; i++) {
			out[i].r = 0.0;
			out[i].i = 0.0;
		}
	}
	if(mode == 0) if(fp != NULL) fclose(fp); 		/* close file */
	lastN = n;
	return npts;
}
/* multiply two complex arrays and put them in the first array
 */
void MultComplex(fcomplex a[], fcomplex b[], int n) {
	int				i;
	static float	real, imag;		/* real and imaginary parts */

	for(i = 0; i < n; i++) {
		real = a[i].r*b[i].r-a[i].i*b[i].i;
		imag = a[i].r*b[i].i+a[i].i*b[i].r;
		a[i].r = real;
		a[i].i = imag;
	}
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
