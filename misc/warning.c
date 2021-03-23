/*

   copyright (c) 1994 The MITRE Corporation Bedford, MA

   $Id: warning.c,v 2.2 1994/07/25 18:30:41 jjb Exp $
   $Log: warning.c,v $
 * Revision 2.2  1994/07/25  18:30:41  jjb
 * added Copyright notice.
 *

*/
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

static char rcsid[] = "$Id: warning.c,v 2.2 1994/07/25 18:30:41 jjb Exp $";

/* warning: print an error message and die */
void warning(char *fmt, ...) {
	va_list args;

	va_start(args, fmt);
	fprintf(stderr, "warning ");
	vfprintf(stderr, fmt, args);
	fprintf(stderr, "\n");
	va_end(args);
}
