/*
 * ploss_jtc.c:  Implements the path loss models outlined in the JTC Air
 * Interface Standard JTC(AIR)/94.09.23-065R6 for an outdoor base station
 * antenna above, at, and below the rooftop level (section 3.4, 3.5, 3.6).
 *
 * Vegetation effects are modeled using the Modified Exponential Decay
 * (MED) model for the simple case of propagation through trees
 * with leaves.
 * 
 * For further details, see documentation
 * 
 * Written by:
 * 
 * The MITRE Corporation Bedford, MA Sept., 1994
 * 

   copyright (c) 1994 The MITRE Corporation Bedford, MA

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

   $Id: ploss_jtc.c,v 1.2 1994/10/21 16:34:45 jjb Exp $
   $Log: ploss_jtc.c,v $
 * Revision 1.2  1994/10/21  16:34:45  jjb
 * modified code to reflect version R6 of JTC document.
 * changed assumption of build sep b=width used in above and at roof
 * with b=2*width.
 * Removed Cost 231-Walfish-Ikegami for below roof and replaced it
 * with JTC's new formula
 *
 * Revision 1.1  1994/09/16  11:24:30  jjb
 * fixed error in computing free space loss when below roof.
 * had to move calculation to before converted distance to km.
 *
 * Revision 1.0  1994/09/07  13:56:48  jjb
 * Initial revision
 *

 */

#define PI 3.141592653589793
#define C 3.0E8
#include <stdio.h>
#include <math.h>
#include <strings.h>

static char rcsid[] = "$Id: ploss_jtc.c,v 1.2 1994/10/21 16:34:45 jjb Exp $";

main(argc, argv)
	int             argc;
	char           *argv[];

{



	int             environ = 0, model = 0;	/* Environmental descriptor */
	float           f, hb, hm, d, hr;	/* User inputs */
	float           height, diff;	/* Intermediates */
	float           dbp, wavelen, width;
	float           radius, theta, tmp1, tmp2;
	float           sig, deltah, L, Lfs, alpha;
	float           b;
	float           phi, rho;
	float           tmp3,tmp4;

	fprintf(stdout,"copyright (c) 1994 The MITRE Corporation Bedford, MA\n");
	fprintf(stdout,"JTC Path Loss model\n");

	/* Get the input parameters */

	if ( argc == 1 ) {
	  printf("enter Environment type:\n");
	  printf("[0]  Urban High-Rise    [1]  Urban/Suburban\n");
	  printf("[2]  Residential        [3]  Rural\n");
	  printf("[4]  Vegetation (MED Model Only)\n");
	  scanf("%d", &environ);

	  if ((environ < 0) || (environ > 4)) {
	    fprintf(stderr, "Re-enter environment type\n");
	    exit(-1);
	  }


	  printf("enter Center frequency [MHz]\n");
	  scanf("%f", &f);

	  if (environ != 4) {
	    printf("enter Base Station antenna height [m]\n");
	    scanf("%f", &hb);

	    printf("enter Mobile antenna height [m]\n");
	    scanf("%f", &hm);

	    printf("enter average rooftop elevation [m]\n");
	    scanf("%f", &hr);
	  }

	  printf("enter Base - Mobile separation distance [m]\n");
	  scanf("%f", &d);

	} else if ( argc == 4 ) {
	  sscanf(argv[1],"%d", &environ);

	  if (environ != 4 ) {
	    fprintf(stderr, "Incorrect number of arguments for non-Vegetation\n");
	    exit(-1);
	  }

	  sscanf(argv[2],"%f", &f);
	  sscanf(argv[3],"%f", &d);

	} else if ( argc == 7 ) {
	  sscanf(argv[1],"%d", &environ);

	  if ((environ < 0) || (environ > 4)) {
	    fprintf(stderr, "Re-enter environment type\n");
	    exit(-1);
	  }
	  if ( environ == 4 ) {
	    fprintf(stderr, "Incorrect number of arguments for Vegetation\n");
	    exit(-1);
	  }

	  sscanf(argv[2],"%f", &f);
	  sscanf(argv[3],"%f", &hb);
	  sscanf(argv[4],"%f", &hm);
	  sscanf(argv[5],"%f", &hr);
	  sscanf(argv[6],"%f", &d);

	} else {
	  fprintf(stderr,"Incorrect number of arguments\n");
	  exit(-1);
	}

	/* Determine if the antenna is below, at, or above the roofline */

	/* model codes are:  [0] below, [1] at, [2] above, [3] Vegetation */

	diff = hb - hr;
	wavelen = C / (f * 1E6);

	if (environ == 0) {	/* If Urban High-Rise */
		if (diff > -2.5) {	/* NOT below */
			if (diff > 2.5) {	/* NOT at */
				model = 2;	/* Must be above */
				width = 25;
				sig = 10;
				printf("Using Urban High-Rise, Above the Rooftop Model\n");
			} else {
				model = 1;	/* Must be at */
				width = 25;
				sig = 12;
				printf("Using Urban High-Rise, At Rooftop Model\n");
			}
		} else {
			model = 0;	/* Must be below */
			width = 25;
			sig = 10;
			printf("Using Urban High-Rise, Below Rooftop Model\n");
		}
	} else if (environ == 1) {	/* If Urban/Suburban */
		if (diff > -1.5) {	/* NOT below */
			if (diff > 1.5) {	/* NOT at */
				model = 2;	/* Must be above */
				width = 35;
				sig = 10;
				printf("Using Urban/Suburban, Above the Rooftop Model\n");
			} else {
				model = 1;	/* Must be at */
				width = 35;
				sig = 10;
				printf("Using Urban/Suburban, At Rooftop Model\n");
			}
		} else {
			model = 0;	/* Must be below */
			width = 35;
			sig = 10;
			printf("Using Urban/Suburban, Below Rooftop Model\n");
		}
	} else if (environ == 2) {	/* If Residential */
		if (diff > -1.0) {	/* NOT below */
			if (diff > 1.0) {	/* NOT at */
				model = 2;	/* Must be above */
				width = 30;
				sig = 10;
				printf("Using Residential, Above the Rooftop Model\n");
			} else {
				model = 1;	/* Must be at */
				width = 30;
				sig = 8;
				printf("Using Residential, At Rooftop Model\n");
			}
		} else {
			model = 0;	/* Must be below */
			width = 30;
			sig = 8;
			printf("Using Residential, Below Rooftop Model\n");
		}
	} else if (environ == 3) {	/* If Rural */
		if (diff > -1.0) {	/* NOT below */
			if (diff > 1.0) {	/* NOT at */
				model = 2;	/* Must be above */
				width = 50;
				sig = 10;
				printf("Using Rural, Above the Rooftop Model\n");
			} else {
				model = 1;	/* Must be at */
				width = 50;
				sig = 8;
				printf("Using Rural, At Rooftop Model\n");
			}
		} else {
			model = 0;	/* Must be below */
			width = 50;
			sig = 8;
			printf("Using Rural, Below Rooftop Model\n");
		}
	} else if (environ == 4) {	/* If Vegetation */
		model = 3;
		sig = 10;
	}

	/* compute building seperation b = 2 * width */

	b = 2 * width;

	/* We now know the environment and the model to use .... */

	if (model == 0) {	/* Below-the-roof model */

	        diff = hr - hb;    /* note: this forces diff to be positive */
		deltah = hr - hm;
		radius = sqrt((deltah * deltah) + (width / 2.0) * (width / 2.0));
		theta = atan(deltah * 2.0 / width);
		rho = sqrt((diff * diff) + (b * b));
		phi = atan(diff/b);
		if (theta < 1E-6) {
			fprintf(stderr, "Diffraction Angle too small; Change building or antenna heights\n");
			exit(-1);
		}
		tmp1 = pow(((1.0 / theta) - 1.0 / (2.0 * PI + theta)), 2.0);
		tmp2 = pow(((1.0 / phi) - 1.0 / (2.0 * PI + phi)), 2.0);
		tmp3 = 10 * log10((wavelen/(2 * PI * PI * radius)) * tmp1);
		tmp4 = 10 * log10( pow(b/(2 * PI * d),2.0) * (wavelen/rho) * tmp2);
		Lfs = 10 * log10(pow(wavelen / (4 * PI * d), 2.0));
		L = Lfs + tmp3 + tmp4;
		if (-L < -Lfs) {
			fprintf(stderr, "Model Integrity Violated, Path Loss < Free Space Loss, L, Lfs = %2.2f %2.2f\n", L, Lfs);
			exit(-1);
		}
	} else if (model == 1) {/* At-the-roof model */

		deltah = hr - hm;
		radius = sqrt((deltah * deltah) + (width / 2.0) * (width / 2.0));
		theta = atan(deltah * 2.0 / width);
		if (theta < 1E-6) {
			fprintf(stderr, "Diffraction Angle too small; Change building or antenna heights\n");
			exit(-1);
		}
		tmp1 = 10 * log10(1.0 / (pow(d, 4.0))) + 10 * log10(pow(wavelen * b / (4.0 * PI), 2.0));
		tmp2 = pow(((1.0 / theta) - 1.0 / (2.0 * PI + theta)), 2.0);
		L = 10 * log10((wavelen / (2 * PI * PI * radius)) * tmp2) + tmp1;
		Lfs = 10 * log10(pow(wavelen / (4 * PI * d), 2.0));
		if (-L < -Lfs) {
			fprintf(stderr, "Model Integrity Violated, Path Loss < Free Space Loss, L, Lfs = %2.2f %2.2f\n", L, Lfs);
			exit(-1);
		}
	} else if (model == 2) {/* Above-the-roof model */
		deltah = hr - hm;
		diff = hb - hr;
		theta = atan(deltah * 2.0 / width);
		if (theta < 1E-6) {
			fprintf(stderr, "Diffraction Angle too small; Change building or antenna heights\n");
			exit(-1);
		}
		radius = sqrt((deltah * deltah) + (width / 2.0) * (width / 2.0));
		tmp1 = 10 * log10(1.0 / pow(d, 3.8));
		tmp2 = 10 * log10(pow((2.35 * wavelen / (4.0 * PI)), 2.0) * pow((diff * sqrt((b) / wavelen)), 1.8));
		L = tmp1 + tmp2 + 10 * log10((wavelen / (2.0 * PI * PI * radius)) * pow((1.0 / theta - 1.0 / (2.0 * PI + theta)), 2.0));
		Lfs = 10 * log10((wavelen / (4.0 * PI * d)) * (wavelen / (4.0 * PI * d)));
		if (-L < -Lfs) {
			fprintf(stderr, "Model Integrity Violated, Path Loss < Free Space Loss, L, Lfs = %2.2f %2.2f\n", L, Lfs);
			exit(-1);
		}
	} else if (model == 3) {/* Vegetation MED model */
		if ((d > 0) && (d <= 14)) {
			alpha = 0.45 * pow((f / 1000.0), 0.284);
		} else if ((d < 400) && (d > 14)) {
			alpha = 1.33 * pow((f / 1000.0), 0.284) * 1.0 / pow(d, 0.412);
		} else {
			fprintf(stderr, "Range must be less than 400 meters\n");
			exit(-1);
		}
		Lfs = 10 * log10(pow(wavelen / (4 * PI * d), 2.0));
		L = -(alpha * d) + Lfs;
	}			/* End If */
	printf("L = %3.2f [dB]\n", L);
	printf("Lfs = %3.2f [dB]\n", Lfs);
	printf("Standard Deviation = %2.1f\n", sig);

}				/* End Main */
