/* Gen Power delay spectrum weights---read in a data file
 *
   copyright (c) 1994 The MITRE Corporation Bedford, MA

   $Id: expweight.c,v 3.0 1994/08/30 18:31:22 jjb Exp $
   $Log: expweight.c,v $

 * Lincoln Lab Revision 12/1995 Linda Riehl and Cathy Keller
 * copied expweight to dataweight and changed to read in a specified
 * file of delay power spectral density samples.
 *
 * Revision 3.0  1994/08/30  18:31:22  jjb
 * created for version 3.0 of software.
 *

 */

#include <stdio.h>
#include <math.h>
#include <mixedchan.h>

static char rcsid[] = "$Id: expweight.c,v 3.0 1994/08/30 18:31:22 jjb Exp $";

int dataweight(
	       float weight_buff[],  /* weights (returned) */
	       int sttapnum[],	     /* starting tap number inside 'weight_buff' */
	       int parnum,	     /* current partition (path) number */
	       char * data_fname)
{
  int i;
  float *buff_ptr;
  float temp,buff[MAXTAPS];
  float total_power;
  FILE *fp;
  int tap_ct;

  /* printf("data_fname is %s\n",data_fname); */
  total_power = 0.;

  if((fp = fopen(data_fname,"r")) == NULL){
	printf("Can't open file %s\n",
						 data_fname);
	exit(1);
  }

  /* first data element in file is no. taps */

  fscanf(fp,"%e",&temp);
  tap_ct=(int)temp;

  /* printf("The tap count is %d\n",tap_ct); */

  for(i=0; i<tap_ct; ++i) {
    fscanf(fp,"%e",&buff[i]);
/*    printf("buff %d is %e\n",i,buff[i]);*/
    total_power += buff[i];
  }
  /* transfer normalized delay weights to output buffer*/

  buff_ptr = weight_buff + sttapnum[parnum];
  for (i = 0; i < tap_ct; i++) 
    *buff_ptr++ = sqrt(buff[i]/total_power);

  return(tap_ct);

}

