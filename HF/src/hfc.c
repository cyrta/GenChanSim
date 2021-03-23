/* Propagation channel, Atmospheric Noise, and Narrowband Interference Models
 * as described in 'Wideband HF Propagation, 
 * Narrowband Interference, and Atmospheric Noise Models for Link Performance 
 * Evaluation', Phil Bello, MTR 93B0000086 and 'Software Wide Band HF Simulation
 * User's Manual', Richard Coutts, MTR93B0000122

   copyright (c) 1994 The MITRE Corporation Bedford, MA

   $Id: hfc.c,v 3.0 1994/08/31 19:02:27 jjb Exp $
   $Log: hfc.c,v $
 * Revision 3.0  1994/08/31  19:02:27  jjb
 * added scatter/discrete simulator to list of choices.
 *
 * Revision 2.2  1994/07/25  19:36:41  jjb
 * added copyright notice.
 *

 * 
 * r.coutts

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

 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* local includes */
#include <Cmdline.h>
#include <pathfile.h>

extern void Atmosphere();
extern void Narrowband();
extern void Propchan();
extern void MobilechanJTC();
extern void MobilechanMacrocell();
extern void Noise();
extern void Signal();
extern void Filesum();
extern void Fileconv();
extern void InvokeProcess(char *processName);

static char rcsid[] = "$Id: hfc.c,v 3.0 1994/08/31 19:02:27 jjb Exp $";

void main(int argc, char **argv) {
	char	*helpStr;			/* help string */

	/* print copyright notice */

	fprintf(stdout,"copyright (c) 1994 The MITRE Corporation Bedford, MA\n");

	/* set command line args for interface */
	CmdButton("-a Atmospheric_Noise", &Atmosphere, NULL);
	CmdButton("-n Narrowband_Interference", &Narrowband, NULL);
	CmdButton("-p HF_Channel", &Propchan, NULL);
	CmdButton("-m1 Mobile_Chan_JTC_Model", &MobilechanJTC, NULL);
	CmdButton("-m2 Mob._Chan._Scat/Dis_Model", &MobilechanMacrocell, NULL);
	CmdButton("-g Gaussian_Noise", &Noise, NULL);
	CmdButton("-s Test_Signal", &Signal, NULL);
	CmdButton("-f Sum_Files", &Filesum, NULL);
	CmdButton("-c Convert_File", &Fileconv, NULL);
	CmdNoOk();
	CmdHelp(helpStr = PathFile("HF/src/hfc.hlp"));
	CmdArgs(argc, argv);
	free(helpStr);
}

/* invoke the atmospheric noise process */
void Atmosphere() {

	InvokeProcess("HF/AddDist/atmosphere/atmosphere -win");
}

/* invoke the narrowband interference process */
void Narrowband() {

	InvokeProcess("HF/AddDist/narrowband/narrowband -win");
}

/* invoke the propcagation channel process */
void Propchan() {

	InvokeProcess("HF/PropChan/propchan -win");
}

/* invoke the mobile comm channel process */
void MobilechanJTC() {

	InvokeProcess("MobileChan/PropChan/LandMobile/cellular/JTCmod/JTCchan -win");
}

/* invoke the mobile comm channel process */
void MobilechanMacrocell() {

	InvokeProcess("MobileChan/PropChan/LandMobile/cellular/mixedmod/mixedchan -win");
}

/* invoke the gaussian noise process */
void Noise() {

	InvokeProcess("HF/AddDist/gaussnoise/gaussnoise -win");
}

/* invoke the test signal process */
void Signal() {

	InvokeProcess("Tools/signal/signal -win");
}

/* invoke the file summing process */
void Filesum() {

	InvokeProcess("Tools/filesum -win");
}

/* invoke the file conversion process */
void Fileconv() {

	InvokeProcess("Tools/fileconv -win");
}

/* invoke a process */
void InvokeProcess(char *process) {
	char	*string;		/* path and filename */
	char	*c;				/* ptr within 'process' */

	/* print process being run */
	c = strchr(process, '/');
	if(c != NULL) c++;		/* skip '/' */
	else c = process;
	printf("running '%s'...\n", c); fflush(stdout);
	/* get path to simulation */
	string = PathFile(process);
	/* run process */
	system(string);
	free(string);
}
