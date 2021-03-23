/* generate tap weights for a single snapshot.

   copyright (c) 1994 The MITRE Corporation Bedford, MA

   $Id: tapwgts.c,v 2.2 1994/07/25 19:20:47 jjb Exp $
   $Log: tapwgts.c,v $
 * Revision 2.2  1994/07/25  19:20:47  jjb
 * added copyright notice
 *

 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>
#include <propchan.h>

static char rcsid[] = "$Id: tapwgts.c,v 2.2 1994/07/25 19:20:47 jjb Exp $";

void tapwgts(
	TAPWGTS *tw,				/* tapwgts structure of parameters */
	fcomplex *wgts[],			/* weights of ea tap */
	int path) 					/* path to get weights */
{
	char	fbfile[100];		/* filter bank file name */
	char	*envpath;			/* path name returned from getenv() */
	FILE	*fbfp = NULL;		/* filter bank fp */
	static int firstcall = YES;	/* flag for first call */
	int	totalntaps;				/* sum of ntaps[] */
	int		i;					/* index */
	int		j;					/* index */
	int		k;					/* index */
	int		t;					/* index */
	
	if(firstcall) {
		/* set up file paths using env variable defined in '.cshrc' */
		if((envpath = getenv("HFDIR")) == (char *)NULL) {
			printf("Error from tapwgts(): must 'setenv HFDIR ~/hf'\n");
			exit(1);
		}
		strcpy(fbfile, envpath);
		strcat(fbfile, FILTBANK);				/* filter bank file name */
	
		/* set all the taps within a partition to one value */
		for(i = 0, j = 0; i < tw->nPaths; i++) {
			for(k = 0; k < tw->nTaps[i]; k++) {
				tw->dplOffset[j++] = tw->dplShift[i];
			}
		}
	
		/* open files */
		if((fbfp = fopen(fbfile, "r")) == NULL) {
			printf("Error from tapwgts(): fb:'%s' not opened\n", fbfile);
			exit(1);
		}
	
		/* read in filter bank file */
		fscanf(fbfp, "%f", &tw->filterBank[0].ampl);
		i = 0;
		while(fscanf(fbfp, "%f%f%f%f", &tw->filterBank[0].sect[i].a1,
			&tw->filterBank[0].sect[i].a2, &tw->filterBank[0].sect[i].b1,
			&tw->filterBank[0].sect[i].b2) != EOF)
		{
			i++;
		}
		fclose(fbfp);			/* close file */

		/* blank remaining filter coefficents */
		for(; i < 4; i++) {
			tw->filterBank[0].sect[i].a1 = tw->filterBank[0].sect[i].a2 =
			tw->filterBank[0].sect[i].b1 = tw->filterBank[0].sect[i].b2 = 0.0;
		}

		/* assign filter bank values */
		t = 0;
		for(i = 0; i < tw->nPaths; i++) {
			for(j = 0; j < tw->nTaps[i]; j++) {
				tw->filterBank[t].ampl = tw->filterBank[0].ampl;
				for(k = 0; k < 4; k++) {
					tw->filterBank[t].sect[k].a1 = tw->filterBank[0].sect[k].a1;
					tw->filterBank[t].sect[k].a2 = tw->filterBank[0].sect[k].a2;
					tw->filterBank[t].sect[k].b1 = tw->filterBank[0].sect[k].b1;
					tw->filterBank[t].sect[k].b2 = tw->filterBank[0].sect[k].b2;
				}
				t++;
			}
		}

		/* get total number of taps in all partitions */
		for(i = 0, totalntaps = 0; i < tw->nPaths; i++) {
			totalntaps += tw->nTaps[i];
		}
	}

	hfchan(tw, wgts, path);			/* get tap-weight values */

	if(firstcall) {					/* if first call to function... */
		PrintTapWgts(tw);			/* print out tap-weight stuff */
		firstcall = NO;				/* don't do this again */
	}
}

void PrintTapWgts(TAPWGTS *tw) {
	int		i;		/* index */

	printf("\n");
	printf("        Sample Rate (Hz): %.3f\n", tw->sampleRate);
	printf("   Total Number of paths: %d\n", tw->nPaths);
	printf("------path------- ");
	for(i = 0; i < tw->nPaths; i++) printf("----%d--- ", i+1);
	printf("\n");
	printf("             mode ");
	for(i = 0; i < tw->nPaths; i++) {
		if(tw->mode[i] == NORM) printf("  normal ", tw->mode[i]);
		else if(tw->mode[i] == DIST) printf(" disturb ", tw->mode[i]);
		else printf("    ???? ");
	}
	printf("\n");
	/* print out thins phase screening values */
	printf("Thin Phase Thresh ");
	for(i = 0; i < tw->nPaths; i++) {
		if(tw->mode[i] == DIST) {
			if(tw->thinPhScr) {
				printf(" %7.3f ", tw->thinPhaseThresh);
			} else {
				printf("no t.p.s ");
			}
		} else {
			printf("     N/A ");
		}
	}
	printf("\n");
	/* print out thins phase screening values */
	printf("     Thin Phase C ");
	for(i = 0; i < tw->nPaths; i++) {
		if(tw->mode[i] == DIST) {
			if(tw->thinPhScr) {
				printf(" %7.3f ", tw->thinPhaseC);
			} else {
				printf("no t.p.s ");
			}
		} else {
			printf("     N/A ");
		}
	}
	printf("\n");
	/* both normal and disturbed modes */
	printf("   number of taps ");
	for(i = 0; i < tw->nPaths; i++) printf("%8d ", tw->nTaps[i]);
	printf("\n");
	printf("    snapshot (Hz) ");
	for(i = 0; i < tw->nPaths; i++) printf("%8.3f ", tw->snapshot[i]);
	printf("\n");
	printf("    Dpl shift(Hz) ");
	for(i = 0; i < tw->nPaths; i++) printf("%8.3f ", tw->dplShift[i]);
	printf("\n");
	/* normal mode */
	printf("Distortn (uS/MHz) ");
	for(i = 0; i < tw->nPaths; i++) {
		if(tw->mode[i] == NORM) printf("%8.3f ", tw->groupDelay[i]);
		else printf("     n/a ");
	}
	printf("\n");
	/* disturbed mode */
	printf("  Dpl Spread (Hz) ");
	for(i = 0; i < tw->nPaths; i++) {
		if(tw->mode[i] == DIST) printf("%8.3f ", tw->dplSpread[i]);
		else printf("     n/a ");
	}
	printf("\n");
	printf("Multi Spread (uS) ");
	for(i = 0; i < tw->nPaths; i++) {
		if(tw->mode[i] == DIST) printf("%8.3f ", tw->multiPathSpr[i]);
		else printf("     n/a ");
	}
	printf("\n");
	printf("\n");
}
