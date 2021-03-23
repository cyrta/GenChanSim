/* filesum.c reads in the files listed on the command line, sums corresponding
 * members, and writes the output to the specified -O file.
 * rcoutts

   copyright (c) 1994 The MITRE Corporation Bedford, MA

   $Id: filesum.c,v 2.2 1994/07/25 18:30:41 jjb Exp $
   $Log: filesum.c,v $
 * Revision 2.2  1994/07/25  18:30:41  jjb
 * added Copyright notice.
 *

 */

#include	<stdio.h>
#include	<math.h>
#include	<string.h>
/* local includes */
#include	<complex.h>
#include	<Cmdline.h>
#include	<fwritebuffer.h>
#include	<pathfile.h>

#define		NPTS		100000
#define		NFILES		5
#define		BUFFSIZE	512

static char rcsid[] = "$Id: filesum.c,v 2.2 1994/07/25 18:30:41 jjb Exp $";

void main(int argc, char **argv) {
	char	inputfile[NFILES][100];		/* input file */
	char	outputfile[100];			/* output file */
	fcomplex data[NPTS];				/* data read in */
	fcomplex sum[NPTS];					/* data read in */
	int		i;							/* index */
	int		f;							/* file index */
	int		npts = 0;					/* number of points read */
	int		lastnpts = 0;				/* total number of points written */
	int		sumnpts = 0;				/* number of points in sum[] */
	int		nfiles = 0;					/* number of files read in */
	float	gain[NFILES] = {0.0, 0.0, 0.0, 0.0, 0.0};/* factor for ea. file */
	float	factor[NFILES];				/* factor for ea. file */
	char	*helpStr;					/* help string */
	FILE	*infp[NFILES];				/* input file pointers */
	FILE	*outfp;						/* output file pointer */

	/* init gains (dB) */
	for(i = 0; i < NFILES; i++) gain[i] = 0.0;

	/* get command line arguments */
	CmdFile("-o Output_File", outputfile, 100, cmdMan);
	CmdString("-i Input_File", inputfile, NFILES, 100, cmdMan);
	CmdFloat("-g Gain_(dB)", gain, NFILES, cmdDef);
	CmdHelp(helpStr = PathFile("misc/filesum.hlp"));
	CmdArgs(argc, argv);
	CmdUnMan();
	free(helpStr);

	/* get the number of files input */
	for(f = 0; f < NFILES; f++) {
		if(inputfile[f][0] == '\0') {
			nfiles = f;
			break;
		}
		if((infp[f] = fopen(inputfile[f], "r")) == NULL) {
			printf("Error from filesum: input file '%s' not opened\n", 
				inputfile[f]);
			exit(1);		/* quit */
		}
	}
	/* convert gain from dB */
	for(f = 0; f < nfiles; f++) {
		factor[f] = pow(10.0, gain[f]/20.0);
	}
	/* open output file */
	if((outfp = fopen(outputfile, "w")) == NULL) {
		printf("Error from filesum: output file '%s' not opened\n", outputfile);
	}
	/* loop until file is empty */
	do {
		for(f = 0; f < nfiles; f++) {
			lastnpts = npts;
			npts = fread(data, sizeof(fcomplex), NPTS, infp[f]);
			if(f == 0) {
				/* initiate sum array */
				sumnpts = npts;
				for(i = 0; i < npts; i++) {
					sum[i].r = factor[f]*data[i].r;
					sum[i].i = factor[f]*data[i].i;
				}
			} else {
				/* integrate sum array */
				for(i = 0; i < npts; i++) {
					sum[i].r += factor[f]*data[i].r;
					sum[i].i += factor[f]*data[i].i;
				}
				if(lastnpts != npts) {
					if(sumnpts < npts) sumnpts = npts;
					printf("WARNING from filesum: size diff, %s:%d %s:%d\n",
						inputfile[f], npts, inputfile[f-1], lastnpts);
				}
			}
		}
		FWriteBuffer((char *)sum, sizeof(fcomplex), sumnpts, outfp, BUFFSIZE);
	} while(npts == NPTS);
	FWriteBuffer((char *)sum, sizeof(fcomplex), sumnpts, outfp, 0);
}
