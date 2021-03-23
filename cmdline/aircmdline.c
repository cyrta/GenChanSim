/* Routines to define and read command-line args and create motif input window.

   copyright (c) 1994 The MITRE Corporation Bedford, MA

   $Id: cmdline.c,v 2.2 1996/03/25 cmk Exp $
   $Log: cmdline.c,v $

 * Revision LL 1.0 March/1997 cmk
 * Move path relative gain parameters to one line for a better fit.
 *
 * Revision LL 0.0 March/1996 lbr & cmk
 * Revised window for the air to air channel parameters to fit.
 *
 * Revision 2.2  1994/07/25  18:18:32  jjb
 * Added copyright notice
 *

 */

#ifndef NO_MOTIF
/* motif include files */
#include <Xm/Xm.h>
#include <X11/Shell.h>
#include <Xm/ArrowB.h>
#include <Xm/ArrowBG.h>
#include <Xm/BulletinB.h>
#include <Xm/CascadeB.h>
#include <Xm/CascadeBG.h>
#include <Xm/Command.h>
#include <Xm/CutPaste.h>
#include <Xm/DialogS.h>
#include <Xm/DrawingA.h>
#include <Xm/DrawnB.h>
#include <Xm/FileSB.h>
#include <Xm/Form.h>
#include <Xm/Frame.h>
#include <Xm/Label.h>
#include <Xm/LabelG.h>
#include <Xm/List.h>
#include <Xm/MainW.h>
#include <Xm/MenuShell.h>
#include <Xm/MessageB.h>
#include <Xm/PanedW.h>
#include <Xm/PushB.h>
#include <Xm/PushBG.h>
#include <Xm/RowColumn.h>
#include <Xm/Scale.h>
#include <Xm/ScrollBar.h>
#include <Xm/ScrolledW.h>
#include <Xm/SelectioB.h>
#include <Xm/SeparatoG.h>
#include <Xm/Separator.h>
#include <Xm/Text.h>
#include <Xm/ToggleB.h>
#include <Xm/ToggleBG.h>
#include <X11/StringDefs.h>
#endif
#include	<stdio.h>
#include	<stdlib.h>
#include	<stdarg.h>
#include	<string.h>
/* local include */
#include	<Cmdline.h>

/* defines */
#define	APPLICATION		"Cmdline"		/* application name */
#define	TITLE			"Cmdline 1.0"		/* application name */
#define	STRLEN			100				/* len of string[] */
#define	TEMPFILE		"_cmdTempFile"	/* temporary file */
#define	NEVENTS			10				/* # events to destroy window */
#define	YSPACING		30				/* y spacing of motif fields */
#define	YSPACE			-20				/* additional space (=YSPACING+YSPACE)*/
#define	YTITLE			(YSPACING/2) 	/* y of title */
#define	SCRIPTFILE		"CmdScript"		/* default script file name */
#define	DEPNARGS		30				/* max # args allowed in cmddep() */
#define	RESOURCEFILE	"Cmdline"		/* resource file name */
#define	WINDOWFLAG		"-win"			/* optional window flag */
#define EXP 0
#define BESSEL 2
#define DATA 1
Widget fnm_text;
char *fname;
/* static char rcsid[] = "$Id: cmdline.c,v 2.2 1994/07/25 18:18:32 jjb Exp $"; */
static char rcsid[] = "$Id: cmdline.c,v LL0.0 1996/03/25  lbrcmk Exp $";

/* functions */
#ifndef NO_MOTIF
extern void CmdOkCB(Widget, XtPointer, XtPointer);
extern void CmdHelpCB(Widget, XtPointer, XtPointer);
extern void CmdCancelCB(Widget, XtPointer, XtPointer);
extern void CmdEnterCB(Widget, XtPointer, XtPointer);
extern void CmdValueChangedCB(Widget, XtPointer, XtPointer);
extern void CmdSingleClickCB(Widget, XtPointer, XtPointer);
extern void CmdOptionalToggleCB(Widget, XtPointer, XtPointer);
extern void CmdFlagCB(Widget, XtPointer, XtPointer);
extern void CmdButtonCB(Widget, XtPointer, XtPointer);
extern void CmdPlusButtonCB(Widget, XtPointer, XtPointer);
extern void CmdMinusButtonCB(Widget, XtPointer, XtPointer);
#endif
extern void CmdArrayButtonCB(CmdInfo *, int);
extern void CmdDuplicateIDs(CmdInfo *);
extern CmdInfo *CmdFind(void *);
extern void CmdUsage(char *);
extern int CmdCheckError(char *);
extern void CmdWin(char *);
extern void CmdUpdateBoard(void);
extern void CmdSetXApplResFile(char *, char *);
extern void CmdLabel(char *, char *, int); /* get label from 'id' */
extern void CmdGetDataFromWidget(CmdInfo *);
extern void CmdPutDataInWidget(CmdInfo *);
extern void CmdGetAllDataFromWidgets(void);
extern void CmdFPrint(FILE *, char *);
extern void CmdPrint(void);
extern void CmdCheckSensitive(CmdInfo *);
extern void CmdInit(CmdInfo *, char *, va_list, enum cmdType);
#ifndef NO_MOTIF
extern void CmdFlagWidget(Widget, Widget, CmdInfo *, char *, int);
extern Widget CmdColumnWidget(Widget, Widget, char *);
extern void CmdTextWidget(Widget, Widget, CmdInfo *, char *, int);
extern void CmdLabelWidget(Widget, Widget, CmdInfo *, char *, int);
extern void CmdCopyrightWidget(Widget, Widget, int);
extern void CmdFileWidget(Widget, Widget, CmdInfo *, char *, int);
extern void CmdOptionalWidget(Widget, Widget, CmdInfo *, char *, int);
extern void CmdEnterWidget(Widget, Widget, CmdInfo *, char *, int);
extern Widget CmdIndexWidget(Widget, Widget, int);
extern void CmdTitleWidget(Widget, Widget, char *, int);
extern Widget CmdButtonWidget(Widget, Widget, char *, char *, int,
	void (*)(Widget, XtPointer, XtPointer), XtPointer);
#endif
extern int CmdSenseSet(CmdInfo *);
extern CmdInfo *CmdGetNextCmd(void);	/* function to add arg to linked list */
#ifndef NO_MOTIF
extern void CmdCreateBulletinBoard(Widget, CmdInfo *, char *);
#endif
extern void CmdNumberPhoneme(int);

/* globals (yeah, I used some globals).  Mostly so information didn't have
 * to be passed by the programmer */
enum {NO, YES};				/* boolean */
Widget	Shell000 = NULL;
#ifndef NO_MOTIF
static	XtAppContext context;
static	XEvent		event;
#endif
CmdInfo	*cmd0 = NULL;		/* top of linked list of command line args */
char	scriptFile[STRLEN] = SCRIPTFILE;	/* script file name */
CmdInfo	*scriptCmd = NULL;	/* script file name arg */
CmdInfo	*windowCmd;			/* optional window arg */
int		windowFlag;			/* flag to invoke command window */
int		okHit = 0;			/* flag for ok button */
int		updatingBoard;		/* flag to tell CmdValueChangedCB() to ignore */
char	*helpFile = NULL;	/* help string */
int		radioBox = 0;		/* radioBox number */
int		nFunctions = 0;		/* number of functions entered */
int		showOkButton = YES;	/* flag to show OK button */

/* set up dependencies among arguments, i.e., only some args will be sensitive
 * when a flag is set, or an integer arg may define how many elements should
 * be entered into an array arg.
 *
 * CmdDep(firstval, dependent1, dependent2, ..., type)
 *
 * type = cmdSensitive, cmdArray, or cmdRadioBox
 */
void CmdDep(void *firstval, ...) {
	va_list	ap;					/* points to each param passed to CmdDep() */
	void	*dependent;			/* member of arg list dependent on firstval */
	CmdInfo	*cmd;				/* member of arg list */
	int		nCmds = 1;			/* arg counter */
	char	*mode;				/* mode (cmdArray, cmdSensitive) */
	static int ncalls = 0;		/* count calls for error reporting */

	ncalls++;					/* count number of calls for error reporting */
	va_start(ap, firstval);	/* make ap point to 1st unnamed arg */
	/* get mode (last value passed) */
	do {
		mode = (void *)va_arg(ap, void *); 	/* get 1st param passed */
		nCmds++;
	} while(mode != (char *)cmdArray && mode != (char *)cmdSensitive && 
		mode != (char *)cmdRadioBox && nCmds < DEPNARGS);
	if(nCmds == DEPNARGS) {
		printf("Error from CmdDep: too many arguments or missing mode on ");
		CmdNumberPhoneme(ncalls);
		printf(" call.\n");
		exit(1);
	}
	if(mode == (char *)cmdRadioBox) {
		radioBox++;	/* incr radioBox counter */
		cmd = CmdFind(firstval);	/* get arg from list */
		if(cmd == NULL) {
			printf("Error from CmdDep: 1st argument not matched on ");
			CmdNumberPhoneme(ncalls);
			printf(" call.\n");
			exit(1);
		}
		cmd->radioBox = radioBox;	/* set radioBox index */
	}
	/* if 'firstval' contols sensitivity of other values... */
	if(mode == (char *)cmdSensitive || mode == (char *)cmdArray) {
		/* set the flag indicating that the argument containing 'firstval'
		 * controls the sensitivity of other args */
		CmdFind(firstval)->controlSens = YES;
	}
	nCmds = 1;						/* count the # cmds processed */
	va_start(ap, firstval);	/* make ap point to 1st unnamed arg */
	/* find the member containing 'dependent' */
	dependent = (void *)va_arg(ap, void *); 	/* get 1st param passed */
	do {							/* loop while params match arg in list */
		cmd = CmdFind(dependent);		/* get arg from list */
		nCmds++;
		if(cmd == NULL) {
			printf("Error from CmdDep(): ");
			CmdNumberPhoneme(nCmds);
			printf(" argument not matched on ");
			CmdNumberPhoneme(ncalls);
			printf(" call.\n");
			exit(1);
		}
		if(mode == (char *)cmdArray) {
			cmd->nReq = firstval;		/* firstval defines [] size */
			cmd->controller[cmd->nControllers++] = firstval;/* sensitive flag */
		} else if(mode == (char *)cmdSensitive) {
			cmd->controller[cmd->nControllers++] = firstval;/* sensitive flag */
		} else if(mode == (char *)cmdRadioBox) {
			cmd->radioBox = radioBox;
		} else {
			printf("Error from CmdDep(): invalid or missing mode on ");
			CmdNumberPhoneme(ncalls);
			printf(" call.\n");
			exit(1);
		}
		dependent = (void *)va_arg(ap, void *); /* get next param passed */
	} while(dependent != (char *)cmdArray && dependent != (char *)cmdSensitive
		&& dependent != (char *)cmdRadioBox);
	va_end(ap);
}

/* return arg in list containing the variable '*data' */
CmdInfo *CmdFind(void *data) {
	CmdInfo	*cmd;			/* arg in list */

	cmd = cmd0;												/* top of list */
	while(cmd != NULL) {
		if((cmd->func == NULL && cmd->data == data) ||
			(void *)(cmd->func) == data) 
		{
			break;					/* cmd found */
		}
		cmd = cmd->next;			/* next arg */
	}
	return cmd;
}

/* print number of calls to function for error reporting */
void CmdNumberPhoneme(int call) {
	printf("%d", call);
	if(call%10 == 1 && call%100 != 11) printf("st");
	else if(call%10 == 2 && call%100 != 12) printf("nd");
	else if(call%10 == 3 && call%100 != 13) printf("rd");
	else printf("th", call);
}

/* add floating point member to list of cmds */
CmdInfo *CmdFloat(char *fmt, ...) {
	va_list	ap;					/* points to each unnamed arg in turn */
	CmdInfo *cmd;

	cmd = CmdGetNextCmd();
	va_start(ap, fmt);			/* make ap point to 1st unnamed arg */
	CmdInit(cmd, fmt, ap, cmdFLOAT);
	return cmd;
}


/* add file selection window to list of cmds */
CmdInfo *CmdFile(char *fmt, ...) {
	va_list	ap;				   
	CmdInfo *cmd;

	cmd = CmdGetNextCmd();
	va_start(ap, fmt);	
	CmdInit(cmd, fmt, ap, cmdFILE);
	return cmd;
}

/* add string member to list of cmds */
CmdInfo *CmdString(char *fmt, ...) {
	va_list	ap;					/* points to each unnamed arg in turn */
	CmdInfo *cmd;
	int		i;

	cmd = CmdGetNextCmd();
	va_start(ap, fmt);	/* make ap point to 1st unnamed arg */
	CmdInit(cmd, fmt, ap, cmdCHAR);
	/* Check if the first char is a NULL.  If it is, then null out the first
	 * char of ea array element.  This is to prevent bogus characters being
	 * written to the script file.
	 */
	if(*(char *)cmd->data == NULL) {		/* if first element is empty... */
		for(i = 1; i < cmd->dim; i++) {		/* loop thru ea element */
			*((char *)cmd->data+i*cmd->len) = NULL; /* blank ea element */
		}
	}
	return cmd;
}

/* add column member to list of cmds */
void CmdColumn(void) {
	CmdInfo *_CmdColumn(char *fmt, ...);
	_CmdColumn("-_column", cmdNA);
}

/* tell cmdline to add ok button */
void CmdNoOk(void) {
	showOkButton = NO;
}

/* add space member to list of cmds */
void CmdSpace(void) {
	CmdInfo *_CmdSpace(char *fmt, ...);
	_CmdSpace("-_space", cmdSPACE);
}

/* init column member to list of cmds */
CmdInfo *_CmdColumn(char *fmt, ...) {
	va_list	ap;					/* points to each unnamed arg in turn */
	CmdInfo *cmd;

	cmd = CmdGetNextCmd();
	va_start(ap, fmt);			/* make ap point to 1st unnamed arg */
	CmdInit(cmd, fmt, ap, cmdCOLUMN);
	return cmd;
}

/* init space member to list of cmds */
CmdInfo *_CmdSpace(char *fmt, ...) {
	va_list	ap;					/* points to each unnamed arg in turn */
	CmdInfo *cmd;

	cmd = CmdGetNextCmd();
	va_start(ap, fmt);			/* make ap point to 1st unnamed arg */
	CmdInit(cmd, fmt, ap, cmdSPACE);
	return cmd;
}

/* add function member to list of cmds */
CmdInfo *CmdButton(char *fmt, ...) {
	va_list	ap;					/* points to each unnamed arg in turn */
	CmdInfo *cmd;

	nFunctions++;				/* count the number of functions */
	cmd = CmdGetNextCmd();
	va_start(ap, fmt);			/* make ap point to 1st unnamed arg */
	CmdInit(cmd, fmt, ap, cmdBUTTON);
	return cmd;
}

/* add integer member to list of cmds */
CmdInfo *CmdInt(char *fmt, ...) {
	va_list	ap;					/* points to each unnamed arg in turn */
	CmdInfo *cmd;

	cmd = CmdGetNextCmd();
	va_start(ap, fmt);			/* make ap point to 1st unnamed arg */
	CmdInit(cmd, fmt, ap, cmdINT);
	return cmd;
}

/* receive help string */
void CmdHelp(char *file) {
	helpFile = file;		/* set global variable to text string */
}

/* add cmdFLAG member to list of cmds */
CmdInfo *CmdFlag(char *fmt, ...) {
	va_list	ap;		/* points to each unnamed arg in turn */
	CmdInfo *cmd;

	cmd = CmdGetNextCmd();
	va_start(ap, fmt);	/* make ap point to 1st unnamed arg */
	CmdInit(cmd, fmt, ap, cmdFLAG);
	return cmd;
}

/* init struct using cmd parameters */
void CmdInit(
	CmdInfo		*cmd,
	char		*fmt,
	va_list		ap,
	enum cmdType type)
{
	int		i;			/* index */

	/* init and blank structure */
	cmd->entered = NO;			/* entered flag initialized to NO */
	cmd->nRead = 0;				/* # read */
	cmd->index = 0;				/* index to *data */
	cmd->nReq = NULL;			/* # array elements required */
	cmd->id = fmt;				/* input ID */
	cmd->len = 1;				/* init to 1-dim (dim=len below) */
	cmd->enterMode = 1;			/* init to 1-dim (len=mandetory below) */
	cmd->nControllers = 0;		/* init number of sensitive dependencies */
	cmd->sensitive = YES;		/* sensitivity flag */
	cmd->radioBox = 0;			/* index to radioBox */
	cmd->label = NULL;			/* widget */
	cmd->controlSens = 0;		/* flag indicating arg controls sensitivity */
	cmd->mainWidget = NULL;		/* main widget */
	cmd->listWidget = NULL;		/* list widget */
	cmd->listCount = 0;			/* number of item in list */
	cmd->scrWinWidget = NULL;	/* list widget */
	cmd->indexWidget = NULL;	/* index widget */
	cmd->optionWidget = NULL;	/* index widget */
	cmd->enterWidget = NULL;	/* ENTER button widget */
	cmd->minusWidget = NULL;	/* index widget */
	cmd->plusWidget = NULL;		/* index widget */
	cmd->func = NULL;			/* function ptr */
	cmd->type = type;			/* save type (cmdINT, etc) */
	/* init data pointer */
	if(type == cmdFLAG) {
		cmd->data = (int *)va_arg(ap, int *);
		if(*(int *)cmd->data) {	/* check that flag is zero */
			printf("Warning from CmdFlag: non-zero initial value found in "
			"pointer for '%s'.  Value set to zero.\n", cmd->id);
		}
		*(int *)cmd->data = NO;	/* set flag to 'no' */
	} else if(type == cmdINT) {
		cmd->data = (int *)va_arg(ap, int *);

	} else if(type == cmdFLOAT) {
		cmd->data = (float *)va_arg(ap, float *);
	} else if(type == cmdCHAR) {
		cmd->data = (char *)va_arg(ap, char *);
	} else if(type == cmdFILE) {
		cmd->data = (char *)va_arg(ap, char *);
	} else if(type == cmdSPACE) {
		cmd->data = (char *)NULL;
	} else if(type == cmdCOLUMN) {
		cmd->data = (char *)NULL;
	} else if(type == cmdBUTTON) {
		cmd->func = (void (*))(va_arg(ap, char *));	/* get function */
		cmd->data = (void *)va_arg(ap, char *);	/* get data array */
	}
	/* get '...' parameters.. */
	if(cmd->type == cmdFLAG) {
		cmd->dim = 1;		/* cmdline doen't handle function data */
		cmd->enterMode = cmdOpt;	/* function must be optional */
	} else if(cmd->type == cmdBUTTON) {
		cmd->dim = 1;		/* cmdline doen't handle function data */
		cmd->enterMode = cmdOpt;	/* function must be optional */
	} else if(cmd->type == cmdSPACE || cmd->type == cmdCOLUMN) {
		cmd->enterMode = cmdNA;	/* enter mode is not applicable */
	} else {				/* else, type = cmdINT, cmdFLOAT, cmdFILE */
		/* loop until 'entermode' is found. */
		do {
			i = va_arg(ap, int);
			cmd->dim = cmd->len;
			cmd->len = cmd->enterMode;
			cmd->enterMode = i;
		} while(i != cmdOpt && i != cmdMan && i != cmdDef && i != cmdNA);
		/* do one more iteration of above in cmdINT or cmdFLOAT to get the array
		 * length into 'dim' */
		if(cmd->type == cmdINT || cmd->type == cmdFLOAT) {
			cmd->dim = cmd->len;
			cmd->len = 1;			/* 'm' dim array is viewed as 'm x 1' */
		} else if(cmd->type == cmdFILE) {
			cmd->dim = 1;	/* only one file name */
		}

	}
	va_end(ap);
}

/* print out struct contents for debugging */
void CmdPrint(void) {
	int		i = 1;
	CmdInfo *cmd;

	cmd = cmd0;
	printf(" #  sensitiv nReq mand nRead index  type ID desc\n");
	printf("-- ---- --- ----- ---- ----- ----- ----- -----------\n");
	while(cmd != NULL) {
		printf("%2d ", i);
		if(cmd->nControllers) printf("flag ");
		else           printf("     ");
		if(cmd->nControllers && CmdSenseSet(cmd)) printf("set ");
		else                            printf("    ");
		if(cmd->nReq) printf("param ");
		else          printf("      ");
		printf("%4d ", cmd->enterMode);
		printf("%5d ", cmd->nRead);
		printf("%5d ", cmd->index);
		if(cmd->type == cmdINT) {
			printf("  cmdINT ");
		} else if(cmd->type == cmdFLOAT) {
			printf("cmdFLOAT ");
		} else if(cmd->type == cmdFILE) {
			printf(" FILE ");
		} else if(cmd->type == cmdCHAR) {
			printf(" cmdCHAR ");
		} else if(cmd->type == cmdFLAG) {
			printf(" cmdFLAG ");
		}
		printf("%s ", cmd->id);
		printf("\n");
		cmd = cmd->next;
	}
	printf("\n");

	cmd = cmd0;
	while(cmd != NULL) {
		if(cmd->entered) {
			printf("%30s ", cmd->id);
			if(cmd->type == cmdFLAG) {
				printf("set ");
			} else if(cmd->type == cmdINT) {
				for(i = 0; i < cmd->nRead; i++) {
					printf("%d ", *((int *)cmd->data+i));
				}
			} else if(cmd->type == cmdFLOAT) {
				for(i = 0; i < cmd->nRead; i++) {
					printf("%g ", *((float *)cmd->data+i));
				}
			} else if(cmd->type == cmdCHAR || cmd->type == cmdFILE) {
				for(i = 0; i < cmd->nRead; i++) {
					printf("%s ", (char *)cmd->data+i*cmd->len);
				}
			}
			printf("\n");
		}
		cmd = cmd->next;
	}
}

/* check if any sensitive dependencies are set.  If no sensitivity dependencies
 * exist, return YES */
int CmdSenseSet(CmdInfo *cmd) {
	int		i;

	if(cmd->nControllers == 0) return YES;
	for(i = 0; i < cmd->nControllers; i++) {
		if(*(cmd->controller[i]) != NULL) return YES;	/* return set */
	}
	return NO;	/* return not set */
}

/* function to alloc arg and add to linked list */
CmdInfo *CmdGetNextCmd(void) {
	static CmdInfo *lastcmd;
	CmdInfo	*out;

	out = (CmdInfo *)malloc(sizeof(CmdInfo));
	if(cmd0 == NULL) cmd0 = out;
	else lastcmd->next = out;
	lastcmd = out;
	lastcmd->next = NULL;
	return(out);
}

/* get input args from user */
void CmdArgs(int ArgC, char **ArgV) {
	static int firstCall = YES;	/* 1st call to function flag */
	CmdInfo	*cmd;				/* c.l.arg */
	char	id[STRLEN];			/* id with dummy phrase stripped */
	int		max;				/* max number of values to read in */
	int		len;				/* length of character strings */
	char	*pathProg;			/* file name being run (including path) */
	char	*progName;			/* program name (no path) */
	int		functionCalled = NO;/* flag to indicate function was called used to
								 * to shut off duplicate arg warning */
	/* if this isn't the 1st call, just execute CmdWin()...don't exicute this
	 * function */
	if(firstCall) {						/* if 1st call... */
		firstCall = NO;					/* clear flag */
		CmdDuplicateIDs(cmd0);				/* check list for duplicate args */
	} else {							/* else NOT 1st call so... */
		/* if this is the second call and the -win option wasn't used, then
		 * the software is being run from a script file, so quit */
		if(!windowCmd->entered) {
			printf("Message from Cmdline:  script file completed.\n");
			exit(1);
		}
#ifndef NO_MOTIF
		CmdWin(NULL);					/* go right to window */
#endif
		return;							/* return (skip this input func) */
	}
	pathProg = ArgV[0];					/* file name is the 1st arg */
	progName = strrchr(pathProg, '/');	/* strip path */
	if(progName == NULL) {				/* if no path... */
		progName = pathProg;			/* set program name (incl path) */
	} else {							/* else, since there was a path */
		progName++;						/* skip over '/' */
	}
	/* add optional script file name to list of args */
	if(showOkButton) {						/* if Ok button exists... */
		scriptCmd = CmdFile("-Script_File", scriptFile, STRLEN, cmdOpt);
		strcpy(scriptFile, SCRIPTFILE);			/* load default value */
	}
#ifndef NO_MOTIF
	/* add optional window flag to list of args */
	windowCmd = CmdFlag(WINDOWFLAG, &windowFlag);
#endif
	/* if only one arg, give usage line */
	if(ArgC == 1) CmdUsage(progName);
	/* read in line arguments */
	/* take a deep breath before plunging into the logic below */
	while(--ArgC > 0 && *(++ArgV)[0] == '-') {
		cmd = cmd0;
		/* find matching arg in linked list */
		while(cmd != NULL) {
			sscanf(cmd->id, "%s", id);
			if(strcmp(*ArgV, id) == 0) {
				/* if already entered and no function was called... */
				if(cmd->entered == YES && functionCalled == NO) {
					printf("Error from Cmdline: duplicate args '%s'\n", *ArgV);
					exit(1);
				}
				break;			/* found match so break */
			}
			cmd = cmd->next;
		}
		if(cmd == NULL) {
			printf("Error from Cmdline: unknown arg '%s'\n", *ArgV);
			CmdUsage(progName);	/* no match found */
			return;
		}
		cmd->entered = YES;		/* says that arg has been entered */
		if(cmd->type == cmdFLAG) {
			*(int *)(cmd->data) = 1;	/* set flag */
		} else if(cmd->type == cmdBUTTON) {
			functionCalled = YES;			/* flag to shut off duplicate arg
											 * warning */
			cmd->func((void *)cmd->data);	/* call function */
		} else if(cmd->type == cmdINT || cmd->type == cmdFLOAT) {
			max = cmd->dim;				/* max array size */
			/* loop while args remain, max not reached and ID not hit (check
			 * if '-' precedes a negative number)
			 */
			while(--ArgC > 0 &&
				(*(++ArgV)[0] != '-' || ((*ArgV)[1] >= '0' && (*ArgV)[1] <= '9')
				|| (*ArgV)[1] == '.')	/* while not negative number... */
				&& max-- > 0)
				/* && cmd->nRead < cmd->dim)	don't overflow! */
			{
				if(cmd->nRead >= cmd->dim) cmd->nRead--; /* don't overflow! */
				if(cmd->type == cmdINT) {
					sscanf(*ArgV, "%d", ((int *)cmd->data)+cmd->nRead);
				} else {
					sscanf(*ArgV, "%f", ((float *)cmd->data)+cmd->nRead);
				}
				cmd->nRead++;
			}
			ArgC++; ArgV--;		/* backup to before while quit */
		} else if(cmd->type == cmdCHAR || cmd->type == cmdFILE) {
			len = cmd->len;
			max = cmd->dim;
			while(--ArgC > 0 && *(++ArgV)[0] != '-' && max-- > 0) {
				strncpy((char *)cmd->data+cmd->nRead*len, *ArgV, len);
				cmd->nRead++;
			}
			ArgC++; ArgV--;		/* backup to before while quit */
		} else {
			printf("Oh, Oh...Big error from Cmdline: unknown type.\n");
			exit(1);
		}
	}
	/* set script arg as being read (even if it was not read) because default
	 * value is given.
	 */
	if(scriptCmd) {
		scriptCmd->entered = YES;
		scriptCmd->nRead = 1;			/* one read */
	}

	/* test for input errors */
#ifndef NO_MOTIF
	if(windowCmd->entered) CmdWin(pathProg);/* invoke window if flag entered */
#endif
	if(CmdCheckError(progName)) {			/* if there is an input error... */
		CmdUsage(progName);				/* show usage line */
	}
}

/* check for duplicate args in linked list */
void CmdDuplicateIDs(CmdInfo *cmd) {
	CmdInfo		*cmd1;						/* member of list */
	char		id[STRLEN];					/* id of 'cmd' */
	char		id1[STRLEN];				/* id of 'cmd1' */

	while(cmd != NULL) {
		sscanf(cmd->id, "%s", id);			/* get id1 */
		cmd1 = cmd->next;
		while(cmd1 != NULL && cmd->type != cmdCOLUMN && cmd->type != cmdSPACE) {
			sscanf(cmd1->id, "%s", id1);	/* get id */
			if(!strcmp(id, id1)) {
				printf("Error from Cmdline:  duplicate args IDs declared: '%s' "
				"and '%s'\n", cmd->id, cmd1->id);
			}
			cmd1 = cmd1->next;
		}
		cmd = cmd->next;
	}
}

/* test if user (or the programmer) did anything silly */
int CmdCheckError(char *filename) {
	struct CmdInfo *cmd;	/* arg in linked list */
	int		rtn = 0;	/* return value (-1 if error, 0 otherwise) */
	int		i;			/* index */
	int		count;		/* number of toggles set in radio-box */

	cmd = cmd0;
	while(cmd != NULL) {
		/* if dependent, check that it has been enabled if params are entered */
		if(cmd->nControllers != 0 && CmdSenseSet(cmd) == 0 &&
			cmd->entered == YES)
		{
			printf("Error from Cmdline: arg entered that is not enabled: '%s'\n"
				, cmd->id);
			rtn = -1;
		}
		/* check if too many params entered for array */
		if(cmd->nReq != NULL && CmdSenseSet(cmd) &&
			((cmd->enterMode == cmdMan && cmd->nRead > *cmd->nReq) ||
			(cmd->enterMode == cmdDef && cmd->entered &&
			cmd->nRead > *cmd->nReq)))
		{
			printf("Warning from Cmdline:  number of values in '%s' ", cmd->id);
			printf("> %d as set by '%s'\n", *cmd->nReq,
				CmdFind(cmd->nReq)->id);
		} else if(cmd->nReq != NULL && CmdSenseSet(cmd) &&
			((cmd->enterMode == cmdMan && cmd->nRead < *cmd->nReq) ||
			(cmd->enterMode == cmdDef && cmd->entered &&
			cmd->nRead < *cmd->nReq)))
		{
			/* else check if too few entered */
			printf("Error from Cmdline:  number of values in '%s' ", cmd->id);
			printf("must be %d as set by '%s'\n", *cmd->nReq,
				CmdFind(cmd->nReq)->id);
			rtn = -1;
		}
		/* check enterMode parameter */
		if(cmd->entered == NO && cmd->enterMode == cmdMan && CmdSenseSet(cmd))
		{
			printf("Error from Cmdline: missing arg '%s'\n", cmd->id);
			rtn = -1;
		}
		cmd = cmd->next;
	}
	/* check the radio-boxes */
	for(i = 0; i < radioBox; i++) {
		count = 0;				/* reset the number of set toggles */
		cmd = cmd0;
		while(cmd != NULL) {
			if(cmd->radioBox == i+1 && *(int *)cmd->data) {
				/* if member of radio-box is set...*/
				count++;				/* add to count */
			}
			cmd = cmd->next;
		}
		if(count != 1) {		/* if not 1-and-only-1 toggle set... */
			rtn = -1;			/* return error condition */
			printf("Error from Cmdline: ");
			if(count < 1) printf("no member of radio-box set\n");
			else printf("more than one member of radio-box set\n");
			printf("    Radio-Box: ");
			cmd = cmd0;
			while(cmd != NULL) {
				if(cmd->radioBox == i+1) printf("%s ", cmd->id);
				cmd = cmd->next;
			}
			printf("\n");
		}
	}
	return rtn;
}

/* print out the usage line and exit */
void CmdUsage(char *filename) {
	CmdInfo	*cmd;		/* linked list of command line args */
	char	string[STRLEN];/* cmd->id with '[---' lopped off */

#ifndef NO_MOTIF
	printf("Use the '-win' command to invoke point-and-click Motif Interface.\n");
#endif
	printf("Usage: %s ", filename);
	cmd = cmd0;	
	do {
		/* don't print out non-programmer arguments... */
		if(cmd == NULL || cmd->type == cmdSPACE) continue;
		if(cmd == scriptCmd) continue;
		strncpy(string, cmd->id, STRLEN);
		if(cmd->enterMode == cmdMan) printf("%s ", string);
		else if(cmd->enterMode == cmdOpt) printf("[%s] ", string);
		else if(cmd->enterMode == cmdDef) {
			printf("[%s(", string);
			if(cmd->type == cmdINT) printf("%d)] ", *(int *)cmd->data);
			else if(cmd->type == cmdFLOAT) printf("%.3g)] ",
				*(float *)cmd->data);
			else if(cmd->type == cmdCHAR || cmd->type == cmdFILE) 
				printf("%s)] ", (char *)cmd->data);
		}
	} while(cmd = cmd->next);
	printf("\n");
	exit(1);				/* exit program */
}
#ifdef NO_MOTIF
/* unmanage the command window (for use by programmer) */
void CmdUnMan(void) { return;}
#else
/* invoke motif window.  From here down is the motif stuff.  Everything else
 * has been strictly handling command-line args */
void CmdWin(char *progName) {
	static int			firstCall = YES;		/* 1st call to function flag */
	Arg 		args[256];
	int			argCnt = 0;
	int			zero = 0;		/* no argc */
	char		xApplResFile[STRLEN];				/* resource file name */
	static Display		*display;

	if(firstCall) {
		firstCall = NO;
		/* set the resource file name */
		CmdSetXApplResFile(xApplResFile, progName);
		XtToolkitInitialize();
		context = XtCreateApplicationContext();
		display = XtOpenDisplay(context, 0, APPLICATION, xApplResFile,
			(XrmOptionDescRec *)args, argCnt, &zero, NULL);
		if(display == NULL) {
			XtWarning("Error from CmdWin: cannot open display.");
			exit(1);
		}
		XtSetArg(args[argCnt], XtNtitle, TITLE); argCnt++;
		Shell000 = XtAppCreateShell(APPLICATION, xApplResFile,
			applicationShellWidgetClass, display, args, argCnt);
		CmdCreateBulletinBoard(Shell000, cmd0, progName);
		XtRealizeWidget(Shell000);
	}
	CmdUpdateBoard();

	/* loop until ok() sets loop to non-zero (this replace XtAppMainLoop() */
	while(!okHit) {
		XtAppNextEvent(context, &event);
		if(okHit) break;
		XtDispatchEvent(&event);
	}
	okHit = NO;					/* reset ok flag */
}

/* unmanage the command window (for use by programmer) */
void CmdUnMan(void) {
	int		i;		/* index */

	if(!windowCmd->entered) return;		/* return if no window was managed */
	if(Shell000 == NULL) {
		printf("Warning from CmdUnMan: can't unmanage shell.  'CmdArgs' must "
		"be called to create shell before 'CmdUnMan' can unmanage it.\n");
		return;
	}
	XtUnrealizeWidget(Shell000);
	for(i = 0; i < NEVENTS; i++) {	/* process NEVENTS events in queue */
		XtDispatchEvent(&event);
		XtAppNextEvent(context, &event);
	}
	windowCmd->entered = NO;		/* window unmanaged */
}

/* set x application resource directory */
void CmdSetXApplResFile(char *xApplResFile, char *progName) {
	char	*xApplResDir;			/* xapplresdir */
	char	path[STRLEN];			/* resource file name and path */
	char	*c;						/* char location within xapplresdir[] */
	FILE	*fp;					/* xapp file ptr */

	xApplResDir = getenv("XAPPLRESDIR");/* get resource directory */
	if(xApplResDir != NULL) {			/* if env variable set.. */
		/* look for application specific file in 'XAPPLRESDIR' */
		strcpy(path, xApplResDir);		/* get resource path */
		strcat(path, "/");				/* append '/' to path name */
		c = strrchr(progName, '/');		/* get file name */
		if(c == NULL) {					/* if no path in file name... */
			c = progName;				/* c is the file name */
		} else {
			c++;						/* if path advance beyond '/' */
		}
		strcat(path, c);				/* add file name to path */
		if((fp = fopen(path, "r")) != NULL) {	/* if resource file exists... */
			strcpy(xApplResFile, c);	/* set resource file name */
			fclose(fp);					/* close file */
			return;
		}
	}
	/* no specific file, so use default */
	strcpy(xApplResFile, RESOURCEFILE);
}

/* create bulletin board by converting all the command-line args into widgets */
void CmdCreateBulletinBoard(
	Widget shell,
	CmdInfo *cmd0,
	char *progName)
{
	CmdInfo	*cmd;				/* command line argument */
	int		y = YTITLE;		/* y location of title */
	char	*c;					/* char ptr within 'id' */
	char	string[STRLEN];		/* */
	char	columnClass[STRLEN];/* column class ("column1", "column2", etc.) */
	int		ncols = 0;			/* number of columns */

	/* motif vars */
	Arg			args[512];
	int			argCnt;
	Widget		topBulletinBoard;
	Widget		bulletinBoard;
	Widget		okWidget;		/* ok button widget */

	/* set up top bulletin board */
	argCnt = 0;
	XtSetArg(args[argCnt], XmNresizePolicy, XmRESIZE_GROW); argCnt++;
	topBulletinBoard = XtCreateWidget("topBulletinBoard",
		xmBulletinBoardWidgetClass, shell, args, argCnt);
    XtManageChild(topBulletinBoard);	/* manage 1st column */
 
	/* set up sub-bulletin board */
	sprintf(columnClass, "column%d", ++ncols);
	bulletinBoard = CmdColumnWidget(shell, topBulletinBoard, columnClass);
 
	/* title */
	c = strrchr(progName, '/');		/* set char to '/' before name */
	if(c == NULL) {					/* if no '/'... */
		c = progName;				/* set char to entire string */
	} else {						/* else, no '/' so... */
		c++;						/* set to 1st char or program name */
	}
	CmdTitleWidget(shell, bulletinBoard, c, y);

	y += YSPACING;			/* incr spacing for next field */



	cmd = cmd0;				/* go to top of list */
	while(cmd != NULL) {
		CmdLabel(string, cmd->id, STRLEN);	/* get label from 'id' */
		switch(cmd->type) {
		case cmdBUTTON:
			cmd->mainWidget = CmdButtonWidget(shell, bulletinBoard,
				"functionButton", string, y, &CmdButtonCB, (XtPointer)cmd);
		break;
		case cmdFLAG:
			if(cmd == windowCmd) break;		/* don't show the window flag */
			CmdFlagWidget(shell, bulletinBoard, cmd, string, y);
	        if(y==205)y=y-70;     /* back up since buttons are now row-wise */
	        if(y==415)y=y-60;     /* back up since buttons are now row-wise */
                if(y==596)y=y-30;
			break;
		case cmdSPACE:		/* insert space */
			y += YSPACE;
		break;
		case cmdCOLUMN:	/* start new column */
			sprintf(columnClass, "column%d", ++ncols);
			bulletinBoard = CmdColumnWidget(shell, topBulletinBoard,
				columnClass);
			y = YTITLE;					/* reset y */
		break;

		default:	/* cmdINT, cmdFLOAT, cmdCHAR, cmdFILE... */
                  /*printf("y is %d   ",y);*/
		  if(y==475)y=y-59; /* back up to be row-wise.*/
                  /*printf("new y is %d\n",y);*/
			CmdLabelWidget(shell, bulletinBoard, cmd, string, y);
			CmdTextWidget(shell, bulletinBoard, cmd, string, y);
			/* scrolling list 
			if(cmd->type == cmdFILE) {
				CmdFileWidget(shell, bulletinBoard, cmd, string, y);
			}*/
			/* optional toggle */
			argCnt = 0;
			if(cmd->enterMode == cmdOpt) {
				CmdOptionalWidget(shell, bulletinBoard, cmd, string, y);
			}
			/* array index window */
			/* if arg contols the sensitivity of other args or is a file input
			 * then an enter button is needed */
			if(cmd->controlSens || cmd->type == cmdFILE ) {
				CmdEnterWidget(shell, bulletinBoard, cmd, string, y);
			}
			if(cmd->dim > 1) {		/* if array... */
				cmd->indexWidget = CmdIndexWidget(shell, bulletinBoard, y);
				cmd->plusWidget = CmdButtonWidget(shell, bulletinBoard,
					"plusButton", "+", y, &CmdPlusButtonCB, (XtPointer)cmd);
				cmd->minusWidget = CmdButtonWidget(shell, bulletinBoard,
					"minusButton", "-", y, &CmdMinusButtonCB, (XtPointer)cmd);
			}
		break;

		}
		if(cmd != windowCmd) {		/* don't count the window arg */
			y += YSPACING;		/* incr spacing for next widget */
		}
		cmd = cmd->next;
	}

	/* add spacing above bottom buttons */

	/* ok button */
	if(nFunctions == 0 || showOkButton) {
		okWidget = CmdButtonWidget(shell, bulletinBoard, "okButton", "OK", y,
			&CmdOkCB, (XtPointer)progName);
	}



	/* help button */
	if(helpFile) {		/* if help text programmed... */
		CmdButtonWidget(shell, bulletinBoard, "helpButton", "HELP", y,
			&CmdHelpCB, (XtPointer)NULL);
	}
	/* cancel button */
	if(nFunctions == 0 || showOkButton) {
		CmdButtonWidget(shell, bulletinBoard, "cancelButton", "CANCEL", y,
			&CmdCancelCB, (XtPointer)NULL);
	}
	if(nFunctions == 0 && showOkButton == NO) {
		printf("Warning from CmdNoOk: Can't remove 'OK' without a call to "
		"'CmdButton()'.\n");
	}
	y=y+30;
	CmdCopyrightWidget(shell, bulletinBoard, y);


}

/* setup button widget */
Widget CmdButtonWidget(
	Widget		shell,					/* */
	Widget		bulletinBoard,			/* */
	char		*className,				/* class name */
	char		*string,				/* string inside button */
	int			y,						/* y location */
	void		(*cb)(Widget, XtPointer, XtPointer),	/* call back */
	XtPointer		val)					/* value passed to call back */
{
	Arg			args[512];
	int			argCnt;
	XmString	xmStr;
	Widget		button;

    argCnt = 0;
    XtSetArg(args[argCnt], XmNlabelString, (xmStr = XmStringCreateLtoR(
        string, XmSTRING_DEFAULT_CHARSET))); argCnt++;
    XtSetArg(args[argCnt], XmNrecomputeSize, False); argCnt++;
    XtSetArg(args[argCnt], XmNy, y); argCnt++;
	XtSetArg(args[argCnt], XmNtraversalOn, True); argCnt++;
    button = XtCreateWidget(className,
        xmPushButtonWidgetClass, bulletinBoard, args, argCnt);
    XmStringFree(xmStr);
    XtAddCallback(button, XmNarmCallback, cb, (XtPointer)val);
    XtManageChild(button);
	return button;				/* return widget */
}

void CmdTitleWidget(
	Widget		shell,
	Widget		bulletinBoard,
	char		*string,
	int			y)
{
	Arg			args[512];
	int			argCnt;
	XmString	xmStr;
	Widget		title;				/* window title */

	argCnt = 0;

	XtSetArg(args[argCnt], XmNrecomputeSize, False); argCnt++;
	XtSetArg(args[argCnt], XmNy, y); argCnt++;
	XtSetArg(args[argCnt], XmNalignment, XmALIGNMENT_CENTER); argCnt++;
	XtSetArg(args[argCnt], XmNtraversalOn, False); argCnt++;
    XtSetArg(args[argCnt], XmNlabelString, (xmStr = XmStringCreateLtoR(
        string, XmSTRING_DEFAULT_CHARSET))); argCnt++;
	title = XtCreateWidget("title",
		xmLabelWidgetClass, bulletinBoard, args, argCnt);
	XtManageChild(title);
}

/* setup array index window widget */
Widget CmdIndexWidget(
	Widget		shell,
	Widget		bulletinBoard,
	int			y)
{
	Arg			args[512];
	int			argCnt;
	Widget		indexWidget;

	argCnt = 0;
	XtSetArg(args[argCnt], XmNrows, (short)1); argCnt++;
	XtSetArg(args[argCnt], XmNy, y); argCnt++;
	XtSetArg(args[argCnt], XmNsensitive, False); argCnt++;
	indexWidget = XtCreateWidget("arrayIndex",
		xmTextWidgetClass, bulletinBoard, args, argCnt);
	XtManageChild(indexWidget);
	return indexWidget;
}

/* setup enter button widget */
void CmdEnterWidget(
	Widget		shell,
	Widget		bulletinBoard,
	CmdInfo		*cmd,
	char		*string,
	int			y)
{
	Arg			args[512];
	int			argCnt;
	XmString	xmStr;

    argCnt = 0;
    XtSetArg(args[argCnt], XmNlabelString,
		(xmStr = XmStringCreateLtoR("ENTER",
        XmSTRING_DEFAULT_CHARSET))); argCnt++;
    XtSetArg(args[argCnt], XmNrecomputeSize, False); argCnt++;
    XtSetArg(args[argCnt], XmNy, y); argCnt++;
    cmd->enterWidget = XtCreateWidget("enterButton",
        xmPushButtonWidgetClass, bulletinBoard, args, argCnt);
    XmStringFree(xmStr);
    XtAddCallback(cmd->enterWidget, XmNactivateCallback, CmdEnterCB,
		(XtPointer)cmd);
}

/* setup optional toggle widget */
void CmdOptionalWidget(
	Widget		shell,
	Widget		bulletinBoard,
	CmdInfo		*cmd,
	char		*string,
	int			y)
{
	Arg			args[512];
	int			argCnt = 0;
	XmString	xmStr;

	XtSetArg(args[argCnt], XmNy, y); argCnt++;
	XtSetArg(args[argCnt], XmNlabelString,
		(xmStr = XmStringCreateLtoR("\0",
		XmSTRING_DEFAULT_CHARSET))); argCnt++;
	cmd->optionWidget = XtCreateWidget("optionalToggle",
		xmToggleButtonWidgetClass, bulletinBoard, args, argCnt);
	XtAddCallback(cmd->optionWidget, XmNarmCallback, CmdOptionalToggleCB,
		(XtPointer)cmd);
	XtManageChild(cmd->optionWidget);
}

/* setup file selection widget */
void CmdFileWidget(
	Widget		shell,
	Widget		bulletinBoard,
	CmdInfo		*cmd,
	char		*string,
	int			y)
{
	Arg			args[512];
	int			argCnt;

	argCnt = 0;
	XtSetArg(args[argCnt], XmNy, y+YSPACING); argCnt++;
	cmd->scrWinWidget = XmCreateScrolledWindow(bulletinBoard,
		"scrolledFileWindow", args, argCnt);
	argCnt = 0;
	cmd->listWidget = XmCreateScrolledList(cmd->scrWinWidget,
		"fileList", args, argCnt);
	/* set double click call back */
	XtAddCallback(cmd->listWidget, XmNdefaultActionCallback,
		CmdEnterCB, (XtPointer)cmd);	/* double click */
	XtAddCallback(cmd->listWidget, XmNbrowseSelectionCallback,
		CmdSingleClickCB, (XtPointer)cmd);	/* single click */
	XtManageChild(cmd->listWidget);			/* manage list */
	XtUnmanageChild(cmd->scrWinWidget);		/* unmanage scrolled window */
}
void CmdTextWidget(
	Widget		shell,
	Widget		bulletinBoard,
	CmdInfo		*cmd,
	char		*string,
	int			y)
{
	Arg			args[512];
	int			argCnt;

	argCnt = 0;
        if(y==385){
   	      y=385;
	      XtSetArg(args[argCnt],XmNx,210);argCnt++;
        }
        if(y==415){
   	      y=385;
	      XtSetArg(args[argCnt],XmNx,310);argCnt++;
        }
        if(y==445){
   	      y=385;
	      XtSetArg(args[argCnt],XmNx,410);argCnt++;
	}
	XtSetArg(args[argCnt++], XmNstringDirection, XmSTRING_DIRECTION_L_TO_R);
	XtSetArg(args[argCnt], XmNrows, (short)1); argCnt++;
	XtSetArg(args[argCnt], XmNy, y); argCnt++;
	cmd->mainWidget = XtCreateWidget("textWindow",
		xmTextWidgetClass, bulletinBoard, args, argCnt);
	/* if arg controls the sensitity of others or is a file or array... */
	if(cmd->controlSens || cmd->type == cmdFILE || cmd->dim > 1) {
		/* produce an enter button when value is changed */
		XtAddCallback(cmd->mainWidget, XmNactivateCallback, CmdEnterCB,
			(XtPointer)cmd);
	}
	XtAddCallback(cmd->mainWidget, XmNvalueChangedCallback, CmdValueChangedCB,
		(XtPointer)cmd);
	XtManageChild(cmd->mainWidget);
}


/* set up label widget */
void CmdLabelWidget(
	Widget		shell,
	Widget		bulletinBoard,
	CmdInfo		*cmd,
	char		*string,
	int			y)
{
	Arg			args[512];
	int			argCnt;
	XmString	xmStr;

	argCnt = 0;
        if(y==415)
   	      y=385;
        if(y==445)
   	      y=385;
	XtSetArg(args[argCnt], XmNstringDirection, XmSTRING_DIRECTION_L_TO_R);
		argCnt++;
	XtSetArg(args[argCnt], XmNalignment, XmALIGNMENT_END); argCnt++;
	XtSetArg(args[argCnt], XmNrecomputeSize, False); argCnt++;
	XtSetArg(args[argCnt], XmNy, y); argCnt++;
	XtSetArg(args[argCnt], XmNlabelString, (xmStr = XmStringCreateLtoR(
        string, XmSTRING_DEFAULT_CHARSET))); argCnt++;
	cmd->label = XtCreateWidget("label",
		xmLabelWidgetClass, bulletinBoard, args, argCnt);
	XtManageChild(cmd->label);
}

/* set up column widget */
Widget CmdColumnWidget(
	Widget		shell,
	Widget		topBulletinBoard,
	char		*className)
{
	Widget		bulletinBoard;
	Arg			args[512];
	int			argCnt;

	argCnt = 0;
	XtSetArg(args[argCnt], XmNresizePolicy, XmRESIZE_GROW); argCnt++;
	bulletinBoard = XtCreateWidget(className,
		xmBulletinBoardWidgetClass, topBulletinBoard, args, argCnt);
	XtManageChild(bulletinBoard);	/* manage column */
	return bulletinBoard;
}

/* set up flag widget */
void CmdFlagWidget(
	Widget		shell,
	Widget		bulletinBoard,
	CmdInfo		*cmd,
	char		*string,
	int			y)
{
	Arg			args[512];
	int			argCnt;
	XmString	xmStr;

	argCnt = 0;
	XtSetArg(args[argCnt], XmNalignment, XmALIGNMENT_CENTER); argCnt++;
	XtSetArg(args[argCnt], XmNstringDirection, XmSTRING_DIRECTION_L_TO_R);
	argCnt++;

/* The radiobox buttons in macromobilechan are hard coded to x,y vals
   because the original design was for vertical layout, and in this case
   we want horizontal layout.
*/
	/*printf("y is %d\n",y);*/

    if(y==145){
	  CmdLabelWidget(shell, bulletinBoard, cmd," Warmup Filters:", y);
	  XtSetArg(args[argCnt],XmNx,210);argCnt++;
	}
    if(y==175){
	  y=145;
	  XtSetArg(args[argCnt],XmNx,285);argCnt++;
	}
    if(y==205){
	  y=145;
	  XtSetArg(args[argCnt],XmNx,360);argCnt++;
	}

    if(y==355){
	  CmdLabelWidget(shell, bulletinBoard, cmd," Paths:", y);
	  XtSetArg(args[argCnt],XmNx,210);argCnt++;
	}
    if(y==385){
	  y=355;
	  XtSetArg(args[argCnt],XmNx,310);argCnt++;
	}
    if(y==415){
	  y=355;
	  XtSetArg(args[argCnt],XmNx,410);argCnt++;
	}
    if(y==596){
	  y=566;
	  XtSetArg(args[argCnt],XmNx,310);argCnt++;
	}
	XtSetArg(args[argCnt], XmNy, y); argCnt++;
	XtSetArg(args[argCnt], XmNtraversalOn, False); argCnt++;
  	XtSetArg(args[argCnt], XmNlabelString, (xmStr = XmStringCreateLtoR(
        string, XmSTRING_DEFAULT_CHARSET))); argCnt++;
	cmd->mainWidget = XtCreateWidget("flag",
		xmToggleButtonWidgetClass, bulletinBoard, args, argCnt);
	XtAddCallback(cmd->mainWidget, XmNarmCallback, CmdFlagCB, (XtPointer)cmd);
	XtManageChild(cmd->mainWidget);
}

/* get arg label from it's 'id' and pack it into a string */
void CmdLabel(char *string, char *id, int strLen) {
	char *c;		/* ptr within strings */

	/* Output only ID description (if exists) for label or ID (sans '-') */
	c = strchr(id, ' ');		/* get blank betw ID and desc'n */
	if(c == NULL) c = id; 		/* if no desc'n, output ID */
	c++;							/* skip '-' or ' ' */
	strncpy(string, c, strLen);		/* copy id into string[] */
	/* get rid of any '_' chars in string[] */
	c = string;						/* set pointer to beginning of string */
	while((c = strchr(c, '_')) != NULL) *(c++) = ' ';	/* blank '_' char */
}

/* function callback */
void CmdButtonCB(
	Widget w,
	XtPointer client,
	XtPointer call)
{
	CmdInfo	*cmd;	/* member of arg list */

	cmd = (CmdInfo *)client;
	CmdGetAllDataFromWidgets();		/* get data from widgets */
	cmd->func((void *)cmd->data);	/* call function */
	CmdUpdateBoard();					/* put data into widgets */
	CmdCheckSensitive(cmd);			/* check and adj sensitivity of widgets */
}

/* flag callback */
void CmdFlagCB(
	Widget w,
	XtPointer client,
	XtPointer call)
{
	CmdInfo	*cmd;	/* member of arg list */
	CmdInfo	*cmd1;	/* member of arg list */

	cmd = (CmdInfo *)client;		/* arg pointer passed */
	if(*(int *)(cmd->data)) {		/* if flag data is non zero... */
		*(int *)(cmd->data) = 0;	/* clear flag */
		cmd->entered = NO;			/* say it wasn't entered */
	} else {						/* flag data is equal to zero */
		*(int *)(cmd->data) = 1;	/* set flag */
		cmd->entered = YES;			/* say it was entered */
	}
	if(cmd->radioBox) {		/* is toggle a member of a radio box... */
		cmd1 = cmd0;		/* goto top of list */
		do {				/* start scanning list for radioBox members */
			if(cmd1 == cmd) continue;	/* skip over current toggle */
			if(cmd1->radioBox == cmd->radioBox && *(int *)cmd1->data) {
				/* clear radioBox member that is set */
				XmToggleButtonSetState(cmd1->mainWidget, False, False);
				*(int *)(cmd1->data) = 0;	/* clear flag */
				cmd1->entered = NO;			/* say it wasn't entered */
			}
		} while(cmd1 = cmd1->next);	/* continue until cmd1 == NULL */
	}
	CmdCheckSensitive(cmd);
}

/* enter button call back updates the board and unmanages the ENTER button */
void CmdEnterCB(
	Widget w,
	XtPointer client,
	XtPointer call)
{
	CmdInfo		*cmd;			/* ptr to arg in list */
	char		*entry;			/* string inside text window */
	cmd = (CmdInfo *)client;	/* arg pointer passed */
	CmdGetAllDataFromWidgets();	/* get all data and pack to memory */
	CmdCheckSensitive(cmd);
	if(cmd->type == cmdFILE) {
		entry = XmTextGetString(cmd->mainWidget);	/* text widget string */
		/* look for wild card character */
		if(strpbrk(entry, "*?")) {		/* if wild card exists... */
			XtManageChild(cmd->scrWinWidget);	/* make sure list is managed */
			/* free the old string array */
		} else {	/* else, no wild card so... */
			XtUnmanageChild(cmd->enterWidget);	/* unmanage ENTER button */
		}
	} else {
		if(cmd->dim == 1) {						/* if one dimensional */
			XtUnmanageChild(cmd->enterWidget);	/* unmanage ENTER button */
		} else {
			CmdArrayButtonCB(cmd, +1);
		}
	}
}

/* dumps a double clicked list string into the main widget */
void CmdSingleClickCB(
	Widget w,
	XtPointer client,
	XtPointer call)
{
	CmdInfo		*cmd;		/* ptr to arg in list */
	XmListCallbackStruct   *lcs = (XmListCallbackStruct *)call;

	cmd = (CmdInfo *)client;		/* arg pointer passed */
	/* put string in widget */
	XmTextSetString(cmd->mainWidget, cmd->list[lcs->item_position-1]);
}

/* manages the ENTER button and sets the 'entered' flag if the value is
 * change in the text entry field.
 */
void CmdValueChangedCB(
	Widget w,
	XtPointer client,
	XtPointer call)
{
	CmdInfo		*cmd;			/* ptr to arg in list */
	char		*string;		/* string in text field */

	if(updatingBoard) return;	/* if initing the board, don't manage buttons */
	cmd = (CmdInfo *)client;	/* arg pointer passed */
	if(cmd->enterWidget) {		/* if enter button exists... */
		XtManageChild(cmd->enterWidget);	/* manage button */
	}
	cmd->entered = YES;			/* set the 'entered' flag */
	string = XmTextGetString(cmd->mainWidget);	/* text widget string */
	/* if array value or scalar, update the number read */
	if(string[0] != NULL) {	/* if field isn't blank... */
		/* adj 'nRead' (+1 is because index starts at zero) */
		if(cmd->index+1 > cmd->nRead) cmd->nRead = cmd->index+1;
	}
	/*if(cmd->type == cmdFILE) XtManageChild(cmd->scrWinWidget);*/
}


/* '+' button callback */
void CmdPlusButtonCB(
	Widget w,
	XtPointer client,
	XtPointer call)
{
	CmdInfo	*cmd;			/* arg list member */

	cmd = (CmdInfo *)client;		/* arg pointer passed */
	CmdArrayButtonCB(cmd, +1);
}

/* '-' button callback */
void CmdMinusButtonCB(
	Widget w,
	XtPointer client,
	XtPointer call)
{
	CmdInfo	*cmd;			/* arg list member */

	cmd = (CmdInfo *)client;		/* arg pointer passed */
	CmdArrayButtonCB(cmd, -1);
}

/* call back for '+' and '-' button */
void CmdArrayButtonCB(
	CmdInfo	*cmd,			/* arg list member */
	int		factor)			/* button hit (-1, +1) */
{
	char	string[STRLEN];	/* */

	/* test if index is within bounds */
	if(cmd->index+factor < 0) {				/* if index too small... */
		return;								/* don't try */
	}
	if(cmd->index+factor >= cmd->dim) {		/* if index too big... */
		printf("Note: array memory boundary reached!\n");
		return;
	}
	/* test if index hits user defined maximum */
	if(cmd->nReq != NULL && cmd->index+factor >= *cmd->nReq) {
		CmdLabel(string, CmdFind(cmd->nReq)->id, STRLEN);	/* get label */
		printf("Note: maximum number of elements set by '%s' to %d\n", string,
			*(int *)CmdFind(cmd->nReq)->data);
		if(cmd->index + factor == *cmd->nReq) return;/* don't PutDataInWidget */
		/* adjust the index */
		cmd->index = *cmd->nReq - 1;
		/* adjust the number read */
		if(cmd->nRead > *cmd->nReq) cmd->nRead = *cmd->nReq;
		CmdPutDataInWidget(cmd);				/* put fix into widget */
		return;
	}
	CmdGetDataFromWidget(cmd);
	cmd->index += factor;					/* adjust index */
	CmdPutDataInWidget(cmd);
}

/* take value from memory and stuff it into the widget */
void CmdPutDataInWidget(CmdInfo *cmd) {
	char	string[STRLEN];

	if(cmd == windowCmd) return;		/* ignore the window arg */
	if(cmd->type == cmdFLAG) {
		if(*(int *)(cmd->data)) {		/* flag data is non zero */
			XmToggleButtonSetState(cmd->mainWidget, True, False);
		} else {						/* flag data is equal to zero */
			XmToggleButtonSetState(cmd->mainWidget, False, False);
		}
	} else if(cmd->type == cmdBUTTON) {
		printf("Error from CmdPutDataInWidget: why put data in function '%s'?",
			cmd->id);
		exit(1);
	} else if(cmd->type == cmdSPACE) {
		printf("Error from CmdPutDataInWidget: why put data in a Space?");
		exit(1);
	} else if(cmd->type == cmdCOLUMN) {
		printf("Error from CmdPutDataInWidget: why put data in a Column?");
		exit(1);
	} else if(cmd->index+1 > cmd->nRead && cmd->enterMode != cmdDef) {
		/* else if value NOT entered and NO default value exists...
		 * blank widget (note: '+1' because 'index' starts at zero) */
		sprintf(string, "\0");					/* put blank into string */
		XmTextSetString(cmd->mainWidget, string);/* put string in widget */
	} else if(cmd->type == cmdINT) {
		sprintf(string, "%-d", *((int *)cmd->data+cmd->index));
		XmTextSetString(cmd->mainWidget, string);	/* update array widget */
	} else if(cmd->type == cmdFLOAT) {
		sprintf(string, "%-g", *((float *)cmd->data+cmd->index));
		XmTextSetString(cmd->mainWidget, string);	/* update array widget */
	} else if(cmd->type == cmdCHAR || cmd->type == cmdFILE) {
		strncpy(string, (char *)cmd->data+cmd->index*cmd->len, STRLEN);
		XmTextSetString(cmd->mainWidget, string);	/* update array widget */
	} else {
		printf("Error from CmdPutDataInWidget: couldn't put anything into '%s'",
			cmd->id);
	}
	if(cmd->optionWidget) {
		if(cmd->entered) {
			XmToggleButtonSetState(cmd->optionWidget, True, False);
		} else {	
			XmToggleButtonSetState(cmd->optionWidget, False, False);
		}
	}
	if(cmd->indexWidget) {							/* if index exists */
		sprintf(string, "%d", cmd->index+1);		/* get index to array */
		XmTextSetString(cmd->indexWidget, string);	/* update array index */
	}
}

/* ok button callback */
void CmdHelpCB(
	Widget w,
	XtPointer client,
	XtPointer call)
{
	char	string[STRLEN];					/* reusable string */
	sprintf(string, "more %s", helpFile);	/* do a more on file */
	system(string);							/* call to UNIX */
}


/* ok button callback */
void CmdOkCB(
	Widget w,
	XtPointer client,
	XtPointer call)
{
	FILE	*outfp = NULL;		/* output file ptr */
	char	*progName;			/* program name */
	char	string[STRLEN];		/* system(string) */


	CmdGetAllDataFromWidgets();
	progName = (char *)client;		/* program name passed thru button cb */
	/* if scriptCmd exists, write script file */
	if(scriptCmd && (scriptCmd->entered == NO ||
	  (outfp = fopen(scriptFile, "w")) == NULL))
	{
		printf("no script file written\n");
	} else {
		CmdFPrint(outfp, progName);		/* write script file */
		printf("script file '%s' written\n", scriptFile);
		fclose(outfp);
		/* make script file executable */
		sprintf(string, "chmod +x %s", scriptFile);
		system(string);
	}
	okHit = YES;						/* flag to exit main loop */
}

/* CmdFPrint prints out the command file for future runs.
 * A $* is put at the bottom of the file so that addtional command-line args
 * may be added when running a script file 
 */
void CmdFPrint(
	FILE		*fp,
	char		*progName)
{
	CmdInfo	*cmd;				/* member of arg list */
	char	string[STRLEN];		/* */
	int		i;					/* index to data */
	int		nwrite;				/* number of values to write */

	/* print commented header */
	fprintf(fp, "#\n");
	fprintf(fp, "# script file written by Cmdline\n");
	fprintf(fp, "#\n");
	fprintf(fp, "# list of available args:\n");
	cmd = cmd0;
	while(cmd != NULL) {
		/* if 'not applicable' type or script file arg... */
		if(cmd->enterMode == cmdNA || cmd == scriptCmd) {
			cmd = cmd->next;
			continue;
		}
		fprintf(fp, "#      %s ", cmd->id);		/* output id and description */
		/* output type of input (int, float, int array, etc. */
		if(cmd->type == cmdFLAG) fprintf(fp, "(int flag");
		else if(cmd->type == cmdINT) fprintf(fp, "(int");
		else if(cmd->type == cmdFLOAT) fprintf(fp, "(float");
		else if(cmd->type == cmdCHAR) fprintf(fp, "(char");
		else if(cmd->type == cmdFILE) fprintf(fp, "(file");
		if(cmd->dim > 1) fprintf(fp, " array");
		fprintf(fp, ") ");
		/* print 'optional' or default value if appropriate */
		if(cmd->enterMode == cmdOpt || cmd->enterMode == cmdDef) {
			fprintf(fp, "optional ");
		}
		if(cmd->enterMode == cmdDef) {
			fprintf(fp, "(default = ");
			if(cmd->type == cmdINT) {
				fprintf(fp, "%d) ", *(int *)cmd->data);
			} else if(cmd->type == cmdFLOAT) {
				fprintf(fp, "%.3g) ", *(float *)cmd->data);
			} else if(cmd->type == cmdCHAR || cmd->type == cmdFILE) {
				fprintf(fp, "%s) ", (char *)cmd->data);
			}
		}
		fprintf(fp, "\n");
		cmd = cmd->next;
	}
/*	fprintf(fp,"data filename");
	fprintf(fp,"%s\n",data_fname);*/
	fprintf(fp, "#\n");
	fprintf(fp, "# '$*' at the bottom of the file allows for additional\n");
	fprintf(fp, "# command-line arguments to be added when running this\n");
	sscanf(WINDOWFLAG, "%s", string);
	fprintf(fp, "# script (e.g., the '%s' flag to invoke the window)\n",string);
	fprintf(fp, "#\n");
	fprintf(fp, "\n");
	fprintf(fp, "%-10s  ", progName);	/* program file name to run */
	/* print args */
	cmd = cmd0;

	do {

		if(cmd == windowCmd) continue; 				/* window flag */

		if(cmd->nReq && *cmd->nReq == 0) continue;	/* empty array */

		/* if not sensitive...continue */
		if(cmd->nControllers && CmdSenseSet(cmd) == 0) continue;
		if(cmd->entered) {

			sscanf(cmd->id, "%s", string);		/* get param ID */
			fprintf(fp, "\\\n %-10s ", string);	/* output cont'n char & ID */
			if(cmd == scriptCmd) {				/* if script file arg... */
				/* the script file name isn't saved because if the user changes
				 * the name of the file, they would also have to edit the name
				 * within it.  $0 takes care of this */
				fprintf(fp, "$0 ");				/* this gets script name */
				continue;						/* skip below */
			}

			/* output data values */
			if(cmd->nReq != NULL) nwrite = *cmd->nReq;	/* if nReq defined... */
			else nwrite = cmd->nRead;
			if(cmd->type == cmdFLAG) {
			} else if(cmd->type == cmdINT) {
				for(i = 0; i < nwrite; i++) {
					fprintf(fp, "%d ", *((int *)cmd->data+i));
				}
			} else if(cmd->type == cmdFLOAT) {
				for(i = 0; i < nwrite; i++) {
					fprintf(fp, "%g ", *((float *)cmd->data+i));
				}
			} else if(cmd->type == cmdCHAR || cmd->type == cmdFILE) {
				for(i = 0; i < nwrite; i++) {
					if(*((char *)cmd->data+i*cmd->len) == NULL) {
						printf("Warning from Cmdline: empty string outputted "
						"in '%s'\n", cmd->id);
					}
					fprintf(fp, "%s ", (char *)cmd->data+i*cmd->len);
				}
			}
		}
	} while(cmd = cmd->next);	/* continue until cmd == NULL */
	fprintf(fp, "\\\n $*\n");							/* last CR */
}

/* get the data from all text and toggle widgets */
void CmdGetAllDataFromWidgets(void) {
	CmdInfo	*cmd;		/* command line arg */

	cmd = cmd0;			/* start at top of list */
	while(cmd != NULL) {
		CmdGetDataFromWidget(cmd);
		cmd = cmd->next;
	}
}

/* show values of all text widgets */
void CmdUpdateBoard(void) {
	CmdInfo	*cmd;			/* command line arg */

	updatingBoard = YES;	/* tells CmdValueChangedCB() to ignore */
	/* put data in widget */
	cmd = cmd0;			/* start at top of list */
	while(cmd) {
		if(cmd->enterMode == cmdDef || cmd->entered) CmdPutDataInWidget(cmd);
		cmd = cmd->next;
	}
	CmdCheckSensitive(NULL);
	updatingBoard = NO;	/* this is flag to tell CmdValueChangedCB() to ignore */
}

/* adjust the sensitivity of dependent args (except 'exception').  Note that the
 * optional toggle is a specific, pain in the ass case */
void CmdCheckSensitive(CmdInfo *exception) {
	int		argCnt = 0;		/* args[] counter */
	Arg		args[512];		/* motif args[] */
	CmdInfo	*cmd;			/* member of arg list */

	cmd = cmd0;				/* start at top of list */
	while(cmd) {
		if(cmd == windowCmd) return;	/* window arg should be last (skip) */
		argCnt = 0;
		/* if arg optional toggle clicked... */
		if(cmd == exception && cmd->optionWidget) {
			if(cmd->entered) {			/* if arg enabled... */
				XtSetArg(args[argCnt], XmNsensitive, True); argCnt++;
			} else {
				XtSetArg(args[argCnt], XmNsensitive, False); argCnt++;
			}
		} else if(CmdSenseSet(cmd) && (cmd->nReq == NULL || *cmd->nReq)) {
			/* if your here, make arg sensitive */
			/* if already sensitive... */
			if(cmd->sensitive && exception != NULL) {
				cmd = cmd->next;		/* don't do anything */
				continue;
			}
			cmd->sensitive = YES;		/* set flag */
			argCnt = 0;
			XtSetArg(args[argCnt], XmNsensitive, True); argCnt++;
			if(cmd->optionWidget) {		/* if optional arg... */
				/* make option button widget sensitive */
				XtSetValues(cmd->optionWidget, args, argCnt);
				/* make other widget in arg not sensitive */
				if(cmd->entered == NO) {
					XtSetArg(args[argCnt], XmNsensitive, False); argCnt++;
				}
			}
			if(cmd->dim > 1) {			/* if multi-dimensional... */
				CmdPutDataInWidget(cmd);	/* put index into window */
			}
		} else { /* arg is NOT sensitive */
			/* if your here, make arg NOT sensitive */
			/* if already NOT sensitive... */
			if(cmd->sensitive == NO && exception != NULL) {
				cmd = cmd->next;		/* don't do anything */
				continue;
			}
			cmd->sensitive = NO;		/* set flag */
			argCnt = 0;
			XtSetArg(args[argCnt], XmNsensitive, False); argCnt++;
			if(cmd->type == cmdFLAG) {
				if(*(int *)(cmd->data) != 0) {		/* if value entered... */
					*(int *)(cmd->data) = 0;		/* reset */
					/* let button up */
					XmToggleButtonSetState(cmd->mainWidget, False, False);
				}
			}
			if(cmd->optionWidget) {
				if(cmd->entered) {
					/* let optional button up */
					XmToggleButtonSetState(cmd->optionWidget,False,False);
				}
				XtSetValues(cmd->optionWidget, args, argCnt);	/* make insen */
			}
			if(cmd->entered) cmd->entered = NO;	/* reset */
		}
		if(cmd->mainWidget) {		/* if widget exists...adj sensitivity */
			XtSetValues(cmd->mainWidget, args, argCnt);
		}
		if(cmd->label) {			/* if label widget exists... */
			XtSetValues(cmd->label, args, argCnt);		/* adj sensitivity */
		}
		if(cmd->minusWidget) {		/* if minusWidget widget exists... */
			XtSetValues(cmd->minusWidget, args, argCnt);/* adj sensitivity */
		}
		if(cmd->plusWidget) {		/* if plusWidget widget exists... */
			XtSetValues(cmd->plusWidget, args, argCnt);	/* adj sensitivity */
		}
		cmd = cmd->next;
	}
}

/* update text field of '*arg' */
void CmdGetDataFromWidget(CmdInfo *cmd) {
	char *string;
	if(cmd == windowCmd) return;		/* ignore the window arg */
	if(cmd->type != cmdFLAG && cmd->type != cmdSPACE && cmd->type != cmdCOLUMN
	   && cmd->type != cmdBUTTON){
		string = XmTextGetString(cmd->mainWidget);	/* get text widget string */
		if(string[0] != NULL) {					/* if string not empty ... */
			if(cmd->dim == 1) cmd->nRead = 1;	/* # args read (not array) */
			/* sscanf data from string */
			if(cmd->type == cmdINT) {
				sscanf(string, "%d", (int *)cmd->data+cmd->index);
			} else if(cmd->type == cmdFLOAT) {
				sscanf(string, "%f", (float *)cmd->data+cmd->index);
			} else if(cmd->type == cmdCHAR || cmd->type == cmdFILE) {
				strncpy((char *)cmd->data+cmd->index*cmd->len,string,cmd->len);
			}
		} else {	/* string empty */
			if(cmd->dim == 1) cmd->nRead = 0;	/* # args read (not array) */
		}
	}
}

/* cancel button callback */
void CmdCancelCB(
	Widget w,
	XtPointer client,
	XtPointer call)
{
	exit(1);
}


/* cancel button callback */
void CmdOptionalToggleCB(
	Widget w,
	XtPointer client,
	XtPointer call)
{
	CmdInfo	*cmd;				/* member of arg list */
	
	cmd = (CmdInfo *)client;		/* arg pointer passed */
	CmdGetDataFromWidget(cmd);		/* get data from window for arg toggled */
	/* toggle */
	if(cmd->entered) {
		cmd->entered = NO;	/* say it wasn't entered */
	} else cmd->entered = YES;				/* say it was entered */
	CmdCheckSensitive(cmd);
}

/* set up label widget */
void CmdCopyrightWidget(
	Widget		shell,
	Widget		bulletinBoard,
	int			y)
{
        char   *copyright="Copyright (c) 1994 MITRE Corp. Bedford, MA\n Revised (1996) MIT Lincoln Lab, Lexington, MA";
	Arg			args[512];
	int			argCnt;
	XmString	xmStr;
	Widget        label;


	argCnt = 0;
	XtSetArg(args[argCnt], XmNstringDirection, XmSTRING_DIRECTION_L_TO_R);
		argCnt++;
	XtSetArg(args[argCnt], XmNalignment, XmALIGNMENT_CENTER); argCnt++;
	XtSetArg(args[argCnt], XmNrecomputeSize, False); argCnt++;
	XtSetArg(args[argCnt], XmNy, y); argCnt++;

        XtSetArg(args[argCnt], XmNwidth, 300); argCnt++;
	XtSetArg(args[argCnt], XmNlabelString, (xmStr = XmStringCreateLtoR(
        copyright, XmSTRING_DEFAULT_CHARSET))); argCnt++;
	label = XtCreateWidget("label",
		xmLabelWidgetClass, bulletinBoard, args, argCnt);
	XtManageChild(label);
}


#endif
