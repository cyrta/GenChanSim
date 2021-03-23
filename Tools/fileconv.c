/* fileconv.c converts a binary stream file to an ascii stream file.
 * rcoutts

   copyright (c) 1994 The MITRE Corporation Bedford, MA

   $Id: fileconv.c,v 2.2 1994/07/25 18:30:41 jjb Exp $
   $Log: fileconv.c,v $
 * Revision 2.2  1994/07/25  18:30:41  jjb
 * added Copyright notice.
 *

 */

#include <stdio.h>
#include <string.h>
/* local includes */
#include <complex.h>
#include <Cmdline.h>
#include <pathfile.h>

#define		NPTS	512			/* block size of data */
#define		NCHARS	100			/* # chars in file name */

typedef struct {				/* matlab header */
	int type;
	int mrows;
	int ncols;
	int imagf;
	int namlen;
} Fmatrix;

static char rcsid[] = "$Id: fileconv.c,v 2.2 1994/07/25 18:30:41 jjb Exp $";

void main(int argc, char **argv) {
	char	infile[NCHARS];			/* input file */
	char	outfile[NCHARS] = "";	/* output file */
	char	varname[NCHARS];		/* matrix name */
	fcomplex data[NPTS];			/* data read in */
	int		i;						/* index */
	int		npts;					/* number of points read */
	int		totalnpts = 0;			/* total number of points written */
	int		matlab = 0;				/* matlab format flag */
	int		ascii = 0;				/* ascii format flag */
	char	*c;						/* character pointer */
	char	*helpStr;				/* help string */
	Fmatrix	x;						/* header */
	FILE	*infp, *outfp;			/* i/o file ptrs */
	/* functions */
	void usage();

	CmdFile("-i Input_File", infile, NCHARS, cmdMan);
	CmdFile("-o Output_File", outfile, NCHARS, cmdOpt);
	CmdFlag("-m Matlab_3.5_Format", &matlab);
	CmdFlag("-a ASCII_Format", &ascii);
	CmdHelp(helpStr = PathFile("Tools/fileconv.hlp"));
	CmdDep(&matlab, &ascii, cmdRadioBox);
	CmdArgs(argc, argv);
	CmdUnMan();
	free(helpStr);

	/* set up output file name */
	if(outfile[0] == '\0') { /* no file name input */
		strcpy(outfile, infile);	/* same name for output */
		if((c = strchr(outfile, '.')) != NULL) *c = '\0'; /* rm suffix */
		if(matlab) {
			strcat(outfile, ".mat");
		} else {
			strcat(outfile, ".asc");
		}
	}

	/* open files */
	if((infp = fopen(infile, "r")) == NULL) {
		printf("Error from fileconv: input file '%s' not opened\n", infile);
		exit(1);
	}
	if((outfp = fopen(outfile, "w")) == NULL) {
		printf("Error from fileconv: output file '%s' not opened\n", outfile);
		exit(1);
	}
	if(matlab) {
		/* get variable name */
		if((c = strrchr(infile, '/')) != NULL) strcpy(varname, c+1);
		else strcpy(varname, infile); /* no path */
		if((c = strchr(varname, '.')) != NULL) *c = '\0'; /* rm suffix */
		x.namlen = strlen(varname)+1;
		x.imagf = 1;		/* imaginary data */
		x.ncols = 1;
		x.type = 1010;	/* [Sun columns float matrix] */
		fwrite(&x, sizeof(Fmatrix), 1, outfp);
		fwrite(varname, sizeof(char), x.namlen, outfp);
		/* read in file and write out real part */
		printf("real part"); fflush(stdout);
		do {
			npts = fread(data, sizeof(fcomplex), NPTS, infp);
			for(i = 0; i < npts; i++) {
				fwrite(&data[i].r, sizeof(float), 1, outfp);
			}
			totalnpts += npts;
			printf("."); fflush(stdout);
		} while(npts == NPTS);
		/* rewind input file and read imaginary */
		rewind(infp);
		printf("\nimag part"); fflush(stdout);
		do {
			npts = fread(data, sizeof(fcomplex), NPTS, infp);
			for(i = 0; i < npts; i++) {
				fwrite(&data[i].i, sizeof(float), 1, outfp);
			}
			printf("."); fflush(stdout);
		} while(npts == NPTS);
		fclose(infp);
		/* rewind output file and write the number of samples to header */
		x.mrows = totalnpts;
		rewind(outfp);
		fwrite(&x, sizeof(Fmatrix), 1, outfp);
		fclose(outfp);
	} else {	/* output ascii */
		printf("writing '%s'", outfile); fflush(stdout);
		do {
			npts = fread(data, sizeof(fcomplex), NPTS, infp);
			for(i = 0; i < npts; i++) {
				fprintf(outfp, "%13.6e\t%13.6e\n", data[i].r, data[i].i);
			}
			totalnpts += npts;
			printf("."); fflush(stdout);
		} while(npts == NPTS);
	}
	printf("\n%d complex points written to '%s'\n", totalnpts, outfile);
}
