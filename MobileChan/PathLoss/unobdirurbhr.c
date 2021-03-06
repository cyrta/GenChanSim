/* Unobstructed Direct Path Urban High Rise pathloss model
   Official name:
        1st order dielectric canyon
   Source:
   "Project 8632L - final report", MITRE Corp. Sept. 1994

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

   $Id: unobdirurbhr.c,v 1.1 1994/09/29 11:31:07 jjb Exp $
   $Log: unobdirurbhr.c,v $
 * Revision 1.1  1994/09/29  11:31:07  jjb
 * changed name printed when program starts to
 * "1st order dielectric canyon"
 *
 * Revision 1.0  1994/09/07  13:56:48  jjb
 * Initial revision
 *
   
*/

#include <stdio.h>
#include <math.h>

static char rcsid[] = "$Id: unobdirurbhr.c,v 1.1 1994/09/29 11:31:07 jjb Exp $";

double unobDirUrbHR();

main(argc,argv)
int argc;
char *argv[];
{
  double loss;              /* loss */
  double frequency;         /* carrier frequency */
  double baseHeight;        /* height of base station */
  double mobileHeight;      /* height of mobile station */
  double distance;          /* distance between stations */
  double slope;             /* far region slope */

  fprintf(stdout,"copyright (c) 1994 The MITRE Corporation Bedford, MA\n");
  fprintf(stdout,"1st order Dielectric Canyon\n\n");

  /* input variables */

  if ( argc != 6 ) {
    fprintf(stderr,"frequency(MHz) ? ");
    fscanf(stdin,"%lf",&frequency);

    fprintf(stderr,"base height (meters) ? ");
    fscanf(stdin,"%lf",&baseHeight);

    fprintf(stderr,"mobile height (meters) ? ");
    fscanf(stdin,"%lf",&mobileHeight);

    fprintf(stderr,"distance (km) ? ");
    fscanf(stdin,"%lf",&distance);

    fprintf(stderr,"far region slope (-1 = default 0f 40) ? ");
    fscanf(stdin,"%lf",&slope);
    if (slope < 30 || slope > 70 )
      slope = 40;
  } else {

    sscanf(argv[1],"%lf",&frequency);
    sscanf(argv[2],"%lf",&baseHeight);
    sscanf(argv[3],"%lf",&mobileHeight);
    sscanf(argv[4],"%lf",&distance);
    sscanf(argv[5],"%lf",&slope);
    if (slope < 30 || slope > 70 )
      slope = 40;

  }

  distance = distance * 1000; /* convert km -> m */

  loss = unobDirUrbHR(frequency, baseHeight, mobileHeight, distance, slope);
   
  fprintf(stdout,"loss = %lf dB\n",loss);
  fprintf(stdout,"standard deviation = 5dB\n");

}

double unobDirUrbHR( frequency, baseHeight, mobileHeight, distance, slope )
  double frequency, baseHeight, mobileHeight, distance, slope;
{
  double fresnelLoss;       /* Fresnel Loss */
  double nD;                /* nD function */
  double fresnelDistance;   /* fresnel distance */
  double wavelength;        /* wavelength */
  double loss;              /* loss */
  double tmp;
  double pi;

  pi=4.0*atan(1.0);

  /* compute wavelength of frequency(MHz) */

  wavelength = 3e8/(frequency * 1e6);

  /* compute Fresnel Distance */

  tmp = 4.0 * baseHeight * mobileHeight / wavelength;
  fresnelDistance = sqrt(tmp*tmp 
			 - (baseHeight*baseHeight + mobileHeight*mobileHeight)
			 + (wavelength*wavelength)/16);

  /* compute Fresnel Loss */

  fresnelLoss = 20.0 * log10( 2.0 * pi * fresnelDistance / wavelength);

  /* compute n(D) */

  if ( distance < fresnelDistance )
    nD = 18;
  else
    nD = slope;

  /* compute loss */

  loss = fresnelLoss + nD * log10(distance/fresnelDistance);

  return(loss);
}
