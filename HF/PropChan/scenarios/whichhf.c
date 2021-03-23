/* Quieries the user on some scenario specifications and then advises
   the user on which scenario script file to use to perform the
   simulation using the HF propchan program.

   MIT Lincoln Laboratory, 1997
   C. M. Keller

   $Id: whichhf.c,v 1.0 May 1997 cmk Exp $
   $Log: whichhf.c,v $

*/

#include <stdio.h>
#include <math.h>

static char rcsid[] = "$Id: whichhf.c,v 1.0 May 1997 cmk Exp $";

enum channel {QUIET, DISTURBED};
enum pathlength {SHORT, LONG, GWNVI, TWOE1F, TWOF3E};
enum dopshift {SMALL, MEDIUM, LARGE};
enum groupdel {GDSMALL, GDMEDIUM, GDLARGE};
enum RMSdop {DOPSMALL, DOPMEDIUM, DOPLARGE};
enum RMSdel {DELSMALL, DELMEDIUM, DELLARGE};

char *fname[];
char *begfname[];
char *midfname[];
char *endfname[];

main(argc,argv)
int argc;
char *argv[];
{
  enum channel chan;      /* one of 2 types */
  enum pathlength paths;  /* five choices */
  enum dopshift ds;       /* three choices */
  enum groupdel gd;       /* three choices */
  enum RMSdop dop;        /* three choices */
  enum RMSdel del;        /* three choices */

  fprintf(stdout,"\n     1997 MIT Lincoln Laboratory, Lexington MA\n");
  fprintf(stdout,"                DEFAULT SCENARIO SELECTION\n\n");

  /* input variables */
  
  /*******************************************/
  /* FIND WHETHER QUIET OR DISTURBED CHANNEL */
  /*******************************************/

  Enterchan:{
    fprintf(stderr,"Enter channel type:\n 0=Quiet\n 1=Disturbed\n ? ");
    fscanf(stdin,"%d",&chan);

    /* check if valid area */
    switch (chan) {
    case QUIET:
      fprintf(stdout,"Channel = QUIET\n\n");
      break;
    case DISTURBED:
      fprintf(stdout,"Channel = DISTURBED\n\n");
      break;
    default:
      fprintf(stderr,"\n Invalid channel type. Please try again \n");
      goto Enterchan;
    }  /* switch chan */
  }  /* label Enterchan */

  if(chan == QUIET)
  {
    Enterpaths:{
      fprintf(stderr,"Enter path structure:\n 0=Short\n 1=Long\n 2=GW/NVI\n 3=2E/1F\n 4=2F/3E\n ? ");
      fscanf(stdin,"%d",&paths);

      /* check if valid path structure */
      switch (paths) {
      case SHORT:
        fprintf(stdout,"Path = SHORT\n\n");
        *begfname="short";
        break;
      case LONG:
        fprintf(stdout,"Path = LONG\n\n");
        *begfname="long";
        break;
      case GWNVI:
        fprintf(stdout,"Path = GW/NVI\n\n");
        *fname="GW-NVI";
        break;
      case TWOE1F:
        fprintf(stdout,"Path = 2E/1F\n\n");
        *fname="2E-1F";
        break;
      case TWOF3E:
        fprintf(stdout,"Path = 2F/3E\n\n");
        *fname="2F-3E";
        break;
      default:
        fprintf(stderr,"\n Invalid path structure. Please try again \n");
        goto Enterpaths;
      }  /* switch paths */
    }  /* label Enterpaths */
    if (paths == SHORT || paths == LONG)
    {
      Entershift:{
        fprintf(stderr,"Enter Doppler shift:\n 0=small\n 1=medium\n 2=large\n ? ");
        fscanf(stdin,"%d",&ds);

        /* check if valid Doppler shift */
        switch (ds) {
        case SMALL:
          fprintf(stdout,"Doppler shift = SMALL\n\n");
          *midfname="ss";
          break;
        case MEDIUM:
          fprintf(stdout,"Doppler shift = MEDIUM\n\n");
          *midfname="ms";
          break;
        case LARGE:
          fprintf(stdout,"Doppler shift = LARGE\n\n");
          *midfname="ls";
          break;
        default:
          fprintf(stderr,"\n Invalid Doppler shift. Please try again \n");
          goto Entershift;
        }  /* switch ds */
      }  /* label Entershift */
      Enterdeldist:{
        fprintf(stderr,"Enter group delay distortion:\n 0=small\n 1=medium\n 2=large\n ? ");
        fscanf(stdin,"%d",&gd);

        /* check if valid group delay distortion */
        switch (gd) {
        case GDSMALL:
          fprintf(stdout,"Group delay distortion = SMALL\n\n");
          *endfname="sd";
          break;
        case GDMEDIUM:
          fprintf(stdout,"Group delay distortion = MEDIUM\n\n");
          *endfname="md";
          break;
        case LARGE:
          fprintf(stdout,"Group delay distortion = LARGE\n\n");
          *endfname="ld";
          break;
        default:
          fprintf(stderr,"\n Invalid group delay distortion. Please try again \n");
          goto Entershift;
        }  /* switch gd */
      }  /* label Enterdeldist */
      fprintf(stdout,"\n\n THE DEFAULT SCENARIO FILE TO USE IS    %s%s%s\n\n\n",*begfname,*midfname,*endfname);
    } /* if(paths == SHORT || paths == LONG) */
    else
      fprintf(stdout,"\n\n THE DEFAULT SCENARIO FILE TO USE IS    %s\n\n\n",*fname);
  }  /* if(chan == QUIET} */

  else  /* chan == DISTURBED */
  {
    Enterdopshft:{
      fprintf(stderr,"Enter Doppler shift:\n 0=small\n 1=medium\n 2=large\n ? ");
      fscanf(stdin,"%d",&ds);

      /* check if valid Doppler shift */
      switch (ds) {
      case SMALL:
        fprintf(stdout,"Doppler shift = SMALL\n\n");
        *begfname="ss";
        break;
      case MEDIUM:
        fprintf(stdout,"Doppler shift = MEDIUM\n\n");
        *begfname="ms";
        break;
      case LARGE:
        fprintf(stdout,"Doppler shift = LARGE\n\n");
        *begfname="ls";
        break;
      default:
        fprintf(stderr,"\n Invalid Doppler shift. Please try again \n");
        goto Enterdopshft;
      }  /* switch ds */
    }  /* label Entershift */
  Enterdopspr:{
    fprintf(stderr,"Enter amount of RMS Doppler spread:\n 0=small\n 1=medium\n 2=large\n ? ");
    fscanf(stdin,"%d",&dop);

    /* check if valid Doppler spread choice */
    switch (dop) {
    case DOPSMALL:
      fprintf(stdout,"Doppler spread = SMALL\n\n");
      *midfname="sDop";
      break;
    case DOPMEDIUM:
      fprintf(stdout,"Doppler spread = MEDIUM\n\n");
      *midfname="mDop";
      break;
    case DOPLARGE:
      fprintf(stdout,"Doppler spread = LARGE\n\n");
      *midfname="lDop";
      break;
    default:
      fprintf(stderr,"Invalid Doppler spread choice. Please try again. \n");
      goto Enterdopspr;
    }  /* switch dop */
  }  /* label Enterdopspr */
  Enterdelspr:{
    fprintf(stderr,"Enter amount of RMS Doppler spread:\n 0=small\n 1=medium\n 2=large\n ? ");
    fscanf(stdin,"%d",&del);

    /* check if valid Doppler spread choice */
    switch (del) {
    case DELSMALL:
      fprintf(stdout,"Delay spread = SMALL\n\n");
      *endfname="sDel";
      break;
    case DELMEDIUM:
      fprintf(stdout,"Delay spread = MEDIUM\n\n");
      *endfname="mDel";
      break;
    case DELLARGE:
      fprintf(stdout,"Delay spread = LARGE\n\n");
      *endfname="lDel";
      break;
    default:
      fprintf(stderr,"Invalid delay spread choice. Please try again. \n");
      goto Enterdelspr;
    }  /* switch del */
  }  /* label Enterdelspr */
    fprintf(stdout,"\n\n THE DEFAULT SCENARIO FILE TO USE IS    dist%s%s%s\n\n\n",*begfname,*midfname,*endfname);
  }

}  /* argv */


