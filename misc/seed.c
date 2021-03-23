/* functions to save and retrieve r.n.g. seeds */
/*
   copyright (c) 1994 The MITRE Corporation Bedford, MA

   $Id: seed.c,v 2.2 1994/07/25 18:30:41 jjb Exp $
   $Log: seed.c,v $
 * Revision 2.2  1994/07/25  18:30:41  jjb
 * added Copyright notice.
 *

*/
#include <stdio.h>
#include <seed.h>

static char rcsid[] = "$Id: seed.c,v 2.2 1994/07/25 18:30:41 jjb Exp $";

/* gets seed from file */
long GetSeed(void) {
	long	seed;				/* seed */
	FILE	*fp;				/* file ptr containing seed */
	char	*envpath;			/* enviromental path var */
	char	file[100];			/* path and file name */
	/* functions */
	char *getenv(char *);

	/* get path to simulation */
	if((envpath = getenv("SIMDIR")) == (char *)NULL) {
		printf("Error from GetSeed: must 'setenv SIMDIR ~/hf'\n");
		exit(1);
	}
	strcpy(file, envpath);          /* add path to file name */
	strcat(file, SEEDFILE);         /* add filename to file name */

	if((fp = fopen(file, "r")) == NULL) {
		printf("Warning: seed file '%s' not found... returning random number "
		"from 'rand()'\n", SEEDFILE);
		return rand();
	}
	fscanf(fp, "%d", &seed);	/* get seed from file */
	fclose(fp);					/* close seed file */
	return seed;				/* return seed from file */
}

/* saves seed to file */
void SaveSeed(long seed) {
	FILE	*fp;				/* file ptr containing seed */
	char	*envpath;			/* enviromental path var */
	char	file[100];			/* path and file name */
	/* functions */
	char *getenv(char *);

	/* get path to simulation */
	if((envpath = getenv("SIMDIR")) == (char *)NULL) {
		printf("Error from SaveSeed: must 'setenv SIMDIR ~/hf'\n");
		exit(1);
	}
	strcpy(file, envpath);          /* add path to file name */
	strcat(file, SEEDFILE);         /* add filename to file name */

	if((fp = fopen(file, "w")) == NULL) {
		printf("Warning: seed file '%s' not found or write protected\n", SEEDFILE);
		return;
	}
	fprintf(fp, "%d", seed);
	fclose(fp);					/* close seed file */
}
