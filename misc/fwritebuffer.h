/* header file for file writing function

   copyright (c) 1994 The MITRE Corporation Bedford, MA

   $Id: fwritebuffer.h,v 2.2 1994/07/25 18:30:41 jjb Exp $
   $Log: fwritebuffer.h,v $
 * Revision 2.2  1994/07/25  18:30:41  jjb
 * added Copyright notice.
 *

 */

#ifndef _FWRITEBUFFER_H
#define _FWRITEBUFFER_H

extern int FWriteBuffer(char *, int, int, FILE *, int);
extern void ncpy(char [], char [], int);

#endif
