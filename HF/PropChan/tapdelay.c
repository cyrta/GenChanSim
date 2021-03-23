/* tapdelay.c
 * this routine sets up a tapped delay line with ndelays pointers within it to
 * be used as ndelays0 delays.
 * 'tap' is the number of the tap being used
 * 'ndelays0' is the number of the delays in the tap
 * 'weight[]' is the weights to be multiplied
 * 's' is the value added to the tap

   copyright (c) 1994 The MITRE Corporation Bedford, MA

   $Id: tapdelay.c,v 2.2 1994/07/25 19:29:19 jjb Exp $
   $Log: tapdelay.c,v $
 * Revision 2.2  1994/07/25  19:29:19  jjb
 * added copyright notice.
 *

 * r.coutts
 */
#include	<malloc.h>
#include	"propchan.h"
#include	<complex.h>

static char rcsid[] = "$Id: tapdelay.c,v 2.2 1994/07/25 19:29:19 jjb Exp $";

void tapdelay(
	fcomplex	*sum,				/* sum of taps (returned) */
	int			path,				/* path index */
	int			ndelays0,
	fcomplex	weight[],
	fcomplex	*s)
{
	static int firstcall[NPATHS] = {1,1,1,1,1,1,1,1,1,1};
										/* flag for if first call to routine */
	static fcomplex *tapbuffer[NPATHS];/* location of first cell of buffer */
	static fcomplex *data[NPATHS];		/* location of incoming data in buffer*/
	fcomplex *d;						/* location of scanned data in buffer */
	static fcomplex *endofbuffer[NPATHS];/* location of last cell of buffer */
	static int ndelays[NPATHS];		/* buffer size */
	int		i;					/* index to weight[] */

	if(firstcall[path]) {
		/* initialize buffer */
		ndelays[path] = ndelays0;
		data[path] = tapbuffer[path] =
			(fcomplex *)calloc(ndelays[path], sizeof(fcomplex));
		endofbuffer[path] = tapbuffer[path]+ndelays[path]-1;
		firstcall[path] = NO;
	}
	/* add sample to buffer */
	data[path]->r = s->r;
	data[path]->i = s->i;
	/* multiply and sum */
	d = data[path];
	sum->r = 0.0; sum->i = 0.0;
	#if DEBUG == 1
		if(path == 0) printf("\n");
		printf("%d: ", path);
	#endif
	for(i = 0; i < ndelays[path]; i++) {
		#if DEBUG == 1
			printf("%.2f+j%.2f ", d->r, d->i);
			if((i+1)%7 == 0) printf("\n   ");
		#endif
		sum->r += weight[i].r*d->r-weight[i].i*d->i;
		sum->i += weight[i].i*d->r+weight[i].r*d->i;
		if(--d < tapbuffer[path]) d = endofbuffer[path];
	}
	#if DEBUG == 1
		printf("\n", path);
	#endif
	/* increment pointer and test for overlap */
	if(++data[path] > endofbuffer[path]) data[path] = tapbuffer[path];
}
