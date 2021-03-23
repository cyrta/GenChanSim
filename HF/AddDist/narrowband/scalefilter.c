/* function to scale filter coefficient to maintain unity throughput */
/*

   copyright (c) 1994 The MITRE Corporation Bedford, MA

   $Id: scalefilter.c,v 2.2 1994/07/25 19:06:52 jjb Exp $
   $Log: scalefilter.c,v $
 * Revision 2.2  1994/07/25  19:06:52  jjb
 * added copyright notice
 *

*/
#include <stdio.h>

#define NPTS 100

static char rcsid[] = "$Id: scalefilter.c,v 2.2 1994/07/25 19:06:52 jjb Exp $";

void main() {
	char file[100];		/* file name */
	float w[NPTS];		/* weights */
	int	i = 0;			/* w[] index */
	int npts;			/* npts in file[] */
	float m;			/* scale factor */
	FILE *fp;

	/* get info */
	printf("filter file: ");
	scanf("%s", file);
	printf("enter scale factor: ");
	scanf("%f", &m);

	/* read file */
	if((fp = fopen(file, "r")) == NULL) {
		printf("Error: '%s' not opened for reading.\n", file);
		exit(1);
	}
	printf("reading '%s'...", file); fflush(stdout);
	while(fscanf(fp, "%f", &w[i]) != EOF && i < NPTS) i++;
	if(i == NPTS) {
		printf("Error: file too long (>%d pts)\n", NPTS);
		exit(1);
	}
	npts = i;
	printf("%d pts read.\n", npts);
	fclose(fp);

	/* write file */
	if((fp = fopen(file, "w")) == NULL) {
		printf("Error: '%s' not opened for writing.\n", file);
		exit(1);
	}
	for(i = 0; i < npts; i++) {
		w[i] *= m;
		fprintf(fp, "%g\n", w[i]);
	}
}
