/* write out to tape drive in chunks.  If it is the first call, mode =
 * buffer size (usually 512, 1024, etc.).  For subsequent calls, mode is
 * ignored except if mode == 0.  If mode == 0, it is the last call and the
 * residual buffer is emptied after the data chunks are written.
 *
 * 'FWriteBuffer' returns the number of words written or -1 if an error occured 
 * Note that the number of words written may be less than the number sent,
 * because the storage buffer retains any modular data for the next call.
 *
 * r.coutts 10/7/92

   copyright (c) 1994 The MITRE Corporation Bedford, MA

   $Id: fwritebuffer.c,v 2.2 1994/07/25 18:30:41 jjb Exp $
   $Log: fwritebuffer.c,v $
 * Revision 2.2  1994/07/25  18:30:41  jjb
 * added Copyright notice.
 *

 *
 * buffer[] looks something like this:
 * |X|X|X|X|X|X|X|X|X|X|X|X|X|O|O|O|O|O|O|O|O|O|O|O| | | | | | | |
 *  0 1 . . .                 ^nbuffers             ^nbytes
 */
#include <stdio.h>
#include <malloc.h>
#include	<fwritebuffer.h>

static char rcsid[] = "$Id: fwritebuffer.c,v 2.2 1994/07/25 18:30:41 jjb Exp $";

int FWriteBuffer(char data[], int size, int ndatas, FILE *fp, int mode) {
	static char *buffer;			/* output buffer */
	static int nbuffers = 0;		/* number of bytes in buffer[] */
	static int buffersize;			/* size of buffer (usually 512 or 1024) */
	static int firstcall = 1;		/* flag for first call */
	int		rtn;					/* return from fwrite */
	int		nbytes = 0;				/* number of bytes outputted */
	int		nbyteswritten = 0;		/* number of bytes written to file */

	if(firstcall) {
		firstcall = 0;
		buffersize = mode;
		buffer = (char *)malloc(buffersize);
	}

	ndatas *= size; 				/* adjust to bytes */

	/* output residual and part of data if enough */
	if(nbuffers != 0 && nbuffers+ndatas >= buffersize) {
		nbytes = buffersize-nbuffers;
		ncpy(&buffer[nbuffers], data, nbytes);
		if((rtn = fwrite(buffer, 1, buffersize, fp)) != buffersize) return -1;
		nbuffers = 0;			/* buffer written */
		nbyteswritten += rtn;
	}

	/* output 'buffersize' chunks of data */
	for(; nbytes <= ndatas-buffersize; nbytes += buffersize) {
		if((rtn = fwrite(&data[nbytes], 1, buffersize, fp)) != buffersize) {
			return -1;
		}
		nbyteswritten += rtn;
	}

	/* save any residual data for next call */
	ncpy(&buffer[nbuffers], &data[nbytes], ndatas-nbytes);
	nbuffers += ndatas-nbytes;
	if(mode == 0) { /* output residual in buffer[] (last call) */
		if((rtn = fwrite(buffer, 1, nbuffers, fp)) != nbuffers) return -1;
		nbyteswritten += rtn;
	}
	/* return number of words written (may be less than requested, see header) */
	return nbyteswritten/size;
}

/* like strncpy, except NULL is ignored */
void ncpy(char dest[], char source[], int n) {
	int		i;

	for(i = 0; i < n; i++) dest[i] = source[i];
}
