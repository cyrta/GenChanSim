/* ignition noise header file

   $Id: ignition.h,v 0.0 1997/02/21 15:06:33 cmk Exp $
   $Log: ignition.h,v $
 */

#include <complex.h>

#define	YES				1
#define	NO				0
#define	MAXRAND			2147483647.0	/* max random number */
#define PI                      3.1415926535

/* functions */
extern float RandPoisson(float);
extern void ScaleComplex(float, fcomplex [], int);
extern int FWriteBuffer(char *, int, int, FILE *, int);

