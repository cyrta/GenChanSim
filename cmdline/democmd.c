/* example program to demonstrate some 'cmdline' functions */
/*

   copyright (c) 1994 The MITRE Corporation Bedford, MA

   $Id: democmd.c,v 2.2 1994/07/25 18:18:32 jjb Exp $
   $Log: democmd.c,v $
 * Revision 2.2  1994/07/25  18:18:32  jjb
 * Added copyright notice
 *

*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <Cmdline.h>
#define	MAXB		10						/* max vector length */
#define	NCHARS	100							/* number characters in outfile[] */

extern void TimesPi(float *);

static char rcsid[] = "$Id: democmd.c,v 2.2 1994/07/25 18:18:32 jjb Exp $";

void main(int argc, char **argv) {
	int			multiply = 0;				/* flag to multiply a*B */
	int			divide = 0;					/* flag to divide B/a */
	int			sum = 0;					/* flag to sum B */
	int			absval = 0;					/* flag to take abs(a) */
	float		a = 1;						/* scalar value */
	float		b[MAXB];					/* vector */
	float		c[MAXB];					/* output vector */
	char		outfile[NCHARS];			/* output file name */
	int			i;							/* index */
	int			nb = 0;						/* number of members in b[] */
	float		total = 0;					/* sum total */
	CmdInfo		*cmdof;						/* argument structure for outfile */
	char		*helpstr = "Sorry, you're on your own!";	/* help string */
	FILE		*outfp = stdout;			/* output direction */
	/* functions */
	void outputvector(FILE *, float [], int);

	/* interface */
	CmdFloat("-Scalar", &a, cmdDef);			/* scalar value */
	CmdFunc("-ds Scalar_x_PI", &TimesPi, &a);	/* a = a * PI */
	CmdInt("-n Length_of_Vector", &nb, cmdMan);	/* number of elements in 'b' */
	CmdFloat("-v Vector", b, MAXB, cmdMan);		/* 'b' is a vector */
	CmdSpace();									/* add spacing */
	CmdFlag("-m Scalar*Vector", &multiply);		/* mult vector and scalar */
	CmdFlag("-d Vector/Scalar", &divide);		/* divide vector by scalar */
	CmdFlag("-s sum(Vector)", &sum);			/* sum vector */
	CmdFlag("-abs abs(Scalar)", &absval);		/* absolute value */
	CmdSpace();									/* add spacing */
	cmdof = CmdFile("-F Vector_Output_File", outfile, NCHARS, cmdOpt);
	CmdSpace();									/* add spacing */
	/* create dependecies */
	CmdDep(&multiply, &divide, &sum, &absval, cmdRadioBox);	/* radio-box */
	CmdDep(&multiply, &a, b, &nb, outfile, &TimesPi, cmdSensitive);
	CmdDep(&divide, &a, b, &nb, outfile, &TimesPi, cmdSensitive);
	CmdDep(&sum, b, &nb, outfile, cmdSensitive);
	CmdDep(&absval, &a, &TimesPi, cmdSensitive);
	CmdDep(&nb, b, cmdArray);
	CmdHelp(helpstr);							/* set up help */
	/* create interface */
	CmdArgs(argc, argv);				/* get user args */
	/* open file */
	if(cmdof->entered) {				/* if user entered output file... */
		if((outfp = fopen(outfile, "w")) == NULL) {
			printf("ERROR: output file '%s' not opened\n", outfile);
			exit(1);
		}
	}
	if(multiply) {
		for(i = 0; i < nb; i++) c[i] = a * b[i];
		fprintf(outfp, "%g * ", a);
		outputvector(outfp, b, nb);
		fprintf(outfp, " = ");
		outputvector(outfp, c, nb);
		fprintf(outfp, "\n");
	}
	if(divide) {
		for(i = 0; i < nb; i++) c[i] = b[i] / a;
		outputvector(outfp, b, nb);
		fprintf(outfp, " / %g = ", a);
		outputvector(outfp, c, nb);
		fprintf(outfp, "\n");
	}
	if(sum) {
		fprintf(outfp, "sum( ");
		outputvector(outfp, b, nb);
		for(i = 0; i < nb; i++) total += b[i];
		fprintf(outfp, " ) = %g\n", total);
	}
	if(absval) {
		fprintf(outfp, "abs( %g ) = %g\n", a, fabs(a));
	}
	if(outfp != stdout) printf("output written to '%s'\n", outfile);
}

/* multiply value by PI */
void TimesPi(float *value) {
	*value = *value * 3.14159;
}

/* writes vector to a file */
void outputvector(
	FILE		*fp,		/* output file */
	float		v[],		/* output vector */
	int			n)			/* length of output vector */
{
	int		i;		/* index */

	fprintf(fp, "[ ");
	for(i = 0; i < n; i++) fprintf(fp, "%g ", v[i]);
	fprintf(fp, "]");
}
