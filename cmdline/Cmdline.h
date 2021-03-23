/* header file for command line argument routines in cmdline.c
 * r.coutts

   copyright (c) 1994 The MITRE Corporation Bedford, MA

   $Id: Cmdline.h,v 2.2 1994/07/25 18:18:32 jjb Exp $
   $Log: Cmdline.h,v $
 * Revision 2.2  1994/07/25  18:18:32  jjb
 * Added copyright notice
 *

 */
#ifdef NO_MOTIF
typedef void *Widget;
#else
#include <Xm/Xm.h>
#include <Xm/Text.h>
#endif

#ifndef _CMDLINE_H
#define    _CMDLINE_H

/* some enumerators */
enum cmdType {                  /* arg type */
    cmdINT,                     /* int arg */
    cmdFLOAT,                   /* float arg */
    cmdCHAR,                    /* char string arg */
    cmdFLAG,                    /* int flag */
    cmdSPACE,                   /* space between args */
    cmdCOLUMN,                  /* start new column */
    cmdFILE,                    /* file argument */
    cmdBUTTON                   /* function argument */
};
enum cmdEnterMode {             /* arg entry mode */
    cmdOpt = 0,                 /* arg entry is optional */
    cmdMan = -1,                /* arg must be entered by user */
    cmdDef = -2,                /* arg has default value */
    cmdNA  = -3                 /* type n/a (for use by cmdline only!) */
};
enum cmdDepMode {               /* dependency mode (cmdArray, etc.) */
    cmdRadioBox = 1,            /* defines toggle as radio-box */
    cmdArray,                   /* defines int as defining array length */
    cmdSensitive                /* defines arg as controling arg sensitivity */
};

/* cmdline structure */
#define    cmdNSENSITIVE  10    /* number of sensitive flags per arg */
#define    cmdNFILES      100   /* max number of files in filter list */
typedef struct CmdInfo {        /* structure containing arg information */
    void    *data;              /* pointer to data (will be cast) */
    void    (*func)(void *);    /* pointer to function */
    int     index;              /* index to *data (if array) */
    enum cmdEnterMode enterMode;/* (cmdOpt, cmdMan, cmdDef, etc.) */
    int     entered;            /* flag != 0 if arg is entered */
    int     nRead;              /* number of values read */
    int     dim;                /* dimension of data */
    int     len;                /* char string length */
    int     *nReq;              /* # of array elements required */
    int     *controller[cmdNSENSITIVE];    /* contols sensitivity of arg */
    int     nControllers;       /* number of sensitive dependencies */
    int     sensitive;          /* sensitivity flag */
    int     controlSens;        /* flag indicating arg controls sensitivity */
    int     radioBox;           /* index to radio box if toggle */
    Widget  label;              /* motif widget label of text field */
    Widget  mainWidget;         /* motif widget (toggle, window, or button) */
    Widget  scrWinWidget;       /* motif widget for scrolled window to list */
    Widget  listWidget;         /* motif widget for list */
    Widget  indexWidget;        /* motif widget to array index window */
    Widget  optionWidget;       /* motif widget to option flag */
    Widget  enterWidget;        /* motif widget to ENTER button */
    Widget  minusWidget;        /* motif widget to '-' button */
    Widget  plusWidget;         /* motif widget to '+' button */
    char    *list[cmdNFILES];   /* list ptr containing file names */
    int     listCount;          /* number of file names in list */
    char    *id;                /* int id, (e.g. "-f number") */
    enum cmdType type;          /* cmdFLOAT, cmdINT, ... */
    struct CmdInfo *next;       /* linked list pointer */
} CmdInfo;
 
#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

/* cmdline functions */
extern CmdInfo *CmdInt(char *, ...);      /* adds int c.l.arg */
extern CmdInfo *CmdFloat(char *, ...);    /* adds float c.l.arg */
extern CmdInfo *CmdString(char *, ...);   /* adds string c.l.arg */
extern CmdInfo *CmdFlag(char *, ...);     /* adds int flag c.l.arg */
extern CmdInfo *CmdButton(char *, ...);   /* adds int flag c.l.arg */
extern CmdInfo *CmdFile(char *, ...);     /* adds file c.l.arg */
extern void CmdDep(void *, ...);          /* define dependencies */
extern void CmdArgs(int, char **);        /* pass args to cmdline */
extern void CmdHelp(char *);              /* adds help */
extern void CmdUnMan(void);               /* unmanage window */
extern void CmdSpace(void);               /* insert between arguments */
extern void CmdColumn(void);              /* insert between arguments */
extern void CmdNoOk(void);                /* insert between arguments */

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
