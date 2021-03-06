/* hata pathloss model

   Source: 
   M. Hata, "Empirical formula for propagation loss in land
   mobile radio services," IEEE Trans. Veh. Technol., vol. VT-29, no. 3,
   pp. 317-325, Aug. 1980

   NOTE: hata model has some ambiquity. 
         1. urban large city model does not have an equation for
	    the correction factor for frequencies between 
	    200 and 400 MHz.
	    SOLUTION: average the equations for the 200 and 400
	    MHz cases.
	 2. the suburban and open area models do not specify which
	    urban model (medium-small or large) model to use as the base
	    model before correction factors added.
	    SOLUTION: use the medium-small city model

   WARNING: the solutions described above are rough guesses and have
   not been blessed by the experts.

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

   $Id: hata.c,v 1.1 1994/09/16 11:24:02 jjb Exp $
   $Log: hata.c,v $
 * Revision 1.1  1994/09/16  11:24:02  jjb
 * added section to check if parameters are out of range
 *
 * Revision 1.0  1994/09/07  13:56:48  jjb
 * Initial revision
 *

*/

#include <stdio.h>
#include <math.h>

static char rcsid[] = "$Id: hata.c,v 1.1 1994/09/16 11:24:02 jjb Exp $";

enum  areaTypes { URBAN_MED_SMALL, URBAN_LARGE, SUBURBAN, OPEN};

double hata();

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
  fprintf(stdout,"Hata path loss prediction model\n\n");

  /* input variables */

  if ( argc != 6 ) {
    fprintf(stderr,"area type(0=urban: med-small,1=urban: larger,2=suburb,3=open) ? ");
    fscanf(stdin,"%d",&area);

    fprintf(stderr,"frequency(MHz)[150-1500]? ");
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

  case SUBURBAN:

    /* suburban area */
    break;

  case OPEN:

    /* open area */
    break;

  default:
    fprintf(stderr,"invalid area\n");
    exit(-1);
  }

  /* test for valid parameters */

  if ( (frequency < 150) || (frequency > 1500) ) {
    fprintf(stderr,"error: invalid frequency, must be in 150-1500 MHz range\n");
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

  planeEarthLoss = hata(frequency, baseHeight, mobileHeight, distance, area);
   
  fprintf(stdout,"Hata model loss = %lf dB\n",planeEarthLoss);

}

double hata( frequency, baseHeight, mobileHeight, distance, area )
  double frequency, baseHeight, mobileHeight, distance;
  enum areaTypes area;
{
  double planeEarthLoss;    /* plane earth propagation loss */
  double terCorFactor;      /* terrain dependent correction factor */

  switch ( area) {

  case URBAN_MED_SMALL:

    /* Urban area: medium-small city */

    terCorFactor = ( 1.1 * log10(frequency) - 0.7) * mobileHeight
      - ( 1.56 * log10(frequency) - 0.8);

    planeEarthLoss = 69.55 + 26.16 * log10(frequency) 
      - 13.82 * log10(baseHeight) - terCorFactor 
	+ (44.9 - 6.55 * log10(baseHeight)) * log10(distance);

    break;

  case URBAN_LARGE:

    /* Urban area: large city */

    if ( frequency <= 200.0 ) {

      terCorFactor = 8.29 * pow(log10(1.54 * mobileHeight), 2.0) - 1.1;

    } else if ( frequency < 400.0) {

      /* average of the < 200MHz and > 400MHz correction factors */

      terCorFactor = 0.5 * (8.29 * pow(log10(1.54 * mobileHeight), 2.0) - 1.1)
	+ 0.5 * (3.2 * pow(log10(11.75 * mobileHeight), 2.0) - 4.97);

    } else {

      terCorFactor = 3.2 * pow(log10(11.75 * mobileHeight), 2.0) - 4.97;

    }

    planeEarthLoss = 69.55 + 26.16 * log10(frequency) 
      - 13.82 * log10(baseHeight) - terCorFactor 
	+ (44.9 - 6.55 * log10(baseHeight)) * log10(distance);
    break;

  case SUBURBAN:

  /* suburban area */

    terCorFactor = ( 1.1 * log10(frequency) - 0.7) * mobileHeight
      - ( 1.56 * log10(frequency) - 0.8); /* medium-small city */

    planeEarthLoss = 69.55 + 26.16 * log10(frequency) 
      - 13.82 * log10(baseHeight) - terCorFactor 
	+ (44.9 - 6.55 * log10(baseHeight)) * log10(distance);

    planeEarthLoss = planeEarthLoss 
      - 2.0 * log10( (frequency/28.0) * (frequency/28.0)) - 5.4; 

    break;

  case OPEN:

  /* open area */

    terCorFactor = ( 1.1 * log10(frequency) - 0.7) * mobileHeight
      - ( 1.56 * log10(frequency) - 0.8); /* medium-small city */

    planeEarthLoss = 69.55 + 26.16 * log10(frequency) 
      - 13.82 * log10(baseHeight) - terCorFactor 
	+ (44.9 - 6.55 * log10(baseHeight)) * log10(distance);

    planeEarthLoss = planeEarthLoss 
      - 4.78 * pow(log10(frequency), 2.0) 
	+ 18.33 * log10(frequency) 
	  - 40.94;

    break;

  default:
    fprintf(stderr,"invalid area\n");
    exit(-1);
  }

  return(planeEarthLoss);
}
