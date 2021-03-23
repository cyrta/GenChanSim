/* Quieries the user on some scenario specifications and then advises
   the user on which scenario script file to use to perform the
   simulation using the mixedchan program.

   MIT Lincoln Laboratory, 1997
   C. M. Keller

   $Id: whichscen.c,v 1.0 Jan 1997 cmk Exp $
   $Log: whichscen.c,v $

*/

#include <stdio.h>
#include <math.h>

static char rcsid[] = "$Id: whichscen.c,v 1.0 Jan 1997 cmk Exp $";

enum areatype {SMALL_OFFICE, LARGE_OFFICE, COMMERCIAL, INDOOR_OUTDOOR};
enum pathstruct {SCAT, SCATDIR, SCATDISC, SCATDIRDISC};
enum dslevel {SMALL, MEDIUM, LARGE};
enum relpowscat {SCATSMALL, SCATMEDIUM, SCATLARGE};
enum relpowdir {DIRSMALL, DIRMEDIUM, DIRLARGE};
enum discretepos {NEAR, FAR};

char *scenario[];
char *begfname[];
char *endfname[];

main(argc,argv)
int argc;
char *argv[];
{
  enum areatype area;      /* one of 4 areas */
  enum dslevel delspr;     /* delay spread amount */
  enum pathstruct paths;   /* types of paths presents */
  enum relpowscat scat;    /* relative power for scatter path */
  enum relpowdir dire;     /* relative power for direct vs  total discrete */
  enum discretepos pos;    /* discrete path delay position */

  fprintf(stdout,"\n     1997 MIT Lincoln Laboratory, Lexington MA\n");
  fprintf(stdout,"                DEFAULT SCENARIO SELECTION\n\n");

  /* input variables */
  
  /*******************************************************************************/
  /* FIND THE SECOND PART OF THE DEFAULT SCENARIO FILE NAME FROM area AND delspr */
  /*******************************************************************************/

  Enterarea:{
    fprintf(stderr,"Enter environment:\n 0=Indoor Small Office\n 1=Indoor Large Office\n 2=Indoor Commercial\n 3=Indoor-Outdoor Microcell\n ? ");
    fscanf(stdin,"%d",&area);

  /* check if valid area */
    switch (area) {
    case SMALL_OFFICE:
      /* Indoor small office */
      fprintf(stdout,"Environment = INDOOR SMALL OFFICE\n\n");
      break;
    case LARGE_OFFICE:
      /* Indoor large office */
      fprintf(stdout,"Environment = INDOOR LARGE OFFICE\n\n");
      break;
    case COMMERCIAL:
      /* Indoor commercial */
      fprintf(stdout,"Environment = INDOOR COMMERCIAL\n\n");
      break;
    case INDOOR_OUTDOOR:
      /* Indoor-outdoor microcell */
      fprintf(stdout,"Environment = INDOOR_OUTDOOR MICROCELL\n\n");
      break;
    default:
      fprintf(stderr,"\n Invalid environment. Please try again \n");
      goto Enterarea;
    }  /* switch area */
  }  /* label Enterarea */

  Enterspr:{
    fprintf(stderr,"Enter amount of delay spread:\n 0=small\n 1=medium\n 2=large\n ? ");
    fscanf(stdin,"%d",&delspr);

    /* check if valid delay spread choice */
    switch (delspr) {
    case SMALL:
      /* small delay spread */
      fprintf(stdout,"Delay spread = SMALL\n\n");
      break;
    case MEDIUM:
      /* medium delay spread */
      fprintf(stdout,"Delay spread = MEDIUM\n\n");
      break;
    case LARGE:
      /* large delay spread */
      fprintf(stdout,"Delay spread = LARGE\n\n");
      break;
    default:
      fprintf(stderr,"Invalid delay spread choice. Please try again. \n");
      goto Enterspr;
    }  /* switch delspr */
  }  /* label Enterspr */

  if (area == SMALL_OFFICE && delspr == SMALL)
    *endfname="a";
  else if (area == SMALL_OFFICE && delspr == MEDIUM)
    *endfname="b";
  else if (area == SMALL_OFFICE && delspr == LARGE)
    *endfname="c";
  else if (area == LARGE_OFFICE && delspr == SMALL)
    *endfname="d";
  else if (area == LARGE_OFFICE && delspr == MEDIUM)
    *endfname="e";
  else if (area == LARGE_OFFICE && delspr == LARGE)
    *endfname="f";
  else if (area == COMMERCIAL && delspr == SMALL)
    *endfname="d";
  else if (area == COMMERCIAL && delspr == MEDIUM)
    *endfname="g";
  else if (area == COMMERCIAL && delspr == LARGE)
    *endfname="h";
  else if (area == INDOOR_OUTDOOR && delspr == SMALL)
    *endfname="d";
  else if (area == INDOOR_OUTDOOR && delspr == MEDIUM)
    *endfname="i";
  else  /* (area == INDOOR_OUTDOOR && delspr == LARGE) */
    *endfname="j";
     

  /*******************************************************************************/
  /* FIND THE FIRST PART OF THE DEFAULT SCENARIO FILE NAME FROM paths, scat AND
                                                                            dire */
  /*******************************************************************************/

  Enterpaths:{
    fprintf(stderr,"Enter channel structure:\n 0=scatter only\n 1=scatter + direct\n 2=scatter + discrete (no direct)\n 3=scatter + direct + discrete\n ? ");
    fscanf(stdin,"%d",&paths);
    /* check if valid path choice */
    switch (paths) {
    case SCAT:
      /* scatter path only */
      fprintf(stdout,"Channel structure = SCATTER ONLY\n\n");
      *begfname="inscen1";
      break;
    case SCATDIR:
      /* Scatter path + direct path only */
      fprintf(stdout,"Channel structure = SCATTER + DIRECT\n\n");
      Enterscat:{
        fprintf(stderr,"Enter power level of SCATTER path relative to total path power:\n 0=small\n 1=medium\n 2=large\n ? ");
        fscanf(stdin,"%d",&scat);
        /* Check if valid power level choice */
        switch (scat){
          case SCATSMALL:
          /* Small scatter path power level */
          fprintf(stdout,"SCATTER path power level = SMALL\n\n");
          *begfname="inscen2";
          break;
        case SCATMEDIUM:
          /* Medium scatter path power level */
          fprintf(stdout,"SCATTER path power level = MEDIUM\n\n");
          *begfname="inscen3";
          break;
        case SCATLARGE:
          /* Large scatter path power level */
          fprintf(stdout,"SCATTER path power level = LARGE\n\n");
          *begfname="inscen4";
          break;
        default:
          fprintf(stderr,"Invalid scatter path power level. Please try again. \n");
          goto Enterscat;
        }  /* switch scat */
      }  /* label Enterscat */
      break;
    case SCATDISC:
      /* Scatter path + discrete paths, but no direct path */
      fprintf(stdout,"Channel structure = SCATTER + DISCRETE\n\n");
      Enterscat2:{
        fprintf(stderr,"Enter power level of SCATTER path relative to total path power:\n 0=small\n 1=medium\n 2=large\n ? ");
        fscanf(stdin,"%d",&scat);
        /* Check if valid power level choice */
        switch (scat){
          case SCATSMALL:
          /* Small scatter path power level */
          fprintf(stdout,"SCATTER path power level = SMALL\n\n");
          break;
        case SCATMEDIUM:
          /* Medium scatter path power level */
          fprintf(stdout,"SCATTER path power level = MEDIUM\n\n");
          break;
        case SCATLARGE:
          /* Large scatter path power level */
          fprintf(stdout,"SCATTER path power level = LARGE\n\n");
          break;
        default:
          fprintf(stderr,"Invalid scatter path power level. Please try again. \n");
          goto Enterscat2;
        }  /* switch scat */
      }  /* label Enterscat2 */
      /* Now enter the delay position for the scatter path (two choices) */
      Enterpos:{
        fprintf(stderr,"Enter the delay position for the discrete path:\n 0=near\n 1=far\n ? ");
        fscanf(stdin,"%d",&pos);
        /* Check if valid discrete path delay position */
          switch(pos){
	  case NEAR:
            fprintf(stdout,"Discrete path DELAY POSITION = NEAR\n\n");
            break;
          case FAR:
            fprintf(stdout,"Discrete path DELAY POSITION = FAR\n\n");
            break;
          default:
            fprintf(stderr,"Invalid discrete path DELAY POSITION. Please try again. \n");
            goto Enterpos;
        }  /* switch pos */
      }  /* label Enterpos */
      /* Now go through the possible combinations for this path structure to 
         specify the first portion of the filename. */
      if (scat == SCATSMALL && pos == NEAR)
        *begfname="inscen5N";
      else if (scat == SCATSMALL && pos == FAR)
        *begfname="inscen5F";
      else if (scat == SCATMEDIUM && pos == NEAR)
        *begfname="inscen6N";
      else if (scat == SCATMEDIUM && pos == FAR)
        *begfname="inscen6F";
      else if (scat == SCATLARGE && pos == NEAR)
        *begfname="inscen7N";
      else /* (scat == SCATLARGE && pos == FAR) */
        *begfname="inscen7F";
      break;
    case SCATDIRDISC:
      /* Scatter path + direct path + other discrete paths */
      fprintf(stdout,"Channel structure = SCATTER + DIRECT + DISCRETE\n\n");
      Enterscat3:{
        fprintf(stderr,"Enter power level of SCATTER path relative to total path power:\n 0=small\n 1=medium\n 2=large\n ? ");
        fscanf(stdin,"%d",&scat);
        /* Check if valid power level choice */
        switch (scat){
          case SCATSMALL:
          /* Small scatter path power level */
          fprintf(stdout,"SCATTER path power level = SMALL\n\n");
          break;
        case SCATMEDIUM:
          /* Medium scatter path power level */
          fprintf(stdout,"SCATTER path power level = MEDIUM\n\n");
          break;
        case SCATLARGE:
          /* Large scatter path power level */
          fprintf(stdout,"SCATTER path power level = LARGE\n\n");
          break;
        default:
          fprintf(stderr,"Invalid scatter path power level. Please try again. \n");
          goto Enterscat3;
        }  /* switch scat */
      }  /* label Enterscat3 */
      Enterdire1:{
        fprintf(stderr,"Enter power level of DIRECT path relative to total discrete-path power:\n 0=small\n 1=medium\n 2=large\n ? ");
        fscanf(stdin,"%d",&dire);
        /* Check if valid power level choice */
        switch (dire){
          case DIRSMALL:
          /* Small direct path power level */
          fprintf(stdout,"DIRECT path power level = SMALL\n\n");
          break;
        case DIRMEDIUM:
          /* Medium direct path power level */
          fprintf(stdout,"DIRECT path power level = MEDIUM\n\n");
          break;
        case DIRLARGE:
          /* Large direct path power level */
          fprintf(stdout,"DIRECT path power level = LARGE\n\n");
          break;
        default:
          fprintf(stderr,"Invalid direct path power level. Please try again. \n");
          goto Enterdire1;
        }  /* switch dire */
      }  /* label Enterdire1 */
      /* Now enter the delay position for the scatter path (two choices) */
      Enterpos2:{
        fprintf(stderr,"Enter the delay position for the discrete path:\n 0=near\n 1=far\n ? ");
        fscanf(stdin,"%d",&pos);
        /* Check if valid discrete path delay position */
        switch(pos){
	  case NEAR:
            fprintf(stdout,"Discrete path DELAY POSITION = NEAR\n\n");
            *begfname="inscen5N";
            break;
          case FAR:
            fprintf(stdout,"Discrete path DELAY POSITION = FAR\n\n");
            *begfname="inscen5F";
            break;
          default:
            fprintf(stderr,"Invalid discrete path DELAY POSITION. Please try again. \n");
            goto Enterpos2;
        }  /* switch pos */
      }  /* label Enterpos2 */
      /* Now go through the possible combinations for this path structure to 
         specify the first portion of the filename. */
      if (scat == SCATSMALL && dire == DIRSMALL && pos == NEAR)
        *begfname="inscen8N";
      else if (scat == SCATSMALL && dire == DIRSMALL && pos == FAR)
        *begfname="inscen8F";
      else if (scat == SCATSMALL && dire == DIRMEDIUM && pos == NEAR)
        *begfname="inscen9N";
      else if (scat == SCATSMALL && dire == DIRMEDIUM && pos == FAR)
        *begfname="inscen9F";
      else if (scat == SCATSMALL && dire == DIRLARGE && pos == NEAR)
        *begfname="inscen10N";
      else if (scat == SCATSMALL && dire == DIRLARGE && pos == FAR)
        *begfname="inscen10F";
      else if (scat == SCATMEDIUM && dire == DIRSMALL && pos == NEAR)
        *begfname="inscen11N";
      else if (scat == SCATMEDIUM && dire == DIRSMALL && pos == FAR)
        *begfname="inscen11F";
      else if (scat == SCATMEDIUM && dire == DIRMEDIUM && pos == NEAR)
        *begfname="inscen12N";
      else if (scat == SCATMEDIUM && dire == DIRMEDIUM && pos == FAR)
        *begfname="inscen12F";
      else if (scat == SCATMEDIUM && dire == DIRLARGE && pos == NEAR)
        *begfname="inscen13N";
      else if (scat == SCATMEDIUM && dire == DIRLARGE && pos == FAR)
        *begfname="inscen13F";
      else if (scat == SCATLARGE && dire == DIRSMALL && pos == NEAR)
        *begfname="inscen14N";
      else if (scat == SCATLARGE && dire == DIRSMALL && pos == FAR)
        *begfname="inscen14F";
      else if (scat == SCATLARGE && dire == DIRMEDIUM && pos == NEAR)
        *begfname="inscen15N";
      else if (scat == SCATLARGE && dire == DIRMEDIUM && pos == FAR)
        *begfname="inscen15F";
      else if (scat == SCATLARGE && dire == DIRLARGE && pos == NEAR)
        *begfname="inscen16N";
      else /* (scat == SCATLARGE && dire == DIRLARGE && pos == FAR) */
        *begfname="inscen16F";
      break;
    default:
      fprintf(stderr,"\n Invalid channel structure choice. Please try again \n");
      goto Enterpaths;
    } /* switch paths */
  }  /* Label Enterpaths */

  fprintf(stdout,"\n\n THE DEFAULT SCENARIO FILE TO USE IS    %s%s\n\n\n",*begfname,*endfname);

}  /* argv */


