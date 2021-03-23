/* Quieries the user on some scenario specifications and then advises
   the user on which scenario script file to use to perform the
   simulation using the mixedchan program.

   MIT Lincoln Laboratory, 1996
   C. M. Keller

   $Id: whichscen.c,v 1.0 Feb 1996 cmk Exp $
   $Log: whichscen.c,v $

*/

#include <stdio.h>
#include <math.h>

static char rcsid[] = "$Id: whichscen.c,v 1.0 Feb 1996 cmk Exp $";

enum areatype {URB_HIGH_RISE, URBSUB_LOW_RISE, RESIDENTIAL};
enum pathstruct {SCAT, SCATDIR, SCATDISC, SCATDIRDISC};
enum dslevel {SMALL, MEDIUM, LARGE};
enum relpowscat {SCATSMALL, SCATMEDIUM, SCATLARGE};
enum relpowdir {DIRSMALL, DIRMEDIUM, DIRLARGE};

char *scenario[];
char *begfname[];
char *endfname[];

main(argc,argv)
int argc;
char *argv[];
{
  enum areatype area;      /* one of 3 areas */
  enum dslevel delspr;     /* delay spread amount */
  enum pathstruct paths;    /* types of paths presents */
  enum relpowscat scat;    /* relative power for scatter path */
  enum relpowdir dire;      /* relative power for direct vs  total discrete */

  fprintf(stdout,"\n     1996 MIT Lincoln Laboratory, Lexington MA\n");
  fprintf(stdout,"                DEFAULT SCENARIO SELECTION\n\n");

  /* input variables */
  
  /*******************************************************************************/
  /* FIND THE SECOND PART OF THE DEFAULT SCENARIO FILE NAME FROM area AND delspr */
  /*******************************************************************************/

  Enterarea:{
    fprintf(stderr,"Enter environment:\n 0=Urban High-Rise\n 1=Urban/Suburban Low-Rise\n 2=Residential\n ? ");
    fscanf(stdin,"%d",&area);

  /* check if valid area */
    switch (area) {
    case URB_HIGH_RISE:
      /* Urban area with high-rise buildings */
      fprintf(stdout,"Environment = URBAN HIGH RISE\n\n");
      break;
    case URBSUB_LOW_RISE:
      /* Urban or suburban area with low-rise buildings */
      fprintf(stdout,"Environment = URBAN/SUBURBAN LOW RISE\n\n");
      break;
    case RESIDENTIAL:
      /* Residential area */
      fprintf(stdout,"Environment = RESIDENTIAL\n\n");
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

  if (area == URB_HIGH_RISE && delspr == SMALL)
    *endfname="a";
  else if (area == URB_HIGH_RISE && delspr == MEDIUM)
    *endfname="b";
  else if (area == URB_HIGH_RISE && delspr == LARGE)
    *endfname="c";
  else if (area == URBSUB_LOW_RISE && delspr == SMALL)
    *endfname="d";
  else if (area == URBSUB_LOW_RISE && delspr == MEDIUM)
    *endfname="e";
  else if (area == URBSUB_LOW_RISE && delspr == LARGE)
    *endfname="f";
  else if (area == RESIDENTIAL && delspr == SMALL)
    *endfname="g";
  else if (area == RESIDENTIAL && delspr == MEDIUM)
    *endfname="h";
  else  /* (area == RESIDENTIAL && delspr == LARGE) */
    *endfname="i";
     

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
      *begfname="scen1";
      break;
    case SCATDIR:
      /* Scatter path + direct path only */
      fprintf(stdout,"Channel structure = SCATTER + DIRECT\n\n");
      Enterscat:{
        fprintf(stderr,"Enter power level of SCATTER path relative to total path power:\n 0=small\n 1=medium\n 2=large\n ? ");
        fscanf(stdin,"%d",&scat);

        /* Check if valid power leverl choice */
        switch (scat){
          case SCATSMALL:
          /* Small scatter path power level */
          fprintf(stdout,"SCATTER path power level = SMALL\n\n");
          *begfname="scen2";
          break;
        case SCATMEDIUM:
          /* Medium scatter path power level */
          fprintf(stdout,"SCATTER path power level = MEDIUM\n\n");
          *begfname="scen3";
          break;
        case SCATLARGE:
          /* Large scatter path power level */
          fprintf(stdout,"SCATTER path power level = LARGE\n\n");
          *begfname="scen4";
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

        /* Check if valid power leverl choice */
        switch (scat){
          case SCATSMALL:
          /* Small scatter path power level */
          fprintf(stdout,"SCATTER path power level = SMALL\n\n");
          *begfname="scen5";
          break;
        case SCATMEDIUM:
          /* Medium scatter path power level */
          fprintf(stdout,"SCATTER path power level = MEDIUM\n\n");
          *begfname="scen6";
          break;
        case SCATLARGE:
          /* Large scatter path power level */
          fprintf(stdout,"SCATTER path power level = LARGE\n\n");
          *begfname="scen7";
          break;
        default:
          fprintf(stderr,"Invalid scatter path power level. Please try again. \n");
          goto Enterscat2;
        }  /* switch scat */
      }  /* label Enterscat2 */
      break;
    case SCATDIRDISC:
      /* Scatter path + direct path + other discrete paths */
      fprintf(stdout,"Channel structure = SCATTER + DIRECT + DISCRETE\n\n");

      Enterscat3:{
        fprintf(stderr,"Enter power level of SCATTER path relative to total path power:\n 0=small\n 1=medium\n 2=large\n ? ");
        fscanf(stdin,"%d",&scat);

        /* Check if valid power leverl choice */
        switch (scat){
          case SCATSMALL:
          /* Small scatter path power level */
          fprintf(stdout,"SCATTER path power level = SMALL\n\n");
          Enterdire1:{
            fprintf(stderr,"Enter power level of DIRECT path relative to total discrete-path power:\n 0=small\n 1=medium\n 2=large\n ? ");
            fscanf(stdin,"%d",&dire);
    
            /* Check if valid power leverl choice */
            switch (dire){
              case DIRSMALL:
              /* Small direct path power level */
              fprintf(stdout,"DIRECT path power level = SMALL\n\n");
              *begfname="scen8";
              break;
            case DIRMEDIUM:
              /* Medium direct path power level */
              fprintf(stdout,"DIRECT path power level = MEDIUM\n\n");
              *begfname="scen9";
              break;
            case DIRLARGE:
              /* Large direct path power level */
              fprintf(stdout,"DIRECT path power level = LARGE\n\n");
              *begfname="scen10";
              break;
            default:
              fprintf(stderr,"Invalid direct path power level. Please try again. \n");
              goto Enterdire1;
            }  /* switch dire */
          }  /* label Enterdire1 */
          break;
        case SCATMEDIUM:
          /* Medium scatter path power level */
          fprintf(stdout,"SCATTER path power level = MEDIUM\n\n");
          Enterdire2:{
            fprintf(stderr,"Enter power level of DIRECT path relative to total discrete-path power:\n 0=small\n 1=medium\n 2=large\n ? ");
            fscanf(stdin,"%d",&dire);
  
            /* Check if valid power leverl choice */
            switch (dire){
              case DIRSMALL:
              /* Small direct path power level */
              fprintf(stdout,"DIRECT path power level = SMALL\n\n");
              *begfname="scen11";
              break;
            case DIRMEDIUM:
              /* Medium direct path power level */
              fprintf(stdout,"DIRECT path power level = MEDIUM\n\n");
              *begfname="scen12";
              break;
            case DIRLARGE:
              /* Large direct path power level */
              fprintf(stdout,"DIRECT path power level = LARGE\n\n");
              *begfname="scen13";
              break;
            default:
              fprintf(stderr,"Invalid direct path power level. Please try again. \n");
              goto Enterdire2;
            }  /* switch dire */
          }  /* label Enterdire2 */
          break;
        case SCATLARGE:
          /* Large scatter path power level */
          fprintf(stdout,"SCATTER path power level = LARGE\n\n");
          Enterdire3:{
            fprintf(stderr,"Enter power level of DIRECT path relative to total discrete-path power:\n 0=small\n 1=medium\n 2=large\n ? ");
            fscanf(stdin,"%d",&dire);
  
            /* Check if valid power leverl choice */
            switch (dire){
              case DIRSMALL:
              /* Small direct path power level */
              fprintf(stdout,"DIRECT path power level = SMALL\n\n");
              *begfname="scen14";
              break;
            case DIRMEDIUM:
              /* Medium direct path power level */
              fprintf(stdout,"DIRECT path power level = MEDIUM\n\n");
              *begfname="scen15";
              break;
            case DIRLARGE:
              /* Large direct path power level */
              fprintf(stdout,"DIRECT path power level = LARGE\n\n");
              *begfname="scen16";
              break;
            default:
              fprintf(stderr,"Invalid direct path power level. Please try again. \n");
              goto Enterdire3;
            }  /* switch dire */
          }  /* label Enterdire3 */

          break;
        default:
          fprintf(stderr,"Invalid scatter path power level. Please try again. \n");
          goto Enterscat3;
        }  /* switch scat */
      }  /* label Enterscat3 */


      break;
    default:
      fprintf(stderr,"\n Invalid channel structure choice. Please try again \n");
      goto Enterpaths;
    } /* switch paths */
  }  /* Label Enterpaths */

  fprintf(stdout,"\n\n THE DEFAULT SCENARIO FILE TO USE IS    %s%s\n\n\n",*begfname,*endfname);

}  /* argv */


