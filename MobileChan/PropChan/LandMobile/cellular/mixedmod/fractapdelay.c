/* 
  fractional delay tap delay line

  copyright (c) 1994 The MITRE Corporation Bedford, MA

  $Id: fractapdelay.c,v 3.0 1994/08/30 18:48:31 jjb Exp $
  $Log: fractapdelay.c,v $
 * Revision 3.0  1994/08/30  18:48:31  jjb
 * new addition to rev 3.0 of code
 *

 */
#include	<malloc.h>
#include        <values.h>
#include	<mixedchan.h>
#include	<complex.h>
#include        <fracdly.h>
static char rcsid[] = "$Id: fractapdelay.c,v 3.0 1994/08/30 18:48:31 jjb Exp $";

void 
FracTapDelay(
	    int *minDelay,         /* min. delay */
	    int *fractDlyFlag,     /* flag to indicate fractional delay */
	    fcomplex * sum,        /* sum of taps (returned) */
	    int update,            /* flag to update tap weights */
	    TAPWGTS *tw,           /* tap weight parameters */
	    float gain[],          /* array of path gains */
	    fcomplex * weight[],   /* array tapweigths [path][tap] */
	    fcomplex * s)          /* sample for input */
{
  static int      firstcall = 1;

  /* flag for if first call to routine */
  static fcomplex *tapbuffer;	/* location of first cell of
				 * buffer */
  static fcomplex *data;	/* location of incoming data in
				 * buffer */
  fcomplex       *d;	/* location of scanned data in buffer */
  static fcomplex *endofbuffer;	/* location of last cell of
				 * buffer */
  static fcomplex *filter;      /* pointer to complex filter taps */
  static float fracFilter[MAXTAPS][FRAC_FILT_SIZE];
  static int intdelays[MAXTAPS];  /* delays converted to sample rate */
  static float fractdelays[MAXTAPS];  /* delays converted to sample rate */
  static int      sttapnum[NPATHS];	/* start tap number of path */
  static int  start[NPATHS];     /* start index for filter op */
  static int  stop[NPATHS];     /* stop index for filter op */
  int    *non0vector;               /* vector to indicate non-zero filt taps */
  int             i;	/* index to weight[] */
  static float deltaDly;
  int p;
  static int totalTaps;
  static int ndelays;
  int j,k,dly;
  int  mindelay,maxdelay;
  float maxpow;

  if (firstcall) {

    *fractDlyFlag = 0;  /* clear flag */
    deltaDly = tw->sampleRate/tw->scatterSampleRate;

    /* find starting filter number of each path */
    sttapnum[0] = 0;
    for (p = 1; p < tw->nPaths; p++) {
      sttapnum[p] = sttapnum[p - 1] + tw->nTaps[p - 1];
    }
    totalTaps = sttapnum[tw->nPaths - 1] + tw->nTaps[tw->nPaths -1];

    /* convert delays to number of samples */
    mindelay = MAXINT;
    maxdelay = 0;
    p=0;
    for(i=0; i<tw->nPaths; ++i) {
      for(j=0; j<tw->nTaps[i]; ++j) {
	fractdelays[p] = (tw->sampleRate * tw->bulkDelay[i]/1e9)
	  + j * deltaDly;
	intdelays[p] = fractdelays[p]; /* get int part */
	fractdelays[p] -= intdelays[p];

	if (fractdelays[p] < MIN_FRACT_DELAY )
	  fractdelays[p] = 0;
	else if (fractdelays[p] > (1- MIN_FRACT_DELAY) ) {
	  fractdelays[p] = 0;
	  intdelays[p]++;
	} else {
	  intdelays[p] -= FRAC_FILT_DLY_BIAS;
	  *fractDlyFlag = 1; /* set flag */
	}

	if ( intdelays[p] < mindelay )
	  mindelay = intdelays[p];
	if ( intdelays[p] > maxdelay )
	  maxdelay = intdelays[p];
	p++;
      }
    }

    /* force mindelay to be = filter bias so alway delay output
       when using fractional delays */

    mindelay = -1 * FRAC_FILT_DLY_BIAS;

    /* change delays into indexes into array */
    maxdelay -= mindelay;
    for(i=0; i<totalTaps; ++i) {
      intdelays[i] -= mindelay;
    }

    /* allocate memory for array */

    /* initialize buffer */
    ndelays = maxdelay+FRAC_FILT_SIZE;
    data = tapbuffer =
      (fcomplex *) calloc(ndelays, sizeof(fcomplex));
    filter = (fcomplex *) calloc(ndelays, sizeof(fcomplex));
    non0vector = (int *) calloc(ndelays, sizeof(int));
    endofbuffer = tapbuffer + ndelays - 1;
    firstcall = NO;

    if ( filter == NULL || tapbuffer == NULL || non0vector == NULL) {
      fprintf(stderr,"unable to allocate memory for filter\n");
      exit(1);
    }

    /* get delay filters */

    for(i=0; i<totalTaps; ++i ){
     
      if ( fractdelays[i] != 0.0 )
	fracDelayFilter(fractdelays[i],fracFilter[i]);
    }

    /* construct tap delay power profile to determine which
       taps are small enough to ignore */

    for(i=0; i<ndelays; ++i)
      (filter+i)->i = (filter+i)->r = 0.0;

    p=0;
    for(i=0; i<tw->nPaths; ++i) {
      for(j=0; j<tw->nTaps[i]; ++j) {
	dly = intdelays[p];
	if ( fractdelays[p] == 0.0) {
	  (filter +dly)->r += gain[i] * tw->weighting[p];

	} else {
	  for(k=0; k<FRAC_FILT_SIZE; ++k) {
	    (filter +dly+k)->r += gain[i] * tw->weighting[p] * fracFilter[p][k];
	  }
	}
	p++;
      }
    }

    /* compute power and find max */

    maxpow = 0.0;
    for(p=0; p<ndelays; ++p) {
      (filter + p)->r = (filter + p)->r * (filter + p)->r;
      if ( (filter + p)->r > maxpow )
	maxpow = (filter + p)->r;
    }
    maxpow *= tw->weightLimit;

    /* construct start and stop indexes */

    for(p=0; p<ndelays; ++p) {
      if ( (filter + p)->r < maxpow )
	*(non0vector + p) = 0;
      else
	*(non0vector + p) = 1;
    }

    for(i=0; i<tw->nPaths; ++i)
      start[i]=stop[i]=ndelays;
    start[0] = 0;

    i=0;
    for(p=0; p<ndelays-1; ++p) {
      if ( (*(non0vector+p) == 0) && (*(non0vector+p+1) != 0)) {
	start[i]=p+1;
      }
      if ( (*(non0vector+p) != 0) && (*(non0vector+p+1) == 0)) {
	stop[i++]=p+1;
      }
    }
    free(non0vector);

    /* return min delay and exit subroutine */
    *minDelay = mindelay;

    if ( (mindelay < 0) && (*fractDlyFlag == 1) ) {
      printf("all output paths delayed by %d samples to allow fractional delay\n",
	     -1*mindelay);
    }
    return;
  }

  /* if update flag set then generate tap weights */

  if (update == 1) {
    for(i=0; i<ndelays; ++i)
      (filter+i)->i = (filter+i)->r = 0.0;

    p=0;
    for(i=0; i<tw->nPaths; ++i) {
      for(j=0; j<tw->nTaps[i]; ++j) {
	dly = intdelays[p];
	if ( fractdelays[p] == 0.0) {
	  (filter +dly)->r += gain[i] * weight[i][j].r;
	  (filter +dly)->i += gain[i] * weight[i][j].i;

	} else {
	  for(k=0; k<FRAC_FILT_SIZE; ++k) {
	    (filter +dly+k)->r += gain[i] * weight[i][j].r * fracFilter[p][k];
	    (filter +dly+k)->i += gain[i] * weight[i][j].i * fracFilter[p][k];
	  }
	}
	p++;
      }
    }

  }

  /* perform tap delay line convolution  */

  /* add sample to buffer */
  data->r = s->r;
  data->i = s->i;
  /* multiply and sum */
  d = data;
  sum->r = 0.0;
  sum->i = 0.0;
  if ((d -= start[0]) < tapbuffer)
    d = endofbuffer - (tapbuffer - d) + 1;
  for(j=0; j<tw->nPaths; ++j) {

    for (i = start[j]; i < stop[j]; i++) {
      sum->r += (filter+i)->r * d->r - (filter+i)->i * d->i;
      sum->i += (filter+i)->i * d->r + (filter+i)->r * d->i;
      if (--d < tapbuffer)
	d = endofbuffer;
    }
    if ((d -= start[j+1] - stop[j]) < tapbuffer)
      d = endofbuffer - (tapbuffer - d) + 1;
    
  }
/*
  for (i = 0; i < ndelays; i++) {
    sum->r += (filter+i)->r * d->r - (filter+i)->i * d->i;
    sum->i += (filter+i)->i * d->r + (filter+i)->r * d->i;
    if (--d < tapbuffer)
      d = endofbuffer;
  }
*/

  /* increment pointer and test for overlap */
  if (++data > endofbuffer)
    data = tapbuffer;

}
