/*  snr2loss.c:  Calculates the propagation loss for a given SNR and Tx power
over a given link for a mobile radio communications channel.  Given the
required input parameters, solves for the Lp for the channel.
Accepts as input the following parameters:
	
	Pt:	Transmitter power [watts]
	Lt:	Loss of transmission system from transmitter amplifier to
		input of ideal transmit antenna [dB]
	Gt:	Gain of ideal transmit antenna [dBi]
	Gr:	Gain of ideal receiver antenna [dBi]
	SNR:    SNR at the input to the demodulator[dB]
	Fr:	Noise figure of receiver [dB(kTob)] 
	Lr:     Receiver circuit losses (includes
		losses due to non-ideal antenna and transmission lines,
		receiver front-end.
	Br:	Bandwidth of receiver [Hz]

and outputs:
	Lp:	Path loss [dB]

For further details, see documentation

Written by:

	Chris Nissen
	The MITRE Corporation
	Bedford, MA
	August, 1994

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

  $Id: snr2loss.c,v 1.0 1994/09/29 11:33:59 jjb Exp $
  $Log: snr2loss.c,v $
 * Revision 1.0  1994/09/29  11:33:59  jjb
 * Initial revision
 *

*/

#define PI 3.141592653589793
#define C 3.0E8
#include <stdio.h>
#include <math.h>
#include <strings.h>

static char rcsid[] = "$Id: snr2loss.c,v 1.0 1994/09/29 11:33:59 jjb Exp $";

main(argc, argv)
int argc;
char *argv[];

{



  float	Pt,Lt,Gt,SNR,Gr,Fr;   	        /* Inputs */
  float	Br,Lr;				/* Inputs */
  float	Lp;			        /* Output */
  float	EIRP,Pn;			/* Intermediates */

  fprintf(stdout,"copyright (c) 1994 The MITRE Corporation Bedford, MA\n");
  if (argc != 9)
    {

      printf("enter Transmitter power [watts]\n");
      scanf("%f", &Pt);
      printf("enter Loss of transmission system [dB]\n");
      scanf("%f", &Lt);
      printf("enter Gain of ideal transmit antenna [dBi]\n"); 
      scanf("%f", &Gt);
      printf("enter SNR at input to demodulator [dB]\n");
      scanf("%f", &SNR);
      printf("enter Gain of ideal receive antenna [dBi]\n");
      scanf("%f", &Gr);
      printf("enter Noise figure of receiver [dB(kTob)]\n");
      scanf("%f", &Fr);
      printf("enter Bandwidth of Receiver [Hz]\n");
      scanf("%f", &Br);
      printf("enter Loss of Receiver [dB]\n");
      scanf("%f", &Lr);
    }
  else
    {
      sscanf(argv[1], "%f", &Pt); 
      sscanf(argv[2], "%f", &Lt); 
      sscanf(argv[3], "%f", &Gt); 
      sscanf(argv[4], "%f", &SNR); 
      sscanf(argv[5], "%f", &Gr); 
      sscanf(argv[6], "%f", &Fr); 
      sscanf(argv[7], "%f", &Br); 
      sscanf(argv[8], "%f", &Lr); 
    }


						/* External noise is -204 dBW/Hz */
     
	EIRP = 10*log10(Pt) + Gt - Lt;		/* [dBW] */
	Pn  = Fr - 204 + 10*log10(Br) - Lr;	/* Internal Noise [dBW] */
	Lp = EIRP + Gr - SNR - Lr - Pn; 	/* [dB] */
	printf("Corresponding Propagation Loss = %3.1f\n", Lp);
}

