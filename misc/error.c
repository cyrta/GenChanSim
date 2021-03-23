/*

   copyright (c) 1994 The MITRE Corporation Bedford, MA

   $Id: error.c,v 2.2 1994/07/25 18:30:41 jjb Exp $
   $Log: error.c,v $
 * Revision 2.2  1994/07/25  18:30:41  jjb
 * added Copyright notice.
 *

*/
#include <stdio.h>
#include <stdarg.h>

static char rcsid[] = "$Id: error.c,v 2.2 1994/07/25 18:30:41 jjb Exp $";

/* error: print an error message and die */
void error(char *fmt, ...) {
	va_list args;

	va_start(args, fmt);
	fprintf(stderr, "error ");
	vfprintf(stderr, fmt, args);
	fprintf(stderr, "\n");
	va_end(args);
	exit(1);
}
