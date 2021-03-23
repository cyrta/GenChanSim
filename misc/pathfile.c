/* prepend the ~/hf path to a file name
 * r.coutts

   copyright (c) 1994 The MITRE Corporation Bedford, MA

   $Id: pathfile.c,v 2.2 1994/07/25 18:30:41 jjb Exp $
   $Log: pathfile.c,v $
 * Revision 2.2  1994/07/25  18:30:41  jjb
 * added Copyright notice.
 *

 */

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

#define	NCHARS	250			/* # chars in elongated file name */

static char rcsid[] = "$Id: pathfile.c,v 2.2 1994/07/25 18:30:41 jjb Exp $";

char *PathFile(char *file) {
	char	*envpath;		/* path to enviromental variable */
	char	*pathAndFile;	/* entire path and file name */

	/* get memory */
	pathAndFile = (char *)malloc(NCHARS);
	/* get path to simulation */
	if((envpath = getenv("HFDIR")) == (char *)NULL) {
		printf("Error from GetFileName:  must 'setenv HFDIR ~/hf'\n");
		exit(1);
	}
	strcpy(pathAndFile, envpath);/* add path to file name */
	if(pathAndFile[strlen(pathAndFile)-1] != '/') strcat(pathAndFile, "/");
	strcat(pathAndFile, file);
	/* check for over-write */
	if(strlen(pathAndFile) > NCHARS) {
		printf("Error from PathFile: File name '%s' longer than %d "
			"characters\n" , pathAndFile, NCHARS);
		exit(1);
	}
	return pathAndFile;
}
