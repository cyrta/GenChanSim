/* COST-231-hata pathloss model

   Source:
   "Urban transmission loss models for mobile radio in the 900- and 1,800-MHz
   bands"(revision 2) COST231 TD(91) 73, Sept. 1991

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

   $Id: cost231hata.c,v 1.1 1994/09/16 11:22:23 jjb Exp $
   $Log: cost231hata.c,v $
 * Revision 1.1  1994/09/16  11:22:23  jjb
 * added section to check parameters to ensure they are in valid ranges.
 *
 * Revision 1.0  1994/09/07  13:54:53  jjb
 * Initial revision
 *

*/

#include <stdio.h>
#include <math.h>

static char rcsid[] = "$Id: cost231hata.c,v 1.1 1994/09/16 11:22:23 jjb Exp $";

enum  areaTypes { URBAN_MED_SMALL, URBAN_LARGE};

double cost231hata();

main(argc,argv)
int argc;
char *argv[];
{
  double planeEarthLoss;    /* plane earth propagation loss */
  double frequency;         /* carrier frequency */
  double baseHeight;        /* height of base station */
  double mobileHeight;      /* height of mobile station */
  double distance;          /* distance between stations */
  enum areaTypes area;      /* one of 4 areas for path loss */

  fprintf(stdout,"copyright (c) 1994 The MITRE Corporation Bedford, MA\n");
  fprintf(stdout,"COST 231 Hata path loss prediction model\n\n");

  /* input variables */

  if ( argc != 6 ) {
    fprintf(stderr,"area type(0=medium sized city and suburb. with mod. tree density\n,1=metropolitan centres) ? ");
    fscanf(stdin,"%d",&area);

    fprintf(stderr,"frequency(MHz)[1500-2000] ? ");
    fscanf(stdin,"%lf",&frequency);

    fprintf(stderr,"base height (meters)[30-200] ? ");
    fscanf(stdin,"%lf",&baseHeight);

    fprintf(stderr,"mobile height (meters)[1-10] ? ");
    fscanf(stdin,"%lf",&mobileHeight);

    fprintf(stderr,"distance height (kilometers)[1-20] ? ");
    fscanf(stdin,"%lf",&distance);

  } else {
    sscanf(argv[1],"%d",&area);
    sscanf(argv[2],"%lf",&frequency);
    sscanf(argv[3],"%lf",&baseHeight);
    sscanf(argv[4],"%lf",&mobileHeight);
    sscanf(argv[5],"%lf",&distance);

  }

  /* check if valid area */

  switch ( area) {

  case URBAN_MED_SMALL:

    /* Urban area: medium-small city */
    break;

  case URBAN_LARGE:

    /* Urban area: large city */
    break;

  default:
    fprintf(stderr,"invalid area\n");
    exit(-1);
  }

  /* test for valid parameters */

  if ( (frequency < 1500) || (frequency > 2000) ) {
    fprintf(stderr,"error: invalid frequency, must be in 1500-2000 MHz range\n");
    exit(-1);
  }

  if ( (baseHeight < 30) || (baseHeight > 200) ) {
    fprintf(stderr,"error: invalid base height, must be in 30-200 m range\n");
    exit(-1);
  }

  if ( (mobileHeight < 1) || (mobileHeight > 10) ) {
    fprintf(stderr,"error: invalid mobile height, must be in 1-10 m range\n");
    exit(-1);
  }

  if ( (distance < 1) || (distance > 20) ) {
    fprintf(stderr,"error: invalid distance, must be in 1-20 km range\n");
    exit(-1);
  }

  /* compute loss */

    planeEarthLoss = cost231hata(frequency, baseHeight, mobileHeight, distance, area);
   
    fprintf(stdout,"Cost231 Hata Model loss = %lf dB\n",planeEarthLoss);

}

double cost231hata( frequency, baseHeight, mobileHeight, distance, area )
  double frequency, baseHeight, mobileHeight, distance;
  enum areaTypes area;
{
  double planeEarthLoss;    /* plane earth propagation loss */
  double terCorFactor;      /* terrain dependent correction factor */

  terCorFactor = ( 1.1 * log10(frequency) - 0.7) * mobileHeight
    - ( 1.56 * log10(frequency) - 0.8);

  planeEarthLoss = 46.3 + 33.9 * log10(frequency) 
    - 13.82 * log10(baseHeight) - terCorFactor 
      + (44.9 - 6.55 * log10(baseHeight)) * log10(distance);

  switch ( area) {

  case URBAN_MED_SMALL:
    /* medium sized city and suburban centres with moderate tree density */
    break;

  case URBAN_LARGE:
    /* metropolitan centres */
    planeEarthLoss += 3;
    break;

  default:
    fprintf(stderr,"invalid area\n");
    exit(-1);
  }

  return(planeEarthLoss);
}
