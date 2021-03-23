/* this routine sets up a circular buffer with npaths pointers within it to
 * be used as npaths delays.
 * rcoutts
 
   copyright (c) 1994 The MITRE Corporation Bedford, MA

   $Id: buffer.c,v 3.0 1994/08/30 18:49:58 jjb Exp $
   $Log: buffer.c,v $
 * Revision 3.0  1994/08/30  18:49:58  jjb
 * updated rev number.
 *
 * Revision 2.2  1994/07/25  18:57:02  jjb
 * added copyright notice
 *

 */
#include	<malloc.h>
#include	<flatfade.h>
#include	<complex.h>

static char rcsid[] = "$Id: buffer.c,v 3.0 1994/08/30 18:49:58 jjb Exp $";

int buffer(int delay[], int npaths, fcomplex *sample[], fcomplex s) {
	static int firstcall = YES;	/* flag for if first call to routine */
	static fcomplex *data;		/* location of data input to buffer */
	static fcomplex *endofbuffer;/* pointer to first location outside buffer */
	static int longestdelay = -1;/* longest delay required */
	static fcomplex *delaybuffer;/* memory location of delay buffer */
	int i;

	if(firstcall) {
		firstcall = NO;
		/* initialize buffer */
		/* test if delays do not exceed buffer size */
		/* find longest delay */
		for(i = 0; i < npaths; i++) {
			if(longestdelay < delay[i]) longestdelay = delay[i];
		}
		/* initialize memory */
		data = delaybuffer =(fcomplex *)calloc((longestdelay+1),
			sizeof(fcomplex));
		/* set up sample pointers to values inside buffer.  if a zero delay
		 * is required, it will be initialized to the pointer value 
		 * delaybuffer-1.  this value will be incremented below, however, before
		 * it is ever attempted to be read.
		 */
		endofbuffer = delaybuffer+longestdelay+1;
		for(i = 0; i < npaths; i++) {
			sample[i] = endofbuffer-delay[i]-1;
		}
	}
	/* add sample to buffer */
	data->r = s.r;
	data->i = s.i;
	/* increment pointers and test for overlap */
	if(++data == endofbuffer) data = delaybuffer;
	for(i = 0; i < npaths; i++) {
		if(++sample[i] == endofbuffer) {
			sample[i] = delaybuffer;
		}
	}
	return(longestdelay);
}
