/*------------------------------------------------------------------------*/
/* lpgauss.c - Generate a pair of Gaussian deviates using Lewis & Payne   */
/*             random uniform generator                                   */
/*------------------------------------------------------------------------*/
/*

   copyright (c) 1994 The MITRE Corporation Bedford, MA

   $Id: lpgauss.c,v 2.2 1994/07/25 18:30:41 jjb Exp $
   $Log: lpgauss.c,v $
 * Revision 2.2  1994/07/25  18:30:41  jjb
 * added Copyright notice.
 *

*/
#include <stdio.h>
#include <complex.h>
#include <propchan.h>
#include <math.h>
#define P 98            	/*Polynomial definition*/
#define Q 27            	/*Polynomial definition*/
#define SIGNMASK 0x80000000	/* to mask off sign bit */
#define CLEAR_10MSB_MASK 0x003FFFFF
#define CLEAR_16MSB_MASK 0x0000FFFF
#define CLEAR_22MSB_MASK 0x000003FF
#define CLEAR_28MSB_MASK 0x0000000F
#define KEEP_9LSB_MASK   0x000001FF
#define POSMASK 0x7FFFFFFF

#define	DEBUG	0

static char rcsid[] = "$Id: lpgauss.c,v 2.2 1994/07/25 18:30:41 jjb Exp $";

void lpgauss(float gausstbl[], unsigned long int seedtbl[NUMSEED],
	      fcomplex iqbuf[], int sttapnum[], int ntaps[],
	      float lpgscal[], int path)
{
	static int sub_j = -1;	/* index to table w*/
	static int sub_k = Q-1;	/* index to table w*/
	long int index1, index2;
	int i;
	double u, y;
	unsigned long int x;
	fcomplex *iqptr;


	#if DEBUG == 1
		static FILE *fp;
		static int	firstcall = 1;
	
		if(firstcall) {
			firstcall = 0;
			if((fp = fopen("lpg.dat", "w")) == NULL) {
				printf("lpg.dat not opened\n");
				exit(1);
			} else printf("lpg.dat file opened\n");
		}
	#endif


	iqptr = iqbuf + sttapnum[path];
	for (i=0; i < 2*ntaps[path]; i++) {
		/* Lewis and Payne generalized feddback shift register pseudo-random
		 * generator with polynomial (x**98+x**27+1). returns 32 bit pseudo
		 * random integer
		 */

		/* generate uniform random number between -1 and 1 */
		if (++sub_j == P) sub_j = 0;
		if (++sub_k == P) sub_k = 0;
		x = seedtbl[sub_j] = seedtbl[sub_j]^seedtbl[sub_k];
		/* x = x | 0x7FF80000;  To force evaluation of table gauss3 */
		index1 = (x >> 22) & KEEP_9LSB_MASK;    /* get integer portion*/
		if ((index1 >> 3) != 63) /*if first 6 bits of index1 are not all ones*/
		{
			index2 = x & CLEAR_10MSB_MASK; /* get fractional portion of index*/
			u = lpgscal[0] * (float) index2;      /* scale to be (0.,1.)*/
			y = gausstbl[index1]+u*(gausstbl[index1+1]-gausstbl[index1]);
			if ((x & SIGNMASK) != 0) y =- y;
			goto next;
		}

		index1 = (x >> 16) & KEEP_9LSB_MASK;    /* get integer portion*/
		if ((index1 >> 3) != 63) /*if first 6 bits of index1 are not all ones*/
		{
			index2 = x & CLEAR_16MSB_MASK; /* get fractional portion of index*/
			u = lpgscal[1] * (float) index2;      /* scale to be (0.,1.)*/
			y = gausstbl[index1+512]+u*(gausstbl[index1+513] -
				gausstbl[index1+512]);
			if ((x & SIGNMASK) != 0) y =- y;
			goto next;
		}

		index1 = (x >> 10) & KEEP_9LSB_MASK;    /* get integer portion*/
		if ((index1 >> 3) != 63) /*if first 6 bits of index1 are not all ones*/
		{
			index2 = x & CLEAR_22MSB_MASK; /* get fractional portion of index*/
			u = lpgscal[2] * (float) index2;      /* scale to be (0.,1.)*/
			y = gausstbl[index1+1024]+u*(gausstbl[index1+1025] -
				gausstbl[index1+1024]);
			if ((x & SIGNMASK) != 0) y =- y;
			goto next;
		}

		index1 = (x >> 4) & KEEP_9LSB_MASK;    /* get integer portion*/
		if (index1  != 511)      /*if 9 bits of index1 are not all ones*/
		{
			index2 = x & CLEAR_28MSB_MASK; /* get fractional portion of index*/
			u = lpgscal[3] * (float) index2;      /* scale to be (0.,1.)*/
			y = gausstbl[index1+1536]+u*(gausstbl[index1+1537] -
				   gausstbl[index1+1536]);
			if ((x & SIGNMASK) != 0) y =- y;
			goto next;
		}

		y = gausstbl[index1+1536];
		if ((x & SIGNMASK) == 1) y = -y;

		next:
		if ((i & 1) == 0) {
			iqptr->r = y;
			#if DEBUG == 1
				fprintf(fp, "%f ", y);
			#endif
		} else {
			(iqptr++)->i = y;
			#if DEBUG == 1
				fprintf(fp, "%f\n", y);
			#endif
		}
	}
}
