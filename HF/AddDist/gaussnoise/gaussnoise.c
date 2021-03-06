/* Gaussian noise generator
 * r.coutts

   copyright (c) 1994 The MITRE Corporation Bedford, MA

   $Id: noise.c,v 2.2 1994/07/25 19:12:54 jjb Exp $
   $Log: noise.c,v $
 * Revision 2.2  1994/07/25  19:12:54  jjb
 * Added copyright notice.
 *

		GENERAL PUBLIC LICENSE
		----------------------

	The MITRE Corporation (MITRE) provides this software to you
without charge to use, copy, modify or enhance for any legitimate
purpose provided you reproduce MITRE's copyright notice in any copy or
derivative work of this software.

	This software is the copyright work of MITRE. No ownership or
other proprietary interest in this software is granted you other than
what is granted in this license.

	Any modification or enhancement of this software must identify
the part of this software that was modified, by whom and when, and
must inherit this license including its warranty disclaimers.

	MITRE IS PROVIDING THE PRODUCT "AS IS" AND MAKES NO WARRANTY,
EXPRESS OR IMPLIED, AS TO THE ACCURACY, CAPABILITY, EFFICIENCY OR
FUNCTIONING OF THIS SOFTWARE AND DOCUMENTATION. IN NO EVENT WILL MITRE
BE LIABLE FOR ANY GENERAL, CONSEQUENTIAL, INDIRECT, INCIDENTAL,
EXEMPLARY OR SPECIAL DAMAGES, EVEN IF MITRE HAS BEEN ADVISED OF THE
POSSIBILITY OF SUCH DAMAGES.

	You, at your expense, hereby indemnify and hold harmless
MITRE, its Board of Trustees, officers, agents and employees, from any
and all liability or damages to third parties, including attorney's
fees, court costs, and other related costs and expenses, arising out
of your use of this software irrespective of the cause of said
liability.

	The export from the United States or the subsequent reexport
of this software is subject to compliance with United States export
control and munitions control restrictions. You agree that in the
event you seek to export this software or any derivative work thereof,
you assume full responsibility for obtaining all necessary export
licenses and approvals and for assuring compliance with applicable
reexport restrictions.

 */
#include <stdio.h>
#include <math.h>
/* local includes */
#include <Cmdline.h>
#include <complex.h>
#include <pathfile.h>

#define	NPTS	100000	/* number of points to generate at a time */

static char rcsid[] = "$Id: noise.c,v 2.2 1994/07/25 19:12:54 jjb Exp $";

void main(int argc, char **argv) {
	char		outfile[100] = "noise.dat";	/* output file name */
	int			srateHz = 1.0;/* # samples/sec (or total number of samples
								 * if 'tT' is not entered) */
	float		tT = 1.0;		/* total time of output */
	int			nsamples;		/* number of samples (tT*srateHz*1000000) */
	int			block;			/* number of samples to output at a time */
	fcomplex	iq[NPTS];		/* complex gaussian noise samples */
	float		factor = 1.0;	/* multiplication factor */
	int			i;				/* index to iq[] */
	char		*helpStr;		/* help string */
	FILE		*outfp;			/* output file pointer */


	/* print copyright notice */

	fprintf(stdout,"copyright (c) 1994 The MITRE Corporation Bedford, MA\n");

	CmdFloat("-t Time(seconds)", &tT, cmdDef);
	CmdInt("-s Sample Rate(MHz)", &srateHz, cmdDef);
	CmdFloat("-a Amplitude", &factor, cmdDef);
	CmdFile("-o Output File", outfile, 100, cmdMan);
	CmdHelp(helpStr = PathFile("HF/AddDist/gaussnoise/gaussnoise.hlp"));
	CmdArgs(argc, argv);
	CmdUnMan();
	free(helpStr);

	/* open output file */
	if((outfp = fopen(outfile, "w")) == NULL) {
		printf("Error from noise: output file '%s' not opened\n", outfile);
	}

	/* write blocks of complex gaussian numbers to file */
	nsamples = tT*srateHz*1000000;	/* total number of samples */
	printf("processing [%d]", NPTS); fflush(stdout);
	do {
		if(NPTS < nsamples) block = NPTS;
		else block = nsamples;
		nsamples -= NPTS;
		cgauss48(iq, block);
		if(factor != 1.0) {
			for(i = 0; i < block; i++) {
				iq[i].r *= factor;
				iq[i].i *= factor;
			}
		}
		fwrite(iq, sizeof(fcomplex), block, outfp);
		printf("."); fflush(stdout);
	} while (nsamples > 0);
}
