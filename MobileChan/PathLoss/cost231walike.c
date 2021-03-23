/* COST-231-Walfish-ikegami pathloss model from 

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

   $Id: cost231walike.c,v 1.1 1994/09/16 11:24:02 jjb Exp $
   $Log: cost231walike.c,v $
 * Revision 1.1  1994/09/16  11:24:02  jjb
 * added section to check if parameters are out of range
 *
 * Revision 1.0  1994/09/07  13:56:48  jjb
 * Initial revision
 *

*/

#include <stdio.h>
#include <math.h>

enum  areaTypes { URBAN_MED_SMALL, URBAN_LARGE};

static char rcsid[] = "$Id: cost231walike.c,v 1.1 1994/09/16 11:24:02 jjb Exp $";

double cost231walike();

main(argc,argv)
int argc;
char *argv[];
{
  double loss;              /* loss */
  double frequency;         /* carrier frequency */
  double baseHeight;        /* height of base station */
  double mobileHeight;      /* height of mobile station */
  double roofHeight;        /* height of roof */
  double distance;          /* distance between stations */
  double roadWidth;         /* road width */
  double buildSep;          /* building seperation */
  double roadOrient;        /* road orientation */
  int lineOfSight;       /* line of sight flag */
  enum areaTypes area;      /* one of 4 areas for path loss */

  fprintf(stdout,"copyright (c) 1994 The MITRE Corporation Bedford, MA\n");

  fprintf(stdout,"COST 231 Walfish and Ikegami path loss prediction model\n\n");

  /* input variables */

  if ( argc != 10 ) {
    fprintf(stderr,"area type(0=medium sized city and suburb. with mod. tree density\n,1=metropolitan centres) ? ");
    fscanf(stdin,"%d",&area);

    fprintf(stderr,"frequency(MHz)[800-2000] ? ");
    fscanf(stdin,"%lf",&frequency);

    fprintf(stderr,"base height (meters)[4-50] ? ");
    fscanf(stdin,"%lf",&baseHeight);

    fprintf(stderr,"mobile height (meters)[1-3] ? ");
    fscanf(stdin,"%lf",&mobileHeight);

    fprintf(stderr,"roof height (meters) ? ");
    fscanf(stdin,"%lf",&roofHeight);

    fprintf(stderr,"distance (kilometers)[0.02-5] ? ");
    fscanf(stdin,"%lf",&distance);

    fprintf(stderr,"road width (meters) ? ");
    fscanf(stdin,"%lf",&roadWidth);

    fprintf(stderr,"building separation (meters) ? ");
    fscanf(stdin,"%lf",&buildSep);

    fprintf(stderr,"road orientation (degrees)[0-90] ?");
    fscanf(stdin,"%lf",&roadOrient);
    /*
      fprintf(stderr,"Is it line of sight (1=yes, 0=no) ?");
      fscanf(stdin,"%d",&lineOfSight);
      */
  } else {

    sscanf(argv[1],"%d",&area);
    sscanf(argv[2],"%lf",&frequency);
    sscanf(argv[3],"%lf",&baseHeight);
    sscanf(argv[4],"%lf",&mobileHeight);
    sscanf(argv[5],"%lf",&roofHeight);
    sscanf(argv[6],"%lf",&distance);
    sscanf(argv[7],"%lf",&roadWidth);
    sscanf(argv[8],"%lf",&buildSep);
    sscanf(argv[9],"%lf",&roadOrient);
    /*
      sscanf(sargv[10],"%d",&lineOfSight);
      */

  }
  lineOfSight = 0;

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

  if ( (frequency < 800) || (frequency > 2000) ) {
    fprintf(stderr,"error: invalid frequency, must be in 800-2000 MHz range\n");
    exit(-1);
  }

  if ( (baseHeight < 4) || (baseHeight > 50) ) {
    fprintf(stderr,"error: invalid base height, must be in 4-50 m range\n");
    exit(-1);
  }

  if ( (mobileHeight < 1) || (mobileHeight > 3) ) {
    fprintf(stderr,"error: invalid mobile height, must be in 1-3 m range\n");
    exit(-1);
  }

  if ( (distance < 0.02) || (distance > 5) ) {
    fprintf(stderr,"error: invalid distance, must be in 0.02-5 km range\n");
    exit(-1);
  }

  if ( (roadOrient < 0) || (roadOrient > 90) ) {
    fprintf(stderr,"error: invalid distance, must be in 0-90 degree range\n");
    exit(-1);
  }

  /* print out setting */

  loss = cost231walike( frequency, baseHeight, mobileHeight, distance, 
		     roofHeight, roadWidth, buildSep, roadOrient, lineOfSight,
		     area);
    fprintf(stdout,"COST 231 Walfish and Ikegami model loss = %lf dB\n",loss);

}

double cost231walike( frequency, baseHeight, mobileHeight, distance, 
		     roofHeight, roadWidth, buildSep, roadOrient, lineOfSight,
		     area)
     double frequency, baseHeight, mobileHeight, distance,
       roofHeight, roadWidth, buildSep, roadOrient;
     int lineOfSight;
     enum areaTypes area;
{
  double Lb;          /* basic transmission loss */
  double L0;          /* free-space loss */
  double Lrts;        /* roof top-to-street diffraction and scatter loss */
  double Lmsd;        /* multi-screen loss */
  double Lori;
  double Lbsh;
  double Ka,Kd,Kf;

  /* check if line of sight then compute aLb and return */

  if (lineOfSight) {
    if ( distance < 0.02 ) { /* if < 20m then invalid */
      fprintf(stderr,"incorrect value for distance must be > 0.02km\n");
      exit(1);
    }
    Lb = 42.6 + 26.0 * log10(distance) + 20.0 * log10(frequency);
    return(Lb);
  }

  /* compute free space loss */

  L0 = 32.4 + 20.0 * log10(distance) + 20.0 * log10(frequency);

  /* compute roof top-to-street loss */

  if ( (roadOrient >= 0) && (roadOrient < 35) ) 
    Lori = -10.0 + 0.354 * roadOrient;
  else if ( (roadOrient >= 35) && (roadOrient < 55) ) 
    Lori = 2.5 + 0.075 * (roadOrient - 35.0);
  else if ( (roadOrient >= 55) && (roadOrient <= 90) ) 
    Lori = 4.0 - 0.114 * (roadOrient - 55.0);
  else {
    fprintf(stderr,"incorrect road orientation\n");
    exit(1);
  }

  Lrts = -16.9 - 10.0 * log10(roadWidth) + 10.0 * log10(frequency)
    + 20.0 * log10(roofHeight - mobileHeight) + Lori;

  /* compute multi-screen diffraction loss */

  if ( baseHeight > roofHeight ){
    Lbsh = -18.0*log10(1 + baseHeight - roofHeight);
    Ka = 54.0;
    Kd = 18.0;
  } else {
    Lbsh = 0.0;
    if ( distance < 0.5 ) 
      Ka = 54.0 - 0.8 * (baseHeight - roofHeight) * distance / 0.5;
    else
      Ka = 54.0 - 0.8 * (baseHeight - roofHeight);
    Kd = 18.0 - 15.0 * (baseHeight - roofHeight)/roofHeight;
  }
  if( area == URBAN_MED_SMALL )
    Kf = -4.0 + 0.7 * (frequency/925 - 1);
  else
    Kf = -4.0 + 1.5 * (frequency/925 - 1);

  Lmsd = Lbsh + Ka + Kd * log10(distance) + Kf * log10(frequency)
    - 9.0 * log10(buildSep);

  /* compute basic transmission loss */

  if ( (Lrts + Lmsd) <= 0)
    Lb = L0;
  else
    Lb = L0 + Lrts + Lmsd;

  return(Lb);
}
