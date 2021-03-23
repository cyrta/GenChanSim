/* non Line-of-sight below roof propagation model.
   Official Name: 
   1st order 2 dimension road guided and Cost231-Walfisch-Ikegami

   NOTE: includes road guided and diffraction losses.
   Road guided loss from equations in year end report and
   diffraction loss computed using COST-231-Walfish-ikegami
   pathloss model from 

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

   $Id: nlosbelrfblk.c,v 1.1 1994/09/29 19:17:30 jjb Exp $
   $Log: nlosbelrfblk.c,v $
 * Revision 1.1  1994/09/29  19:17:30  jjb
 * changed loss calculation
 *
 * Revision 1.0  1994/09/29  11:32:03  jjb
 * Initial revision
 *

*/

#include <stdio.h>
#include <math.h>

#define CORNER_LOSS  15.0 /* corner turning loss */
#define CORNER_DIST  10.0 /* distance over which corner turning loss applies */
#define FAR_SLOPE    40.0 /* far region slope */

enum  areaTypes { URBAN_MED_SMALL, URBAN_LARGE};

static char rcsid[] = "$Id: nlosbelrfblk.c,v 1.1 1994/09/29 19:17:30 jjb Exp $";

double cost231walike();
double roadguided();

main(argc,argv)
int argc;
char *argv[];
{
  double loss;              /* loss */
  double diffLoss;          /* diffraction loss */
  double roadGuideLoss;     /* road guided loss */
  double diffAtt;           /* diffraction attenuation */
  double roadGuideAtt;      /* road guided attenuation */
  double frequency;         /* carrier frequency */
  double baseHeight;        /* height of base station */
  double mobileHeight;      /* height of mobile station */
  double roofHeight;        /* height of roof */
  double distance;          /* distance between stations */
  double roadWidth;         /* road width */
  double buildSep;          /* building seperation */
  double roadOrient;        /* road orientation */
  double slope;             /* far region slope */
  int lineOfSight;          /* line of sight flag */
  enum areaTypes area;      /* one of 4 areas for path loss */
  double cornerLoss;        /* corner turning loss */
  double cornerDist;        /* corner distance */

  fprintf(stdout,"copyright (c) 1994 The MITRE Corporation Bedford, MA\n");

  fprintf(stdout,"1st order 2D road quided and Cost231-Walfisch-Ikegami models\n");
  fprintf(stdout,"ASSUMPTION: base in center of main street in middle of block\n\n");

  /* input variables */

  if ( argc < 10 || argc > 13 ) {
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

    fprintf(stderr,"far region slope (-1 = default of %lf) ? ",FAR_SLOPE);
    fscanf(stdin,"%lf",&slope);
    if (slope < 30 || slope > 70 )
      slope = 40;

    fprintf(stderr,"corner turning loss (-1 = default of %lfdB) ? ",CORNER_LOSS);
    fscanf(stdin,"%lf",&cornerLoss);
    if (cornerLoss < 0 )
      cornerLoss = CORNER_LOSS;

    fprintf(stderr,"corner turning breakpoint distance (-1 = default of %lf) ? ",
	    CORNER_DIST);
    fscanf(stdin,"%lf",&cornerDist);
    if (cornerDist < 0)
      cornerDist = CORNER_DIST;

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
    if ( argc > 10 )
      sscanf(argv[10],"%lf",&slope);
    else
      slope = FAR_SLOPE;
    if (slope < 30 || slope > 70 )
      slope = FAR_SLOPE;

    if ( argc > 11 )
      sscanf(argv[11],"%lf",&cornerLoss);
    else
      cornerLoss = CORNER_LOSS;
    if ( argc > 12 )
      sscanf(argv[12],"%lf",&cornerDist);
    else
      cornerDist = CORNER_DIST;


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

  /* compute road guided loss and diffraction loss */

  roadGuideLoss = roadguided( frequency, baseHeight, mobileHeight, distance, 
		     roadWidth, buildSep, roadOrient, slope, cornerLoss,
			     cornerDist);

  diffLoss = cost231walike( frequency, baseHeight, mobileHeight, distance, 
		     roofHeight, roadWidth, buildSep, roadOrient, lineOfSight,
		     area);

  /* compute total loss */

  roadGuideAtt = pow(10.0, roadGuideLoss/10);
  diffAtt = pow(10.0, diffLoss/10);
  loss = roadGuideLoss + diffLoss - 10 * log10(roadGuideAtt + diffAtt);

  fprintf(stdout,"Road Guided Loss = %lf, Diffraction Loss = %lf\n",
	  roadGuideLoss, diffLoss);
  fprintf(stdout,"Combined Loss = %lf dB\n",loss);

}

/* subroutine for road guided loss */

double roadguided( frequency, baseHeight, mobileHeight, distance, 
		  roadWidth, buildSep, roadOrient, slope, cornerLoss,
		  cornerDist)
     double frequency, baseHeight, mobileHeight, distance,
       roadWidth, buildSep, roadOrient, slope, cornerLoss,
       cornerDist;
{
  double unobDirUrbHR();
  double lossRG;             /* road guided loss */
  double lossMain;           /* loss along main street */
  double lossCross;          /* loss around corner into cross street */
  double lossTrans;          /* loss around corner into transvere street */
  double x,y;                /* distance in x and y dimensions */
  double dm, dc, dt;         /* distances over main, cross and
				transverse streets */
  double d;
  double tmp, tmp2;          /* temp. variable */
  double pi;
  int   dmBlocks;            /* number of whole blocks along main st. */
  int   dcBlocks;            /* number of whole blocks along cross st. */

  pi=4.0*atan(1.0);

  /* compute distances dm, dc dt */
  /* NOTE: dm is from base to center of cross street entrance
           dc is from entrance of cross st. to 
	             center of transverse st. entrance.
	   dt is from tranverse st entrance to mobile
  */

  x = fabs(distance * 1000 * cos(roadOrient * pi / 180.0 ));
  y = fabs(distance * 1000 * sin(roadOrient * pi / 180.0 ));

  if ( y < roadWidth/2 ) {
    /* LOS therefore dm = distance */
    dm = distance * 1000;
    dc=dt=0.0;

  } else {

    /* non LOS */
    
    /* first compute distances to edge of intersection parallel to street */

    tmp = ( buildSep - roadWidth ) /2;

    if (x < tmp) {

      dm = tmp;
      dt = tmp - x;

    } else {

      dmBlocks = (int) (x - tmp)/buildSep;
      dm = tmp + (double)dmBlocks * buildSep;
      dt = x - dm - roadWidth;
    }
    dc = y - roadWidth/2;

    if ( dt < 0 )
      dt = 0;

    /* if dt != 0 must force dc to be middle of intersection */

    if ( dt != 0 ) {
      dcBlocks = (int) ((y + (roadWidth/2) )/buildSep);
      dc = (double)dcBlocks * buildSep;
      dc = dc - roadWidth;
    }

    /* convert to distance to center of street entrance */

    dm = sqrt( pow(roadWidth/2, 2.0) + pow(dm+(roadWidth/2), 2.0));
    if ( dt != 0)
      dc = sqrt( pow(roadWidth/2, 2.0) + pow(dc+(roadWidth/2), 2.0));
  }

  /* compute total distance d */

  d = dm + dc + dt;

  /* check if y is valid i.e. not in middle of a building */

  tmp2 = y - (double)( (int) (y/buildSep) ) * buildSep;
  if ( (dt != 0) && (tmp2 >  (roadWidth/2)) 
      && (tmp2 < buildSep -  (roadWidth/2)) ) {
    fprintf(stderr,"Error: mobile position does not fall on a valid street\n");
    fprintf(stderr,"rerun with new distance or road orientation\n");
    exit(-1);
  }

  /* compute path loss along main street */
  
  lossMain = unobDirUrbHR(frequency, baseHeight, mobileHeight, dm, slope );

  /* compute loss over cross street */

  if ( dc < 1e-10 ) {
    lossCross = 0;
  } else if ( dc < cornerDist ) {
    lossCross = cornerLoss * (d - dm)/cornerDist;
  } else {
    lossCross = cornerLoss;
  }

  /* compute loss over transverse street */

  if ( dt < 1e-10 ) {
    lossTrans = 0;
  } else if ( dt < cornerDist ) {
    lossTrans = cornerLoss * (d - (dm+dc))/cornerDist;
  } else {
    lossTrans = cornerLoss;
  }

  if ( d > dm )
    lossRG = lossMain + lossCross + lossTrans + 40.0 * log10(d/dm);
  else
    lossRG = lossMain;


#ifdef DEBUG
  /* debug */
  fprintf(stdout,"slope = %lf, corner loss=%lf, corner distance=%lf\n",
	  slope,cornerLoss,cornerDist);
  fprintf(stdout,"dm=%lf, dc=%lf, dt=%lf\n",dm,dc,dt);
  fprintf(stdout,"Lm=%lf, Lc=%lf, Ll=%lf, Lrg=%lf\n",lossMain,
	  lossCross,lossTrans,lossRG);
#endif
  /* return total loss */

  return(lossRG);

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


/* function for diffraction loss */

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
