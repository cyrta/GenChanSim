/* Quieries the user on some scenario specifications and then advises
   the user on which scenario script file to use to perform the
   simulation using the airairchan program.

   MIT Lincoln Laboratory, 1997
   C. M. Keller

   $Id: whichscen.c,v 1.0 Mar 1997 cmk Exp $
   $Log: whichscen.c,v $

*/

#include <stdio.h>
#include <math.h>

static char rcsid[] = "$Id: whichscen.c,v 1.0 Mar 1997 cmk Exp $";

enum geometry {NEAR, MEDIUM, FAR};
enum earthtype {LAND,SEA};
enum surfacetype {SMOOTH,AVERAGE,MULTIFACET};
enum specpow {SPECSMALL,SPECMED,SPECLARGE};
enum scatpow {SCATSMALL,SCATMED,SCATLARGE};
enum seastate {SEASMOOTH,SEAMED,SEAROUGH};
enum windspeed {LOW,HIGH};
enum antgain {ANTLOW,ANTMED,ANTHIGH};
enum polarization {VERTICAL,HORIZONTAL};

char *scenario[];
char *begfname[];
char *midfname[];
char *numfname[];
char *letfname[];
char *polfname[];

main(argc,argv)
int argc;
char *argv[];
{
  enum geometry geom;      /* one of 3 geometries */
  enum earthtype earth;    /* either land or sea */
  enum surfacetype surface;   /* surface smooth or roughness */
  enum specpow spec;    /* relative power for specular path */
  enum specpow spec2;    /* relative power for second specular path */
  enum scatpow scat;      /* relative power for scatter path */
  enum seastate state;  /* one of 3 sea states */
  enum windspeed wind;  /* one of 2 wind speeds */
  enum antgain gain;  /* one of 3 ant gain choices */
  enum polarization pol; /* one of 2 choices */

  fprintf(stdout,"\n     1997 MIT Lincoln Laboratory, Lexington MA\n");
  fprintf(stdout,"                DEFAULT SCENARIO SELECTION\n\n");

  /* Initialize */
  geom=10;
  earth=10;
  surface=10;
  spec=10;
  spec2=10;
  scat=10;
  state=10;
  wind=10;
  gain=10;

  /* input variables */
  
  /***************************************************************************/
  /* FIND THE FIRST PART OF THE DEFAULT SCENARIO FILE NAME FROM EARTHTYPE    */
  /***************************************************************************/

  Enterearth:{
    fprintf(stderr,"Enter whether the ground terminal is on land or sea:\n 0=land\n 1=sea\n ? ");
    fscanf(stdin,"%d",&earth);

  /* check if valid earthtype */
    switch (earth) {
    case LAND:
      /* Ground terminal on land */
      fprintf(stdout,"Ground terminal on LAND\n\n");
      *begfname="land";
      break;
    case SEA:
      /* Ground terminal on sea */
      fprintf(stdout,"Ground terminal on SEA\n\n");
      *begfname="sea";
      break;
    default:
      fprintf(stderr,"\n Invalid earth type. Please try again \n");
      goto Enterearth;
    }  /* switch earth */
  }  /* label Enterearth */

  Entergeom:{
    fprintf(stderr,"Enter the aircraft to ground geometry and velocity configuration:\n 0=near\n 1=medium\n 2=far\n ? ");
    fscanf(stdin,"%d",&geom);

    /* check if valid geometry choice */
    switch (geom) {
    case NEAR:
      /* aircraft and ground terminal are near */
      fprintf(stdout,"Aircraft and ground terminal are at NEAR range.\n\n");
      *midfname="near";
      break;
    case MEDIUM:
      /* aircraft and ground terminal are at medium range */
      fprintf(stdout,"Aircraft and ground terminal are at MEDIUM range.\n\n");
      *midfname="med";
      break;
    case FAR:
      /* Aircraft are far */
      fprintf(stdout,"Aircraft and ground terminal are at FAR range.\n\n");
      *midfname="far";
      break;
    default:
      fprintf(stderr,"Invalid aircraft geometry and velocity configuration. Please try again. \n");
      goto Entergeom;
    }  /* switch geom */
  }  /* label Entergeom */

  /*******************************************************************************/
  /* FIND THE LAST PART OF THE DEFAULT SCENARIO FILE NAME FROM OTHER PARAMETERS  */
  /*******************************************************************************/

  if(earth == LAND){
    Entersurface:{
      fprintf(stderr,"Enter land surface roughness:\n 0=smooth\n 1=average\n 2=multifacet\n ? ");
      fscanf(stdin,"%d",&surface);

      /* check if valid path choice */
      switch (surface) {
      case SMOOTH:
        fprintf(stdout,"Land is SMOOTH\n\n");
        Enterspec:{
  	  fprintf(stderr,"Enter the specular path power relative to the direct path power:\n 0=small\n 1=medium\n 2=large\n ? ");
          fscanf(stdin,"%d",&spec);
          /* check valid choices */
          switch (spec) {
          case SPECSMALL:
            fprintf(stdout,"The specular path power is SMALL relative to the direct path power\n\n");
            *numfname="1";
            break;
          case SPECMED:
            fprintf(stdout,"The specular path power is MEDIUM relative to the direct path power\n\n");
            *numfname="2";
            break;
          case SPECLARGE:
            fprintf(stdout,"The specular path power is LARGE relative to the direct path power\n\n");
            *numfname="3";
            break;
          default:
            fprintf(stderr,"Invalid relative power. Please try again. \n");
            goto Enterspec;
          } /* switch spec */
	} /* label Enterspec */
        break;
      case AVERAGE:
        fprintf(stdout,"Land is AVERAGE\n\n");
        Enterscat1:{
  	  fprintf(stderr,"Enter the scatter path power relative to the direct path power:\n 0=small\n 1=medium\n 2=large\n ? ");
          fscanf(stdin,"%d",&scat);
          /* check valid choices */
          switch (scat) {
          case SCATSMALL:
            fprintf(stdout,"The scatter path power is SMALL relative to the direct path power\n\n");
            *numfname="4";
            break;
          case SCATMED:
            fprintf(stdout,"The scatter path power is MEDIUM relative to the direct path power\n\n");
            *numfname="5";
            break;
          case SCATLARGE:
            fprintf(stdout,"The scatter path power is LARGE relative to the direct path power\n\n");
            *numfname="6";
            break;
          default:
            fprintf(stderr,"Invalid relative power. Please try again. \n");
            goto Enterscat1;
          } /* switch scat */
	} /* label Enterscat1 */
        break;
      case MULTIFACET:
        fprintf(stdout,"Land is MULTIFACET\n\n");
        Enterspec2:{
  	  fprintf(stderr,"Enter the first specular path power relative to the direct path power:\n 0=small\n 1=medium\n 2=large\n ? ");
          fscanf(stdin,"%d",&spec);
          /* check valid choices */
          switch (spec) {
          case SPECSMALL:
            fprintf(stdout,"The first specular path power is SMALL relative to the direct path power\n\n");
            break;
          case SPECMED:
            fprintf(stdout,"The first specular path power is MEDIUM relative to the direct path power\n\n");
            break;
          case SPECLARGE:
            fprintf(stdout,"The first specular path power is LARGE relative to the direct path power\n\n");
            break;
          default:
            fprintf(stderr,"Invalid relative power. Please try again. \n");
            goto Enterspec2;
          } /* switch spec */
	} /* label Enterspec2 */
        Enterspec3:{
  	  fprintf(stderr,"Enter the second specular path power relative to the direct path power:\n 0=small\n 1=medium\n 2=large\n ? ");
          fscanf(stdin,"%d",&spec2);
          /* check valid choices */
          switch (spec2) {
          case SPECSMALL:
            fprintf(stdout,"The second specular path power is SMALL relative to the direct path power\n\n");
            break;
          case SPECMED:
            fprintf(stdout,"The second specular path power is MEDIUM relative to the direct path power\n\n");
            break;
          case SPECLARGE:
            fprintf(stdout,"The second specular path power is LARGE relative to the direct path power\n\n");
            break;
          default:
            fprintf(stderr,"Invalid relative power. Please try again. \n");
            goto Enterspec3;
          } /* switch spec2 */
	} /* label Enterspec3 */
        /* Now assign file name based on two specular paths relative powers */
        if (spec==SPECSMALL & spec2 ==SPECSMALL)
          *numfname="7";
        if (spec==SPECSMALL & spec2 ==SPECMED)
          *numfname="8";
        if (spec==SPECSMALL & spec2 ==SPECLARGE)
          *numfname="9";
        if (spec==SPECMED & spec2 ==SPECSMALL)
          *numfname="10";
        if (spec==SPECMED & spec2 ==SPECMED)
          *numfname="11";
        if (spec==SPECMED & spec2 ==SPECLARGE)
          *numfname="12";
        if (spec==SPECLARGE & spec2 ==SPECSMALL)
          *numfname="13";
        if (spec==SPECLARGE & spec2 ==SPECMED)
          *numfname="14";
        if (spec==SPECLARGE & spec2 ==SPECLARGE)
          *numfname="151";
        break;
      default:
        fprintf(stderr,"Invalid surface type. Please try again. \n");
        goto Entersurface;
      } /* switch surface */
    } /* Label entersurface */
    fprintf(stdout,"\n\n THE DEFAULT SCENARIO FILE TO USE IS    %s%s%s\n\n\n",*begfname,*midfname,*numfname);
  } /* end if earth == LAND */

  if(earth == SEA){
    Entersea:{
      fprintf(stderr,"Enter the sea state:\n 0=smooth\n 1=medium\n 2=rough\n ? ");
      fscanf(stdin,"%d",&state);

      /* check if valid path choice */
      switch (state) {
      case SEASMOOTH:
        fprintf(stdout,"Sea is SMOOTH\n\n");
        break;
      case SEAMED:
        fprintf(stdout,"Sea is MEDIUM\n\n");
        break;
      case SEAROUGH:
        fprintf(stdout,"Sea is ROUGH\n\n");
        break;
      default:
      fprintf(stderr,"\n Invalid sea state. Please try again \n");
      goto Entersea;
      }  /* switch state */
    }  /* label Entersea */
       
    Enterwind:{
      fprintf(stderr,"Enter the wind speed:\n 0=low\n 1=high\n ? ");
      fscanf(stdin,"%d",&wind);

      /* check if valid path choice */
      switch (wind) {
      case LOW:
        fprintf(stdout,"Wind speed is LOW\n\n");
        break;
      case HIGH:
        fprintf(stdout,"Wind speed is HIGH\n\n");
        break;
      default:
      fprintf(stderr,"\n Invalid wind speed. Please try again \n");
      goto Enterwind;
      }  /* switch wind */
    }  /* label Enterwind */

    if (state == SEASMOOTH && wind == LOW)
      *letfname="a";
    else if (state == SEASMOOTH && wind == HIGH)
      *letfname="b";
    else if (state == SEAMED && wind == LOW)
      *letfname="c";
    else if (state == SEAMED && wind == HIGH)
      *letfname="d";
    else if (state == SEAROUGH && wind == LOW)
      *letfname="e";
    else /* state == SEAROUGH && wind == HIGH */
      *letfname="f";

    Entergain:{
      fprintf(stderr,"Enter the antenna gain in the specular path direction relative to the direct path direction:\n 0=low\n 1=medium\n 2=high\n ? ");
      fscanf(stdin,"%d",&gain);

      /* check if valid path choice */
      switch (gain) {
      case ANTLOW:
        fprintf(stdout,"Relative antenna gain is LOW\n\n");
        *numfname="1";
        break;
      case ANTMED:
        fprintf(stdout,"Relative antenna gain is MEDIUM\n\n");
        *numfname="2";
        break;
      case ANTHIGH:
        fprintf(stdout,"Relative antenna gain is HIGH\n\n");
        *numfname="3";
        break;
      default:
      fprintf(stderr,"\n Invalid relative antenna gain. Please try again \n");
      goto Entergain;
      }  /* switch gain */
    }  /* label Entergain */
       
    Enterpol:{
      fprintf(stderr,"Enter the transmitted signal polarization:\n 0=vertical\n 1=horizontal\n ? ");
      fscanf(stdin,"%d",&pol);

      /* check if valid path choice */
      switch (pol) {
      case VERTICAL:
        fprintf(stdout,"Signal polarization is VERTICAL\n\n");
        *polfname="v";
        break;
      case HORIZONTAL:
        fprintf(stdout,"Signal polarization is HORIZONTAL\n\n");
        *polfname="h";
        break;
      default:
      fprintf(stderr,"\n Invalid polarization. Please try again \n");
      goto Enterpol;
      }  /* switch pol */
    }  /* label Enterpol */

  fprintf(stdout,"\n\n THE DEFAULT SCENARIO FILE TO USE IS    %s%s%s%s%s\n\n\n",*begfname,*midfname,*letfname,*numfname,*polfname);
       
  }   /* if earth == SEA */

}  /* argv */



