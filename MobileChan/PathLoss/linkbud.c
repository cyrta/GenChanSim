/*  linkbud.c:  Calculates a link budget for a mobile radio communications
channel.  Given the required input parameters, solves for the SNR at the input
to the demodulator.  Accepts as input the following parameters:
	
	Pt:	Transmitter power [watts]
	Lt:	Loss of transmission system from transmitter amplifier to
		input of ideal transmit antenna [dB]
	Gt:	Gain of ideal transmit antenna [dBi]
	Lp:	Path loss [dB]
	sig:	Sigma on Lp [dB]
	Gr:	Gain of ideal receiver antenna [dBi]
	Fr:	Noise figure of receiver [dB(kTob)] 
	Lr:     Receiver circuit losses (includes
		losses due to non-ideal antenna and transmission lines,
		receiver front-end.
	Br:	Bandwidth of receiver [Hz]

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

  $Id: linkbud.c,v 1.0 1994/09/07 13:56:48 jjb Exp $
  $Log: linkbud.c,v $
 * Revision 1.0  1994/09/07  13:56:48  jjb
 * Initial revision
 *

*/

#define PI 3.141592653589793
#define C 3.0E8
#include <stdio.h>
#include <math.h>
#include <strings.h>

static char rcsid[] = "$Id: linkbud.c,v 1.0 1994/09/07 13:56:48 jjb Exp $";

main(argc, argv)
int argc;
char *argv[];

{



  float	Pt,Lt,Gt,Lp,Gr,Fr,sig;   	/* Inputs */
  float	Br,Lr;				/* Inputs */
  float	SNR,SNR1,SNR2;			/* Output */
  float	EIRP,Pn;			/* Intermediates */

  fprintf(stdout,"copyright (c) 1994 The MITRE Corporation Bedford, MA\n");
  if (argc != 10)
    {

      printf("enter Transmitter power [watts]\n");
      scanf("%f", &Pt);
      printf("enter Loss of transmission system [dB]\n");
      scanf("%f", &Lt);
      printf("enter Gain of ideal transmit antenna [dBi]\n"); 
      scanf("%f", &Gt);
      printf("enter Path loss [dB]\n");
      scanf("%f", &Lp);
      printf("enter sigma on path loss [dB]\n");
      scanf("%f", &sig);
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
      sscanf(argv[4], "%f", &Lp); 
      sscanf(argv[5], "%f", &sig);
      sscanf(argv[6], "%f", &Gr); 
      sscanf(argv[7], "%f", &Fr); 
      sscanf(argv[8], "%f", &Br); 
      sscanf(argv[9], "%f", &Lr); 
    }


						/* External noise is -204 dBW/Hz */
     
	EIRP = 10*log10(Pt) + Gt - Lt;		/* [dBW] */
	Pn  = Fr - 204 + 10*log10(Br) - Lr;	/* Internal Noise [dBW] */
	SNR = EIRP + Gr - Lp - Lr - Pn; 	/* [dB] */
	SNR1 = SNR - sig;
	SNR2 = SNR + sig;
	printf("SNR = %3.1f\n", SNR);
	printf("High/Low Standard Deviation Bounds on SNR = %3.1f %3.1f\n",SNR1, SNR2);
}

